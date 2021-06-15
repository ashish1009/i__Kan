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

    // Set the current Scene to scene hierarchy pannel
    m_Viewport.SceneHierarchyPannel.SetContext(m_ActiveScene);

    // Frame buffer specifications
    Framebuffer::Specification specs;
    specs.Attachments = { Framebuffer::TextureSpecification::TextureFormat::RGBA8,
        Framebuffer::TextureSpecification::TextureFormat::DEPTH24STENCIL8,
        Framebuffer::TextureSpecification::TextureFormat::R32I };
    
    // Creating instance for Frame buffer in viewport
    m_Viewport.Data.FrameBuffer = Framebuffer::Create(specs);

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
    m_Viewport.OnUpdate(m_ActiveScene, ts);
}

// ******************************************************************************
// Imgui Render for Scene Editor
// ******************************************************************************
void MarioLayer::OnImguiRender(Timestep ts)
{
    ImGuiAPI::StartDcocking();

    ImGui::ShowDemoWindow();

    // Viewport Imgui Renderer
    m_Viewport.OnImguiRenderer(ts);

    ImGuiAPI::EndDcocking();
}

// ******************************************************************************
// Scene Editor Events
// ******************************************************************************
void MarioLayer::OnEvent(Event& event)
{
    m_Viewport.OnEvent(event);
}
