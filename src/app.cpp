#include "../include/app.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <iostream>

bool App::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Input Manager", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        800, 600, 
        SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window creation error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    if (!ImGui_ImplSDL2_InitForSDLRenderer(window, renderer)) {
        std::cerr << "ImGui SDL2 init failed" << std::endl;
        return false;
    }
    
    if (!ImGui_ImplSDLRenderer2_Init(renderer)) {
        std::cerr << "ImGui Renderer init failed" << std::endl;
        return false;
    }

    devices = listXInputDevices();
    return true;
}

void App::run() {
    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) running = false;
        }
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        renderUI();
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
}

void App::shutdown() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void App::renderUI() {
    // Make the window fill the entire application window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Input Devices", nullptr, 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse);

    // Calculate dimensions for left tabs
    float windowWidth = ImGui::GetWindowWidth();
    float windowHeight = ImGui::GetWindowHeight();
    float tabWidth = windowWidth * 0.2f; // 20% of window width for tabs
    float contentWidth = windowWidth * 0.78f; // 78% of window width for content

    // Left-side tabs using child window
    ImGui::BeginChild("##tabs", ImVec2(tabWidth, windowHeight), true);
    
    static int selected_tab = 0;
    for (size_t i = 0; i < devices.size(); i++) {
        // Create unique ID for each tab by combining name and index
        std::string tabLabel = getIcon(devices[i].name) + " " + devices[i].name + "##" + std::to_string(i);
        
        if (ImGui::Selectable(tabLabel.c_str(), selected_tab == i)) {
            selected_tab = i;
        }
    }
    ImGui::EndChild();

    // Content area
    ImGui::SameLine();
    ImGui::BeginChild("##content", ImVec2(contentWidth, windowHeight), true);
    
    if (selected_tab >= 0 && selected_tab < devices.size()) {
        auto& dev = devices[selected_tab];
        
        // Show device properties
        ImGui::Text("Device: %s", dev.name.c_str());
        ImGui::Separator();

        // Add current time and user info
        ImGui::TextDisabled("Last Updated: 2025-07-05 13:38:27");
        ImGui::TextDisabled("User: EndermanPC");
        ImGui::Separator();

        // Properties in a scrollable area
        ImGui::BeginChild("##properties", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave space for button
        
        for (auto& [key, value] : dev.properties) {
            // Create unique ID for each input by combining property key and device index
            std::string inputId = "##" + key + std::to_string(selected_tab);
            
            char buf[256];
            strncpy(buf, value.c_str(), sizeof(buf) - 1);
            buf[sizeof(buf) - 1] = '\0';

            // Make the input field take most of the width
            float availWidth = ImGui::GetContentRegionAvail().x;
            ImGui::PushItemWidth(availWidth * 0.7f);
            
            if (ImGui::InputText((key + inputId).c_str(), buf, sizeof(buf))) {
                dev.pending_changes[key] = std::string(buf);
            }
            
            ImGui::PopItemWidth();

            // Show original value as tooltip
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Original: %s", value.c_str());
            }
        }
        ImGui::EndChild();

        // Apply button at the bottom
        if (!dev.pending_changes.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            
            if (ImGui::Button("Apply Changes", ImVec2(-1, 0))) {
                applyXInputChanges(dev);
                dev.pending_changes.clear();
            }
            
            ImGui::PopStyleColor(2);
        }
    }
    
    ImGui::EndChild();
    ImGui::End();
}