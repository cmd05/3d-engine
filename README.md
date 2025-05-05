# 3D Engine

## Current Status


![](./docs/assets/render-sponza-multiple-lights.png)
![](./docs/assets/sponza-shadow-mapping-1.png)


- Shadow Mapping
- Physically Based Rendering (PBR)
- Blinn Phong Lighting
- Multiple Lights
- Normal mapping
- ImGui Integration

## How to Build

```sh
cmake -B build
cmake --build build -j($nproc)
```

## Libraries Used

- [glad](https://github.com/Dav1dde/glad)
- [stb](https://github.com/nothings/stb)
- [assimp](https://github.com/assimp/assimp)
- [GLFW](https://github.com/glfw/glfw/)
- [GLM](https://github.com/g-truc/glm)
- [ImGui](https://github.com/ocornut/imgui/)

The assets used belong to their respective owners. Licenses/Sources for the assets can be found in their respective directories in `resources/`.