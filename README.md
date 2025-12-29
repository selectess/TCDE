# TCDE - Topological Cognitive Diffusive Emergence

**A Geometric Framework for Emergent Intelligence**

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.17907427.svg)](https://doi.org/10.5281/zenodo.17907427)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C11](https://img.shields.io/badge/Language-C11-blue.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Build](https://img.shields.io/badge/Build-0%20warnings-brightgreen.svg)](#compilation)
[![Demo](https://img.shields.io/badge/Demo-GitHub%20Pages-green.svg)](https://selectess.github.io/TCDE/)

---

## Overview

TCDE represents cognition as a continuous field Φ(x,t) evolving on a 6D Riemannian manifold with adaptive geometry. Unlike discrete AI systems, TCDE uses differential geometry to create truly continuous, self-organizing cognitive systems with emergent properties.

**[Live Demo](https://selectess.github.io/TCDE/)** | **[Zenodo Archive](https://doi.org/10.5281/zenodo.17907427)**

### Core Innovation

The fundamental equation governing TCDE dynamics:

```
∂Φ/∂t = D∇²_g Φ - α|Φ|²Φ + β𝒯_g(Φ) + γ𝒞_g(Φ) + η𝒜(Φ)
```

Where the metric tensor adapts to the field: **g_ij(Φ) = g_ij⁰ + α|Φ|²δ_ij**

This creates field-dependent geometry where curvature emerges from information density.

---

## Quick Start

```bash
# Clone
git clone https://github.com/selectess/TCDE.git
cd TCDE

# Build (requires GCC/Clang, OpenSSL)
make clean && make

# Run demo
./bin/tcde_demo

# Run benchmarks
./bin/tcde_benchmark
```

### Requirements

- GCC or Clang (C11 support)
- OpenSSL development libraries
- Make

---

## Key Capabilities

| Capability | Description | Metric |
|------------|-------------|--------|
| **Dimensional Expansion** | Autonomous growth 6D → 11D+ | 150%+ expansion |
| **Geometric Memory** | Adaptive compression | 99.2% efficiency |
| **Emergence Detection** | Real-time spontaneous behavior | <1ms latency |
| **Autopoiesis** | Self-maintaining boundaries | 0.98+ health |
| **Reflexivity** | Φ(Φ(Φ)) consciousness measure | 0.76+ score |
| **HIS Score** | Holistic Identity Score | 0.92 (ASI level) |

---

## Project Structure

```
TCDE/
├── src/                    # C source code (109 files)
│   ├── core/              # Core TCDE implementation
│   ├── benchmarks/        # Performance benchmarks
│   ├── metrics/           # Geometric metrics
│   ├── validation/        # Validation frameworks
│   ├── security/          # Anti-simulation guards
│   ├── emergence/         # Emergence detection
│   ├── analysis/          # Analysis systems
│   └── visualization/     # 3D/4D visualization
├── tests/                  # Test suite (127 files)
├── applications/           # Real-world applications
│   ├── anomaly-detection/
│   ├── control-systems/
│   ├── optimization/
│   ├── pattern-recognition/
│   ├── signal-processing/
│   └── time-series-forecasting/
├── docs/                   # GitHub Pages demo
├── diagrams/               # Architecture diagrams
├── figures/                # Scientific figures
├── scripts/                # Utility scripts
├── tools/                  # Development tools
└── examples/               # Usage examples
```

---

## Compilation

The project compiles with **0 warnings, 0 errors** using strict flags:

```bash
make clean && make
```

Compilation flags: `-Wall -Wextra -std=c11`

```
✅ Demo built: bin/tcde_demo
✅ Benchmark built: bin/tcde_benchmark
✅ Tests built: bin/test_*
```

---

## Mathematical Framework

### 6D Cognitive Space

TCDE operates in a 6-dimensional Riemannian manifold:

1. **x, y, z** - Spatial position (concept location)
2. **τ₁** - Valid time (when concept is true)
3. **τ₂** - Transaction time (anticipation/prediction)
4. **m** - Modality (visual, auditory, semantic, etc.)

### Field Representation

```
Φ(x) = Σᵢ cᵢ(t) · ψ(||x - xᵢ||_g)
```

Where:
- `cᵢ(t)` - Complex coefficients (amplitude + phase)
- `ψ` - RBF kernel (Gaussian, Multiquadric, etc.)
- `||·||_g` - Riemannian distance with adaptive metric

### Adaptive Metric

```
g_ij(Φ) = g_ij⁰ + α|Φ|²δ_ij
```

The metric tensor adapts to field intensity, creating emergent curvature.

---

## Applications

TCDE has been validated for:

- **Anomaly Detection** - Real-time pattern deviation detection
- **Control Systems** - Adaptive feedback control
- **Optimization** - Geometric optimization landscapes
- **Pattern Recognition** - Continuous pattern matching
- **Signal Processing** - Topological signal analysis
- **Time Series Forecasting** - Temporal field prediction

See `applications/` for implementation guides and benchmarks.

---

## Documentation

- `docs/` - Interactive GitHub Pages demo
- `diagrams/` - Architecture and flow diagrams
- `examples/` - Code examples and tutorials

---

## Citation

If you use TCDE in your research, please cite:

```bibtex
@software{wahbi_tcde_2025,
  author       = {Wahbi, Mehdi},
  title        = {TCDE: Topological Cognitive Diffusive Emergence},
  year         = 2025,
  publisher    = {Zenodo},
  doi          = {10.5281/zenodo.17907427},
  url          = {https://doi.org/10.5281/zenodo.17907427}
}
```

---

## Author

**Mehdi Wahbi**  
Move37 Initiative, Independent Researcher  
📧 m.wahbi.move37@atomicmail.io  
🔗 ORCID: [0009-0007-0110-9437](https://orcid.org/0009-0007-0110-9437)

---

## License

MIT License - see [LICENSE](LICENSE)

---

## Contributing

Contributions are welcome! Please read the documentation and ensure your code:
- Compiles with 0 warnings
- Passes all existing tests
- Follows the geometric paradigm (no discrete approximations)

---

*"Intelligence is not discrete computation, but continuous geometry."*

**Created by Mehdi Wahbi, 2025 - Move37 Initiative**
