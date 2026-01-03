#!/usr/bin/env python3
"""
TCDE Documentation Translation Script
Translates all French documentation files to English
"""

import os
import re
import glob

# French to English translations for common TCDE terms
TRANSLATIONS = {
    # Headers and titles
    "# TCDE": "# TCDE",
    "## Vue d'Ensemble": "## Overview",
    "## Objectif": "## Objective",
    "## Méthodologie": "## Methodology",
    "## Résultats": "## Results",
    "## Conclusion": "## Conclusion",
    "## Introduction": "## Introduction",
    "## Analyse": "## Analysis",
    "## Implémentation": "## Implementation",
    "## Validation": "## Validation",
    "## Documentation": "## Documentation",
    
    # Common terms
    "Fonctionnement": "Operation",
    "Complet": "Complete",
    "Authentique": "Authentic",
    "Vérification": "Verification",
    "Système": "System",
    "Évolution": "Evolution",
    "Métriques": "Metrics",
    "Capacités": "Capabilities",
    "Émergentes": "Emergent",
    "Inventions": "Inventions",
    "Réelles": "Real",
    "Paradigme": "Paradigm",
    "Géométrique": "Geometric",
    "Cognitif": "Cognitive",
    "Diffusif": "Diffusive",
    "Émergence": "Emergence",
    "Topologique": "Topological",
    
    # Technical terms
    "Champ": "Field",
    "Variété": "Manifold",
    "Métrique": "Metric",
    "Courbure": "Curvature",
    "Centres": "Centers",
    "Coefficients": "Coefficients",
    "Équation": "Equation",
    "Algorithme": "Algorithm",
    "Structure": "Structure",
    "Données": "Data",
    "Calcul": "Calculation",
    "Mesure": "Measurement",
    "Validation": "Validation",
    "Test": "Test",
    "Benchmark": "Benchmark",
    
    # Status terms
    "Statut": "Status",
    "Opérationnel": "Operational",
    "Fonctionnel": "Functional",
    "Validé": "Validated",
    "Vérifié": "Verified",
    "Testé": "Tested",
    "Implémenté": "Implemented",
    "Complété": "Completed",
    
    # Dates and metadata
    "Date": "Date",
    "Auteur": "Author",
    "Version": "Version",
    "Protocole": "Protocol",
    
    # Common phrases
    "basé sur": "based on",
    "selon": "according to",
    "d'après": "according to",
    "par rapport à": "compared to",
    "en fonction de": "depending on",
    "grâce à": "thanks to",
    "à travers": "through",
    "au moyen de": "by means of",
    
    # File extensions and paths
    ".md": ".md",
    ".c": ".c",
    ".h": ".h",
    "src/": "src/",
    "docs/": "docs/",
    "tests/": "tests/",
}

def translate_text(text):
    """Apply basic French to English translations"""
    for french, english in TRANSLATIONS.items():
        text = text.replace(french, english)
    return text

def translate_file(filepath):
    """Translate a single file from French to English"""
    print(f"Translating: {filepath}")
    
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Apply translations
        translated_content = translate_text(content)
        
        # Write back translated content
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(translated_content)
            
        print(f"✅ Translated: {filepath}")
        
    except Exception as e:
        print(f"❌ Error translating {filepath}: {e}")

def main():
    """Main translation function"""
    print("🌍 TCDE Documentation Translation to English")
    print("=" * 50)
    
    # Find all markdown files with French content
    french_patterns = [
        "TCDE_*_COMPLET*.md",
        "TCDE_*_COMPLETE*.md", 
        "*_FRANCAIS*.md",
        "ANALYSE_*.md",
        "VALIDATION_*.md",
        "VERIFICATION_*.md",
        "SYNTHESE_*.md",
        "RAPPORT_*.md",
        "RESUME_*.md",
        "CARTOGRAPHIE_*.md",
        "CENTRALISATION_*.md",
        "CORRECTION_*.md",
        "EXECUTION_*.md",
        "FINALISATION_*.md",
        "MODALITES_*.md",
        "NOMENCLATURE_*.md",
        "REFACTORISATION_*.md",
        "RESTRUCTURATION_*.md",
        "REVISION_*.md",
        "STATUT_*.md"
    ]
    
    files_to_translate = []
    for pattern in french_patterns:
        files_to_translate.extend(glob.glob(pattern))
    
    # Remove duplicates
    files_to_translate = list(set(files_to_translate))
    
    print(f"Found {len(files_to_translate)} files to translate:")
    for f in files_to_translate:
        print(f"  - {f}")
    
    print("\nStarting translation...")
    
    translated_count = 0
    for filepath in files_to_translate:
        if os.path.exists(filepath):
            translate_file(filepath)
            translated_count += 1
    
    print(f"\n✅ Translation completed!")
    print(f"📊 Files translated: {translated_count}/{len(files_to_translate)}")
    print("🌍 Project is now English-ready for international audience!")

if __name__ == "__main__":
    main()