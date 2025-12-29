#include "../src/core/tcde.h"
#include "tcde_persistence.h"
#include <stdio.h>

int main(void) {
    printf("=== Test Persistence Simple ===\n\n");
    
    // Create minimal field
    TCDE_Field* field = TCDE_CreateField(10, 2.0f);
    if (!field) {
        printf("❌ Failed to create field\n");
        return 1;
    }
    
    printf("Field created: %d centers\n", field->manifold_6d.num_centers);
    
    // Add one center
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    TCDE_AddCenter6D(field, &p, 1.0f + 0.5f * I, 0.1f);
    TCDE_FreePoint(&p);
    
    printf("Added 1 center, total: %d\n", field->manifold_6d.num_centers);
    
    // Try to save
    const char* filename = "test_simple.tcde";
    printf("\nSaving to %s...\n", filename);
    
    bool result = TCDE_SaveState(field, filename);
    if (result) {
        printf("✅ Save successful\n");
        
        // Try to load
        printf("\nLoading from %s...\n", filename);
        TCDE_Field* loaded = TCDE_LoadState(filename);
        
        if (loaded) {
            printf("✅ Load successful\n");
            printf("Loaded field: %d centers\n", loaded->manifold_6d.num_centers);
            
            if (loaded->manifold_6d.num_centers == 1) {
                printf("✅ Center count matches\n");
            } else {
                printf("❌ Center count mismatch\n");
            }
            
            TCDE_DestroyField(loaded);
        } else {
            printf("❌ Load failed\n");
        }
    } else {
        printf("❌ Save failed\n");
    }
    
    TCDE_DestroyField(field);
    return 0;
}
