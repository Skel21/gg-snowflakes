#ifndef GG_VIS_H
#define GG_VIS_H

#include "../src/gg_model.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <algorithm>
#include <cstdint>
#include "colormap.h"


class Visualizer {
    public:
        Visualizer(ModelSettings& settings, int windowSize);
        ~Visualizer();
        bool init();
        void draw(Grid&);
        int getWindowSize();
        void resizeWindow(int newWindowSize);
        void resizeGrid(int newGridSize);
        void changeDrawingScale(float delta);
    private:
        int windowSize;
        float hexHorizontalDistance;
        float hexVerticalDistance;
        float drawingScale;
        int gridMiddle;
        int windowMiddle;

        ModelSettings* settings;

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        uint32_t* pixels = nullptr;
        std::vector<Point> pixelToHex; // Cache: maps each pixel to its nearest hex cell

        void getHexIndex(float x, float y, int& row, int& col);
};


Visualizer::~Visualizer() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Visualizer::Visualizer(ModelSettings& settings, int windowSize)
    : settings(&settings), windowSize(windowSize) {
    // Calculate hex spacing based on window and grid size
    drawingScale = 1.0f;
    hexVerticalDistance = windowSize / settings.gridSize * sqrt(3.0f) / 2.0f * drawingScale;
    hexHorizontalDistance = (2.0f / sqrt(3.0f)) * hexVerticalDistance;
    gridMiddle = settings.gridSize / 2;
    windowMiddle = windowSize / 2;
    init();
}

bool Visualizer::init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Vis", windowSize, windowSize, 0, &window, &renderer);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, windowSize, windowSize);

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    const char* name = SDL_GetRendererName(renderer);
    SDL_SetRenderDrawColorFloat(renderer, 0., 0., 0., 1.);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    pixels = new uint32_t[windowSize * windowSize];

    // Pre-compute which hex each pixel belongs to
    pixelToHex = std::vector<Point>(windowSize * windowSize, Point{0, 0});
    int row, col;
    for (int y = 0; y < windowSize; ++y) {
        for (int x = 0; x < windowSize; ++x) {
            getHexIndex(x, y, row, col);
            if (row < 0 || row >= settings->gridSize || col < 0 || col >= settings->gridSize) {
                pixelToHex[y * windowSize + x] = Point{0 ,0};
            } 
            else {
                pixelToHex[y * windowSize + x] = Point{row, col};
            }
        }
    }

    return true;
}

int Visualizer::getWindowSize() {
    return windowSize;
}

void Visualizer::resizeWindow(int newWindowSize) {
    windowSize = newWindowSize;
    windowMiddle = windowSize / 2;
    
    // Recalculate hex spacing
    hexVerticalDistance = windowSize / settings->gridSize * sqrt(3.0f) / 2.0f * drawingScale;
    hexHorizontalDistance = (2.0f * sqrt(3.0f) / 3.0f) * hexVerticalDistance;
    
    SDL_DestroyTexture(texture);
    delete[] pixels;
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
                                SDL_TEXTUREACCESS_STREAMING, windowSize, windowSize);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    pixels = new uint32_t[windowSize * windowSize];
    
    // Rebuild pixel-to-hex mapping
    pixelToHex.resize(windowSize * windowSize);
    int row, col;
    for (int y = 0; y < windowSize; ++y) {
        for (int x = 0; x < windowSize; ++x) {
            getHexIndex(x, y, row, col);
            if (row < 0 || row >= settings->gridSize || col < 0 || col >= settings->gridSize) {
                pixelToHex[y * windowSize + x] = Point{0, 0};
            } else {
                pixelToHex[y * windowSize + x] = Point{row, col};
            }
        }
    }
}

void Visualizer::resizeGrid(int newGridSize) {
    settings->gridSize = newGridSize;
    gridMiddle = settings->gridSize / 2;

    // Recalculate hex spacing
    hexVerticalDistance = windowSize / settings->gridSize * sqrt(3.0f) / 2.0f * drawingScale;
    hexHorizontalDistance = (2.0f * sqrt(3.0f) / 3.0f) * hexVerticalDistance;

    // Rebuild pixel-to-hex mapping
    int row, col;
    for (int y = 0; y < windowSize; ++y) {
        for (int x = 0; x < windowSize; ++x) {
            getHexIndex(x, y, row, col);
            if (row < 0 || row >= settings->gridSize || col < 0 || col >= settings->gridSize) {
                pixelToHex[y * windowSize + x] = Point{0, 0};
            } else {
                pixelToHex[y * windowSize + x] = Point{row, col};
            }
        }
    }
}


void Visualizer::getHexIndex(float x, float y, int& row, int& col) {
    // Convert pixel coordinates to hex grid coordinates (floating point)
    float rowF = gridMiddle + (y - windowMiddle) / hexVerticalDistance;
    float colF = gridMiddle + (x - windowMiddle) / hexHorizontalDistance
                          + (rowF - gridMiddle) * 0.5f;

    // Check 4 candidate hex cells around this position
    int r0 = static_cast<int>(std::floor(rowF));
    int c0 = static_cast<int>(std::floor(colF));
    
    // Find closest hex center
    float minDist = std::numeric_limits<float>::max();
    for (int dr = 0; dr <= 1; dr++) {
        for (int dc = 0; dc <= 1; dc++) {
            int testRow = r0 + dr;
            int testCol = c0 + dc;
            float hx = windowMiddle + (testCol - gridMiddle - (testRow - gridMiddle) * 0.5f) * hexHorizontalDistance;
            float hy = windowMiddle + (testRow - gridMiddle) * hexVerticalDistance;
            float dist = (x - hx) * (x - hx) + (y - hy) * (y - hy);
            if (dist < minDist) {
                minDist = dist;
                row = testRow;
                col = testCol;
            }
        }
    }
}


void Visualizer::draw(Grid& grid) {
    // Find max values for normalization
    float maxC = 0.0f, maxD = 0.0f;
    for (int i = 0; i < settings->gridSize; i++) {
        for (int j = 0; j < settings->gridSize; j++) {
            if (grid.crystalMass[i][j] > maxC) maxC = grid.crystalMass[i][j];
            if (grid.diffusiveMass[i][j] > maxD) maxD = grid.diffusiveMass[i][j];
        }
    }

    // Color each pixel based on its hex cell's values
    uint32_t color;
    int row, col;
    Point p;
    for (int x = 0; x < windowSize; x++) {
        for (int y = 0; y < windowSize; y++) {
            p = pixelToHex[y * windowSize + x];
            pixels[y * windowSize + x] = colorMap(grid, p.first, p.second, maxC, maxD);
        }
    }

    // Render to screen
    SDL_UpdateTexture(texture, NULL, pixels, windowSize * sizeof(uint32_t));
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}


void Visualizer::changeDrawingScale(float delta) {
    drawingScale *= delta;
    drawingScale = std::clamp(drawingScale, 0.5f, 100.0f);
    hexVerticalDistance = windowSize / settings->gridSize * sqrt(3.0f) / 2.0f * drawingScale;
    hexHorizontalDistance = (2.0f / sqrt(3.0f)) * hexVerticalDistance;

    // Rebuild pixel-to-hex mapping
    int row, col;
    for (int y = 0; y < windowSize; ++y) {
        for (int x = 0; x < windowSize; ++x) {
            getHexIndex(x, y, row, col);
            if (row < 0 || row >= settings->gridSize || col < 0 || col >= settings->gridSize) {
                pixelToHex[y * windowSize + x] = Point{settings->gridSize-1, 0};
            } else {
                pixelToHex[y * windowSize + x] = Point{row, col};
            }
        }
    }
}
#endif // GG_VIS_H