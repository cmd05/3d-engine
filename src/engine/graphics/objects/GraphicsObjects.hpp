#pragma once

#include <engine/graphics/objects/CubeObject.hpp>

struct GraphicsObjects {
    CubeObject cube;
    void init() {
        cube.init();
    }
};

extern GraphicsObjects g_graphics_objects;