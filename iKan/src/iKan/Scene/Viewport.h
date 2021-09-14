// ******************************************************************************
// File         : Viewport.h
// Description  : stores all the information of Viewport
// Project      : iKan
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/FrameBuffer.h>
#include <iKan/Scene/Entity.h>
#include <iKan/Scene/SceneSerializer.h>
#include <iKan/Core/TimeStep.h>
#include <iKan/Core/Events/KeyEvent.h>
#include <iKan/Core/Events/ApplicationEvent.h>
#include <iKan/Editor/SceneHierarchyPannel.h>
#include <iKan/Editor/ContetBrowserPannel.h>

namespace iKan {
    
    class Viewport
    {
    private:
        const Ref<Texture> m_PauseTexture   = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/Icons/Pause.png");
        const Ref<Texture> m_PlayeTexture   = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/Icons/Play.png");

    public:
        struct PropFlag
        {
            bool IsFrameRate     = true;
            bool IsRendererStats = true;
            bool IsVendorType    = true;

            // Flag for the presence of Viewport
            bool Present = true;

            // Flag for Imgui pannel
            bool IsImguiPannel = true;
        };

        struct ViewportData
        {
            // Viewport focused or hovered
            bool Focused = false, Hovered = false;

            // Dimentsion of Viewport. Height of viewport is including the Title Tab height too
            float Width  = 0.0f, Height = 0.0f;

            // Position of Mouse inside Viewport
            int32_t MousePosX = -1, MousePosY = -1;

            // Size of Viewport Height of this size is only view port Area
            glm::vec2 Size = { 0.0f, 0.0f };

            // position of Top left and Bottom right edges with respect to screen
            // 0 -> Top left, 1 -> bottom right
            glm::vec2 Bounds[2];

            // Viewport background color
            glm::vec4 BgColor = {0.1f, 0.1f, 0.1f, 1.0f};

            // Cursorn position of View port. Not the mouse pos
            ImVec2 CursorPos;

            // Frame buffer to render scene
            Ref<Framebuffer> FrameBuffer;

            // Entity and ID hovered by mouse position
            int32_t HoveredEntityID;
            Entity  HoveredEntity;

            // Selected entity in the viewport
            Entity SelectedEntity;
        };

    private:
        Viewport(const std::string& contentBrowserPath);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
        bool OnMouseWindowResize(WindowResizeEvent& e);
        bool OnKeyPressed(KeyPressedEvent& e);

        void OnUpdateImGui();
        void PropertyMenu();
        void ViewMenu();
        void RendererStats(Timestep ts);
        void RendereViewportProp();
        void UpdateHoveredEntity();
        void UpdateMousePos();
        void UpdateBounds();

    public:
        ~Viewport() = default;

        Viewport(const Viewport& other) = delete;
        Viewport(Viewport&& other) = delete;

        Viewport& operator =(const Viewport& other) = delete;
        Viewport& operator =(Viewport&& other) = delete;

        void OnUpdate(Timestep ts);
        void OnImguiRenderer(Timestep ts);
        void ShowMenu();
        void OnEvent(Event& event);

        void NewScene();
        void OpenScene(const std::string& path);
        void SaveScene();
        void SaveSceneAs();
        void CloseScene();

        ViewportData& GetDataRef() { return m_Data; }
        Ref<Scene> GetScene() { return m_ActiveScene; }

        // ******************************************************************************
        // Create singleton instance of Viewport
        // ******************************************************************************
        static Viewport& Create(const std::string& contentBrowserPath)
        {
            s_Instance = new Viewport(contentBrowserPath);
            return *s_Instance;
        }

        // ******************************************************************************
        // Return the cretaed instance of viewport
        // ******************************************************************************
        static Viewport& Get()
        {
            IK_CORE_ASSERT(s_Instance, "Instance not created Yet");
            return *s_Instance;
        }

    private:
        static Viewport* s_Instance;

        bool m_SaveFile   = false;
        bool m_SaveFileAs = false;

        Ref<Scene>                 m_ActiveScene;
        PropFlag                   m_Flags;
        ViewportData               m_Data;
        iKan::SceneHeirarchyPannel m_SceneHierarchyPannel;
        iKan::ContentBrowserPannel m_ContentBrowserPannel;
    };
    
}
