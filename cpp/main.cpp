#include "./vis/vis.h"
#include "./src/gg_model.h"
#include "./src/presets.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_events.h>
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/bind.h>
#endif

ModelSettings *settings;
Model *model;
Visualizer *visualizer;
int iterationsPerFrame = 1;
int current_iteration = 0;
SDL_Event event;


void main_loop()
{
    // Handle events
    while (SDL_PollEvent(&event)) {
        #ifndef __EMSCRIPTEN__
        if (event.type == SDL_EVENT_QUIT) {
            exit(0);
        }
        #endif
        if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            visualizer->changeDrawingScale(1.0f + event.wheel.y / 20.0f);
        }
    }

    // Advance model
    model->time_step();
    current_iteration++;
    if (current_iteration >= iterationsPerFrame)
    {
        current_iteration = 0;
        visualizer->draw(model->snowflake);
    }
}

void init()
{
    #ifdef __EMSCRIPTEN__
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR, "#simulation");
    #endif

    settings = new ModelSettings();
    settings->gridSize = 400;
    settings->rho = 0.635f;
    settings->beta = 1.6f;
    settings->kappa = 0.005f;
    settings->mu =0.015f;
    settings->gamma = 0.0005f;
    settings->theta = 0.025f;
    settings->sigma = 0.0f;
    settings->alpha = 0.4f;

    model = new Model(*settings);
    visualizer = new Visualizer(*settings, 1000);
}

void reset()
{
    model->initialize();
}

void set_window_size(int size)
{
    #ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop();
    #endif
    
    visualizer->resizeWindow(size);
    
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
    #endif
}

void set_grid_size(int size)
{
    #ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop();
    #endif
    
    visualizer->resizeGrid(size);
    delete model;
    model = new Model(*settings);
    
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
    #endif
}


void set_beta(float beta) { settings->beta = beta; }
void set_rho(float rho) { settings->rho = rho; }
void set_theta(float theta) { settings->theta = theta; }
void set_alpha(float alpha) { settings->alpha = alpha; }
void set_mu(float mu) { settings->mu = mu; }
void set_kappa(float kappa) { settings->kappa = kappa; }
void set_iterations_per_frame(int iterations) { iterationsPerFrame = iterations; }

float get_current_alpha() { return settings->alpha; }
float get_current_beta() { return settings->beta; }
float get_current_mu() { return settings->mu; }
float get_current_kappa() { return settings->kappa; }
float get_current_rho() { return settings->rho; }
float get_current_theta() { return settings->theta; }
float get_current_gamma() { return settings->gamma; }
int get_current_grid_size() { return settings->gridSize; }


int get_preset_count() { return static_cast<int>(getPresetCount()); }


std::string get_preset_info(int index)
{   
    const auto& preset = getPreset(index);
    return preset.name;
}


void apply_preset(int index)
{
    if (index < 0 || index >= static_cast<int>(getPresetCount())) {
        return;
    }
    
    #ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop();
    #endif
    
    const auto& preset = getPreset(index);
    
    set_alpha(preset.settings.alpha);
    set_beta(preset.settings.beta);
    set_mu(preset.settings.mu);
    set_kappa(preset.settings.kappa);
    set_rho(preset.settings.rho);
    set_theta(preset.settings.theta);
    settings->gamma = preset.settings.gamma;
    settings->sigma = preset.settings.sigma; // TODO: sigma does not do anything
    set_grid_size(preset.settings.gridSize);
    reset();
    
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
    #endif
}



#ifdef __EMSCRIPTEN__

void set_main_loop()
{
    emscripten_set_main_loop(main_loop, 0, 1);
}

void play_pause(bool paused)
{
    if (paused)
    {
        emscripten_cancel_main_loop();
    }
    else
    {
        emscripten_set_main_loop(main_loop, 0, 1);
    }
}

EMSCRIPTEN_BINDINGS()
{
    emscripten::function("init", &init);
    emscripten::function("main_loop", &main_loop);
    emscripten::function("set_main_loop", &set_main_loop);
    emscripten::function("reset", &reset);
    emscripten::function("set_beta", &set_beta);
    emscripten::function("set_rho", &set_rho);
    emscripten::function("set_theta", &set_theta);
    emscripten::function("set_alpha", &set_alpha);
    emscripten::function("set_mu", &set_mu);
    emscripten::function("set_kappa", &set_kappa);
    emscripten::function("play_pause", &play_pause);
    emscripten::function("set_iterations_per_frame", &set_iterations_per_frame);
    emscripten::function("set_window_size", &set_window_size);
    emscripten::function("set_grid_size", &set_grid_size);

    emscripten::function("get_current_alpha", &get_current_alpha);
    emscripten::function("get_current_beta", &get_current_beta);
    emscripten::function("get_current_mu", &get_current_mu);
    emscripten::function("get_current_kappa", &get_current_kappa);
    emscripten::function("get_current_rho", &get_current_rho);
    emscripten::function("get_current_theta", &get_current_theta);
    emscripten::function("get_current_gamma", &get_current_gamma);
    emscripten::function("get_current_grid_size", &get_current_grid_size);

    emscripten::function("get_preset_count", &get_preset_count);
    emscripten::function("get_preset_info", &get_preset_info);
    emscripten::function("apply_preset", &apply_preset);
}
#endif

#ifndef __EMSCRIPTEN__
int main()
{
    init();
    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
        main_loop();
    }
    return 0;
}
#endif
