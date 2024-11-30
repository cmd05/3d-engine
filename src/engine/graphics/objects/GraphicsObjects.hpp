#pragma once

#include <engine/graphics/objects/CubeObject.hpp>
#include <engine/graphics/objects/QuadObject.hpp>

struct GraphicsObjects {
    CubeObject cube;
    QuadObject quad_object;

    void init() {
        cube.init();
        quad_object.init();
    }
};

extern GraphicsObjects g_graphics_objects;