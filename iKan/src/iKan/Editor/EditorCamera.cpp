// ******************************************************************************
// File         : EditorCamera.cpp
// Description  : Implementation of Editor camera to support 3D view
// Project      : iKan : Editor
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "EditorCamera.h"
#include <iKan/Core/Input.h>
#include <iKan/Editor/ScenePropertyGrid.h>

namespace iKan {
    
    // ******************************************************************************
    // Editor camera constructor
    // ******************************************************************************
    EditorCamera::EditorCamera(float fov, float aspectRatio, float near, float far)
    : m_PerspectiveFOV(fov), m_AspectRatio(aspectRatio), m_PerspectiveNear(near), m_PerspectiveFar(far),
    Camera(glm::perspective(glm::radians(fov), aspectRatio, near, far))
    {
        IK_CORE_INFO("Editor Camera created...");

        m_Rotation   = glm::vec3(90.0f, 0.0f, 0.0f);
        m_FocalPoint = glm::vec3(0.0f);
        
        glm::vec3 position = { -15, 5, 5};
        m_Distance = glm::distance(position, m_FocalPoint);
        
        m_Yaw = 3.0f * (float)M_PI / 4.0f;
        m_Pitch = M_PI / 4.0f;
        
        UpdateProjectionMatrix();
        UpdateCameraView();
    }
    
    // ******************************************************************************
    // Set Camera view port
    // ******************************************************************************
    void EditorCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth  = width;
        m_ViewportHeight = height;
        m_AspectRatio    = (float)m_ViewportWidth / (float)m_ViewportHeight;
        UpdateProjectionMatrix();
    }
    
    // ******************************************************************************
    // Update Camera view
    // ******************************************************************************
    void EditorCamera::UpdateCameraView()
    {
        m_Position = CalculatePosition();
        glm::quat orientation = GetOrientation();
        
        m_Rotation   = glm::eulerAngles(orientation) * (180.0f / (float)M_PI);
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
        m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }
    
    // ******************************************************************************
    // get pan speed
    // ******************************************************************************
    std::pair<float, float> EditorCamera::PanSpeed() const
    {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;
        
        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;
        
        return { xFactor, yFactor };
    }

    // ******************************************************************************
    // get rotation speed
    // ******************************************************************************
    float EditorCamera::RotationSpeed() const
    {
        return 0.8f;
    }
    
    // ******************************************************************************
    // get Zoom speed
    // ******************************************************************************
    float EditorCamera::ZoomSpeed() const
    {
        float distance  = m_Distance * 0.2f;
        distance        = std::max(distance, 0.0f);
        
        float speed = distance * distance;
        speed       = std::min(speed, 100.0f);
        
        return speed;
    }
    
    // ******************************************************************************
    // Update camera each frame
    // ******************************************************************************
    void EditorCamera::OnUpdate(Timestep ts)
    {
        if (Input::IsKeyPressed(KeyCode::LeftAlt))
        {
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
            glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
            m_InitialMousePosition = mouse;
            
            if (Input::IsMouseButtonPressed(MouseCode::ButtonLeft))
                MouseRotate(delta);
            if (Input::IsMouseButtonPressed(MouseCode::ButtonMiddle))
                MousePan(delta);
            if (Input::IsMouseButtonPressed(MouseCode::ButtonRight))
                MouseZoom(delta.y);
        }
        
        UpdateCameraView();
    }

    // ******************************************************************************
    // Camera event
    // ******************************************************************************
    void EditorCamera::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(IK_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
    }
    
    // ******************************************************************************
    // Mousescroll event
    // ******************************************************************************
    bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
    {
        float delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        UpdateCameraView();
        return false;
    }
    
    // ******************************************************************************
    // Moue pan
    // ******************************************************************************
    void EditorCamera::MousePan(const glm::vec2& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
    }
    
    // ******************************************************************************
    // Mouse rorate
    // ******************************************************************************
    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw   += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }
    
    // ******************************************************************************
    // Mouse Zoom
    // ******************************************************************************
    void EditorCamera::MouseZoom(float delta)
    {
        m_Distance -= delta * ZoomSpeed();
        if (m_Distance < 1.0f)
        {
            m_FocalPoint += GetForwardDirection();
            m_Distance = 1.0f;
        }
    }

    // ******************************************************************************
    // Get Up Direction
    // ******************************************************************************
    glm::vec3 EditorCamera::GetUpDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    // ******************************************************************************
    // Get Right Direction
    // ******************************************************************************
    glm::vec3 EditorCamera::GetRightDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    
    // ******************************************************************************
    // Get Forward Direction
    // ******************************************************************************
    glm::vec3 EditorCamera::GetForwardDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    // ******************************************************************************
    // Calculate Position
    // ******************************************************************************
    glm::vec3 EditorCamera::CalculatePosition() const
    {
        return m_FocalPoint - GetForwardDirection() * m_Distance;
    }

    // ******************************************************************************
    // GetOrientation
    // ******************************************************************************
    glm::quat EditorCamera::GetOrientation() const
    {
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
    }

    // ******************************************************************************
    // Reender Viewport Imgui pannel, if flag is true then render
    // ******************************************************************************
    void EditorCamera::OnImguiRenderer()
    {
        ImGui::Begin("Editor Camera Properties", &IsImguiPannel);
        ImGui::PushID("Editor Camera Properties");

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 100);
        ImGui::Text("Distance");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 100);
        ImGui::Text("%f", m_Distance);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(0, 100);
        ImGui::Text("pitch");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 100);
        ImGui::Text("%f", m_Pitch);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(0, 100);
        ImGui::Text("Yaw");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 100);
        ImGui::Text("%f", m_Yaw);
        ImGui::NextColumn();

        ImGui::Separator();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 100);
        ImGui::Text("Near, Far");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 100);
        ImGui::Text("%d, %d", (int32_t)m_PerspectiveNear, (int32_t)m_PerspectiveFar);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 100);
        ImGui::Text("Aspect Ratio ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 100);
        ImGui::Text("%d : %d", m_ViewportWidth, m_ViewportHeight);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 100);
        ImGui::Text("Position ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 100);
        ImGui::Text("%d : %d : %d", (int32_t)m_Position.x, (int32_t)m_Position.y, (int32_t)m_Position.z);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 100);
        ImGui::Text("Rotation ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 100);
        ImGui::Text("%d : %d : %d", (uint32_t)m_Rotation.x, (uint32_t)m_Rotation.y, (uint32_t)m_Rotation.z);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 100);
        ImGui::Text("Focal Point ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 100);
        ImGui::Text("%d : %d : %d", (uint32_t)m_FocalPoint.x, (uint32_t)m_FocalPoint.y, (uint32_t)m_FocalPoint.z);
        ImGui::NextColumn();

        ImGui::Separator();

        PropertyGrid::Float("FOV", m_PerspectiveFOV, nullptr, 1.0f);

        ImGui::Columns(1);

        ImGui::PopID();
        ImGui::End();
    }


}
