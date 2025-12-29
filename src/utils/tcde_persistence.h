/**
 * @file tcde_persistence.h
 * @brief TCDE Persistence Module - State Serialization and Deserialization
 * 
 * Provides functions for saving and loading TCDE field state to/from disk,
 * enabling continuous evolution across sessions (End-to-Infinite paradigm).
 * 
 * @version 1.0
 * @date October 22, 2025
 */

#ifndef TCDE_PERSISTENCE_H
#define TCDE_PERSISTENCE_H

#include "../core/tcde_core.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// CONSTANTS
// ============================================================================

#define TCDE_STATE_MAGIC 0x54434445  // "TCDE" in hex
#define TCDE_STATE_VERSION 1

// ============================================================================
// STATE SERIALIZATION
// ============================================================================

/**
 * @brief Save complete TCDE field state to binary file
 * 
 * Saves all field data including:
 * - 6D manifold centers (positions, coefficients, epsilons)
 * - 2D slice centers and projection parameters
 * - Metrics (6D and 2D)
 * - Metadata (fractal dimension, time, etc.)
 * - Checksum for integrity verification
 * 
 * @param field Field to save
 * @param filename Output file path
 * @return true if successful, false on error
 */
bool TCDE_SaveState(const TCDE_Field* field, const char* filename);

/**
 * @brief Load TCDE field state from binary file
 * 
 * Restores complete field state from file, including all centers,
 * metrics, and metadata. Verifies checksum for integrity.
 * 
 * @param filename Input file path
 * @return Restored field, or NULL on error
 */
TCDE_Field* TCDE_LoadState(const char* filename);

/**
 * @brief Verify integrity of saved state file
 * 
 * Checks magic number, version compatibility, and checksum
 * without fully loading the state.
 * 
 * @param filename File to verify
 * @return true if file is valid and compatible
 */
bool TCDE_VerifyStateFile(const char* filename);

#ifdef __cplusplus
}
#endif

#endif // TCDE_PERSISTENCE_H
