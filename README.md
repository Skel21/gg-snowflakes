# Gravner-Griffeath Snowflake Simulator

A WebAssembly implementation of the Gravner-Griffeath cellular automaton model for snowflake crystal growth. The simulation core is written in easily readable C++ and compiled to WebAssembly using Emscripten, to run in the browser with good performance.

**Live demo:** [https://skel21.github.io/gg-snowflakes/](https://skel21.github.io/gg-snowflakes/)

## Project structure
```
.
├── cpp/
│   ├── main.cpp              # Main entry point, Emscripten bindings
│   ├── src/
│   │   ├── gg_model.h        # Model implementation
│   │   └── presets.h         # Parameter presets
│   └── vis/
│       ├── colormap.h        # Color mapping
│       └── vis.h             # Visualization
└── web/
    ├── DESCRIPTION.md        # Model description
    ├── index.html            # Web interface
    ├── css/
    │   └── style.css         # Tailwind styles
    └── js/
        └── simulation.js     # JavaScript bindings
```
