# Rendering Engine

A rendering engine developed to explore computer graphics techniques and game engines. It uses DirectX 11 as its graphics API and currently supports Windows only.

---

## Features

- Phong lighting with specular highlights.
- Importing 3D models and dynamically deciding shaders based on model's attributes.
- Render queue system for ordering draw calls based on their priority.
- Node system for 3D models that allows shader attributes to be modified dynamically at runtime.

More rendering experiments and engine features will be added over time as the project evolves.

---

## Libraries

- [Dear ImGui](https://github.com/ocornut/imgui)   
  Used for building in-engine tools.

- [Assimp](https://github.com/assimp/assimp)  
  Used for importing 3D models and handling various asset formats.

---

## Build

### Requirements

- CMake
- Visual Studio 2022 

### Steps


- Clone the project
- Run the `build.bat` file
- Run `RenderingEngine.exe` located in `REngine/build/Debug`
