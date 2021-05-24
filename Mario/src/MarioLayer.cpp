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
    m_Viewport.FrameBuffer = Framebuffer::Create(specs);

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

    // Creating Temp Entity
    ent1 = m_ActiveScene->CreateEntity("Entity");
    ent1.GetComponent<TransformComponent>().Translation = glm::vec3(8.5, 0.0, 0.0);
    ent1.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f));
    ent1.AddComponent<BoxCollider2DComponent>();

    // Creating Temp Entity
    ent2 = m_ActiveScene->CreateEntity("Entity");
    ent2.GetComponent<TransformComponent>().Translation = glm::vec3(-0.5, 0.0, 0.0);
    ent2.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f));
    ent2.AddComponent<BoxCollider2DComponent>();
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
    }

    RendererStatistics::Reset();

    if (m_Viewport.SelectedEntity != Entity(entt::null, nullptr))
    {
        m_SceneHierarchyPannel.SetSelectedEntity(m_Viewport.SelectedEntity);
    }

    m_Viewport.FrameBuffer->Bind();
    {
        Renderer::Clear(Mario::Background::s_BgColor);
        m_ActiveScene->OnUpdateRuntime(ts);

        m_Viewport.OnUpdate(m_ActiveScene);
    }
    m_Viewport.FrameBuffer->Unbind();

    if (m_Viewport.Focused)
    {
        auto& pos = ent1.GetComponent<TransformComponent>().Translation;
        float speed = 3 * ts;
        if (Input::IsKeyPressed(KeyCode::Left))
        {
            if (1 != m_ActiveScene->OnBoxColloider(ent1, speed))
            {
                IK_CORE_INFO("Input : {0}", pos.x);
                pos.x -= speed;
            }
//            else
//                pos.x += speed;
        }
        if (Input::IsKeyPressed(KeyCode::Right))
        {
            if (1 != m_ActiveScene->OnBoxColloider(ent1, speed))
            pos.x += speed;
        }
        if (Input::IsKeyPressed(KeyCode::Up))
        {
            if (1 != m_ActiveScene->OnBoxColloider(ent1, speed))
            pos.y += speed;
        }
        if (Input::IsKeyPressed(KeyCode::Down))
        {
            if (1 != m_ActiveScene->OnBoxColloider(ent1, speed))
            pos.y -= speed;
        }
    }
}

// ******************************************************************************
// Imgui Render for Scene Editor
// ******************************************************************************
void MarioLayer::OnImguiRender(Timestep ts)
{
    ImGuiAPI::StartDcocking();

    ShowMenu();
    RendererStats(ts);

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
            m_Viewport.OnUpdateImGui();
        }

        ImGui::End(); // ImGui::Begin("Viewport");
        ImGui::PopStyleVar(); // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    }

    // Viewport Imgui Renderer
    if (m_Viewport.IsImguiPannel)
    {
        m_Viewport.OnImguiRenderer();
    }

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
            if (ImGui::MenuItem("Scene Heirarchy Panel", nullptr, s_PropFlag.IsSceneHeirarchypanel))
            {
                s_PropFlag.IsSceneHeirarchypanel = !s_PropFlag.IsSceneHeirarchypanel;
            }

            if (ImGui::MenuItem("Frame Rate", nullptr, s_PropFlag.IsFrameRate))
            {
                s_PropFlag.IsFrameRate = !s_PropFlag.IsFrameRate;
            }

            if (ImGui::MenuItem("Render Stats", nullptr, s_PropFlag.IsRendererStats))
            {
                s_PropFlag.IsRendererStats = !s_PropFlag.IsRendererStats;
            }

            if (ImGui::MenuItem("Vendor Types", nullptr, s_PropFlag.IsVendorType))
            {
                s_PropFlag.IsVendorType = !s_PropFlag.IsVendorType;
            }

            if (ImGui::MenuItem("Mario Settings", nullptr, s_PropFlag.IsSettings))
            {
                s_PropFlag.IsSettings = !s_PropFlag.IsSettings;
            }

            if (ImGui::BeginMenu("Viewport"))
            {
                if (ImGui::MenuItem("Present", nullptr, m_Viewport.Present))
                {
                    m_Viewport.Present = !m_Viewport.Present;
                }

                if (ImGui::MenuItem("Imgui", nullptr, m_Viewport.IsImguiPannel))
                {
                    m_Viewport.IsImguiPannel = !m_Viewport.IsImguiPannel;
                }
                ImGui::EndMenu();
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
void MarioLayer::RendererStats(Timestep ts)
{
    if (s_PropFlag.IsFrameRate)
    {
        ImGuiAPI::FrameRate(ts, &s_PropFlag.IsFrameRate);
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
