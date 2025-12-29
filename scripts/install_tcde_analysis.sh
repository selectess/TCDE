#!/bin/bash

# TCDE EXHAUSTIVE ANALYSIS ENGINE - INSTALLATION SCRIPT
# Version: 1.0.0
# Date: 12 Décembre 2025
# Protocole: ZÉRO TOLÉRANCE

set -euo pipefail

# Configuration
TCDE_VERSION="1.0.0"
INSTALL_DIR="/usr/local/tcde"
BIN_DIR="/usr/local/bin"
DOC_DIR="/usr/local/share/doc/tcde"
CONFIG_DIR="/etc/tcde"

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Fonctions utilitaires
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Vérification des prérequis
check_prerequisites() {
    log_info "Vérification des prérequis système..."
    
    # Vérifier GCC/Clang
    if ! command -v gcc &> /dev/null && ! command -v clang &> /dev/null; then
        log_error "GCC ou Clang requis pour la compilation"
        exit 1
    fi
    
    # Vérifier Make
    if ! command -v make &> /dev/null; then
        log_error "Make requis pour la compilation"
        exit 1
    fi
    
    # Vérifier les permissions
    if [[ $EUID -ne 0 ]]; then
        log_error "Ce script doit être exécuté en tant que root (sudo)"
        exit 1
    fi
    
    # Vérifier l'espace disque (minimum 2GB)
    available_space=$(df / | awk 'NR==2 {print $4}')
    required_space=2097152  # 2GB en KB
    
    if [[ $available_space -lt $required_space ]]; then
        log_error "Espace disque insuffisant. Requis: 2GB, Disponible: $((available_space/1024/1024))GB"
        exit 1
    fi
    
    # Vérifier la mémoire (minimum 4GB)
    total_memory=$(grep MemTotal /proc/meminfo | awk '{print $2}')
    required_memory=4194304  # 4GB en KB
    
    if [[ $total_memory -lt $required_memory ]]; then
        log_warning "Mémoire système faible. Recommandé: 4GB, Disponible: $((total_memory/1024/1024))GB"
    fi
    
    log_success "Prérequis système validés"
}

# Création des répertoires
create_directories() {
    log_info "Création des répertoires d'installation..."
    
    mkdir -p "$INSTALL_DIR"/{bin,lib,include,share}
    mkdir -p "$DOC_DIR"
    mkdir -p "$CONFIG_DIR"
    mkdir -p /var/log/tcde
    mkdir -p /var/lib/tcde
    
    log_success "Répertoires créés"
}

# Compilation du système
compile_system() {
    log_info "Compilation du TCDE Exhaustive Analysis Engine..."
    
    # Nettoyer les builds précédents
    make clean 2>/dev/null || true
    
    # Compiler avec optimisations
    if make all CFLAGS="-O3 -DNDEBUG -march=native"; then
        log_success "Compilation réussie"
    else
        log_error "Échec de la compilation"
        exit 1
    fi
    
    # Vérifier les binaires
    if [[ ! -f "tcde_analysis" ]]; then
        log_error "Binaire principal non trouvé après compilation"
        exit 1
    fi
    
    log_success "Binaires générés avec succès"
}

# Installation des fichiers
install_files() {
    log_info "Installation des fichiers système..."
    
    # Binaires
    cp tcde_analysis "$INSTALL_DIR/bin/"
    chmod +x "$INSTALL_DIR/bin/tcde_analysis"
    ln -sf "$INSTALL_DIR/bin/tcde_analysis" "$BIN_DIR/tcde_analysis"
    
    # Bibliothèques
    if [[ -f "libtcde.a" ]]; then
        cp libtcde.a "$INSTALL_DIR/lib/"
    fi
    
    # Headers
    find src -name "*.h" -exec cp {} "$INSTALL_DIR/include/" \;
    
    # Documentation
    cp -r docs/* "$DOC_DIR/"
    
    # Configuration par défaut
    cat > "$CONFIG_DIR/tcde.conf" << EOF
# TCDE Exhaustive Analysis Engine Configuration
# Version: $TCDE_VERSION

[general]
log_level = INFO
max_memory = 4GB
max_threads = auto
output_format = html

[analysis]
zero_tolerance = true
deep_analysis = true
parallel_processing = true

[validation]
authenticity_check = true
quality_threshold = 90
standards_compliance = ieee,acm,tcde

[reporting]
generate_html = true
generate_json = true
generate_markdown = false
EOF
    
    log_success "Fichiers installés"
}

# Configuration du système
configure_system() {
    log_info "Configuration du système..."
    
    # Créer l'utilisateur tcde
    if ! id "tcde" &>/dev/null; then
        useradd -r -s /bin/false -d /var/lib/tcde tcde
        log_success "Utilisateur tcde créé"
    fi
    
    # Permissions
    chown -R tcde:tcde /var/lib/tcde
    chown -R tcde:tcde /var/log/tcde
    chmod 755 "$INSTALL_DIR/bin/tcde_analysis"
    
    # Variables d'environnement
    cat > /etc/profile.d/tcde.sh << EOF
# TCDE Environment Variables
export TCDE_HOME="$INSTALL_DIR"
export TCDE_CONFIG="$CONFIG_DIR/tcde.conf"
export PATH="\$PATH:$INSTALL_DIR/bin"
EOF
    
    log_success "Système configuré"
}

# Tests post-installation
run_tests() {
    log_info "Exécution des tests post-installation..."
    
    # Test de base
    if tcde_analysis --version &>/dev/null; then
        log_success "Test version: OK"
    else
        log_error "Test version: ÉCHEC"
        return 1
    fi
    
    # Test de santé
    if tcde_analysis --health-check &>/dev/null; then
        log_success "Test santé: OK"
    else
        log_warning "Test santé: AVERTISSEMENT"
    fi
    
    # Test auto-diagnostic
    if tcde_analysis --self-test &>/dev/null; then
        log_success "Test auto-diagnostic: OK"
    else
        log_warning "Test auto-diagnostic: AVERTISSEMENT"
    fi
    
    log_success "Tests post-installation terminés"
}

# Création du service systemd (optionnel)
create_service() {
    log_info "Création du service systemd..."
    
    cat > /etc/systemd/system/tcde-analysis.service << EOF
[Unit]
Description=TCDE Exhaustive Analysis Engine
After=network.target

[Service]
Type=oneshot
User=tcde
Group=tcde
ExecStart=$INSTALL_DIR/bin/tcde_analysis --daemon
WorkingDirectory=/var/lib/tcde
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
EOF
    
    systemctl daemon-reload
    log_success "Service systemd créé"
}

# Affichage des informations finales
show_completion_info() {
    log_success "Installation TCDE Exhaustive Analysis Engine terminée!"
    echo
    echo -e "${BLUE}=== INFORMATIONS D'INSTALLATION ===${NC}"
    echo -e "Version installée: ${GREEN}$TCDE_VERSION${NC}"
    echo -e "Répertoire d'installation: ${GREEN}$INSTALL_DIR${NC}"
    echo -e "Configuration: ${GREEN}$CONFIG_DIR/tcde.conf${NC}"
    echo -e "Documentation: ${GREEN}$DOC_DIR${NC}"
    echo -e "Logs: ${GREEN}/var/log/tcde${NC}"
    echo
    echo -e "${BLUE}=== UTILISATION ===${NC}"
    echo -e "Commande principale: ${GREEN}tcde_analysis${NC}"
    echo -e "Aide: ${GREEN}tcde_analysis --help${NC}"
    echo -e "Version: ${GREEN}tcde_analysis --version${NC}"
    echo -e "Test: ${GREEN}tcde_analysis --self-test${NC}"
    echo
    echo -e "${BLUE}=== EXEMPLES ===${NC}"
    echo -e "Analyse simple: ${GREEN}tcde_analysis --project . --output results/${NC}"
    echo -e "Analyse complète: ${GREEN}tcde_analysis --project . --mode exhaustive --zero-tolerance${NC}"
    echo
    echo -e "${YELLOW}Consultez la documentation complète: $DOC_DIR/USER_GUIDE.md${NC}"
}

# Fonction principale
main() {
    echo -e "${BLUE}╔══════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║        TCDE EXHAUSTIVE ANALYSIS ENGINE INSTALLER             ║${NC}"
    echo -e "${BLUE}║                    Version $TCDE_VERSION                           ║${NC}"
    echo -e "${BLUE}║                Protocole ZÉRO TOLÉRANCE                      ║${NC}"
    echo -e "${BLUE}╚══════════════════════════════════════════════════════════════╝${NC}"
    echo
    
    check_prerequisites
    create_directories
    compile_system
    install_files
    configure_system
    run_tests
    create_service
    show_completion_info
    
    log_success "Installation complète avec succès!"
}

# Gestion des erreurs
trap 'log_error "Installation interrompue"; exit 1' ERR

# Exécution
main "$@"