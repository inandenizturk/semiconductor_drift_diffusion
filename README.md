# ⚡ Semiconductor Carrier Dynamics: 2D Drift-Diffusion Simulation

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Domain](https://img.shields.io/badge/Domain-Computational%20Semiconductor%20Physics-darkgreen.svg)](#)
[![Method](https://img.shields.io/badge/Method-Monte%20Carlo%20%7C%20Drift--Diffusion-orange.svg)](#)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A high-performance C++ numerical framework modeling nonequilibrium charge carrier transport in a crystalline semiconductor lattice under an applied electrostatic field. The engine combines discrete stochastic Monte Carlo scattering dynamics with continuous spatial carrier density packet evolution.

![Semiconductor Carrier Transport Demo](demo.gif)

---

## 🔬 Theoretical Formulation

Carrier transport in semiclassical semiconductor device physics is governed by the Boltzmann Transport Equation (BTE). In the diffusive regime under low-to-moderate electric fields, macroscopic electron flow is parameterized by the **Drift-Diffusion Continuity Equation**:

> **∂n(r, t) / ∂t = ∇ · [ D ∇n(r, t) + μ E n(r, t) ] - R(n) + G**

Where:
* **n(r, t)**: Spatial carrier concentration
* **μ**: Carrier mobility (vd = μ · E)
* **E**: External electric field along the +x axis
* **D**: Thermal diffusion coefficient
* **G, R**: Generation and recombination rates

### Stochastic Lattice Discretization
On the discrete 2D spatial lattice, external electric field coupling is resolved via anisotropic transition probability kernels:

* **Forward Drift Bias (+x):** `P(+x) = 0.37` (Biased transport along electric field lines)
* **Thermal Scattering:** `P(-x) = P(+y) = P(-y) = 0.21` (Isotropic random thermal walk)
* **Einstein Relation:** Mobility and thermal diffusion are intrinsically linked in quasi-equilibrium:
  `D / μ = (kB · T) / q`
* **Wave Packet Dispersal:** The spatial ensemble variance evolves diffusively:
  `σ(t) = sqrt(σ₀² + 2Dt)`
  exhibiting classical Gaussian broadening centered around the drifting coordinate `⟨x(t)⟩ ≈ vd · t`.

---

## 🏗️ Architecture & Pipeline

The pipeline runs entirely headless in native C++ without external numerical dependencies:

```text
+-------------------------------------------------------------+
|               Monte Carlo Engine (C++17)                    |
|    - 2D Discrete Lattice Random Walk                        |
|    - Microscopic Momentum Relaxation Tracking               |
+------------------------------+------------------------------+
                               |
                               v (Generates trajectory.dat)
+-------------------------------------------------------------+
|            Auxiliary Dynamic Spatial Density Resolver       |
|    - Analytical Gaussian Packet Kernel Evaluation           |
|    - Spatiotemporal Profile Discretization                  |
+------------------------------+------------------------------+
                               |
                               v
+-------------------------------------------------------------+
|              Vectorized Rendering Pipeline                  |
|    - Gnuplot Multiplot (pm3d Map + Lattice Trajectory)      |
|    - FFmpeg High-Efficiency Video Encoding (libx264, yuv420)|
+-------------------------------------------------------------+
