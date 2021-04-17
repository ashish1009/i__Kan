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

## Road Map

## How to use Library

### C++ Version
---------------------
Set the C++ version to C++17

### Precompiled Header file
-----------------------------------
Add path for Precompiled header file (Prefix Header File) 
path `${PROJECT_DIR}/../Editor/src/PrefixHeader.pch`

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
