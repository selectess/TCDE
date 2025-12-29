#include "core/tcde_core.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * TCDE Demo - Simple demonstration of TCDE functionality
 */
int main(void) {
    printf("🚀 TCDE Demo Starting...\n");
    printf("========================\n");
    
    // Create a simple TCDE field
    TCDE_Field* field = TCDE_CreateField(10, 2.5f);
    if (!field) {
        printf("❌ Failed to create TCDE field\n");
        return EXIT_FAILURE;
    }
    
    printf("✅ TCDE Field created successfully\n");
    printf("   Capacity: %d centers\n", field->manifold_6d.capacity);
    printf("   Fractal dimension: %.2f\n", field->fractal_dimension);
    
    // Clean up
    TCDE_DestroyField(field);
    
    printf("✅ Demo completed successfully\n");
    return EXIT_SUCCESS;
}