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
#include <iKan/Scene/ScriptableEntity.h>
#include <iKan/Renderer/SceneRenderer.h>
#include <iKan/Renderer/Renderer2D.h>
#include <iKan/Renderer/Renderer.h>

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

#include <glad/glad.h>

namespace iKan {
    
    Scene::NativeData Scene::s_NativeData;
    
    // ******************************************************************************
    // body type Converstion
    // ******************************************************************************
    static b2BodyType RigidBody2DTypeToBox2D(RigidBody2DComponent::BodyType type)
    {
        switch (type)
        {
            case RigidBody2DComponent::BodyType::Static:  return b2BodyType::b2_staticBody;
            case RigidBody2DComponent::BodyType::Dynamic: return b2BodyType::b2_dynamicBody;
            case RigidBody2DComponent::BodyType::Kinematic: return b2BodyType::b2_kinematicBody;
        }
        IK_CORE_ASSERT(false, "Invalid");
        return b2BodyType::b2_staticBody;
    }

    // ******************************************************************************
    // Copy the component
    // ******************************************************************************
    template<typename Component>
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        auto view = src.view<Component>();
        for (auto e : view)
        {
            UUID uuid = src.get<IDComponent>(e).ID;
            IK_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(), "");
            entt::entity dstEnttID = enttMap.at(uuid);
            
            auto& component = src.get<Component>(e);
            dst.emplace_or_replace<Component>(dstEnttID, component);
        }
    }
    
    // ******************************************************************************
    // Copy the component If exist
    // ******************************************************************************
    template<typename Component>
    static void CopyComponentIfExist(Entity& dst, Entity& src)
    {
        if (src.HasComponent<Component>())
            dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
    }
    
    // ******************************************************************************
    // Copy the Scene Data
    // ******************************************************************************
    Ref<Scene> Scene::Copy(const Ref<Scene>& other)
    {
        IK_CORE_INFO("Copying Scene ...");
        
        Ref<Scene> newScene = CreateRef<Scene>();
        
        for (auto texMap : other->m_Data.TextureMap)
            newScene->m_Data.TextureMap[texMap.first] = texMap.second;
        
        std::unordered_map<UUID, entt::entity> enttMap;
        
        auto& srcSceneRegistry = other->m_Registry;
        auto& dstSceneRegistry = newScene->m_Registry;
        
        auto idView = srcSceneRegistry.view<IDComponent>();
        
        // Create Entities in new scene
        for (auto e : idView)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
            const std::string name = srcSceneRegistry.get<TagComponent>(e).Tag;
            
            Entity entity = newScene->CreateEntity(name, uuid);
            enttMap[uuid] = entity;
        }
        
        // Copy Components
        CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<SceneHierarchyPannelProp>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<AABBColloiderComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<RigidBody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<BoxColloider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        
        dstSceneRegistry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
                                                      {
            // nsc.Scripts is the Vector to store multiple Scripts for 1 entity
            for (auto script : nsc.Scripts)
                if (    script->m_Created)
                    script->m_Created = false;
        });


        return newScene;
    }
    
    // ******************************************************************************
    // Reset the static Native Data
    // ******************************************************************************
    void Scene::ResetNativeData()
    {
        s_NativeData.CameraWarning      = false;
        s_NativeData.SceneState         = NativeData::State::Edit;
        s_NativeData.SceneType          = NativeData::Type::Scene2D;
        s_NativeData.ViewportHeight     = 1280.0f;
        s_NativeData.ViewportWidth      = 720.0f;
    }
    
    // ******************************************************************************
    // Entity Collision callbacks ater collision
    // ******************************************************************************
    void Scene::CollisionCallbacks(Entity& colloidedEntity1, Entity& colloidedEntity2)
    {

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
        if (s_NativeData.EditorCamera)
        {
            s_NativeData.EditorCamera.reset();
            s_NativeData.EditorCamera = nullptr;
        }
    }

    // ******************************************************************************
    // Add the Editor camera to the scene
    // ******************************************************************************
    void Scene::SetEditorCamera(float fov, float aspectRatio, float near, float far)
    {
        if (!s_NativeData.EditorCamera)
            s_NativeData.EditorCamera = CreateRef<iKan::EditorCamera>(fov, aspectRatio, near, far);
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
        if (s_NativeData.CameraWarning)
        {
            ImGui::Begin("Scene Warning ");

            std::string warningMsg = ((s_NativeData.SceneState == NativeData::State::Edit) ? ( s_NativeData.SceneType == NativeData::Type::Scene3D ? "Editor Camera is not created." : "No Editor Camera Entity is present") : "Run-Time Primary Cmaera component is not present");
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), warningMsg.c_str());
            ImGui::End();
        }
        // Editor Camera Imgui Renderer
        if (s_NativeData.EditorCamera && s_NativeData.EditorCamera->IsImguiPannel && (s_NativeData.SceneState == NativeData::State::Edit))
            s_NativeData.EditorCamera->OnImguiRenderer();

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

        IK_CORE_ASSERT((m_Data.EntityIDMap.find(uuid) == m_Data.EntityIDMap.end()), "Entity Already Added");
        m_Data.EntityIDMap[uuid] = entity;
        
        IK_CORE_TRACE("Entity {0} with ID: {1} is created in the Active Scene", entity.GetComponent<TagComponent>().Tag.c_str(), entity.GetComponent<IDComponent>().ID);
        IK_CORE_TRACE("Number of entities Added in Scene : {0}, Maximum ID Given to Entity {0}", m_Data.NumEntities++, m_Data.MaxEntityId++);
        
        return entity;
    }
    
    // ******************************************************************************
    // Duplicate the entity
    // ******************************************************************************
    Entity Scene::DuplicateScene(Entity& entity)
    {
        Entity newEntity = CreateEntity(entity.GetName());

        IK_CORE_INFO("Duplicating the Entity with ID {0}, name : {1}", newEntity.GetUUID(), newEntity.GetName());

        // Copy Components
        CopyComponentIfExist<TransformComponent>(newEntity, entity);
        CopyComponentIfExist<CameraComponent>(newEntity, entity);
        CopyComponentIfExist<SpriteRendererComponent>(newEntity, entity);
        CopyComponentIfExist<SceneHierarchyPannelProp>(newEntity, entity);
        CopyComponentIfExist<AABBColloiderComponent>(newEntity, entity);
        CopyComponentIfExist<NativeScriptComponent>(newEntity, entity);
        CopyComponentIfExist<RigidBody2DComponent>(newEntity, entity);
        CopyComponentIfExist<BoxColloider2DComponent>(newEntity, entity);
        CopyComponentIfExist<CircleRendererComponent>(newEntity, entity);
        
        return newEntity;
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
        
        if (s_NativeData.EditorCamera)
            s_NativeData.EditorCamera->OnEvent(event);
    }

    // ******************************************************************************
    // Update Scene. Editor camera helps to  Edit the Scene
    // ******************************************************************************
    void Scene::OnUpdateEditor(Timestep ts)
    {
        InstantiateScripts(ts);
        
        if (s_NativeData.SceneType == Scene::NativeData::Type::Scene3D)
        {
            if (s_NativeData.EditorCamera)
            {
                s_NativeData.EditorCamera->OnUpdate(ts);

                const auto& camera = *s_NativeData.EditorCamera.get();

                SceneRenderer::BeginScene(this, { camera, camera.GetViewProjection() });
                RenderSpriteComponent();
                {
                    auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
                    for (auto entity : view)
                    {
                        const auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
                        Renderer2D::DrawCircle(transform.GetTransform(), circle.ColorComp, circle.Thickness, circle.Fade, (int)entity);
                    }
                }
                SceneRenderer::EndScene();

                s_NativeData.CameraWarning = false;
            }
            else
            {
                s_NativeData.CameraWarning = true;
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
                {
                    auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
                    for (auto entity : view)
                    {
                        const auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
                        Renderer2D::DrawCircle(transform.GetTransform(), circle.ColorComp, circle.Thickness, circle.Fade, (int)entity);
                    }
                }
                SceneRenderer::EndScene();
                
                s_NativeData.CameraWarning = false;
            }
            else
            {
                s_NativeData.CameraWarning = true;
            }

        }
    }

    // ******************************************************************************
    // Update runtime Scene
    // ******************************************************************************
    void Scene::OnUpdateRuntime(Timestep ts)
    {
        InstantiateScripts(ts);
        
        // Physics
        {
            const int32_t velocityIteration = 6;
            const int32_t positionIteration = 2;
            
            m_PhysicsWorld->Step(ts, velocityIteration, positionIteration);
            
            // Get Transform
            auto view = m_Registry.view<RigidBody2DComponent>();
            for (auto e : view)
            {
                Entity entity = { e, this };
                
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
                
                b2Body* body = (b2Body*)rb2d.RuntimeBody;
                if (body != nullptr)
                {
                    const auto& position = body->GetPosition();
                    
                    transform.Rotation.z = body->GetAngle();
                    transform.Translation.y = position.y;
                    
                    // If Entity hav Nativ Script then no need to update the x position,
                    // as it will be taken care in script
                    if (!entity.HasComponent<NativeScriptComponent>())
                        transform.Translation.x = position.x;
                }
            }
        }

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
            {
                auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
                for (auto entity : view)
                {
                    const auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
                    Renderer2D::DrawCircle(transform.GetTransform(), circle.ColorComp, circle.Thickness, circle.Fade, (int)entity);
                }
            }
            SceneRenderer::EndScene();

            s_NativeData.CameraWarning = false;
        }
        else
        {
            s_NativeData.CameraWarning = true;
        }
    }
    
    // ******************************************************************************
    // Runtime Starting the scene
    // ******************************************************************************
    void Scene::OnRuntimeStart()
    {
        s_NativeData.SceneState = NativeData::State::Play;
        
        m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
        auto view = m_Registry.view<RigidBody2DComponent>();
        for (auto e : view)
        {
            Entity entity = { e, this };
            auto& transform = entity.GetComponent<TransformComponent>();
            auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
            
            b2BodyDef bodyDef;
            bodyDef.type = RigidBody2DTypeToBox2D(rb2d.Type);
            bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
            bodyDef.angle = transform.Rotation.z;
            
            b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rb2d.FixedRotation);
            
            rb2d.RuntimeBody = body;
            
            if (entity.HasComponent<BoxColloider2DComponent>())
            {
                auto& bc2d = entity.GetComponent<BoxColloider2DComponent>();
                
                b2PolygonShape polygonShape;
                polygonShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);
                
                b2FixtureDef fixtureDef;
                fixtureDef.shape = & polygonShape;
                fixtureDef.density = bc2d.Density;
                fixtureDef.friction = bc2d.Friction;
                fixtureDef.restitution = bc2d.Restitution;
                fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
                
                body->CreateFixture(&fixtureDef);
            }
        }
    }

    // ******************************************************************************
    // Runtime Stoping the scene
    // ******************************************************************************
    void Scene::OnRuntimeStop()
    {
        s_NativeData.SceneState = NativeData::State::Edit;
        
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
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
        s_NativeData.ViewportWidth  = width;
        s_NativeData.ViewportHeight = height;
        
        // Resize our non-FixedAspectRatio cameras
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
                cameraComponent.Camera.SetViewportSize(width, height);
        }

        if (s_NativeData.EditorCamera)
            s_NativeData.EditorCamera->SetViewportSize(width, height);
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
                
                script->OnUpdate(ts);
            }
        });
    }
    
    // ******************************************************************************
    // Activate the Alive Entity
    // ******************************************************************************
    void Scene::OnActivateEntity(Entity& currEntity)
    {

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
        auto view = m_Registry.view<AABBColloiderComponent>();
        for (auto entity : view)
        {
            // no operation for same enitity
            if (currEntity == entity)
                continue;

            // Extract iKan Entity from entt::entity
            Entity e = { entity, this };

            // Coilloider entity (still or moving) property (Size and position)
            auto& transform = e.GetComponent<TransformComponent>();
            auto& boxColl   = e.GetComponent<AABBColloiderComponent>();

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
