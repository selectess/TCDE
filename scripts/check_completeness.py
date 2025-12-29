#!/usr/bin/env python3
"""
TCDE Complete Validation - Completeness Checker
Date: November 10, 2025
Protocol: Zero Tolerance v3.0

Verifies that all required validation components are present and complete.
"""

import os
import sys
from typing import List, Tuple

class CompletenessChecker:
    """Checks completeness of validation package"""
    
    def __init__(self):
        self.required_files = []
        self.missing_files = []
        self.present_files = []
    
    def check_file(self, filepath: str, description: str) -> bool:
        """Check if a file exists"""
        exists = os.path.exists(filepath)
        
        if exists:
            self.present_files.append((filepath, description))
            print(f"✓ {description}: {filepath}")
        else:
            self.missing_files.append((filepath, description))
            print(f"✗ {description}: {filepath} [MISSING]")
        
        return exists
    
    def check_validation_framework(self) -> bool:
        """Check validation framework files"""
        print("\n=== Validation Framework ===")
        
        all_present = True
        all_present &= self.check_file(
            "src/validation/tcde_capability_validator.h",
            "Capability Validator Header"
        )
        all_present &= self.check_file(
            "src/validation/tcde_capability_validator.c",
            "Capability Validator Implementation"
        )
        all_present &= self.check_file(
            "src/validation/tcde_statistical_validator.h",
            "Statistical Validator Header"
        )
        all_present &= self.check_file(
            "src/validation/tcde_statistical_validator.c",
            "Statistical Validator Implementation"
        )
        
        return all_present
    
    def check_test_executables(self) -> Tuple[int, int]:
        """Check test executable files"""
        print("\n=== Test Executables ===")
        
        test_categories = [
            ("tests/test_consciousness_complete", "Consciousness Tests"),
            ("tests/test_temporality_complete", "Temporality Tests"),
            ("tests/test_intentionality_complete", "Intentionality Tests"),
            ("tests/test_creativity_complete", "Creativity Tests"),
            ("tests/test_autopoiesis_complete", "Autopoiesis Tests"),
            ("tests/test_emergence_complete", "Emergence Tests"),
            ("tests/test_memory_complete", "Memory Tests"),
            ("tests/test_geometry_complete", "Geometry Tests"),
            ("tests/test_coupling_complete", "Coupling Tests"),
            ("tests/test_multimodality_complete", "Multimodality Tests"),
        ]
        
        present = 0
        total = len(test_categories)
        
        for test_path, description in test_categories:
            if self.check_file(test_path, description):
                present += 1
        
        return present, total
    
    def check_scripts(self) -> bool:
        """Check automation scripts"""
        print("\n=== Automation Scripts ===")
        
        all_present = True
        all_present &= self.check_file(
            "scripts/run_complete_validation.sh",
            "Master Test Runner"
        )
        all_present &= self.check_file(
            "scripts/collect_validation_results.py",
            "Results Collector"
        )
        all_present &= self.check_file(
            "scripts/check_completeness.py",
            "Completeness Checker"
        )
        
        return all_present
    
    def check_documentation(self) -> bool:
        """Check documentation files"""
        print("\n=== Documentation ===")
        
        all_present = True
        all_present &= self.check_file(
            ".kiro/specs/tcde-complete-validation/requirements.md",
            "Requirements Document"
        )
        all_present &= self.check_file(
            ".kiro/specs/tcde-complete-validation/design.md",
            "Design Document"
        )
        all_present &= self.check_file(
            ".kiro/specs/tcde-complete-validation/tasks.md",
            "Tasks Document"
        )
        
        return all_present
    
    def generate_report(self):
        """Generate completeness report"""
        print("\n" + "="*60)
        print("COMPLETENESS REPORT")
        print("="*60)
        
        total_required = len(self.present_files) + len(self.missing_files)
        present_count = len(self.present_files)
        missing_count = len(self.missing_files)
        
        print(f"\nTotal Required Files: {total_required}")
        print(f"Present: {present_count}")
        print(f"Missing: {missing_count}")
        print(f"Completeness: {(present_count/total_required)*100:.2f}%")
        
        if missing_count > 0:
            print("\n⚠ Missing Files:")
            for filepath, description in self.missing_files:
                print(f"  - {description}: {filepath}")
        
        print("\n" + "="*60)
        
        if missing_count == 0:
            print("✓ PACKAGE COMPLETE")
            return True
        else:
            print("✗ PACKAGE INCOMPLETE")
            return False

def main():
    """Main execution"""
    
    print("TCDE Complete Validation - Completeness Check")
    print("Protocol: Zero Tolerance v3.0")
    print("="*60)
    
    checker = CompletenessChecker()
    
    # Check all components
    framework_complete = checker.check_validation_framework()
    tests_present, tests_total = checker.check_test_executables()
    scripts_complete = checker.check_scripts()
    docs_complete = checker.check_documentation()
    
    # Generate report
    is_complete = checker.generate_report()
    
    # Exit with appropriate code
    sys.exit(0 if is_complete else 1)

if __name__ == "__main__":
    main()
