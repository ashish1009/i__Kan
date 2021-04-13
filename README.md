# iKan
___________________________________________________________________________________________
Game engine (Graphics Renderer Engine) in C++. 

![](/Resources/Branding/iKan.png)

iKan is primarily an early-stage interactive application and rendering engine for Mac

## Getting Started
___________________________________________________________________________________________
XCode is recommended

You can clone the repository to a local destination using git:

`git clone --recursive https://github.com/ashish1009/iKan`

Make sure that you do a `--recursive` clone to fetch all of the submodules!

## Features
___________________________________________________________________________________________

## Road Map
___________________________________________________________________________________________

## How to use Library
___________________________________________________________________________________________

### C++ Version
___________________________________________________________________________________________
Set the C++ version to C++17

### Precompiled Header file.
___________________________________________________________________________________________
Add path for Precompiled header file (Prefix Header File) 
path `${PROJECT_DIR}/../Editor/src/PrefixHeader.pch`

### Library
___________________________________________________________________________________________
Link the libiKan.a in your project

### Header Paths
___________________________________________________________________________________________
${PROJECT_DIR}/../iKan/src

### Application
___________________________________________________________________________________________
User defined Application need to be created at Client side which should be derived from `iKan::Application`.  In this application we can Push the user defined layer in the current application. Layers can be pushed in the application using shared pointer. `PushLayer(Ref<Layer>)` accepts `Ref<>` which is an alliase for `std::shared_ptr`. Multiple layers can be added in the application.  

### Layer
___________________________________________________________________________________________
User defined layer can be defined in Client. This should be derived from `iKan::Layer`. 
