cmake_minimum_required(VERSION 3.14)
project(3dengine)

# C:/msys64/ucrt64/bin/c++.exe
# message (${CMAKE_CXX_COMPILER})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(3dengine)

target_compile_options(
    3dengine
    PRIVATE
    # -fno-exceptions
    -fdiagnostics-color=always
    -Wall
    -Wno-reorder
    # -Wno-unknown-pragmas
    # -Wno-error=unknown-pragmas
    # Wno-unknown-pragmas
)

###########################################################
# DIRECTORIES
###########################################################

set(INCLUDES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/includes)

###########################################################
# 3RD PARTY LIBRARIES
###########################################################

# GLFW (window creation and input)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(${INCLUDES_DIR}/glfw-3.4)

# glad (OpenGL function loading)
add_subdirectory(${INCLUDES_DIR}/glad)

# glm
# find_package(glm CONFIG REQUIRED)
add_subdirectory(${INCLUDES_DIR}/glm)

# assimp
SET(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "Build Assimp Exporters")
SET(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build Assimp Tests")
SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Assimp shared library")
SET(ASSIMP_NO_EXPORT ON CACHE BOOL "Disable Assimp's export functionality.")

add_subdirectory(${INCLUDES_DIR}/assimp)

# stb
# add_subdirectory(${INCLUDES_DIR}/stb)

# imgui
# target_sources(
#     3dengine
#     PRIVATE
#     includes/imgui/imgui.cpp
#     includes/imgui/imgui_demo.cpp
#     includes/imgui/imgui_draw.cpp
#     includes/imgui/imgui_impl_glfw.cpp
#     includes/imgui/imgui_impl_opengl3.cpp
#     includes/imgui/imgui_stdlib.cpp
#     includes/imgui/imgui_tables.cpp
#     includes/imgui/imgui_widgets.cpp
# )

# Add the imgui subdirectory
add_subdirectory(${INCLUDES_DIR}/imgui)

    3dengine
    PRIVATE
    # -fno-exceptions
    -fdiagnostics-color=always
    -Wall
    -Wno-reorder
    # -Wno-unknown-pragmas
    # -Wno-error=unknown-pragmas
    # Wno-unknown-pragmas
)

# set( Boost_DEBUG ON) 
# set(Boost_USE_STATIC_LIBS ON)
# set(Boost_USE_MULTITHREADED ON)
# message("Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
# message("Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
# include_directories( ${Boost_INCLUDE_DIRS} )
# find_package( Boost REQUIRED COMPONENTS json )
# find_package(assimp)

target_sources(
    3dengine
    PRIVATE
    src/engine/graphics/Shader.cpp
    src/application/main.cpp
    
    src/engine/ecs/systems/CameraControlSystem.cpp
    src/engine/ecs/systems/PhysicsSystem.cpp
    src/engine/ecs/systems/PlayerControlSystem.cpp
    src/engine/ecs/systems/RenderSystem.cpp
    
    src/engine/window/WindowManager.cpp

    # model loading
    # include stb manually for now
    ${INCLUDES_DIR}/stb/stb_image.c

    # src/engine/graphics/Mesh.cpp
    # src/engine/graphics/ModelLoader.cpp

    src/engine/graphics/MeshProcessor.cpp
    src/engine/graphics/ModelProcessor.cpp
    src/engine/graphics/ModelManager.cpp
    src/engine/graphics/TextureManager.cpp
    src/engine/graphics/CameraWrapper.cpp
)

target_sources(
    3dengine
    PRIVATE
    src/engine/ecs/components/Camera.hpp
    src/engine/ecs/components/Gravity.hpp
    src/engine/ecs/components/Player.hpp
    src/engine/ecs/components/Renderable.hpp
    src/engine/ecs/components/RigidBody.hpp
    src/engine/ecs/components/Thrust.hpp
    src/engine/ecs/components/Transform.hpp

    src/engine/ecs/core/ComponentArray.hpp
    src/engine/ecs/core/ComponentManager.cpp
    src/engine/ecs/core/Scene.cpp
    src/engine/ecs/core/EntityManager.cpp
    src/engine/ecs/core/EventManager.cpp
    src/engine/ecs/core/SystemManager.cpp
    src/engine/ecs/core/Event.cpp

    src/engine/ecs/core/System.hpp
    src/engine/ecs/core/Types.hpp

    src/engine/ecs/core/SceneView.hpp

    src/engine/input/InputHandler.cpp

    # src/lib/math/Vec2.hpp
    # src/lib/math/Vec3.hpp
    # src/lib/math/Vec4.hpp
)

target_include_directories(
    3dengine
    PRIVATE
    src
)

target_link_libraries(
    3dengine
    PRIVATE
    glad
    glfw
    glm::glm
    assimp
    imgui
)

target_compile_definitions(3dengine PUBLIC FS_SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src/engine/shaders/")
target_compile_definitions(3dengine PUBLIC FS_RESOURCES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src/resources/")