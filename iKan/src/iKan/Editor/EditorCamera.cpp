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

}
