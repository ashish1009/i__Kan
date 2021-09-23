// ******************************************************************************
// File         : Scene.h
// Description  : Class tp store and manage Scene
// Project      : iKan : Scene
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <entt.hpp>
#include <iKan/Core/UUID.h>
#include <iKan/Core/TimeStep.h>
#include <iKan/Editor/EditorCamera.h>
#include <iKan/Renderer/Texture.h>

namespace iKan {
    
    class Entity;
    class SceneHeirarchyPannel;
    class Scene
    {
    public:
        enum class BoxCollisionSide
        {
            Right   = BIT(0),
            Left    = BIT(1),
            Top     = BIT(2),
            Bottom  = BIT(3)
        };

        struct Data
        {
            Data() = default;
            ~Data() = default;
            
            enum Type { Scene2D, Scene3D };
            Type SceneType = Scene2D;

            bool CameraWarning = false;

            // Flag to check is editor is under editing proces sor run time
            // if "false" then editor is run time state
            bool Editing = true;

            uint32_t NumEntities = 0;
            uint32_t ViewportWidth = 1280.0f, ViewportHeight = 720.0f;

            // Stores the file name and path
            std::string FileName, FilePath;

            // vector of Textures that are present in the Scene
            // These textures might be used as Sprite as well
            std::unordered_map<std::string, Ref<Texture>> TextureMap;

            // Store the map of Entityes present in the Scene with their UUID
            std::unordered_map<UUID, Entity> EntityIDMap;

            // Stores the Editor Camera
            Ref<iKan::EditorCamera> EditorCamera;
        };

    public:
        Scene(const std::string& path = "");
        ~Scene();
        
        Entity CreateEntity(const std::string& name = "Unknown Entity", UUID uuid = UUID());
        
        void DestroyEntity(Entity entity);
        void OnEvent(Event& event);
        void OnUpdateEditor(Timestep ts);
        void OnViewportResize(uint32_t width, uint32_t height);
        void OnUpdateRuntime(Timestep ts);
        void OnImguiRenderer();
        void DeleteEditorCamera();
        void SetEditorCamera(float fov = glm::radians(45.0f), float aspectRatio = 1800.0f/800.0f, float near = 0.01f, float far = 10000.0f);
        void GetEntityIdFromPixels(int32_t mx, int32_t my, int32_t& pixelData);
        
        uint32_t GetNumEntities() const { return m_Data.NumEntities; }

        const std::string& GetFileName() const { return m_Data.FileName; }
        const std::string& GetFilePath() const { return m_Data.FilePath; }

        void SetFilePath(const std::string& path);

        Data& GetDataRef() { return m_Data; }

        Ref<EditorCamera> GetEditorCamera() { return m_Data.EditorCamera; }
        Ref<Texture> AddTextureToScene(const std::string& texturePath);
        
        bool IsEditing() const { return m_Data.Editing; }
        void SetEditingFlag(bool flag) { m_Data.Editing = flag; }
        
        Data::Type GetSceneType() const { return m_Data.SceneType; }
        void SetSceneType(Data::Type type) { m_Data.SceneType = type; }
        
        int32_t OnBoxColloider(Entity& currEntity, float speed);
        void OnActivateEntity(Entity& currEntity);

        bool IsRightCollision(Entity& currEntity, float speed)
        {
            return (int32_t)Scene::BoxCollisionSide::Right & OnBoxColloider(currEntity, speed);
        }
        
        bool IsLeftCollision(Entity& currEntity, float speed)
        {
            return (int32_t)Scene::BoxCollisionSide::Left & OnBoxColloider(currEntity, -speed);
        }
        
        bool IsTopCollision(Entity& currEntity, float speed)
        {
            return (int32_t)Scene::BoxCollisionSide::Top & OnBoxColloider(currEntity, speed);
        }
        
        bool IsBottomCollision(Entity& currEntity, float speed)
        {
            return (int32_t)Scene::BoxCollisionSide::Bottom & OnBoxColloider(currEntity, -speed);
        }

        Entity GetMainCameraEntity();
        Entity GetEditorCameraEntity();
        
        static void CollisionCallbacks(Entity& colloidedEntity1, Entity& colloidedEntity2);
        
    private:
        void InstantiateScripts(Timestep ts);
        void RenderSpriteComponent();

    private:
        // Container that contain all the entities
        entt::registry m_Registry;

        // Instacne for Scene Data
        Data m_Data;
        
        friend class Entity;
        friend class SceneHeirarchyPannel;
        friend class SceneSerializer;
    };
    
}
