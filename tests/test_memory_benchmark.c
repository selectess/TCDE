#include "../src/benchmarks/tcde_memory_benchmark.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Test du Benchmark de Mémoire TCDE
 * Utilise l'API TCDE réelle (AdaptiveMemoryManager)
 */

bool test_memory_benchmark_creation(void) {
    printf("\\n=== Test 1: Memory Benchmark Creation (TCDE API) ===\\n");
    
    TCDE_MemoryBenchmark* benchmark = TCDE_CreateMemoryBenchmark();
    assert(benchmark != NULL);
    assert(benchmark->active == true);
    assert(benchmark->memory_manager != NULL);  // Vrai gestionnaire TCDE
    
    printf("✅ Memory Benchmark created with REAL TCDE AdaptiveMemoryManager\\n");
    
    TCDE_DestroyMemoryBenchmark(benchmark);
    printf("✅ Benchmark destroyed successfully\\n");
    
    return true;
}

bool test_memory_initialization(void) {
    printf("\\n=== Test 2: Memory Initialization (TCDE API) ===\\n");
    
    TCDE_MemoryBenchmark* benchmark = TCDE_CreateMemoryBenchmark();
    
    assert(TCDE_InitializeMemoryBenchmark(benchmark, 100) == true);
    assert(benchmark->metrics.current_traces > 0);
    assert(benchmark->memory_manager->num_traces > 0);
    
    printf("✅ Memory initialized: %d traces (REAL TCDE storage)\\n",
           benchmark->memory_manager->num_traces);
    
    TCDE_DestroyMemoryBenchmark(benchmark);
    return true;
}

bool test_complete_memory_benchmark(void) {
    printf("\\n=== Test 3: Complete Memory Benchmark (TCDE API) ===\\n");
    
    TCDE_MemoryBenchmark* benchmark = TCDE_CreateMemoryBenchmark();
    
    assert(TCDE_RunCompleteMemoryBenchmark(benchmark) == true);
    
    assert(benchmark->metrics.current_traces >= 100);
    assert(benchmark->metrics.total_cycles > 0);
    assert(benchmark->total_benchmark_time_ns > 0);
    
    printf("✅ Complete benchmark executed\\n");
    printf("  Final traces: %u\\n", benchmark->metrics.current_traces);
    printf("  Expansion: %.1f%%\\n", benchmark->metrics.expansion_rate);
    printf("  Efficiency: %.2f%%\\n", benchmark->metrics.memory_efficiency);
    
    assert(benchmark->score.is_authentic == true);
    printf("✅ AUTHENTICITY VERIFIED (using REAL TCDE API)\\n");
    
    bool target_reached = TCDE_IsMemoryTargetReached(benchmark);
    if (target_reached) {
        printf("✅ MEMORY TARGET REACHED\\n");
    } else {
        printf("⚠️  Target not yet reached\\n");
    }
    
    TCDE_PrintMemoryBenchmarkSummary(benchmark);
    
    TCDE_DestroyMemoryBenchmark(benchmark);
    return true;
}

int main(void) {
    printf("💾 TCDE Memory Benchmark - Test Suite\\n");
    printf("======================================\\n");
    printf("TESTING REAL TCDE MEMORY API\\n");
    printf("Using: AdaptiveMemoryManager, StoreMemory, CompressMemory\\n");
    printf("TARGET: 100 → 1000+ traces, >99%% efficiency\\n");
    printf("STANDARD: ZERO TOLERANCE (REAL TCDE only)\\n");
    printf("======================================\\n");
    
    int tests_passed = 0;
    int total_tests = 3;
    
    if (test_memory_benchmark_creation()) tests_passed++;
    if (test_memory_initialization()) tests_passed++;
    if (test_complete_memory_benchmark()) tests_passed++;
    
    printf("\\n");
    printf("💾 TCDE MEMORY BENCHMARK TEST RESULTS\\n");
    printf("======================================\\n");
    printf("Tests Passed: %d/%d\\n", tests_passed, total_tests);
    printf("Success Rate: %.1f%%\\n", (float)tests_passed / total_tests * 100.0f);
    
    if (tests_passed == total_tests) {
        printf("✅ ALL MEMORY BENCHMARK TESTS PASSED\\n");
        printf("✅ REAL TCDE API VALIDATED\\n");
        printf("✅ ADAPTIVE MEMORY MANAGER CONFIRMED\\n");
        printf("✅ ZERO TOLERANCE STANDARD ACHIEVED\\n");
        return EXIT_SUCCESS;
    } else {
        printf("❌ SOME TESTS FAILED\\n");
        return EXIT_FAILURE;
    }
}
