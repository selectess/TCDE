/**
 * @file tcde_monolithic.h
 * @brief TCDE Monolithic Header for Testing
 * 
 * Simplified header for the monolithic TCDE implementation
 * Contains all necessary declarations without modular dependencies
 */

#ifndef TCDE_MONOLITHIC_H
#define TCDE_MONOLITHIC_H

#include <stdbool.h>
#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Type Definitions
// ============================================================================

typedef float complex TCDE_Complex;

// Topology modes
typedef enum {
    TCDE_TOPOLOGY_2D,
    TCDE_TOPOLOGY_6D,
    TCDE_TOPOLOGY_UNIFIED
} TCDE_TopologyMode;

// RBF kernel types
typedef enum {
    TCDE_RBF_GAUSSIAN,
    TCDE_RBF_MULTIQUADRIC,
    TCDE_RBF_INVERSE_MULTIQUADRIC,
    TCDE_RBF_THIN_PLATE_SPLINE
} TCDE_RBFType;

// Point in N-dimensional space
typedef struct {
    int dimension;
    float* coords;
} TCDE_Point;

// Metric tensor
typedef struct {
    int dimension;
    float** g;          // Metric tensor
    float** g_inv;      // Inverse metric
    float det;          // Determinant
    bool is_valid;
} TCDE_Metric;

// RBF center
typedef struct {
    TCDE_Point point;
    TCDE_Complex coeff;
    float epsilon;
    TCDE_Metric metric;
} TCDE_Center;

// Manifold structure
typedef struct {
    TCDE_Center* centers;
    int num_centers;
    int capacity;
    TCDE_Metric metric;
} TCDE_Manifold;

// 2D Projection configuration
typedef struct {
    int axis1;
    int axis2;
    float slice_coords[4];
    bool auto_update;
} TCDE_Projection;

// Adaptive parameters
typedef struct {
    float error_threshold;
    float min_epsilon;
    float max_epsilon;
    int min_centers;
    int max_centers;
} TCDE_AdaptiveParams;

// Main field structure
typedef struct {
    TCDE_TopologyMode topology;
    
    // 6D manifold
    TCDE_Manifold manifold_6d;
    
    // 2D projection
    TCDE_Manifold slice_2d;
    TCDE_Projection projection;
    
    // Synchronization
    bool sync_enabled;
    float sync_strength;
    
    // RBF configuration
    TCDE_RBFType rbf_type;
    
    // Field properties
    float time;
    float energy;
    bool energy_valid;
    float fractal_dimension;
    float temporal_dimension;
    
    // Adaptive mesh
    TCDE_AdaptiveParams adaptive;
} TCDE_Field;

// Evolution parameters
typedef struct {
    float dt;       // Time step
    float D;        // Diffusion coefficient
    float alpha;    // Nonlinearity strength
    float beta;     // Torsion strength
    float gamma;    // Coupling strength
    float sigma;    // Coupling scale
} TCDE_Parameters;

// Validation results
typedef struct {
    float reflexive_coherence;
    float predictive_strength;
    int intuitive_connections;
    float anticipation_score;
    float cross_modal_coherence;
    bool validated;
} TCDE_ValidationResults;

// Modality structure
typedef struct {
    char name[32];
    int id;
    int embedding_dim;
    float* embedding;
} TCDE_Modality;

// Modality relations
typedef struct {
    int num_modalities;
    TCDE_Modality** modalities;
    float** similarity_matrix;
} TCDE_ModalityRelations;

// ============================================================================
// Core Functions
// ============================================================================

// Point operations
TCDE_Point TCDE_CreatePoint(int dimension, const float* coords);
void TCDE_FreePoint(TCDE_Point* point);
bool TCDE_ValidatePoint(const TCDE_Point* point);

// Field management
TCDE_Field* TCDE_CreateField(int capacity, float fractal_dim);
void TCDE_DestroyField(TCDE_Field* field);

// Field operations (6D)
bool TCDE_AddCenter6D(TCDE_Field* field, const TCDE_Point* point, 
                      TCDE_Complex coeff, float epsilon);
bool TCDE_RemoveCenter6D(TCDE_Field* field, int index);
TCDE_Complex TCDE_Evaluate6D(const TCDE_Field* field, const TCDE_Point* point);
void TCDE_Gradient6D(const TCDE_Field* field, const TCDE_Point* point, 
                     TCDE_Complex* gradient);
TCDE_Complex TCDE_Laplacian6D(const TCDE_Field* field, const TCDE_Point* point);

// Field operations (2D)
TCDE_Complex TCDE_Evaluate2D(const TCDE_Field* field, const TCDE_Point* point);
void TCDE_Gradient2D(const TCDE_Field* field, const TCDE_Point* point, 
                     TCDE_Complex* gradient);
TCDE_Complex TCDE_Laplacian2D(const TCDE_Field* field, const TCDE_Point* point);

// Projection management
void TCDE_ConfigureProjection(TCDE_Field* field, int axis1, int axis2, 
                              const float* slice_coords);
void TCDE_UpdateProjection(TCDE_Field* field);
TCDE_Point TCDE_Project6Dto2D(const TCDE_Field* field, const TCDE_Point* point_6d);
TCDE_Point TCDE_Lift2Dto6D(const TCDE_Field* field, const TCDE_Point* point_2d);
void TCDE_SetSynchronization(TCDE_Field* field, bool enabled, float strength);

// Geometry
float TCDE_GeodesicDistance(const TCDE_Point* p1, const TCDE_Point* p2, 
                            const TCDE_Metric* metric);

// Topological operations
TCDE_Complex TCDE_TopologicalTorsion(const TCDE_Field* field, 
                                     const TCDE_Point* point);
TCDE_Complex TCDE_ContextualCoupling(const TCDE_Field* field, 
                                     const TCDE_Point* point,
                                     float sigma, int num_samples);
float TCDE_TemporalDimension(const TCDE_Field* field);

// Energy
float TCDE_ComputeEnergy(TCDE_Field* field);

// Evolution
void TCDE_ConfigureParameters(TCDE_Parameters* params, const TCDE_Field* field);
void TCDE_EvolveStep(TCDE_Field* field, const TCDE_Parameters* params);

// Initialization
void TCDE_InitializeFractal(TCDE_Field* field, const TCDE_Point* center,
                            float width, float amplitude, int num_centers);

// Adaptive mesh
int TCDE_AdaptiveMeshRefinement(TCDE_Field* field);
void TCDE_AdaptMetric(TCDE_Field* field, float kappa);

// Validation
float TCDE_ValidateReflexivity(const TCDE_Field* field);
float TCDE_ValidatePrediction(TCDE_Field* field);
int TCDE_ValidateIntuition(TCDE_Field* field, TCDE_ModalityRelations* relations);
float TCDE_VerifyFractalDimension(const TCDE_Field* field);
TCDE_ValidationResults TCDE_Validate(TCDE_Field* field, 
                                     TCDE_ModalityRelations* relations);

// Multimodal
TCDE_ModalityRelations* TCDE_CreateModalityRelations(const char** names,
                                                      int num_modalities,
                                                      int embedding_dim);
void TCDE_DestroyModalityRelations(TCDE_ModalityRelations* relations);
void TCDE_ProcessInput(TCDE_Field* field, TCDE_ModalityRelations* relations,
                      const void* input_data, int modality_id, float intensity);
void TCDE_TransformModality(TCDE_Field* field, TCDE_ModalityRelations* relations,
                           int source_modality, int target_modality);

// Utility
void TCDE_PrintField(const TCDE_Field* field);
bool TCDE_SaveField(const TCDE_Field* field, const char* filename);
TCDE_Field* TCDE_LoadField(const char* filename);
const char* TCDE_GetVersion(void);
void TCDE_PrintVersion(void);

#ifdef __cplusplus
}
#endif

#endif // TCDE_MONOLITHIC_H
