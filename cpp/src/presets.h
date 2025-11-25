#ifndef PRESETS_H
#define PRESETS_H

#include "gg_model.h"
#include <vector>
#include <string>

struct SnowflakePreset {
    std::string name;
    ModelSettings settings;
};

// Presets based on Gravner-Griffeath paper
static const std::vector<SnowflakePreset> SNOWFLAKE_PRESETS = {
    // Fig 2 tr: A classic sectored plate form.
    {
        "Sectored Plate",
        {
            .gridSize = 512,
            .rho = 0.8f,
            .beta = 2.9f,
            .kappa = 0.05f,
            .mu = 0.015f,
            .gamma = 0.0001f,
            .theta = 0.004f,
            .sigma = 0.00002f,
            .alpha = 0.006f
        }
    },
    // Fig 9: A very low-beta, compact plate.
    {
        "Low-Beta Plate",
        {
            .gridSize = 512,
            .rho = 1.3f,
            .beta = 0.08f,
            .kappa = 0.07f,
            .mu = 0.00005f,
            .gamma = 0.0f,
            .theta = 0.003f,
            .sigma = 0.0f,
            .alpha = 0.025f
        }
    },
    // Fig 10: Illustrates tip faceting at a very low beta.
    {
        "Tip Faceting",
        {
            .gridSize = 512,
            .rho = 0.8f,
            .beta = 0.004f,
            .kappa = 0.015f,
            .mu = 0.0001f,
            .gamma = 0.0f,
            .theta = 0.05f,
            .sigma = 0.0f,
            .alpha = 0.001f
        }
    },
    // Fig 11: A highly branched dendritic form.
    {
        "Highly Branched Dendrite",
        {
            .gridSize = 512,
            .rho = 0.635f,
            .beta = 1.6f,
            .kappa = 0.015f,
            .mu = 0.0005f,
            .gamma = 0.0f,
            .theta = 0.025f,
            .sigma = 0.0f,
            .alpha = 0.4f
        }
    },
    // Fig 12: A delicate, sparse stellar dendrite.
    {
        "Delicate Stellar Dendrite",
        {
            .gridSize = 512,
            .rho = 0.5f,
            .beta = 1.4f,
            .kappa = 0.001f,
            .mu = 0.001f,
            .gamma = 0.001f,
            .theta = 0.005f,
            .sigma = 0.0f,
            .alpha = 0.1f
        }
    },
    // Fig 13 l: Simple six-pointed star.
    {
        "Simple Star",
        {
            .gridSize = 512,
            .rho = 0.65f,
            .beta = 1.75f,
            .kappa = 0.15f,
            .mu = 0.015f,
            .gamma = 0.00001f,
            .theta = 0.2f,
            .sigma = 0.0f,
            .alpha = 0.026f
        }
    },
    // Fig 13 m: A stellar plate form.
    {
        "Stellar Plate",
        {
            .gridSize = 512,
            .rho = 0.36f,
            .beta = 1.09f,
            .kappa = 0.0001f,
            .mu = 0.14f,
            .gamma = 0.00001f,
            .theta = 0.0745f,
            .sigma = 0.0f,
            .alpha = 0.01f
        }
    },
    // Fig 13 r: A plate with prominent dendritic ends.
    {
        "Plate with Dendrite Ends",
        {
            .gridSize = 512,
            .rho = 0.38f,
            .beta = 1.06f,
            .kappa = 0.001f,
            .mu = 0.14f,
            .gamma = 0.0006f,
            .theta = 0.112f,
            .sigma = 0.0f,
            .alpha = 0.35f
        }
    }
};


inline const SnowflakePreset& getPreset(size_t index) {
    return SNOWFLAKE_PRESETS[index % SNOWFLAKE_PRESETS.size()];
}

inline size_t getPresetCount() {
    return SNOWFLAKE_PRESETS.size();
}

#endif // PRESETS_H