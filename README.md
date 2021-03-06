# iKan
___________________________________________________________________________________________
Game engine (Graphics Renderer Engine) in C++. 

![](/Resources/Branding/iKan.png)

iKan is primarily an early-stage interactive application and rendering engine for Mac

## Getting Started
XCode is recommended

You can clone the repository to a local destination using git:

`git clone --recursive https://github.com/ashish1009/iKan`

Make sure that you do a `--recursive` clone to fetch all of the submodules!

## Features
1. SPD Logger is added. iKan::Log class can be used to initialise the log.
2. Add core application and layer class (To Grow with time)
3. GLFW Window handling is done
4. Add Imgui Functionality for having a GUI to control the window and rendering (GUI grow with time)
5. Renderer Buffers and Array (Open GL)
6. Add the Framebuffers
7. Handling of Adding Texture files (Upload the texture to renderer)
8. Add Shader source code and its compiler
9. Create Shader library to store multiple shaders
10. 2D Renderere API
11. Entity COmponent System
12. Content browser pannel
13. UI Editor to that shoes viewport information.
14. New scene can be created in empty editor
15. Open Scene from already saved file
16.  Save the current scene to any folder

## Road Map
- Multi Thread
- Add Side menu for content browser pannel (partially developed)
- Add material class
- Add Light and multiple shaders
- Add Mesh and assimp library
- Add Guizmo


## How to use Library

### C++ Version
---------------------
Set the C++ version to C++17

### Precompiled Header file
-----------------------------------
Add path for Precompiled header file (Prefix Header File) 
path `${PROJECT_DIR}/../iKan/src/PrefixHeader.pch`

### Library
--------------
Link the libiKan.a in your project

### Frameworks
---------------------
Add the following frameworks
1. Cocoa.framework
2. OpenGL.framework
3. IOKit.framework

### Header Paths
-----------------------
${PROJECT_DIR}/../iKan/src
${PROJECT_DIR}/../iKan/vendor/spdlog/include
${PROJECT_DIR}/../iKan/vendor/glm
${PROJECT_DIR}/../iKan/vendor/imgui/Imgui
${PROJECT_DIR}/../iKan/vendor/EnTT/include

### Preprocessors
------------------------
Add the following preprocessors
1. `IK_ENABLE_LOG` to enable logging
2. `IK_ENABLE_ASSERT` to enable Trigger exception
3. `IK_PLATFORM_MAC` for MAC Platform

### Application
--------------------
User defined Application need to be created at Client side which should be derived from `iKan::Application`. Base application should be initilized with `iKan::Application::Property` wuth the following parameters.
1. string                             Title.
2. uint32_t                         Width.
3. uint32_t                         Height.
4. iKan::Renderer::API       Api.

In this application we can Push the user defined layer in the current application. Layers can be pushed in the application using shared pointer. `PushLayer(Ref<Layer>)` accepts `Ref<>` which is an alliase for `std::shared_ptr`. Multiple layers can be added in the application.  

### Layer
-------------
User defined layer can be defined in Client. This should be derived from `iKan::Layer`. 


### Bugs Identified (just to keep trac while developing)
1. File tile menu not jumping on any "iKan" folder


## Currunt Output
![](/Output/Output.png)
