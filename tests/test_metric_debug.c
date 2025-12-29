#include "../src/core/tcde.h"
#include <stdio.h>

int main(void) {
    printf("=== Debug Metric ===\n\n");
    
    // Create field and add center
    TCDE_Field* field = TCDE_CreateField(10, 2.0f);
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
    
    TCDE_Center* center = &field->manifold_6d.centers[0];
    TCDE_Metric* metric = &center->metric;
    
    printf("Metric info:\n");
    printf("  dimension: %d\n", metric->dimension);
    printf("  is_valid: %d\n", metric->is_valid);
    printf("  det: %.6f\n", metric->det);
    printf("  g: %p\n", (void*)metric->g);
    printf("  g_inv: %p\n", (void*)metric->g_inv);
    
    if (metric->g) {
        printf("\nMetric tensor g:\n");
        for (int i = 0; i < 2 && i < metric->dimension; i++) {
            printf("  g[%d]: %p\n", i, (void*)metric->g[i]);
            if (metric->g[i]) {
                printf("    values: %.3f %.3f ...\n", metric->g[i][0], metric->g[i][1]);
            }
        }
    }
    
    // Try to write metric
    FILE* fp = fopen("test_metric.bin", "wb");
    if (fp) {
        printf("\nWriting metric...\n");
        
        // Write dimension
        if (fwrite(&metric->dimension, sizeof(int), 1, fp) != 1) {
            printf("❌ Failed to write dimension\n");
        } else {
            printf("✅ Wrote dimension\n");
        }
        
        // Write validity
        if (fwrite(&metric->is_valid, sizeof(bool), 1, fp) != 1) {
            printf("❌ Failed to write is_valid\n");
        } else {
            printf("✅ Wrote is_valid\n");
        }
        
        // Write det
        if (fwrite(&metric->det, sizeof(float), 1, fp) != 1) {
            printf("❌ Failed to write det\n");
        } else {
            printf("✅ Wrote det\n");
        }
        
        // Try to write tensors
        if (metric->g && metric->g_inv) {
            printf("\nWriting tensors...\n");
            int dim = metric->dimension;
            bool success = true;
            
            for (int i = 0; i < dim && success; i++) {
                if (!metric->g[i]) {
                    printf("❌ g[%d] is NULL\n", i);
                    success = false;
                    break;
                }
                if (fwrite(metric->g[i], sizeof(float), dim, fp) != (size_t)dim) {
                    printf("❌ Failed to write g[%d]\n", i);
                    success = false;
                    break;
                }
            }
            
            if (success) {
                printf("✅ Wrote g tensor\n");
                
                for (int i = 0; i < dim && success; i++) {
                    if (!metric->g_inv[i]) {
                        printf("❌ g_inv[%d] is NULL\n", i);
                        success = false;
                        break;
                    }
                    if (fwrite(metric->g_inv[i], sizeof(float), dim, fp) != (size_t)dim) {
                        printf("❌ Failed to write g_inv[%d]\n", i);
                        success = false;
                        break;
                    }
                }
                
                if (success) {
                    printf("✅ Wrote g_inv tensor\n");
                }
            }
        }
        
        fclose(fp);
    }
    
    TCDE_FreePoint(&p);
    TCDE_DestroyField(field);
    return 0;
}
