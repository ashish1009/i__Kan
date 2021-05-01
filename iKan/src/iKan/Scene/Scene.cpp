// ******************************************************************************
// File         : Scene.cpp
// Project      : iKan : Scene
// Description  : Class tp store and manage Scene
//
// Created on   : 28/04/21.
// ******************************************************************************

#include "Scene.h"
#include <iKan/Scene/Entity.h>
#include <iKan/Scene/Component.h>

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
    
    // ******************************************************************************
    // Create Entity in Scene with UUID
    // ******************************************************************************
    Entity Scene::CreateEntity(const std::string& name, UUID uuid)
    {
        auto entity = Entity{ m_Registry.create(), this };
        
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<TagComponent>(name);
        entity.AddComponent<TransformComponent>();

        IK_CORE_ASSERT((m_EntityIDMap.find(uuid) == m_EntityIDMap.end()), "Entity Already Added");
        m_EntityIDMap[uuid] = entity;
        
        IK_CORE_TRACE("Entity {0} with ID: {1} is created in the Active Scene", entity.GetComponent<TagComponent>().Tag.c_str(), entity.GetComponent<IDComponent>().ID);
        
        return entity;
    }
    
    // ******************************************************************************
    // Destroy Entity from Scene
    // ******************************************************************************
    void Scene::DestroyEntity(Entity entity)
    {
        IK_CORE_WARN("Destrying Entity '{0}' with ID {0} from the scene", entity.GetComponent<TagComponent>().Tag.c_str(), entity.GetComponent<IDComponent>().ID);
        m_Registry.destroy(entity);
    }

}
