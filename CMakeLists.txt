cmake_minimum_required(VERSION 3.16)
project(EasyInputManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)
if(NOT SDL2_FOUND)
    message(FATAL_ERROR "SDL2 not found!")
endif()

# ImGui source files
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
if(NOT EXISTS ${IMGUI_DIR})
    message(FATAL_ERROR "ImGui directory not found! Please run: git clone https://github.com/ocornut/imgui.git imgui")
endif()

include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${IMGUI_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(EasyInputManager
    src/main.cpp
    src/app.cpp
    include/app.h
    src/devicemanager.cpp
    include/devicemanager.h
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

target_link_libraries(EasyInputManager PRIVATE ${SDL2_LIBRARIES})

# Install target
install(TARGETS EasyInputManager DESTINATION bin)
