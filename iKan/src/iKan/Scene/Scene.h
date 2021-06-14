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
            Up      = BIT(2),
            Down    = BIT(3)
        };

        struct Data
        {
            Data() = default;
            ~Data() = default;

            uint32_t NumEntities = 0;
            uint32_t ViewportWidth = 1280.0f, ViewportHeight = 720.0f;

            // vector of Textures that are present in the Scene
            // These textures might be used as Sprite as well
            std::unordered_map<std::string, Ref<Texture>> TextureMap;

            // Store the map of Entityes present in the Scene with their UUID
            std::unordered_map<UUID, Entity> EntityIDMap;

            // Stores the Editor Camera
            Ref<iKan::EditorCamera> EditorCamera;
        };

    public:
        Scene();
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

        int32_t GetEntityIdFromPixels(int32_t mx, int32_t my);
        int32_t OnBoxColloider(Entity& currEntity, float speed);
        uint32_t GetNumEntities() const { return m_Data.NumEntities; }

        Ref<EditorCamera> GetEditorCamera() { return m_Data.EditorCamera; }

        Data& GetDataRef() { return m_Data; }

        Ref<Texture> AddTextureToScene(const std::string& texturePath);

    private:
        Entity GetMainCameraEntity();
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
