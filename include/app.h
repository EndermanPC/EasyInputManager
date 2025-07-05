#pragma once
#include <SDL.h>
#include <vector>
#include "devicemanager.h"

class App {
public:
    bool init();
    void run();
    void shutdown();
private:
    void renderUI();
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::vector<InputDevice> devices;
    
    int selected_device = -1;
    bool show_settings = false;
};