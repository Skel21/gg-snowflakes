#ifndef GG_COLORMAP_H
#define GG_COLORMAP_H

#include "../src/gg_model.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cstdint>
#include <cmath>

constexpr int LUT_SIZE = 256;

// Linear interpolation helper
inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Smooth step interpolation for smoother transitions
inline float smoothstep(float t) {
    return t * t * (3.0f - 2.0f * t);
}

// Build lookup tables
inline void buildColorLUT(uint8_t redLUT[LUT_SIZE],
                          uint8_t greenLUT[LUT_SIZE],
                          uint8_t blueLUT[LUT_SIZE]) {
    for (int i = 0; i < LUT_SIZE; ++i) {
        float t = i / float(LUT_SIZE - 1);
        
        float r, g, b;

        if (t <= 0.5f) {
            // Vapor background
            float localT = smoothstep(t / 0.5f);
            r = lerp(0.05f, 0.25f, localT);
            g = lerp(0.08f, 0.35f, localT);
            b = lerp(0.15f, 0.50f, localT);
        } else {
            // Crystal
            float localT = smoothstep((t - 0.5f) / 0.5f);
            r = lerp(0.20f, 1.00f, localT);
            g = lerp(0.50f, 1.00f, localT);
            b = lerp(0.70f, 1.00f, localT);
        }

        redLUT[i]   = static_cast<uint8_t>(std::clamp(r * 255.f, 0.f, 255.f));
        greenLUT[i] = static_cast<uint8_t>(std::clamp(g * 255.f, 0.f, 255.f));
        blueLUT[i]  = static_cast<uint8_t>(std::clamp(b * 255.f, 0.f, 255.f));
    }
}

// Global LUTs
static uint8_t redLUT[LUT_SIZE];
static uint8_t greenLUT[LUT_SIZE];
static uint8_t blueLUT[LUT_SIZE];
static bool lutInitialized = false;

// Color mapping
inline uint32_t colorMap(Grid& grid, int i, int j,
                         float maxCrystalMass, float maxDiffusiveMass) {
    if (!lutInitialized) {
        buildColorLUT(redLUT, greenLUT, blueLUT);
        lutInitialized = true;
    }

    float value = 0.0f;
    if (grid.isCrystal[i][j]) {
        if (maxCrystalMass > 0.0f) {
            float normalizedMass = grid.crystalMass[i][j] / maxCrystalMass;
            value = std::pow(normalizedMass, 0.5f);
        }
    } else {
        if (maxDiffusiveMass > 0.0f) {
            // Keep vapor scaling the same: dark and subtle
            float normalizedMass = grid.diffusiveMass[i][j] / maxDiffusiveMass;
            value = -std::pow(normalizedMass, 1.5f);
        }
    }

    // Remap value from [-1,1] -> [0,1]
    float t = (value + 1.0f) * 0.5f;
    t = std::clamp(t, 0.0f, 1.0f);

    int idx = static_cast<int>(t * (LUT_SIZE - 1));
    uint8_t r = redLUT[idx];
    uint8_t g = greenLUT[idx];
    uint8_t b = blueLUT[idx];

    return 0xFF000000 | (r << 16) | (g << 8) | b;
}

#endif // GG_COLORMAP_H