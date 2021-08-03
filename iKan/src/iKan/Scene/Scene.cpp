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

#include <glad/glad.h>

namespace iKan {
    
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
        Ref<Texture> texture;
        if (m_Data.TextureMap.find(texturePath) != m_Data.TextureMap.end())
            texture = m_Data.TextureMap[texturePath];
        else
            texture = Texture::Create(texturePath);

        m_Data.TextureMap[texturePath] = texture;
        return texture;
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
        if (m_Data.EditorCamera && m_Data.EditorCamera->IsImguiPannel)
            m_Data.EditorCamera->OnImguiRenderer();
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
        m_Registry.destroy(entity);
    }

    // ******************************************************************************
    // Get the pixel from scene
    // mx -> Mouse position X
    // my -> Mouse position Y
    // ******************************************************************************
    int32_t Scene::GetEntityIdFromPixels(int32_t mx, int32_t my)
    {
        // TODO: move this to Open GL FIles
        glReadBuffer(GL_COLOR_ATTACHMENT1);

        int32_t pixelData = -1;
        glReadPixels(mx, my, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

        return pixelData;
    }

    // ******************************************************************************
    // Handler events of Scene
    // ******************************************************************************
    void Scene::OnEvent(Event& event)
    {
        if (m_Data.EditorCamera)
            m_Data.EditorCamera->OnEvent(event);
    }

    // ******************************************************************************
    // Update Scene. Editor camera helps to  Edit the Scene
    // ******************************************************************************
    void Scene::OnUpdateEditor(Timestep ts)
    {
        if (m_Data.EditorCamera)
        {
            m_Data.EditorCamera->OnUpdate(ts);

            SceneRenderer::BeginScene(*m_Data.EditorCamera.get());
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
                SceneRenderer::DrawQuad(transform.GetTransform(), sprite.Texture.Component, (int32_t)entity, sprite.TilingFactor, sprite.ColorComp);
            else if (sprite.SubTexComp)
                SceneRenderer::DrawQuad(transform.GetTransform(), sprite.SubTexComp, (int32_t)entity, sprite.TilingFactor, sprite.ColorComp);
            else
                SceneRenderer::DrawQuad(transform.GetTransform(), sprite.ColorComp, (int32_t)entity);
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
    // Resize scene view port
    // ******************************************************************************
    int32_t Scene::OnBoxColloider(Entity& currEntity, float speed)
    {
        // Store the side of collisions as bit mask (check BoxCollisionSide enum for bit representation)
        int32_t result = 0;

        // Current (Moving) Entity Property (Position and Size)
        const auto& ceTc   = currEntity.GetComponent<TransformComponent>();
        const auto& cePos  = ceTc.Translation;
        const auto& ceSize = ceTc.Scale;

        // Traverse entire Entities to get Box colloider entity one by one
        auto view = m_Registry.view<BoxCollider2DComponent>();
        for (auto entity : view)
        {
            if (currEntity == entity)
            {
                // no operation for same enitity
                continue;
            }

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
                    {
                        result |= ((speed > 0) ? (int32_t)BoxCollisionSide::Right : (int32_t)BoxCollisionSide::Left);
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
                        result |= ((speed > 0) ? (int32_t)BoxCollisionSide::Up : (int32_t)BoxCollisionSide::Down);
                    }
                }
            }
        }
        return result;
    }

}
