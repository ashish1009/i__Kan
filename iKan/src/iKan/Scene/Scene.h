// ******************************************************************************
// File         : Scene.h
// Project      : iKan : Scene
// Description  : Class tp store and manage Scene
//
// Created on   : 28/04/21.
// ******************************************************************************

#pragma once

#include <entt.hpp>
#include <iKan/Core/UUID.h>

namespace iKan {
    
    class Entity;
    class Scene
    {
    public:
        Scene();
        ~Scene();
        
        Entity CreateEntity(const std::string& name = "Unknown Entity", UUID uuid = UUID());
        void DestroyEntity(Entity entity);
        
    private:
        // Container that contain all the entities
        entt::registry m_Registry;
        
        // Store the map of Entityes present in the Scene with their UUID
        std::unordered_map<UUID, Entity> m_EntityIDMap;
        
        friend class Entity;
    };
    
}
