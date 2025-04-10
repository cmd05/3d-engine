/*
cmake --build build && ./build/3dengine.exe
cmake --build build > log.txt && ./build/3dengine.exe

cmake --build build > log.txt && ./build/3dengine.exe
*/

#include <chrono>
#include <random>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>

#include <nlohmann/json.hpp>

#include <engine/ecs/core/Scene.hpp>

#include <engine/ecs/systems/CameraControlSystem.hpp>
#include <engine/ecs/systems/PhysicsSystem.hpp>
#include <engine/ecs/systems/PlayerControlSystem.hpp>
#include <engine/ecs/systems/RenderSystem.hpp>

#include <engine/ecs/components/Cubemap.hpp>
#include <engine/ecs/components/Camera.hpp>
#include <engine/ecs/components/Gravity.hpp>
#include <engine/ecs/components/Player.hpp>
#include <engine/ecs/components/Renderable.hpp>
#include <engine/ecs/components/RigidBody.hpp>
#include <engine/ecs/components/Thrust.hpp>
#include <engine/ecs/components/Transform.hpp>
#include <engine/ecs/components/Model.hpp>
#include <engine/ecs/components/PointLight.hpp>

#include <engine/window/WindowManager.hpp>
#include <engine/input/InputHandler.hpp>
#include <engine/gui/GUIMain.hpp>

#include <engine/config/WindowConfig.hpp>
#include <engine/config/Events.hpp>

#include <application/Application.hpp>

int main() {
    Application app {};
    app.init();

    app.run();
}
