// ******************************************************************************
// File         : MarioLayer.cpp
// Description  : Client (Mario) Main Layer to be added in iKan Application
// Project      : Mario
//
// Created by Ashish on 05/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "MarioLayer.h"

MarioLayer::PropertyFlag MarioLayer::s_PropFlag;

// ******************************************************************************
// MarioLayer Constructor
// ******************************************************************************
MarioLayer::MarioLayer()
: Layer("Mario"), m_EditorCamera(glm::radians(45.0f), 1800.0f/800.0f, 0.01f, 10000.0f)
{
    IK_INFO("{0} Layer created", GetName().c_str());

    m_ActiveScene = CreateRef<Scene>();

    // Frame buffer specifications
    Framebuffer::Specification specs;
    specs.Attachments = { Framebuffer::TextureSpecification::TextureFormat::RGBA8,
                          Framebuffer::TextureSpecification::TextureFormat::DEPTH24STENCIL8,
                          Framebuffer::TextureSpecification::TextureFormat::R32I };

    specs.Width  = Window::Property::DefaultWidth;
    specs.Height = Window::Property::DefaultHeight;

    // Creating instance for Frame buffer in viewport
    m_Viewport.FrameBuffer = Framebuffer::Create(specs);

    // Set the current Scene to scene hierarchy pannel
    m_SceneHierarchyPannel.SetContext(m_ActiveScene);
}

// ******************************************************************************
// MarioLayer Destructor
// ******************************************************************************
MarioLayer::~MarioLayer()
{
    IK_WARN("{0} Layer destroyed", GetName().c_str());
}

// ******************************************************************************
// Attach Scene Editor
// ******************************************************************************
void MarioLayer::OnAttach()
{
    IK_INFO("Attaching {0} Layer to Application", GetName().c_str());
}

// ******************************************************************************
// Dettach Scene Editor
// ******************************************************************************
void MarioLayer::OnDetach()
{
    IK_WARN("Detaching {0} Layer", GetName().c_str());
}

// ******************************************************************************
// Update Editor
// ******************************************************************************
void MarioLayer::OnUpdate(Timestep ts)
{
    // If resize the window call the update the Scene View port and Frame buffer should be resized
    if (Framebuffer::Specification spec = m_Viewport.FrameBuffer->GetSpecification();
        m_Viewport.Size.x > 0.0f && m_Viewport.Size.y > 0.0f && // zero sized framebuffer is invalid
        (spec.Width != m_Viewport.Size.x || spec.Height != m_Viewport.Size.y))
    {
        m_Viewport.FrameBuffer->Resize((uint32_t)m_Viewport.Size.x, (uint32_t)m_Viewport.Size.y);
        m_ActiveScene->OnViewportResize((uint32_t)m_Viewport.Size.x, (uint32_t)m_Viewport.Size.y);
        m_EditorCamera.SetViewportSize((uint32_t)m_Viewport.Size.x, (uint32_t)m_Viewport.Size.y);
    }

    RendererStatistics::Reset();
    m_EditorCamera.OnUpdate(ts);

    m_Viewport.FrameBuffer->Bind();
    {
        Renderer::Clear({ 0.1f, 0.1f, 0.1f, 1.0f });
        m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
        m_Viewport.UpdateMousePos();
    }
    m_Viewport.FrameBuffer->Unbind();
}

// ******************************************************************************
// Imgui Render for Scene Editor
// ******************************************************************************
void MarioLayer::OnImguiRender()
{
    ImGuiAPI::StartDcocking();

    ShowMenu();
    RendererStats();

    // Render Scene Hierarchy pannel in imgui
    if (s_PropFlag.IsSceneHeirarchypanel)
    {
        m_SceneHierarchyPannel.OnImguiender(&s_PropFlag.IsSceneHeirarchypanel);
    }

    // Viewport Update
    if (m_Viewport.Present)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport", &m_Viewport.Present);
        {
            m_Viewport.OnUpdate();
            m_Viewport.UpdateBounds();
        }

        ImGui::End(); // ImGui::Begin("Viewport");
        ImGui::PopStyleVar(); // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    }

    ImGuiAPI::EndDcocking();
}

// ******************************************************************************
// Scene Editor Events
// ******************************************************************************
void MarioLayer::OnEvent(Event& event)
{
    m_EditorCamera.OnEvent(event);
}

// ******************************************************************************
// Menu for Imgui View port
// ******************************************************************************
void MarioLayer::ShowMenu()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", "Cmd + Q"))
            {
                Application::Get().Close();
            }
            ImGui::EndMenu(); // ImGui::BeginMenu("File")
        } // if (ImGui::BeginMenuBar())

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Scene Heirarchy Panel", NULL, s_PropFlag.IsSceneHeirarchypanel))
            {
                s_PropFlag.IsSceneHeirarchypanel = !s_PropFlag.IsSceneHeirarchypanel;
            }

            if (ImGui::MenuItem("Frame Rate", NULL, s_PropFlag.IsFrameRate))
            {
                s_PropFlag.IsFrameRate = !s_PropFlag.IsFrameRate;
            }

            if (ImGui::MenuItem("Render Stats", NULL, s_PropFlag.IsRendererStats))
            {
                s_PropFlag.IsRendererStats = !s_PropFlag.IsRendererStats;
            }

            if (ImGui::MenuItem("Vendor Types", NULL, s_PropFlag.IsVendorType))
            {
                s_PropFlag.IsVendorType = !s_PropFlag.IsVendorType;
            }

            if (ImGui::MenuItem("Viewport", NULL, m_Viewport.Present))
            {
                m_Viewport.Present = !m_Viewport.Present;
            }

            ImGui::EndMenu(); // if (ImGui::BeginMenu("View"))
        }

        if (ImGui::BeginMenu("Properties"))
        {
            ImGui::EndMenu(); // ImGui::BeginMenu("Properties")
        } // if (ImGui::BeginMenu("Properties"))

        ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
    }
}

// ******************************************************************************
// Show the renderer stats
// ******************************************************************************
void MarioLayer::RendererStats()
{
    if (s_PropFlag.IsFrameRate)
    {
        ImGuiAPI::FrameRate(&s_PropFlag.IsFrameRate);
    }

    if (s_PropFlag.IsRendererStats)
    {
        ImGuiAPI::RendererStats(&s_PropFlag.IsRendererStats);
    }

    if (s_PropFlag.IsVendorType)
    {
        ImGuiAPI::RendererVersion(&s_PropFlag.IsVendorType);
    }
}
