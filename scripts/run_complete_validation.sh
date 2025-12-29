#!/bin/bash
# TCDE Complete Validation - Master Test Runner
# Date: November 10, 2025
# Protocol: Zero Tolerance v3.0

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
OUTPUT_DIR="validation_results"
JSON_OUTPUT="${OUTPUT_DIR}/complete_validation_results.json"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
LOG_FILE="${OUTPUT_DIR}/validation_${TIMESTAMP}.log"

# Create output directory
mkdir -p "${OUTPUT_DIR}"

# Initialize JSON output
echo "{" > "${JSON_OUTPUT}"
echo "  \"validation_run\": {" >> "${JSON_OUTPUT}"
echo "    \"timestamp\": \"$(date -u +%Y-%m-%dT%H:%M:%SZ)\"," >> "${JSON_OUTPUT}"
echo "    \"protocol\": \"Zero Tolerance v3.0\"," >> "${JSON_OUTPUT}"
echo "    \"total_capabilities\": 31," >> "${JSON_OUTPUT}"
echo "    \"test_categories\": [" >> "${JSON_OUTPUT}"

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to log messages
log_message() {
    local level=$1
    shift
    local message="$@"
    echo -e "${message}" | tee -a "${LOG_FILE}"
}

# Function to run test category
run_test_category() {
    local category_name=$1
    local test_executable=$2
    local capability_ids=$3
    
    log_message "INFO" "${BLUE}=== Testing ${category_name} ===${NC}"
    
    if [ -f "${test_executable}" ]; then
        log_message "INFO" "Executing: ${test_executable}"
        
        # Run test and capture output
        if ./"${test_executable}" > "${OUTPUT_DIR}/${category_name}_output.txt" 2>&1; then
            log_message "INFO" "${GREEN}✓ ${category_name}: PASSED${NC}"
            PASSED_TESTS=$((PASSED_TESTS + 1))
            
            # Add to JSON
            echo "      {" >> "${JSON_OUTPUT}"
            echo "        \"category\": \"${category_name}\"," >> "${JSON_OUTPUT}"
            echo "        \"status\": \"PASSED\"," >> "${JSON_OUTPUT}"
            echo "        \"capability_ids\": [${capability_ids}]," >> "${JSON_OUTPUT}"
            echo "        \"output_file\": \"${category_name}_output.txt\"" >> "${JSON_OUTPUT}"
            echo "      }," >> "${JSON_OUTPUT}"
        else
            log_message "ERROR" "${RED}✗ ${category_name}: FAILED${NC}"
            FAILED_TESTS=$((FAILED_TESTS + 1))
            
            # Add to JSON
            echo "      {" >> "${JSON_OUTPUT}"
            echo "        \"category\": \"${category_name}\"," >> "${JSON_OUTPUT}"
            echo "        \"status\": \"FAILED\"," >> "${JSON_OUTPUT}"
            echo "        \"capability_ids\": [${capability_ids}]," >> "${JSON_OUTPUT}"
            echo "        \"output_file\": \"${category_name}_output.txt\"" >> "${JSON_OUTPUT}"
            echo "      }," >> "${JSON_OUTPUT}"
        fi
        
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
    else
        log_message "WARN" "${YELLOW}⚠ ${test_executable} not found - skipping${NC}"
    fi
}

# Header
log_message "INFO" "${BLUE}╔════════════════════════════════════════════════════════╗${NC}"
log_message "INFO" "${BLUE}║   TCDE COMPLETE VALIDATION - MASTER TEST RUNNER       ║${NC}"
log_message "INFO" "${BLUE}║   Protocol: Zero Tolerance v3.0                       ║${NC}"
log_message "INFO" "${BLUE}║   Date: $(date +%Y-%m-%d\ %H:%M:%S)                           ║${NC}"
log_message "INFO" "${BLUE}╚════════════════════════════════════════════════════════╝${NC}"
log_message "INFO" ""

# Phase 1: Infrastructure Validation
log_message "INFO" "${BLUE}━━━ PHASE 1: INFRASTRUCTURE VALIDATION ━━━${NC}"
run_test_category "ultra_rigorous_validator" "tests/test_ultra_rigorous_validator" "0"
run_test_category "nanosecond_metrics" "tests/test_nanosecond_metrics" "0"
run_test_category "emergence_detector" "tests/test_emergence_detector" "0"

# Phase 2: Consciousness Capabilities (IDs 3, 4, 5)
log_message "INFO" ""
log_message "INFO" "${BLUE}━━━ PHASE 2: CONSCIOUSNESS CAPABILITIES ━━━${NC}"
run_test_category "consciousness_complete" "tests/test_consciousness_complete" "3,4,5"

# Phase 3: Temporality Capabilities (IDs 7, 8, 9)
log_message "INFO" ""
log_message "INFO" "${BLUE}━━━ PHASE 3: TEMPORALITY CAPABILITIES ━━━${NC}"
run_test_category "temporality_complete" "tests/test_temporality_complete" "7,8,9"
run_test_category "bitemporal_control" "tests/test_bitemporal_control" "7"

# Phase 4: Intentionality Capabilities (IDs 12, 13, 14, 15)
log_message "INFO" ""
log_message "INFO" "${BLUE}━━━ PHASE 4: INTENTIONALITY CAPABILITIES ━━━${NC}"
run_test_category "intentionality_complete" "tests/test_intentionality_complete" "12,13,14,15"

# Phase 5: Creativity Capabilities (IDs 17, 18)
log_message "INFO" ""
log_message "INFO" "${BLUE}━━━ PHASE 5: CREATIVITY CAPABILITIES ━━━${NC}"
run_test_category "creativity_complete" "tests/test_creativity_complete" "17,18"

# Phase 6: Autopoiesis Capabilities (ID 22)
log_message "INFO" ""
log_message "INFO" "${BLUE}━━━ PHASE 6: AUTOPOIESIS CAPABILITIES ━━━${NC}"
run_test_category "autopoiesis_complete" "tests/test_autopoiesis_complete" "22"
run_test_category "autopoiesis_benchmark" "tests/test_autopoiesis_benchmark" "22"

# Phase 7: Emergence Capabilities (IDs 25, 26, 27)
log_message "INFO" ""
log_message "INFO" "${BLUE}━━━ PHASE 7: EMERGENCE CAPABILITIES ━━━${NC}"
run_test_category "emergence_complete" "tests/test_emergence_complete" "25,26,27"

# Phase 8: Memory Capabilities (IDs 30, 31, 32, 33)
log_message "INFO" ""
log_message "INFO" "${BLUE}━━━ PHASE 8: MEMORY CAPABILITIES ━━━${NC}"
run_test_category "memory_complete" "tests/test_memory_complete" "30,31,32,33"
run_test_category "geometric_memory" "tests/test_geometric_memory" "30"
run_test_category "adaptive_memory" "tests/test_adaptive_memory" "31"

# Phase 9: Geometry Capabilities (IDs 34, 35, 36, 37)
log_message "INFO" ""
log_message "INFO" "${BLUE}━━━ PHASE 9: GEOMETRY CAPABILITIES ━━━${NC}"
run_test_category "geometry_complete" "tests/test_geometry_complete" "34,35,36,37"
run_test_category "geometric_curvature" "tests/test_geometric_curvature" "37"

# Phase 10: Coupling Capabilities (IDs 39, 40, 41, 42)
log_message "INFO" ""
log_message "INFO" "${BLUE}━━━ PHASE 10: COUPLING CAPABILITIES ━━━${NC}"
run_test_category "coupling_complete" "tests/test_coupling_complete" "39,40,41,42"

# Phase 11: Multimodality Capabilities (IDs 43, 44, 45)
log_message "INFO" ""
log_message "INFO" "${BLUE}━━━ PHASE 11: MULTIMODALITY CAPABILITIES ━━━${NC}"
run_test_category "multimodality_complete" "tests/test_multimodality_complete" "43,44,45"

# Close JSON array and add summary
echo "      {}" >> "${JSON_OUTPUT}"  # Dummy entry to avoid trailing comma
echo "    ]," >> "${JSON_OUTPUT}"
echo "    \"summary\": {" >> "${JSON_OUTPUT}"
echo "      \"total_tests\": ${TOTAL_TESTS}," >> "${JSON_OUTPUT}"
echo "      \"passed\": ${PASSED_TESTS}," >> "${JSON_OUTPUT}"
echo "      \"failed\": ${FAILED_TESTS}," >> "${JSON_OUTPUT}"
echo "      \"success_rate\": $(awk "BEGIN {printf \"%.2f\", (${PASSED_TESTS}/${TOTAL_TESTS})*100}")," >> "${JSON_OUTPUT}"
echo "      \"zero_tolerance_compliant\": $([ ${FAILED_TESTS} -eq 0 ] && echo "true" || echo "false")" >> "${JSON_OUTPUT}"
echo "    }" >> "${JSON_OUTPUT}"
echo "  }" >> "${JSON_OUTPUT}"
echo "}" >> "${JSON_OUTPUT}"

# Final Summary
log_message "INFO" ""
log_message "INFO" "${BLUE}╔════════════════════════════════════════════════════════╗${NC}"
log_message "INFO" "${BLUE}║              VALIDATION SUMMARY                        ║${NC}"
log_message "INFO" "${BLUE}╚════════════════════════════════════════════════════════╝${NC}"
log_message "INFO" ""
log_message "INFO" "Total Tests:    ${TOTAL_TESTS}"
log_message "INFO" "Passed:         ${GREEN}${PASSED_TESTS}${NC}"
log_message "INFO" "Failed:         ${RED}${FAILED_TESTS}${NC}"

SUCCESS_RATE=$(awk "BEGIN {printf \"%.2f\", (${PASSED_TESTS}/${TOTAL_TESTS})*100}")
log_message "INFO" "Success Rate:   ${SUCCESS_RATE}%"
log_message "INFO" ""

if [ ${FAILED_TESTS} -eq 0 ]; then
    log_message "INFO" "${GREEN}✓ ZERO TOLERANCE STANDARD: ACHIEVED${NC}"
    log_message "INFO" "${GREEN}✓ ALL TESTS PASSED${NC}"
    log_message "INFO" "${GREEN}✓ VALIDATION COMPLETE${NC}"
    EXIT_CODE=0
else
    log_message "ERROR" "${RED}✗ ZERO TOLERANCE STANDARD: NOT MET${NC}"
    log_message "ERROR" "${RED}✗ ${FAILED_TESTS} TEST(S) FAILED${NC}"
    log_message "ERROR" "${RED}✗ VALIDATION INCOMPLETE${NC}"
    EXIT_CODE=1
fi

log_message "INFO" ""
log_message "INFO" "Results saved to: ${JSON_OUTPUT}"
log_message "INFO" "Log file: ${LOG_FILE}"
log_message "INFO" ""

exit ${EXIT_CODE}
