#ifndef GG_MODEL_H
#define GG_MODEL_H

#include <vector>
#include <cmath>

using FloatGrid = std::vector<std::vector<float>>;
using IntGrid = std::vector<std::vector<int>>;
using Point = std::pair<int, int>;

struct ModelSettings {
    int gridSize;
    float rho;      // Initial vapor density
    float beta;     // Anisotropy parameter (threshold for tips/flat spots)
    float kappa;    // Proportion of diffusive mass that crystallizes directly
    float mu;       // Melting rate for boundary mass
    float gamma;    // Melting rate for crystal mass
    float theta;    // Diffusive mass threshold for knife-edge instability
    float sigma;    // Noise parameter NOT WORKING
    float alpha;    // Reduced boundary mass threshold when diffusive mass < theta
};

struct Grid {
    IntGrid isCrystal;
    FloatGrid boundaryMass;
    FloatGrid crystalMass;
    FloatGrid diffusiveMass;
};

class Model {
    public:
        Model(ModelSettings&);
        void initialize();
        void time_step();
        Grid snowflake;
    private:
        const float kernelWeight = 1.0f / 7.0f;
        int lower_bound_row, upper_bound_row;
        int lower_bound_col, upper_bound_col;
        
        void diffusion();
        void freezing();
        void attachment();
        void melting();

        ModelSettings* settings;
        Point center;
        FloatGrid intermediateDiffusiveMass;
        IntGrid isBoundary;
        
        const std::vector<Point> neighbors = {
            {-1, -1}, {-1, 0},
            {0, -1}, {0, 1},
            {1, 0}, {1, 1}
        };
};

Model::Model(ModelSettings& settings) : settings(&settings) {
    initialize();
}

void Model::initialize() {
    center = {settings->gridSize / 2, settings->gridSize / 2};
    lower_bound_row = 0;
    upper_bound_row = settings->gridSize;
    lower_bound_col = 0;
    upper_bound_col = settings->gridSize;

    snowflake.isCrystal = IntGrid(settings->gridSize, std::vector<int>(settings->gridSize, 0));
    snowflake.boundaryMass = FloatGrid(settings->gridSize, std::vector<float>(settings->gridSize, 0.0));
    snowflake.crystalMass = FloatGrid(settings->gridSize, std::vector<float>(settings->gridSize, 0.0));
    snowflake.diffusiveMass = FloatGrid(settings->gridSize, std::vector<float>(settings->gridSize, settings->rho));
    intermediateDiffusiveMass = FloatGrid(settings->gridSize, std::vector<float>(settings->gridSize, 0.0));
    isBoundary = IntGrid(settings->gridSize, std::vector<int>(settings->gridSize, 0));
    
    // Initial crystal seed
    snowflake.isCrystal[center.first][center.second] = 1;
    snowflake.crystalMass[center.first][center.second] = 1.0;
    snowflake.diffusiveMass[center.first][center.second] = 0.0;
    for (auto& neighbor : neighbors) {
        isBoundary[center.first + neighbor.first][center.second + neighbor.second] = 1;
    }
}

void Model::time_step() {
    diffusion();
    freezing();
    attachment();
    melting();
}

void Model::diffusion() {
    const int N = settings->gridSize;
    
    for (int i = lower_bound_row; i < upper_bound_row; ++i) {
        for (int j = lower_bound_col; j < upper_bound_col; ++j) {
            // Crystal sites have no diffusive mass
            if (snowflake.isCrystal[i][j]) {
                intermediateDiffusiveMass[i][j] = 0.0;
                continue;
            }
            
            // Sum contributions from center and 6 neighbors
            float sum = snowflake.diffusiveMass[i][j];
            
            for (auto& neighbor : neighbors) {
                int x = (N + i + neighbor.first) % N;
                int y = (N + j + neighbor.second) % N;
                
                if (snowflake.isCrystal[x][y]) {
                    // Reflecting boundary: use current cell's value instead of crystal neighbor
                    sum += snowflake.diffusiveMass[i][j];
                } else {
                    // Normal diffusion from non-crystal neighbor
                    sum += snowflake.diffusiveMass[x][y];
                }
            }
            
            intermediateDiffusiveMass[i][j] = kernelWeight * sum;
        }
    }
    
    std::swap(snowflake.diffusiveMass, intermediateDiffusiveMass);
}

void Model::freezing() {
    for (int i = lower_bound_row; i < upper_bound_row; ++i) {
        for (int j = lower_bound_col; j < upper_bound_col; ++j) {
            // Ensure crystal sites have no diffusive mass
            if (snowflake.isCrystal[i][j]) {
                snowflake.diffusiveMass[i][j] = 0.0;
                continue;
            }
            
            // Only boundary sites participate in freezing
            if (isBoundary[i][j]) {
                // Proportion kappa crystallizes directly
                snowflake.crystalMass[i][j] += settings->kappa * snowflake.diffusiveMass[i][j];
                
                // Proportion (1-kappa) becomes boundary mass (quasi-liquid)
                snowflake.boundaryMass[i][j] += (1.0f - settings->kappa) * snowflake.diffusiveMass[i][j];
                
                // All diffusive mass at boundary is now converted
                snowflake.diffusiveMass[i][j] = 0.0;
            }
        }
    }
}

void Model::attachment() {
    const int N = settings->gridSize;
    
    // Use temporary grid to avoid modifying during iteration
    IntGrid newIsBoundary = isBoundary;
    IntGrid newIsCrystal = snowflake.isCrystal;
    
    for (int i = lower_bound_row; i < upper_bound_row; ++i) {
        for (int j = lower_bound_col; j < upper_bound_col; ++j) {
            // Skip if already crystal
            if (snowflake.isCrystal[i][j] == 1) continue;
            
            // Count attached neighbors
            int attachedNeighbors = 0;
            for (auto& neighbor : neighbors) {
                int x = (N + i + neighbor.first) % N;
                int y = (N + j + neighbor.second) % N;
                if (snowflake.isCrystal[x][y] == 1) {
                    attachedNeighbors++;
                }
            }
            
            // Skip if not a boundary site
            if (attachedNeighbors == 0) continue;
            
            bool shouldAttach = false;
            
            // Case 1 & 2: Tips and flat spots (1 or 2 attached neighbors)
            if (attachedNeighbors == 1 || attachedNeighbors == 2) {
                if (snowflake.boundaryMass[i][j] >= settings->beta) {
                    shouldAttach = true;
                }
            }
            // Case 3: Concavities (3 attached neighbors)
            else if (attachedNeighbors == 3) {
                // Always attach if boundary mass >= 1
                if (snowflake.boundaryMass[i][j] >= 1.0f) {
                    shouldAttach = true;
                }
                // Knife-edge instability: attach if low diffusive mass and boundary mass >= alpha
                else {
                    // Calculate neighborhood diffusive mass (center + 6 neighbors)
                    float neighbourhoodDiffusiveMass = snowflake.diffusiveMass[i][j];
                    
                    for (auto& neighbor : neighbors) {
                        int x = (N + i + neighbor.first) % N;
                        int y = (N + j + neighbor.second) % N;
                        // Only count diffusive mass at non-crystal sites
                        if (snowflake.isCrystal[x][y] == 0) {
                            neighbourhoodDiffusiveMass += snowflake.diffusiveMass[x][y];
                        }
                    }
                    
                    // If vapor is depleted AND boundary mass exceeds alpha, attach
                    if (neighbourhoodDiffusiveMass < settings->theta && 
                        snowflake.boundaryMass[i][j] >= settings->alpha) {
                        shouldAttach = true;
                    }
                }
            }
            // Case 4+: Highly concave (4+ attached neighbors) - always attach
            else { // attachedNeighbors >= 4
                shouldAttach = true;
            }
            
            if (shouldAttach) {
                // Mark as crystal
                newIsCrystal[i][j] = 1;
                
                // Transfer boundary mass to crystal mass (equation 3d)
                snowflake.crystalMass[i][j] += snowflake.boundaryMass[i][j];
                snowflake.boundaryMass[i][j] = 0.0;
                
                // Mark all non-crystal neighbors as boundary sites
                for (auto& neighbor : neighbors) {
                    int x = (N + i + neighbor.first) % N;
                    int y = (N + j + neighbor.second) % N;
                    if (snowflake.isCrystal[x][y] == 0) {
                        newIsBoundary[x][y] = 1;
                    }
                }
            }
        }
    }
    
    // Update grids
    snowflake.isCrystal = newIsCrystal;
    isBoundary = newIsBoundary;
}

void Model::melting() {
    for (int i = lower_bound_row; i < upper_bound_row; ++i) {
        for (int j = lower_bound_col; j < upper_bound_col; ++j) {
            // Only boundary sites participate in melting
            if (isBoundary[i][j] && snowflake.isCrystal[i][j] == 0) {
                // Calculate melted amounts
                float meltedBoundary = settings->mu * snowflake.boundaryMass[i][j];
                float meltedCrystal = settings->gamma * snowflake.crystalMass[i][j];
                
                // Reduce boundary and crystal mass
                snowflake.boundaryMass[i][j] -= meltedBoundary;
                snowflake.crystalMass[i][j] -= meltedCrystal;
                
                // Add melted mass back to diffusive mass
                snowflake.diffusiveMass[i][j] += meltedBoundary + meltedCrystal;
            }
        }
    }
}

#endif // GG_MODEL_H