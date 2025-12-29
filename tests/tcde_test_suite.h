/**
 * @file tcde_test_suite.h
 * @brief TCDE Complete Test Suite Header
 * 
 * @version 1.0
 * @date October 24, 2025
 */

#ifndef TCDE_TEST_SUITE_H
#define TCDE_TEST_SUITE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    float pass_rate;
    char** failed_test_names;
} TCDE_TestResults;

// Task 17.3: Unit Tests
TCDE_TestResults TCDE_RunAllUnitTests(void);

// Task 17.4: Integration Tests
TCDE_TestResults TCDE_RunIntegrationTests(void);

// Task 17.5: Cognitive Validation
TCDE_TestResults TCDE_RunCognitiveTestSuite(void);

// Complete Suite
int TCDE_RunCompleteTestSuite(void);

#ifdef __cplusplus
}
#endif

#endif // TCDE_TEST_SUITE_H
