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
#include <iKan/Scene/Viewport.h>
#include <iKan/Renderer/SceneRenderer.h>
#include <iKan/Renderer/Renderer2D.h>
#include <iKan/Renderer/Renderer.h>

#include <glad/glad.h>

namespace iKan {
    
    // ******************************************************************************
    // Entity Collision callbacks ater collision
    // ******************************************************************************
    void Scene::CollisionCallbacks(Entity& colloidedEntity1, Entity& colloidedEntity2)
    {
//        auto currEntityScripts = colloidedEntity1.GetComponent<NativeScriptComponent>().Scripts;
//        for (auto script : currEntityScripts)
//            script->OnCollision(colloidedEntity2);
//
//        auto colloidedEntityScripts = colloidedEntity2.GetComponent<NativeScriptComponent>().Scripts;
//        for (auto script : colloidedEntityScripts)
//            script->OnCollision(colloidedEntity1);
    }
    
    // ******************************************************************************
    // Scene Constructor
    // ******************************************************************************
    Scene::Scene(const std::string& path)
    {
        m_Data.FilePath = path;
        m_Data.FileName = Utils::GetNameFromFilePath(path);

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
    // Set the new scene path
    // ******************************************************************************
    void Scene::SetFilePath(const std::string& path)
    {
        m_Data.FilePath = path;
        m_Data.FileName = Utils::GetNameFromFilePath(m_Data.FilePath);
    }

    // ******************************************************************************
    // Add the texture to Texture map in Scene data. Also extract the name of texture
    // ******************************************************************************
    Ref<Texture> Scene::AddTextureToScene(const std::string& texturePath)
    {
        if (m_Data.TextureMap.find(texturePath) == m_Data.TextureMap.end())
            m_Data.TextureMap[texturePath] = Texture::Create(texturePath);;
        return m_Data.TextureMap[texturePath];
    }

    // ******************************************************************************
    // Delete the Editor camera from the scene
    // ******************************************************************************
    void Scene::DeleteEditorCamera()
    {
        if (m_Data.EditorCamera)
        {
            m_Data.EditorCamera.reset();
            m_Data.EditorCamera = nullptr;
        }
    }

    // ******************************************************************************
    // Add the Editor camera to the scene
    // ******************************************************************************
    void Scene::SetEditorCamera(float fov, float aspectRatio, float near, float far)
    {
        if (!m_Data.EditorCamera)
            m_Data.EditorCamera = CreateRef<iKan::EditorCamera>(fov, aspectRatio, near, far);
        else
            IK_CORE_WARN("Editor camera already added to Active Scene");
    }

    // ******************************************************************************
    // Imgui renderer for Scene
    // NOTE : Imgui::Begin and Imgui::End should be taken care at the place where it
    // is getting called
    // ******************************************************************************
    void Scene::OnImguiRenderer()
    {
        if (m_Data.CameraWarning)
        {
            ImGui::Begin("Scene Warning ");

            std::string warningMsg = ((m_Data.Editing) ? "Editor Camera is not created." : "Run-Time Primary Cmaera component is not present");
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), warningMsg.c_str());
            ImGui::End();
        }
        // Editor Camera Imgui Renderer
        if (m_Data.EditorCamera && m_Data.EditorCamera->IsImguiPannel && m_Data.Editing)
            m_Data.EditorCamera->OnImguiRenderer();

        // Rendere Entity Scrip ImuGui
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
                                                      {
            // nsc.Scripts is the Vector to store multiple Scripts for 1 entity
            for (auto script : nsc.Scripts)
            {
                script->ImguiRenderer();
            }
        });

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
        entity.AddComponent<BoxCollider2DComponent>(false);

        IK_CORE_ASSERT((m_Data.EntityIDMap.find(uuid) == m_Data.EntityIDMap.end()), "Entity Already Added");
        m_Data.EntityIDMap[uuid] = entity;
        
        IK_CORE_TRACE("Entity {0} with ID: {1} is created in the Active Scene", entity.GetComponent<TagComponent>().Tag.c_str(), entity.GetComponent<IDComponent>().ID);
        IK_CORE_TRACE("Number of entities Added in Scene : {0}", m_Data.NumEntities++);
        
        return entity;
    }
    
    // ******************************************************************************
    // Destroy Entity from Scene
    // ******************************************************************************
    void Scene::DestroyEntity(Entity entity)
    {
        IK_CORE_WARN("Destrying Entity '{0}' with ID {0} from the scene", entity.GetComponent<TagComponent>().Tag.c_str(), entity.GetComponent<IDComponent>().ID);
        IK_CORE_TRACE("Number of entities Left in Scene : {0}", m_Data.NumEntities--);

        m_Registry.destroy(entity);
    }

    // ******************************************************************************
    // Get the pixel from scene
    // mx -> Mouse position X
    // my -> Mouse position Y
    // pixeldata -> get the pixel value
    // ******************************************************************************
    void Scene::GetEntityIdFromPixels(int32_t mx, int32_t my, int32_t& pixelData)
    {
        Renderer::GetEntityIdFromPixels(mx, my, pixelData);
    }

    // ******************************************************************************
    // Handler events of Scene
    // ******************************************************************************
    void Scene::OnEvent(Event& event)
    {
        // Handlel Event for all Entity Scrip
        m_Registry.view<NativeScriptComponent>().each([=, &event](auto entity, auto& nsc)
                                                      {
            // nsc.Scripts is the Vector to store multiple Scripts for 1 entity
            for (auto script : nsc.Scripts)
            {
                script->OnEvent(event);
            }
        });
        
        if (m_Data.EditorCamera)
            m_Data.EditorCamera->OnEvent(event);
    }

    // ******************************************************************************
    // Update Scene. Editor camera helps to  Edit the Scene
    // ******************************************************************************
    void Scene::OnUpdateEditor(Timestep ts)
    {
        InstantiateScripts(ts);
        
        if (m_Data.SceneType == Scene::Data::Type::Scene3D)
        {
            if (m_Data.EditorCamera)
            {
                m_Data.EditorCamera->OnUpdate(ts);

                const auto camera = *m_Data.EditorCamera.get();

                SceneRenderer::BeginScene(this, { camera, camera.GetViewProjection() });
                RenderSpriteComponent();
                SceneRenderer::EndScene();

                m_Data.CameraWarning = false;
            }
            else
            {
                m_Data.CameraWarning = true;
            }
        }
        // TODO: Change logic in future
        else // Scene 2D
        {
            Camera* editorCamera = nullptr;
            glm::mat4 cameraTransform;
            if (Entity cameraEntity = GetEditorCameraEntity();
                cameraEntity!= Entity(entt::null, nullptr))
            {
                editorCamera    = &cameraEntity.GetComponent<CameraComponent>().Camera;
                cameraTransform = cameraEntity.GetComponent<TransformComponent>().GetTransform();
                
                glm::mat4 viewProj = editorCamera->GetProjection() * glm::inverse(cameraTransform);
                
                SceneRenderer::BeginScene(this, { *editorCamera, viewProj });
                RenderSpriteComponent();
                SceneRenderer::EndScene();
                
                m_Data.CameraWarning = false;
            }
            else
            {
                m_Data.CameraWarning = true;
            }

        }
    }

    // ******************************************************************************
    // Update runtime Scene
    // ******************************************************************************
    void Scene::OnUpdateRuntime(Timestep ts)
    {
        InstantiateScripts(ts);

        Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform;
        if (Entity cameraEntity = GetPrimaryCameraEntity();
            cameraEntity!= Entity(entt::null, nullptr))
        {
            mainCamera      = &cameraEntity.GetComponent<CameraComponent>().Camera;
            cameraTransform = cameraEntity.GetComponent<TransformComponent>().GetTransform();

            glm::mat4 viewProj = mainCamera->GetProjection() * glm::inverse(cameraTransform);

            SceneRenderer::BeginScene(this, { *mainCamera, viewProj });
            RenderSpriteComponent();
            SceneRenderer::EndScene();

            m_Data.CameraWarning = false;
        }
        else
        {
            m_Data.CameraWarning = true;
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
            if (sprite.Texture.Use)
                Renderer2D::DrawQuad(transform.GetTransform(), sprite.Texture.Component, (int32_t)entity, sprite.TilingFactor, sprite.ColorComp);
            else if (sprite.SubTexComp)
                Renderer2D::DrawQuad(transform.GetTransform(), sprite.SubTexComp, (int32_t)entity, sprite.TilingFactor, sprite.ColorComp);
            else
                Renderer2D::DrawQuad(transform.GetTransform(), sprite.ColorComp, (int32_t)entity);
        }
    }

    // ******************************************************************************
    // get the canera component. First camera component which is found to be Primary
    // ******************************************************************************
    Entity Scene::GetPrimaryCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& comp = view.get<CameraComponent>(entity);
            if (comp.GetPrimary())
                return { entity, this };
        }
        return {};
    }
    
    // ******************************************************************************
    // get the canera component. First camera component which is found to be Primary
    // ******************************************************************************
    Entity Scene::GetEditorCameraEntity()
    {
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& comp = view.get<CameraComponent>(entity);
            if (comp.GetEditor())
                return { entity, this };
        }
        return {};
    }
    
    // ******************************************************************************
    // Resize scene view port
    // ******************************************************************************
    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        IK_CORE_INFO("Scene Viewport resized to {0} x {1}", width, height);
        m_Data.ViewportWidth  = width;
        m_Data.ViewportHeight = height;
        
        // Resize our non-FixedAspectRatio cameras
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
                cameraComponent.Camera.SetViewportSize(width, height);
        }

        if (m_Data.EditorCamera)
            m_Data.EditorCamera->SetViewportSize(width, height);
    }
    
    // ******************************************************************************
    // Instantiate the native script component of each entity
    // ******************************************************************************
    void Scene::InstantiateScripts(Timestep ts)
    {
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
                                                      {
            // nsc.Scripts is the Vector to store multiple Scripts for 1 entity
            for (auto script : nsc.Scripts)
            {
                // If a script is not created before then create the script and update the function
                if (!script->m_Created)
                {
                    script->m_Entity = { entity, this };
                    script->m_Created = true;
                    script->OnCreate();
                }
                
                OnActivateEntity(script->m_Entity);
                
                if (script->m_Entity.template HasComponent<AliveComponent>() && script->m_Entity.template GetComponent<AliveComponent>().Activated)
                    script->OnUpdate(ts);
            }
        });
    }
    
    // ******************************************************************************
    // Activate the Alive Entity
    // ******************************************************************************
    void Scene::OnActivateEntity(Entity& currEntity)
    {
        if (!currEntity.HasComponent<AliveComponent>())
            return;
        
        // Traverse entire Entities to get Box colloider entity one by one
        auto view = m_Registry.view<AliveComponent>();
        for (auto entity : view)
        {
            // no operation for same enitity
            if (currEntity == entity)
                continue;
            
            // DO Operation and Activate the Entity
        }
    }

    // ******************************************************************************
    // Resize scene view port
    // currEntity : Movaing entity
    // speed : Amount of movemet in 1 frame along with direction
    // ******************************************************************************
    int32_t Scene::OnBoxColloider(Entity& currEntity, float speed)
    {
        // Store the side of collisions as bit mask (check BoxCollisionSide enum for bit representation)
        int32_t result = 0;

        // Current (Moving) Entity Property (Position and Size)
        const auto& ceTc   = currEntity.GetComponent<TransformComponent>();
        const auto& ceSize = glm::vec3(abs(ceTc.Scale.x), abs(ceTc.Scale.y), abs(ceTc.Scale.z));
        const auto& cePos  = ceTc.Translation - (ceSize - 1.0f) / 2.0f;

        // Traverse entire Entities to get Box colloider entity one by one
        auto view = m_Registry.view<BoxCollider2DComponent>();
        for (auto entity : view)
        {
            // no operation for same enitity
            if (currEntity == entity)
                continue;

            // Extract iKan Entity from entt::entity
            Entity e = { entity, this };

            // Coilloider entity (still or moving) property (Size and position)
            auto& transform = e.GetComponent<TransformComponent>();
            auto& boxColl   = e.GetComponent<BoxCollider2DComponent>();

            // If coilloider is rigid
            if (boxColl.IsRigid)
            {
                const auto& entSize = transform.Scale;

                // Modifying the position ot entity because, in case of Entity Size greater than 1
                // then its actual position would be in middle, but we need to assume it at the left
                // edge of Entity in both x and y axis, so we subtract the Half of the difference of
                // Entity size to 1 (1 because position of entity of size 1 is always at edge)
                const auto& entPos  = transform.Translation - (entSize - 1.0f) / 2.0f;

                // if aligned in same y - Axis
                if (cePos.y < entPos.y + entSize.y &&
                    cePos.y + ceSize.y > entPos.y)
                {
                    // If alligned in same x - A  xis
                    if (cePos.x + speed < entPos.x + entSize.x &&
                        cePos.x + speed + ceSize.x > entPos.x)
                        
                    // Collision Callbacks
                    {
                        result |= ((speed > 0) ? (int32_t)BoxCollisionSide::Right : (int32_t)BoxCollisionSide::Left);
                        Scene::CollisionCallbacks(currEntity, e);
                    }
                }

                // if aligned in same x - Axis
                if (cePos.x < entPos.x + entSize.x &&
                    cePos.x + ceSize.x > entPos.x)
                {
                    // if aligned in same y - Axis
                    if (cePos.y + speed < entPos.y + entSize.y &&
                        cePos.y + speed + ceSize.y > entPos.y)
                    {
                        result |= ((speed > 0) ? (int32_t)BoxCollisionSide::Top : (int32_t)BoxCollisionSide::Bottom);
                        Scene::CollisionCallbacks(currEntity, e);
                    }
                }
            }
        }
        return result;
    }

}
