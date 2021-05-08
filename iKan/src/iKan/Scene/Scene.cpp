// ******************************************************************************
// File         : Scene.cpp
// Description  : Class tp store and manage Scene
// Project      : iKan : Scene
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Scene.h"
#include <iKan/Scene/Entity.h>
#include <iKan/Scene/Component.h>
#include <iKan/Renderer/SceneRenderer.h>

namespace iKan {
    
    // ******************************************************************************
    // Scene Constructor
    // ******************************************************************************
    Scene::Scene()
    {
        IK_CORE_INFO("Creating Scene instance");
    }
    
    // ******************************************************************************
    // Scene Destructor
    // ******************************************************************************
    Scene::~Scene()
    {
        IK_CORE_WARN("Destroying Scene instance");
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
        entity.AddComponent<SceneHierarchyPannelProp>(true);

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

    // ******************************************************************************
    // Update Scene. Editor camera helps to  Edit the Scene
    // ******************************************************************************
    void Scene::OnUpdateEditor(Timestep ts, EditorCamera &editorCamera)
    {
        SceneRenderer::BeginScene(editorCamera);
        RenderSpriteComponent();
        SceneRenderer::EndScene();
    }

    // ******************************************************************************
    // Update runtime Scene
    // ******************************************************************************
    void Scene::OnUpdateRuntime(Timestep ts)
    {
        Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform;
        if (Entity cameraEntity = GetMainCameraEntity();
            cameraEntity!= Entity(entt::null, nullptr))
        {
            mainCamera      = &cameraEntity.GetComponent<CameraComponent>().Camera;
            cameraTransform = cameraEntity.GetComponent<TransformComponent>().GetTransform();

            SceneRenderer::BeginScene(*mainCamera, cameraTransform);
            RenderSpriteComponent();
            SceneRenderer::EndScene();
        }
        else
        {
            // TODO: Should it be assert or Warning
             IK_CORE_WARN("No Camera is Binded to the Scene or none of them is set to primary !!! ");
        }
    }

    // ******************************************************************************
    // Internal intermediate function to call renderer for each sub component of
    // sprite component
    // ******************************************************************************
    void Scene::RenderSpriteComponent()
    {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            const auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            if (sprite.TextureComp)
            {
                SceneRenderer::DrawQuad(transform.GetTransform(), sprite.TextureComp, (int32_t)entity, sprite.TilingFactor, sprite.ColorComp);
            }
            else if (sprite.SubTexComp)
            {
                SceneRenderer::DrawQuad(transform.GetTransform(), sprite.SubTexComp, (int32_t)entity, sprite.TilingFactor, sprite.ColorComp);
            }
            else
            {
                SceneRenderer::DrawQuad(transform.GetTransform(), sprite.ColorComp, (int32_t)entity);
            }
        }
    }

    // ******************************************************************************
    // get the canera component. First camera component which is found to be Primary
    // ******************************************************************************
    Entity Scene::GetMainCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& comp = view.get<CameraComponent>(entity);
            if (comp.Primary)
            {
                return { entity, this };
            }
        }
        return {};
    }
    
    // ******************************************************************************
    // Resize scene view port
    // ******************************************************************************
    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        IK_CORE_INFO("Scene Viewport resized to {0} x {1}", width, height);
        m_ViewportWidth  = width;
        m_ViewportHeight = height;
        
        // Resize our non-FixedAspectRatio cameras
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
                cameraComponent.Camera.SetViewportSize(width, height);
        }
    }
    
}
