#!/bin/bash

# TCDE ASI E2I Evolution Report - PDF Generation Script
# Génère un PDF professionnel à partir du rapport markdown

echo "🔄 Génération du rapport PDF TCDE ASI E2I Evolution..."
echo "Date: $(date)"
echo "========================================================"

# Vérifier que pandoc est installé
if ! command -v pandoc &> /dev/null; then
    echo "❌ Pandoc n'est pas installé"
    echo "Installation requise:"
    echo "  macOS: brew install pandoc"
    echo "  Ubuntu: sudo apt install pandoc"
    echo "  Windows: choco install pandoc"
    exit 1
fi

# Vérifier que le fichier source existe
if [ ! -f "TCDE_ASI_E2I_EVOLUTION_REPORT.md" ]; then
    echo "❌ Fichier source non trouvé: TCDE_ASI_E2I_EVOLUTION_REPORT.md"
    exit 1
fi

# Créer le répertoire de sortie
mkdir -p reports

# Générer le PDF avec pandoc
echo "📄 Génération du PDF avec Pandoc..."

pandoc "TCDE_ASI_E2I_EVOLUTION_REPORT.md" \
    -o "reports/TCDE_ASI_E2I_Evolution_Report.pdf" \
    --pdf-engine=xelatex \
    --template=eisvogel \
    --listings \
    --number-sections \
    --toc \
    --toc-depth=3 \
    --metadata title="TCDE ASI End-to-Infinite Evolution Report" \
    --metadata author="Mehdi Wahbi" \
    --metadata date="$(date '+%d %B %Y')" \
    --metadata subject="Artificial Superintelligence Research" \
    --metadata keywords="TCDE,ASI,E2I,Evolution,Consciousness,Autopoiesis" \
    --metadata lang="fr-FR" \
    --variable geometry:margin=2.5cm \
    --variable fontsize=11pt \
    --variable documentclass=article \
    --variable classoption=twoside \
    --variable colorlinks=true \
    --variable linkcolor=blue \
    --variable urlcolor=blue \
    --variable citecolor=blue \
    --variable toccolor=black \
    2>/dev/null

# Vérifier le succès de la génération
if [ $? -eq 0 ] && [ -f "reports/TCDE_ASI_E2I_Evolution_Report.pdf" ]; then
    echo "✅ PDF généré avec succès!"
    echo "📍 Emplacement: reports/TCDE_ASI_E2I_Evolution_Report.pdf"
    echo "📊 Taille: $(du -h reports/TCDE_ASI_E2I_Evolution_Report.pdf | cut -f1)"
    echo "📄 Pages: $(pdfinfo reports/TCDE_ASI_E2I_Evolution_Report.pdf 2>/dev/null | grep Pages | awk '{print $2}' || echo 'N/A')"
else
    echo "⚠️  Génération PDF échouée avec template eisvogel"
    echo "🔄 Tentative avec template par défaut..."
    
    # Fallback: génération simple sans template
    pandoc "TCDE_ASI_E2I_EVOLUTION_REPORT.md" \
        -o "reports/TCDE_ASI_E2I_Evolution_Report.pdf" \
        --pdf-engine=pdflatex \
        --number-sections \
        --toc \
        --toc-depth=3 \
        --metadata title="TCDE ASI End-to-Infinite Evolution Report" \
        --metadata author="Mehdi Wahbi" \
        --metadata date="$(date '+%d %B %Y')" \
        --variable geometry:margin=2.5cm \
        --variable fontsize=11pt \
        --variable documentclass=article \
        --variable colorlinks=true \
        --variable linkcolor=blue
    
    if [ $? -eq 0 ] && [ -f "reports/TCDE_ASI_E2I_Evolution_Report.pdf" ]; then
        echo "✅ PDF généré avec succès (template par défaut)!"
        echo "📍 Emplacement: reports/TCDE_ASI_E2I_Evolution_Report.pdf"
        echo "📊 Taille: $(du -h reports/TCDE_ASI_E2I_Evolution_Report.pdf | cut -f1)"
    else
        echo "❌ Échec de génération PDF"
        echo "Vérifiez l'installation de LaTeX:"
        echo "  macOS: brew install --cask mactex"
        echo "  Ubuntu: sudo apt install texlive-full"
        exit 1
    fi
fi

# Générer également une version HTML pour prévisualisation
echo "🌐 Génération version HTML..."
pandoc "TCDE_ASI_E2I_EVOLUTION_REPORT.md" \
    -o "reports/TCDE_ASI_E2I_Evolution_Report.html" \
    --standalone \
    --toc \
    --toc-depth=3 \
    --css=https://cdn.jsdelivr.net/npm/github-markdown-css@5/github-markdown-light.css \
    --metadata title="TCDE ASI End-to-Infinite Evolution Report" \
    --metadata author="Mehdi Wahbi" \
    --metadata date="$(date '+%d %B %Y')"

if [ -f "reports/TCDE_ASI_E2I_Evolution_Report.html" ]; then
    echo "✅ Version HTML générée: reports/TCDE_ASI_E2I_Evolution_Report.html"
fi

# Résumé final
echo ""
echo "📋 RÉSUMÉ DE GÉNÉRATION"
echo "======================="
echo "✅ Rapport source: TCDE_ASI_E2I_EVOLUTION_REPORT.md"
if [ -f "reports/TCDE_ASI_E2I_Evolution_Report.pdf" ]; then
    echo "✅ PDF généré: reports/TCDE_ASI_E2I_Evolution_Report.pdf"
fi
if [ -f "reports/TCDE_ASI_E2I_Evolution_Report.html" ]; then
    echo "✅ HTML généré: reports/TCDE_ASI_E2I_Evolution_Report.html"
fi
echo ""
echo "🔗 Pour ouvrir le PDF:"
echo "   macOS: open reports/TCDE_ASI_E2I_Evolution_Report.pdf"
echo "   Linux: xdg-open reports/TCDE_ASI_E2I_Evolution_Report.pdf"
echo "   Windows: start reports/TCDE_ASI_E2I_Evolution_Report.pdf"
echo ""
echo "🚀 Génération terminée avec succès!"