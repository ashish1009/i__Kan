// ******************************************************************************
// File         : Scene.h
// Project      : iKan : Scene
// Description  : Class tp store and manage Scene
//
//  Created by Ashish . on 28/04/21.
// ******************************************************************************

#pragma once

#include <entt.hpp>

namespace iKan {
    
    class Scene
    {
    public:
        Scene();
        ~Scene();
        
    private:
        /* Container that contain all the entities */
        entt::registry m_Registry;
    };
    
}
