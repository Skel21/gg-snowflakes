# Gravner-Griffeath Snowflake Simulation

Welcome to an interactive simulation of one of the first realistic snowflake growth models—the Gravner-Griffeath (GG) model, introduced in their 2008 paper *Modeling snow crystal growth II: A mesoscopic lattice map with plausible dynamics* [1].

## What is it?

The GG model is a semi-empirical cellular automaton that uses local rules to simulate realistic growth of snowflakes in 2D. Unlike purely physical simulations, it captures the essential dynamics of crystal growth through carefully designed discrete rules on a hexagonal lattice, balancing computational efficiency with morphological accuracy.

## How do snowflakes work?

Snowflakes begin as a tiny hexagonal ice crystal—a seed—that grows as water vapor in the air deposits onto its surface. The final shape depends critically on temperature and humidity conditions. We aim to capture several characteristic processes:

- **Vapor deposition**: Water vapor deposits directly onto the ice surface, causing crystallization
- **Surface diffusion**: Water molecules landing on the surface can migrate along the crystal before settling, creating smoother growth patterns and filling concave regions
- **Phase transitions**: Parts of the snowflake can melt or sublimate back into vapor
- **Humidity effects**: Higher vapor density $\rho$ leads to faster growth
- **Geometric effects**: The more frozen neighbors a cell has, the easier water molecules attach there, creating the characteristic dendritic (branching) structures

The interplay of these processes produces the remarkable diversity of snowflake morphologies observed in nature.

## How the model works

### State Variables

At each point $\mathbf{x}$ of the hexagonal lattice, we track four variables at time $t$:

- $a_t(\mathbf{x}) \in \{0,1\}$: **Crystal indicator** — whether the cell is frozen (1) or not (0)
- $b_t(\mathbf{x}) \geq 0$: **Boundary mass** — quasi-liquid water on the crystal surface
- $c_t(\mathbf{x}) \geq 0$: **Crystal mass** — the amount of ice in the cell
- $d_t(\mathbf{x}) \geq 0$: **Diffusive mass** — water vapor density in the cell

### Parameters

The model has 7 parameters controlling different physical processes:

- $\rho$: Background vapor density (humidity level)
- $\beta$: Attachment threshold for tips and edges (higher $\beta$ makes attachment harder at tips, promoting branching)
- $\kappa$: Proportion of vapor that crystallizes directly (vs. becoming boundary mass)
- $\mu$: Proportion of boundary mass that evaporates back to vapor per time step
- $\gamma$: Proportion of crystal mass that melts back to boundary mass per time step
- $\theta$: Diffusive mass threshold for knife-edge instability
- $\alpha$: Reduced attachment threshold when diffusive mass is below $\theta$

### Update Rules

At each discrete time step $t \to t+1$, we update all cells according to:

#### 1. Diffusion
The vapor spreads according to a discrete diffusion equation:
$$d_{t+1}(\mathbf{x}) = \frac{1}{7}\left(d_t(\mathbf{x}) + \sum_{\mathbf{y} \in N(\mathbf{x})} d_t(\mathbf{y})\right)$$
where $N(\mathbf{x})$ denotes the 6 nearest neighbors on the hexagonal lattice.

#### 2. Freezing
At each boundary site, a proportion $\kappa$ of the diffusive mass crystallizes directly, while the remainder $(1-\kappa)$ becomes boundary mass:
$$c_{t+1}(\mathbf{x}) \leftarrow c_t(\mathbf{x}) + \kappa \cdot d_t(\mathbf{x})$$
$$b_{t+1}(\mathbf{x}) \leftarrow b_t(\mathbf{x}) + (1-\kappa) \cdot d_t(\mathbf{x})$$
$$d_{t+1}(\mathbf{x}) \leftarrow 0$$

#### 3. Attachment (Freezing)
When boundary mass exceeds certain thresholds, the cell joins the crystal. The threshold depends on the local geometry:

**For cells with 1 or 2 frozen neighbors (tips or edges):**
$$\text{If } b_t(\mathbf{x}) \geq \beta \text{, then } a_{t+1}(\mathbf{x}) = 1$$

**For cells with 3 frozen neighbors (shallow concavities):**
$$\text{If } b_t(\mathbf{x}) \geq 1 \text{ or } \left(b_t(\mathbf{x}) \geq \alpha \text{ and } \sum_{\mathbf{y} \in N(\mathbf{x})} d_t(\mathbf{y}) < \theta\right) \text{, then } a_{t+1}(\mathbf{x}) = 1$$

**For cells with 4+ frozen neighbors (deep concavities):**
$$\text{Attachment occurs automatically: } a_{t+1}(\mathbf{x}) = 1$$

When a cell attaches, all its boundary mass converts to crystal mass:
$$c_{t+1}(\mathbf{x}) \leftarrow c_t(\mathbf{x}) + b_t(\mathbf{x}), \quad b_{t+1}(\mathbf{x}) \leftarrow 0$$

The knife-edge instability (second condition for 3 neighbors) allows thin edges to grow more easily when vapor is low, creating the characteristic sharp tips and fine structures seen in natural snowflakes.

#### 4. Melting and Evaporation
Reverse processes maintain dynamic equilibrium. A proportion $\mu$ of boundary mass evaporates, and a proportion $\gamma$ of crystal mass melts:
$$b_{t+1}(\mathbf{x}) \leftarrow (1-\mu) \cdot b_t(\mathbf{x}) + \gamma \cdot c_t(\mathbf{x})$$
$$c_{t+1}(\mathbf{x}) \leftarrow (1-\gamma) \cdot c_t(\mathbf{x})$$
$$d_{t+1}(\mathbf{x}) \leftarrow d_t(\mathbf{x}) + \mu \cdot b_t(\mathbf{x})$$

These terms allow for realistic sublimation and melting at higher temperatures.

## Explore the patterns

The simulation lets you adjust all parameters in real time. Try the presets to see classic snowflake morphologies, or experiment with your own combinations. Notice how small parameter changes can produce dramatically different structures—just like in nature!

---

### References

[1] Gravner, J., & Griffeath, D. (2008). Modeling snow crystal growth II: A mesoscopic lattice map with plausible dynamics. *Physica D: Nonlinear Phenomena*, 237(3), 385-404. [https://doi.org/10.1016/j.physd.2007.09.008](https://doi.org/10.1016/j.physd.2007.09.008)