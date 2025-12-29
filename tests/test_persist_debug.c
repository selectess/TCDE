#include "../src/core/tcde.h"
#include <stdio.h>

// Copie simplifiée pour debug
bool test_write_point(FILE* fp, const TCDE_Point* point) {
    if (!fp || !point || !point->coords) {
        printf("  write_point: invalid params (fp=%p, point=%p, coords=%p)\n",
               (void*)fp, (void*)point, point ? (void*)point->coords : NULL);
        return false;
    }
    
    printf("  write_point: dim=%d\n", point->dimension);
    
    if (fwrite(&point->dimension, sizeof(int), 1, fp) != 1) {
        printf("  write_point: failed to write dimension\n");
        return false;
    }
    
    size_t coord_size = point->dimension * sizeof(float);
    if (fwrite(point->coords, 1, coord_size, fp) != coord_size) {
        printf("  write_point: failed to write coords\n");
        return false;
    }
    
    printf("  write_point: success\n");
    return true;
}

int main(void) {
    printf("=== Debug Persistence ===\n\n");
    
    // Create field
    TCDE_Field* field = TCDE_CreateField(10, 2.0f);
    printf("Field: %p\n", (void*)field);
    printf("Centers: %d/%d\n", field->manifold_6d.num_centers, field->manifold_6d.capacity);
    
    // Add center
    float coords[6] = {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f};
    TCDE_Point p = TCDE_CreatePoint(6, coords);
    printf("\nPoint created: dim=%d, coords=%p\n", p.dimension, (void*)p.coords);
    
    bool added = TCDE_AddCenter6D(field, &p, 1.0f, 0.1f);
    printf("AddCenter result: %d\n", added);
    printf("Centers after add: %d\n", field->manifold_6d.num_centers);
    
    if (field->manifold_6d.num_centers > 0) {
        TCDE_Center* center = &field->manifold_6d.centers[0];
        printf("\nCenter 0:\n");
        printf("  point.dimension: %d\n", center->point.dimension);
        printf("  point.coords: %p\n", (void*)center->point.coords);
        printf("  epsilon: %.3f\n", center->epsilon);
        printf("  metric.g: %p\n", (void*)center->metric.g);
        printf("  metric.dimension: %d\n", center->metric.dimension);
        
        // Try to write just the point
        FILE* fp = fopen("test_debug.bin", "wb");
        if (fp) {
            printf("\nTrying to write point...\n");
            bool result = test_write_point(fp, &center->point);
            printf("Result: %d\n", result);
            fclose(fp);
        }
    }
    
    TCDE_FreePoint(&p);
    TCDE_DestroyField(field);
    return 0;
}
