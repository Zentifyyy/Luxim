<img src="https://github.com/Zentifyyy/Luxim/blob/master/LuximApp/img/AppIcon.png?raw=true" width="48">

Luxim is the text editor made for ease of use. Add favourite files to your sidebar, re-order them for accessibility and open them with a click of a button. Luxim is made using the [Walnut Framework](https://github.com/StudioCherno/Walnut), making it simple, elegant and pretty performant.


<p align="middle">
    <img src="https://github.com/Zentifyyy/Luxim/blob/master/LuximPreview.png?raw=true" width="350">
    <img src="https://github.com/Zentifyyy/Luxim/blob/master/LuximPreview2.png?raw=true" width="350">
</p>

### Features
- File Opening, File Creation Editing and saving (Obviously).
- Native file dialogs with [libtinyfiledialogs](https://github.com/IamtheMZI/libtinyfiledialogs).
- Simple and elegant design.
- Tab / Window docking, made possible by [Dear ImGui](https://github.com/ocornut/imgui).
- Persistant sidebar with list of favourite files which you can personalise the order of.
- Custom titlebar with themed titlebar buttons, made using the [Walnut Framework](https://github.com/StudioCherno/Walnut).
- Now with preferences and automatic file saving.


## Getting Started
#### For compiled binaries check the [github releases page](https://github.com/Zentifyyy/Luxim/releases).

- Otherwise, make sure you have the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) and [CMake](https://cmake.org/download) installed.

- Some of these instructions are [Visual Studio 2022](https://visualstudio.microsoft.com) specific, which is recommended but not strictly necessary.

- Clone the repository.
```
git clone --recursive https://github.com/Zentifyyy/Luxim.git
```
- Then, run ```scripts/Setup-Luxim.bat``` to build the libtinyfiledialogs library and generate a Visual Studio solution file for the main project.

- Then open up the ```LuximApp.sln``` in the base directory. We will then need to add ```tinyfiledialogs``` as a dependency by right clicking on Dependencies in your solution explorer, navigating to add and pressing Existing Project... Then go to ```vendor/libtinyfiledialogs/build``` and add ```tinyfiledialogs.vcxproj```.

- And finally, navigate to the WalnutApp solution in Visual Studio and right click references and then add reference. Tick ```libtinyfiledialogs``` and you are good to go.

## Additional
- Luxim uses the [Walnut application framework](https://github.com/StudioCherno/Walnut) made by Studio Cherno, ([MIT License](https://github.com/Zentifyyy/Luxim/blob/master/LICENSE.txt)).

- Luxim uses the [Roboto](https://fonts.google.com/specimen/Roboto) Font, [(Apache License, Version 2.0)](https://www.apache.org/licenses/LICENSE-2.0).

### 3rd Party Libraries
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [stb_image](https://github.com/nothings/stb)
- [GLM](https://github.com/g-truc/glm)
- [libtinyfiledialogs](https://github.com/IamtheMZI/libtinyfiledialogs)