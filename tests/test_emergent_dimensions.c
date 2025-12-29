/**
 * @file test_emergent_dimensions.c
 * @brief Test for Emergent Dimensions Module - ASI Phase 1
 *
 * Tests dynamic dimension emergence from field complexity.
 */

#include "tcde_emergent_dimensions.h"
#include "tcde_core.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("=== TCDE Emergent Dimensions Test ===\n\n");

    // Create a simple field for testing
    TCDE_Field* field = TCDE_CreateField(100, 2.5f);
    if (!field) {
        fprintf(stderr, "Failed to create field\n");
        return 1;
    }

    // Add some centers to create complexity
    for (int i = 0; i < 50; i++) {
        float coords[6] = {
            (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX,
            1.0f, 0.0f, 0.4f
        };
        TCDE_Point p = TCDE_CreatePoint(6, coords);
        TCDE_AddCenter6D(field, &p, 1.0f + 0.0f*I, 0.1f);
        TCDE_FreePoint(&p);
    }

    printf("✓ Created field with %d centers\n", field->manifold_6d.num_centers);

    // Create emergent field
    TCDE_EmergentField* emergent_field = TCDE_CreateEmergentField(field, 12); // Max 12 dimensions
    if (!emergent_field) {
        fprintf(stderr, "Failed to create emergent field\n");
        TCDE_DestroyField(field);
        return 1;
    }

    printf("✓ Emergent field created with max %d dimensions\n", 12);

    // Test dimension manager
    TCDE_DimensionManager* dim_manager = emergent_field->dim_manager;
    printf("Base dimensions: %d\n", dim_manager->base_dimensions);
    printf("Current total dimensions: %d\n", dim_manager->total_dimensions);

    // Check emergence criteria
    bool can_emerge = TCDE_CheckEmergenceCriteria(field, dim_manager);
    printf("Emergence criteria met: %s\n", can_emerge ? "YES" : "NO");

    if (can_emerge) {
        // Try to emerge a dimension
        bool emerged = TCDE_EmergeDimension(dim_manager, field,
                                          "test_dimension",
                                          "Test emergent dimension from field complexity");
        printf("Dimension emergence: %s\n", emerged ? "SUCCESS" : "FAILED");

        if (emerged) {
            printf("New total dimensions: %d\n", dim_manager->total_dimensions);
        }
    }

    // Test evaluation
    TCDE_Point test_point = TCDE_CreatePoint(6, (float[]){0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.4f});
    TCDE_Complex value = TCDE_EvaluateEmergent(emergent_field, &test_point);
    printf("Field evaluation at test point: %.3f + %.3fi\n", crealf(value), cimagf(value));
    TCDE_FreePoint(&test_point);

    // Print status
    TCDE_PrintEmergentStatus(emergent_field);

    // Get final dimension count
    int final_dims = TCDE_GetEmergentDimensions(emergent_field);
    printf("Final dimension count: %d\n", final_dims);

    // Cleanup
    TCDE_DestroyEmergentField(emergent_field);
    TCDE_DestroyField(field);

    printf("\n✓ Emergent Dimensions Test Complete\n");
    return 0;
}
