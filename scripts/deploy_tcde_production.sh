#!/bin/bash

# TCDE EXHAUSTIVE ANALYSIS ENGINE - PRODUCTION DEPLOYMENT SCRIPT
# Version: 1.0.0
# Date: 12 Décembre 2025
# Protocole: ZÉRO TOLÉRANCE

set -euo pipefail

# Configuration
DEPLOYMENT_VERSION="1.0.0"
PRODUCTION_USER="tcde"
PRODUCTION_GROUP="tcde"
BACKUP_DIR="/backup/tcde/pre_deployment_$(date +%Y%m%d_%H%M%S)"
LOG_FILE="/var/log/tcde/deployment_$(date +%Y%m%d_%H%M%S).log"

# Couleurs
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Fonctions utilitaires
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1" | tee -a "$LOG_FILE"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1" | tee -a "$LOG_FILE"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1" | tee -a "$LOG_FILE"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1" | tee -a "$LOG_FILE"
}

# Vérifications pré-déploiement
pre_deployment_checks() {
    log_info "=== VÉRIFICATIONS PRÉ-DÉPLOIEMENT ==="
    
    # Vérifier privilèges
    if [[ $EUID -ne 0 ]]; then
        log_error "Ce script doit être exécuté en tant que root"
        exit 1
    fi
    
    # Vérifier environnement de production
    if [[ ! -f "/etc/production.flag" ]]; then
        log_warning "Environnement de production non détecté"
        read -p "Continuer le déploiement? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            log_info "Déploiement annulé par l'utilisateur"
            exit 0
        fi
    fi
    
    # Vérifier ressources système
    local available_memory=$(free -m | awk 'NR==2{print $7}')
    if [[ $available_memory -lt 2048 ]]; then
        log_error "Mémoire disponible insuffisante: ${available_memory}MB (requis: 2048MB)"
        exit 1
    fi
    
    local available_disk=$(df / | awk 'NR==2 {print $4}')
    if [[ $available_disk -lt 2097152 ]]; then
        log_error "Espace disque insuffisant: $((available_disk/1024/1024))GB (requis: 2GB)"
        exit 1
    fi
    
    # Vérifier dépendances
    local missing_deps=()
    for dep in gcc make systemctl; do
        if ! command -v "$dep" &> /dev/null; then
            missing_deps+=("$dep")
        fi
    done
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        log_error "Dépendances manquantes: ${missing_deps[*]}"
        exit 1
    fi
    
    log_success "Vérifications pré-déploiement réussies"
}

# Sauvegarde système existant
backup_existing_system() {
    log_info "=== SAUVEGARDE SYSTÈME EXISTANT ==="
    
    mkdir -p "$BACKUP_DIR"
    
    # Sauvegarder installation existante si présente
    if [[ -d "/usr/local/tcde" ]]; then
        log_info "Sauvegarde installation existante..."
        cp -r /usr/local/tcde "$BACKUP_DIR/"
        
        # Arrêter services existants
        if systemctl is-active --quiet tcde-analysis; then
            log_info "Arrêt service existant..."
            systemctl stop tcde-analysis
        fi
    fi
    
    # Sauvegarder configuration existante
    if [[ -d "/etc/tcde" ]]; then
        log_info "Sauvegarde configuration existante..."
        cp -r /etc/tcde "$BACKUP_DIR/"
    fi
    
    # Sauvegarder données existantes
    if [[ -d "/var/lib/tcde" ]]; then
        log_info "Sauvegarde données existantes..."
        cp -r /var/lib/tcde "$BACKUP_DIR/"
    fi
    
    log_success "Sauvegarde complétée: $BACKUP_DIR"
}

# Tests de validation pré-déploiement
run_pre_deployment_tests() {
    log_info "=== TESTS DE VALIDATION PRÉ-DÉPLOIEMENT ==="
    
    # Compiler et tester en mode staging
    log_info "Compilation en mode staging..."
    if ! make clean && make all CFLAGS="-O3 -DNDEBUG -DPRODUCTION"; then
        log_error "Échec compilation en mode production"
        exit 1
    fi
    
    # Tests unitaires
    log_info "Exécution tests unitaires..."
    if ! make test; then
        log_error "Échec tests unitaires"
        exit 1
    fi
    
    # Tests d'intégration
    log_info "Exécution tests d'intégration..."
    if [[ -f "tests/test_deployment_validation" ]]; then
        if ! ./tests/test_deployment_validation; then
            log_error "Échec tests de déploiement"
            exit 1
        fi
    fi
    
    log_success "Tous les tests pré-déploiement réussis"
}

# Installation en production
install_production_system() {
    log_info "=== INSTALLATION SYSTÈME PRODUCTION ==="
    
    # Créer utilisateur/groupe de production
    if ! id "$PRODUCTION_USER" &>/dev/null; then
        log_info "Création utilisateur de production: $PRODUCTION_USER"
        useradd -r -s /bin/false -d /var/lib/tcde -c "TCDE Analysis Engine" "$PRODUCTION_USER"
    fi
    
    # Installation binaires
    log_info "Installation binaires de production..."
    mkdir -p /usr/local/tcde/{bin,lib,include,share}
    
    cp tcde_analysis /usr/local/tcde/bin/
    chmod 755 /usr/local/tcde/bin/tcde_analysis
    chown root:root /usr/local/tcde/bin/tcde_analysis
    
    # Installation bibliothèques
    if [[ -f "libtcde.a" ]]; then
        cp libtcde.a /usr/local/tcde/lib/
        chmod 644 /usr/local/tcde/lib/libtcde.a
    fi
    
    # Installation headers
    find src -name "*.h" -exec cp {} /usr/local/tcde/include/ \;
    chmod 644 /usr/local/tcde/include/*.h
    
    # Lien symbolique global
    ln -sf /usr/local/tcde/bin/tcde_analysis /usr/local/bin/tcde_analysis
    
    log_success "Installation binaires complétée"
}

# Configuration production
configure_production_system() {
    log_info "=== CONFIGURATION SYSTÈME PRODUCTION ==="
    
    # Configuration principale
    mkdir -p /etc/tcde
    cat > /etc/tcde/tcde.conf << EOF
# TCDE Exhaustive Analysis Engine - Production Configuration
# Version: $DEPLOYMENT_VERSION
# Deployed: $(date)

[general]
log_level = INFO
max_memory = 8GB
max_threads = auto
output_format = html
environment = production

[analysis]
zero_tolerance = true
deep_analysis = true
parallel_processing = true
cache_results = true
timeout = 3600

[validation]
authenticity_check = true
quality_threshold = 95
standards_compliance = ieee,acm,tcde
strict_mode = true

[reporting]
generate_html = true
generate_json = true
generate_markdown = false
compress_output = true

[security]
enable_audit_log = true
restrict_file_access = true
validate_inputs = true

[performance]
enable_profiling = false
memory_monitoring = true
performance_alerts = true
EOF
    
    chmod 644 /etc/tcde/tcde.conf
    chown root:root /etc/tcde/tcde.conf
    
    # Répertoires de travail
    mkdir -p /var/lib/tcde/{cache,temp,results}
    mkdir -p /var/log/tcde
    
    chown -R "$PRODUCTION_USER:$PRODUCTION_GROUP" /var/lib/tcde
    chown -R "$PRODUCTION_USER:$PRODUCTION_GROUP" /var/log/tcde
    
    chmod 750 /var/lib/tcde
    chmod 750 /var/log/tcde
    
    # Variables d'environnement
    cat > /etc/profile.d/tcde.sh << EOF
# TCDE Production Environment
export TCDE_HOME="/usr/local/tcde"
export TCDE_CONFIG="/etc/tcde/tcde.conf"
export TCDE_LOG_DIR="/var/log/tcde"
export PATH="\$PATH:/usr/local/tcde/bin"
EOF
    
    chmod 644 /etc/profile.d/tcde.sh
    
    log_success "Configuration production complétée"
}

# Configuration service systemd
configure_systemd_service() {
    log_info "=== CONFIGURATION SERVICE SYSTEMD ==="
    
    cat > /etc/systemd/system/tcde-analysis.service << EOF
[Unit]
Description=TCDE Exhaustive Analysis Engine
Documentation=file:///usr/local/share/doc/tcde/
After=network.target
Wants=network.target

[Service]
Type=notify
User=$PRODUCTION_USER
Group=$PRODUCTION_GROUP
ExecStart=/usr/local/tcde/bin/tcde_analysis --daemon --config /etc/tcde/tcde.conf
ExecReload=/bin/kill -HUP \$MAINPID
KillMode=mixed
Restart=on-failure
RestartSec=5
TimeoutStartSec=60
TimeoutStopSec=30

# Sécurité
NoNewPrivileges=true
ProtectSystem=strict
ProtectHome=true
ReadWritePaths=/var/lib/tcde /var/log/tcde /tmp
PrivateTmp=true
ProtectKernelTunables=true
ProtectControlGroups=true
RestrictRealtime=true

# Ressources
LimitNOFILE=65536
LimitNPROC=4096
MemoryMax=8G
CPUQuota=400%

# Logs
StandardOutput=journal
StandardError=journal
SyslogIdentifier=tcde-analysis

[Install]
WantedBy=multi-user.target
EOF
    
    # Recharger systemd
    systemctl daemon-reload
    systemctl enable tcde-analysis
    
    log_success "Service systemd configuré"
}

# Configuration monitoring
configure_monitoring() {
    log_info "=== CONFIGURATION MONITORING ==="
    
    # Script de health check
    cat > /usr/local/bin/tcde_health_check.sh << 'EOF'
#!/bin/bash
# TCDE Health Check Script

HEALTH_STATUS=$(tcde_analysis --health-check --quiet 2>/dev/null)
EXIT_CODE=$?

if [ $EXIT_CODE -eq 0 ]; then
    echo "OK - TCDE system healthy"
    exit 0
elif [ $EXIT_CODE -eq 1 ]; then
    echo "WARNING - TCDE system degraded"
    exit 1
else
    echo "CRITICAL - TCDE system failure"
    exit 2
fi
EOF
    
    chmod +x /usr/local/bin/tcde_health_check.sh
    
    # Configuration logrotate
    cat > /etc/logrotate.d/tcde << EOF
/var/log/tcde/*.log {
    daily
    rotate 30
    compress
    delaycompress
    missingok
    notifempty
    create 640 $PRODUCTION_USER $PRODUCTION_GROUP
    postrotate
        systemctl reload tcde-analysis > /dev/null 2>&1 || true
    endscript
}
EOF
    
    # Cron job pour monitoring
    cat > /etc/cron.d/tcde-monitoring << EOF
# TCDE Monitoring Jobs
SHELL=/bin/bash
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin

# Health check every 5 minutes
*/5 * * * * root /usr/local/bin/tcde_health_check.sh >> /var/log/tcde/health_check.log 2>&1

# Daily cleanup
0 2 * * * $PRODUCTION_USER find /var/lib/tcde/temp -type f -mtime +1 -delete

# Weekly optimization
0 3 * * 0 $PRODUCTION_USER tcde_analysis --optimize-cache >> /var/log/tcde/optimization.log 2>&1
EOF
    
    log_success "Monitoring configuré"
}

# Tests post-déploiement
run_post_deployment_tests() {
    log_info "=== TESTS POST-DÉPLOIEMENT ==="
    
    # Démarrer le service
    log_info "Démarrage service TCDE..."
    systemctl start tcde-analysis
    
    # Attendre démarrage
    sleep 5
    
    # Vérifier statut service
    if ! systemctl is-active --quiet tcde-analysis; then
        log_error "Service TCDE non démarré"
        systemctl status tcde-analysis
        exit 1
    fi
    
    # Tests fonctionnels
    log_info "Tests fonctionnels post-déploiement..."
    
    # Test version
    if ! tcde_analysis --version &>/dev/null; then
        log_error "Test version échoué"
        exit 1
    fi
    
    # Test health check
    if ! /usr/local/bin/tcde_health_check.sh &>/dev/null; then
        log_error "Health check échoué"
        exit 1
    fi
    
    # Test analyse simple
    mkdir -p /tmp/tcde_production_test
    echo 'int main() { return 0; }' > /tmp/tcde_production_test/test.c
    
    if tcde_analysis --project /tmp/tcde_production_test --output /tmp/tcde_production_results &>/dev/null; then
        log_success "Test analyse simple réussi"
        rm -rf /tmp/tcde_production_test /tmp/tcde_production_results
    else
        log_error "Test analyse simple échoué"
        exit 1
    fi
    
    log_success "Tous les tests post-déploiement réussis"
}

# Finalisation déploiement
finalize_deployment() {
    log_info "=== FINALISATION DÉPLOIEMENT ==="
    
    # Marquer comme déployé en production
    echo "TCDE_VERSION=$DEPLOYMENT_VERSION" > /etc/tcde/deployment.info
    echo "DEPLOYMENT_DATE=$(date)" >> /etc/tcde/deployment.info
    echo "DEPLOYMENT_USER=$(whoami)" >> /etc/tcde/deployment.info
    
    # Documentation
    mkdir -p /usr/local/share/doc/tcde
    cp -r docs/* /usr/local/share/doc/tcde/
    
    # Permissions finales
    chown -R root:root /usr/local/tcde
    chown -R root:root /etc/tcde
    chown -R "$PRODUCTION_USER:$PRODUCTION_GROUP" /var/lib/tcde
    chown -R "$PRODUCTION_USER:$PRODUCTION_GROUP" /var/log/tcde
    
    # Nettoyage
    make clean &>/dev/null || true
    
    log_success "Déploiement finalisé"
}

# Affichage informations finales
display_deployment_summary() {
    echo
    log_success "╔══════════════════════════════════════════════════════════════╗"
    log_success "║              DÉPLOIEMENT PRODUCTION RÉUSSI                   ║"
    log_success "╚══════════════════════════════════════════════════════════════╝"
    echo
    echo -e "${BLUE}=== INFORMATIONS DÉPLOIEMENT ===${NC}"
    echo -e "Version déployée: ${GREEN}$DEPLOYMENT_VERSION${NC}"
    echo -e "Date déploiement: ${GREEN}$(date)${NC}"
    echo -e "Utilisateur: ${GREEN}$PRODUCTION_USER${NC}"
    echo -e "Configuration: ${GREEN}/etc/tcde/tcde.conf${NC}"
    echo -e "Logs: ${GREEN}/var/log/tcde/${NC}"
    echo -e "Documentation: ${GREEN}/usr/local/share/doc/tcde/${NC}"
    echo -e "Sauvegarde: ${GREEN}$BACKUP_DIR${NC}"
    echo
    echo -e "${BLUE}=== COMMANDES UTILES ===${NC}"
    echo -e "Statut service: ${GREEN}systemctl status tcde-analysis${NC}"
    echo -e "Logs service: ${GREEN}journalctl -u tcde-analysis -f${NC}"
    echo -e "Health check: ${GREEN}/usr/local/bin/tcde_health_check.sh${NC}"
    echo -e "Analyse: ${GREEN}tcde_analysis --project /path/to/project${NC}"
    echo
    echo -e "${BLUE}=== MONITORING ===${NC}"
    echo -e "Health checks automatiques toutes les 5 minutes"
    echo -e "Rotation logs quotidienne (30 jours de rétention)"
    echo -e "Optimisation cache hebdomadaire"
    echo
    echo -e "${YELLOW}Consultez la documentation complète: /usr/local/share/doc/tcde/${NC}"
}

# Fonction de rollback en cas d'erreur
rollback_deployment() {
    log_error "Erreur détectée - Rollback en cours..."
    
    # Arrêter service si démarré
    systemctl stop tcde-analysis &>/dev/null || true
    systemctl disable tcde-analysis &>/dev/null || true
    
    # Restaurer depuis sauvegarde si disponible
    if [[ -d "$BACKUP_DIR" ]]; then
        log_info "Restauration depuis sauvegarde..."
        
        if [[ -d "$BACKUP_DIR/tcde" ]]; then
            rm -rf /usr/local/tcde
            cp -r "$BACKUP_DIR/tcde" /usr/local/
        fi
        
        if [[ -d "$BACKUP_DIR/etc/tcde" ]]; then
            rm -rf /etc/tcde
            cp -r "$BACKUP_DIR/etc/tcde" /etc/
        fi
        
        if [[ -d "$BACKUP_DIR/var/lib/tcde" ]]; then
            rm -rf /var/lib/tcde
            cp -r "$BACKUP_DIR/var/lib/tcde" /var/lib/
        fi
    fi
    
    log_error "Rollback complété - Système restauré"
    exit 1
}

# Fonction principale
main() {
    # Créer répertoire de logs
    mkdir -p /var/log/tcde
    
    echo -e "${BLUE}╔══════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║           TCDE PRODUCTION DEPLOYMENT SCRIPT                  ║${NC}"
    echo -e "${BLUE}║                    Version $DEPLOYMENT_VERSION                           ║${NC}"
    echo -e "${BLUE}║                Protocole ZÉRO TOLÉRANCE                      ║${NC}"
    echo -e "${BLUE}╚══════════════════════════════════════════════════════════════╝${NC}"
    echo
    
    log_info "Début déploiement production - $(date)"
    
    # Gestion des erreurs avec rollback
    trap rollback_deployment ERR
    
    # Étapes de déploiement
    pre_deployment_checks
    backup_existing_system
    run_pre_deployment_tests
    install_production_system
    configure_production_system
    configure_systemd_service
    configure_monitoring
    run_post_deployment_tests
    finalize_deployment
    
    # Désactiver trap d'erreur
    trap - ERR
    
    display_deployment_summary
    
    log_success "Déploiement production complété avec succès!"
}

# Exécution
main "$@"