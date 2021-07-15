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
#include <iKan/Renderer/Material.h>

namespace iKan {
    
    class Entity;
    class SceneHeirarchyPannel;

    // ******************************************************************************
    // Scene Class
    // ******************************************************************************
    class Scene
    {
    public:

        // ******************************************************************************
        // Environmant data
        // ******************************************************************************
        struct Environment
        {
            std::string FilePath;
            Ref<TextureCube> RadianceMap;
            Ref<TextureCube> IrradianceMap;

            static Environment Load(const std::string& filepath);
        };

        // ******************************************************************************
        // Light Data
        // ******************************************************************************
        struct Light
        {
            glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
            glm::vec3 Radiance = { 0.0f, 0.0f, 0.0f };

            float Multiplier = 1.0f;
        };

        // ******************************************************************************
        // Collision side
        // ******************************************************************************
        enum class BoxCollisionSide
        {
            Right   = BIT(0),
            Left    = BIT(1),
            Up      = BIT(2),
            Down    = BIT(3)
        };

        // ******************************************************************************
        // Scene Data
        // ******************************************************************************
        struct Data
        {
            Data() = default;
            ~Data() = default;

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

            // Environment data
            Scene::Light                 Light;
            Scene::Environment           Environment;
            Ref<iKan::TextureCube>       SkyboxTexture;
            Ref<iKan::MaterialInstance>  SkyboxMaterial;
        };

    public:
        Scene(const std::string& path = "");
        ~Scene();

        void Init();

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

        const std::string& GetFileName() const { return m_Data.FileName; }
        const std::string& GetFilePath() const { return m_Data.FilePath; }

        void SetFilePath(const std::string& path);

        Ref<EditorCamera> GetEditorCamera() { return m_Data.EditorCamera; }

        Data& GetDataRef() { return m_Data; }

        Ref<Texture> AddTextureToScene(const std::string& texturePath);

        void SetEnvironment(const Environment& environment);
        void SetSkybox(const Ref<TextureCube>& skybox);

        const Environment& GetEnvironment() const { return m_Data.Environment; }

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
        friend class SceneRenderer;
    };
    
}
