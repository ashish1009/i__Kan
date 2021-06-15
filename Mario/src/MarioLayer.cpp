// ******************************************************************************
// File         : MarioLayer.cpp
// Description  : Client (Mario) Main Layer to be added in iKan Application
// Project      : Mario
//
// Created by Ashish on 05/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "MarioLayer.h"
#include "Background.h"

using namespace Mario;

MarioLayer::PropertyFlag MarioLayer::s_PropFlag;

// ******************************************************************************
// MarioLayer Constructor
// ******************************************************************************
MarioLayer::MarioLayer()
: Layer("Mario")
{
    IK_INFO("{0} Layer created", GetName().c_str());
}

// ******************************************************************************
// MarioLayer Destructor
// ******************************************************************************
MarioLayer::~MarioLayer()
{
    Player::Destroy();
    IK_WARN("{0} Layer destroyed", GetName().c_str());
}

// ******************************************************************************
// Attach Scene Editor
// ******************************************************************************
void MarioLayer::OnAttach()
{
    IK_INFO("Attaching {0} Layer to Application", GetName().c_str());

    m_ActiveScene = CreateRef<Scene>();

    // Frame buffer specifications
    Framebuffer::Specification specs;
    specs.Attachments = { Framebuffer::TextureSpecification::TextureFormat::RGBA8,
        Framebuffer::TextureSpecification::TextureFormat::DEPTH24STENCIL8,
        Framebuffer::TextureSpecification::TextureFormat::R32I };

    specs.Width  = Window::Property::DefaultWidth;
    specs.Height = Window::Property::DefaultHeight;

    // Creating instance for Frame buffer in viewport
    m_Viewport.Data.FrameBuffer = Framebuffer::Create(specs);

    // Set the current Scene to scene hierarchy pannel
    m_SceneHierarchyPannel.SetContext(m_ActiveScene);

    // Setup the Camera Entity
    m_CameraEntity        = m_ActiveScene->CreateEntity("Camera");
    auto& cameraComponent = m_CameraEntity.AddComponent<CameraComponent>();
    cameraComponent.Camera.SetProjectionType(SceneCamera::ProjectionType::Orthographic);
    cameraComponent.Camera.SetOrthographicSize(18.0f);

    auto& cameraPositionX = m_CameraEntity.GetComponent<TransformComponent>().Translation.x;
    cameraPositionX = 18.0f;

    // Creating Entities for background tiles
    Mario::Background::CreateEntities(m_ActiveScene);
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
    if (Framebuffer::Specification spec = m_Viewport.Data.FrameBuffer->GetSpecification();
        m_Viewport.Data.Size.x > 0.0f && m_Viewport.Data.Size.y > 0.0f && // zero sized framebuffer is invalid
        (spec.Width != m_Viewport.Data.Size.x || spec.Height != m_Viewport.Data.Size.y))
    {
        m_Viewport.Data.FrameBuffer->Resize((uint32_t)m_Viewport.Data.Size.x, (uint32_t)m_Viewport.Data.Size.y);
        m_ActiveScene->OnViewportResize((uint32_t)m_Viewport.Data.Size.x, (uint32_t)m_Viewport.Data.Size.y);
    }

    RendererStatistics::Reset();

    if (m_Viewport.Data.SelectedEntity != Entity(entt::null, nullptr))
    {
        m_SceneHierarchyPannel.SetSelectedEntity(m_Viewport.Data.SelectedEntity);
    }

    m_Viewport.Data.FrameBuffer->Bind();
    {
        Renderer::Clear(Mario::Background::s_BgColor);
        m_ActiveScene->OnUpdateRuntime(ts);

        m_Viewport.OnUpdate(m_ActiveScene);
    }
    m_Viewport.Data.FrameBuffer->Unbind();
}

// ******************************************************************************
// Imgui Render for Scene Editor
// ******************************************************************************
void MarioLayer::OnImguiRender(Timestep ts)
{
    ImGuiAPI::StartDcocking();

    ImGui::ShowDemoWindow();

    ShowMenu();
    
    // Render Scene Hierarchy pannel in imgui
    m_SceneHierarchyPannel.OnImguiender(&m_SceneHierarchyPannel.isSceneHeirarchypanel);

    // Viewport Imgui Renderer
    m_Viewport.OnImguiRenderer(ts);

    // Show mario Setting in Imgui
    if (MarioLayer::s_PropFlag.IsSettings)
    {
        ImGui::Begin("Setting", &MarioLayer::s_PropFlag.IsSettings);
        Mario::Background::ImGuiRenderer();
        ImGui::End();
    }

    ImGuiAPI::EndDcocking();
}

// ******************************************************************************
// Scene Editor Events
// ******************************************************************************
void MarioLayer::OnEvent(Event& event)
{
    m_Viewport.OnEvent(event);
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
            if (ImGui::MenuItem("Scene Heirarchy Panel", nullptr, m_SceneHierarchyPannel.isSceneHeirarchypanel))
            {
                m_SceneHierarchyPannel.isSceneHeirarchypanel = !m_SceneHierarchyPannel.isSceneHeirarchypanel;
            }

            if (ImGui::MenuItem("Mario Settings", nullptr, s_PropFlag.IsSettings))
            {
                s_PropFlag.IsSettings = !s_PropFlag.IsSettings;
            }
            m_Viewport.ViewMenu();

            ImGui::EndMenu(); // if (ImGui::BeginMenu("View"))
        }

        if (ImGui::BeginMenu("Properties"))
        {
            ImGui::EndMenu(); // ImGui::BeginMenu("Properties")
        } // if (ImGui::BeginMenu("Properties"))

        ImGui::EndMenuBar(); // ImGui::BeginMenuBar()
    }
}
