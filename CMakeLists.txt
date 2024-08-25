cmake_minimum_required(VERSION 3.14)
project(3dengine)

# C:/msys64/ucrt64/bin/c++.exe
# message (${CMAKE_CXX_COMPILER})

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
# add_subdirectory(${INCLUDES_DIR}/assimp)

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
    src/graphics/Shader.cpp
    src/main.cpp
    
    src/engine/systems/CameraControlSystem.cpp
    src/engine/systems/PhysicsSystem.cpp
    src/engine/systems/PlayerControlSystem.cpp
    src/engine/systems/RenderSystem.cpp
    src/window/WindowManager.cpp
)

target_sources(
    3dengine
    PRIVATE
    src/engine/components/Camera.hpp
    src/engine/components/Gravity.hpp
    src/engine/components/Player.hpp
    src/engine/components/Renderable.hpp
    src/engine/components/RigidBody.hpp
    src/engine/components/Thrust.hpp
    src/engine/components/Transform.hpp

    src/engine/core/ComponentArray.hpp
    src/engine/core/ComponentManager.cpp
    src/engine/core/Scene.cpp
    src/engine/core/EntityManager.cpp
    src/engine/core/EventManager.cpp
    src/engine/core/SystemManager.cpp
    src/engine/core/Event.cpp

    src/engine/core/System.hpp
    src/engine/core/Types.hpp

    src/engine/core/SceneView.hpp

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
    # assimp
    # ${Boost_LIBRARIES}
)

target_compile_definitions(3dengine PUBLIC FS_SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/")