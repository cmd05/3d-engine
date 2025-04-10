cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(3dengine)

message("[cmake_log_3dengine] C++ Compiler: " ${CMAKE_CXX_COMPILER})

### Compilation Flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(3dengine)

target_compile_options(
    3dengine
    PRIVATE
    -fdiagnostics-color=always
    -Wall
)

# -Wno-reorder
if (MSVC)
    target_compile_options(3dengine PRIVATE /wd5038)
else()
    target_compile_options(3dengine PRIVATE -Wno-reorder)
endif()

### Set /includes directory
set(INCLUDES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/includes)

###########################################################
# 3RD PARTY LIBRARIES
###########################################################

### nlohmann-json
# https://json.nlohmann.me/integration/cmake/#fetchcontent
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(json)

### Glad
add_subdirectory(${INCLUDES_DIR}/glad)

### STB
add_subdirectory(${INCLUDES_DIR}/stb)

### GLFW
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.4
)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)

FetchContent_MakeAvailable(glfw)

### Assimp
FetchContent_Declare(assimp
    GIT_REPOSITORY https://github.com/assimp/assimp
    # commit should be after https://github.com/assimp/assimp/commit/4275213306a123211b9fe124644927d2e74af6aa
    GIT_TAG 2690e354da0c681db000cfd892a55226788f2743
)

SET(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "Build Assimp Exporters")
SET(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build Assimp Tests")
SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Assimp shared library")
SET(ASSIMP_NO_EXPORT ON CACHE BOOL "Disable Assimp's export functionality.")
SET(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Treat all warnings as errors.")
SET(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "Build Assimp view tool (requires DirectX).")

FetchContent_MakeAvailable(assimp)

### GLM
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	    2d4c4b4dd31fde06cfffad7915c2b3006402322f
)

FetchContent_MakeAvailable(glm)

### ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    # use "docking" branch
    GIT_TAG 126d004f9e1eef062bf4b044b3b2faaf58d48c51 # ver 1.91.9 (docking)
)

FetchContent_MakeAvailable(imgui)

add_library(imgui)

set(IMGUI_INCLUDE_DIR ${imgui_SOURCE_DIR})
target_include_directories(imgui PUBLIC 
    ${IMGUI_INCLUDE_DIR}
    ${IMGUI_INCLUDE_DIR}/backends
)

target_sources(imgui
    PUBLIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp

    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(imgui PRIVATE glfw)

###########################################################
# SOURCES
###########################################################

target_sources(
    3dengine
    PRIVATE

    src/application/main.cpp
    src/application/Application.cpp

    src/engine/ecs/core/ComponentManager.cpp
    src/engine/ecs/core/Scene.cpp
    src/engine/ecs/core/EntityManager.cpp
    src/engine/ecs/core/EventManager.cpp
    src/engine/ecs/core/SystemManager.cpp
    src/engine/ecs/core/Event.cpp

    src/engine/ecs/systems/CameraControlSystem.cpp
    src/engine/ecs/systems/PhysicsSystem.cpp
    src/engine/ecs/systems/PlayerControlSystem.cpp
    src/engine/ecs/systems/RenderSystem.cpp
    
    src/engine/window/WindowManager.cpp

    src/engine/graphics/Shader.cpp
    src/engine/graphics/MeshProcessor.cpp
    src/engine/graphics/ModelProcessor.cpp
    src/engine/graphics/ModelManager.cpp
    src/engine/graphics/TextureManager.cpp
    src/engine/graphics/CameraWrapper.cpp
    src/engine/graphics/LightRenderer.cpp
    src/engine/graphics/objects/GraphicsObjects.cpp
    src/engine/graphics/lib/GraphicsHelper.cpp

    src/engine/input/InputHandler.cpp

    src/engine/gui/GUIMain.cpp
    src/engine/gui/GUIWindow.cpp
)

### Include Directories
target_include_directories(
    3dengine
    PRIVATE
    src
    ${INCLUDES_DIR}
)

### Link Libraries
target_link_libraries(
    3dengine
    PRIVATE
    glad
    glfw
    glm::glm
    assimp
    stb
    imgui
    nlohmann_json::nlohmann_json
)

### Macros used in source code
target_compile_definitions(3dengine PUBLIC FS_SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src/engine/shaders/")
target_compile_definitions(3dengine PUBLIC FS_RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
