// ******************************************************************************
// File         : Scene.cpp
// Project      : iKan : Scene
// Description  : Class tp store and manage Scene
//
// Created on   : 28/04/21.
// ******************************************************************************

#include "Scene.h"

namespace iKan {
    
    // ******************************************************************************
    // Scene Constructor
    // ******************************************************************************
    Scene::Scene()
    {
        IK_CORE_INFO("Creating Scene !!");
    }
    
    // ******************************************************************************
    // Scene Destructor
    // ******************************************************************************
    Scene::~Scene()
    {
        IK_CORE_WARN("Destroying Scene !!");
    }

}