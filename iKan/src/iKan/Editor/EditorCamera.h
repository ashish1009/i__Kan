// ******************************************************************************
// File         : EditorCamera.h
// Project      : iKan : Editor
// Description  : Implementation of Editor camera to support 3D view
//
// Created on   : 28/04/21.
// ******************************************************************************
#pragma once

#include <iKan/Renderer/Camera.h>

#include <iKan/Core/TimeStep.h>
#include <iKan/Core/Events/MouseEvent.h>

namespace iKan {
    
    // ******************************************************************************
    // Editr camera class. Msjor 3D functionality
    // ******************************************************************************
    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        virtual ~EditorCamera() = default;
        
        EditorCamera(float fov, float aspectRatio, float near, float far);
        
        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);
        
        float GetDistance() const { return m_Distance; }
        void SetDistance(float distance) { m_Distance = distance; }
        
        void SetViewportSize(uint32_t width, uint32_t height);
        
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::vec3& GetPosition() const { return m_Position; }

        glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }
        
        glm::vec3 GetUpDirection() const;
        glm::vec3 GetRightDirection() const;
        glm::vec3 GetForwardDirection() const;
        glm::quat GetOrientation() const;
        
        float GetExposure() const { return m_Exposure; }
        float& GetExposure() { return m_Exposure; }
        
        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }
        
    private:
        void UpdateProjectionMatrix() { m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar); } 
        void UpdateCameraView();
        
        bool OnMouseScroll(MouseScrolledEvent& e);
        
        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);
        
        glm::vec3 CalculatePosition() const;
        
        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;

    private:
        float m_PerspectiveFOV  = glm::radians(45.0f);
        float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;
        
        float m_AspectRatio = 0.0f;
        
        glm::mat4 m_ViewMatrix;
        glm::vec3 m_Position, m_Rotation, m_FocalPoint;
        
        bool m_Panning, m_Rotating;
        glm::vec2 m_InitialMousePosition;
        glm::vec3 m_InitialFocalPoint, m_InitialRotation;
        
        float m_Distance;
        float m_Pitch, m_Yaw;
        
        float m_Exposure = 0.8f;
        
        uint32_t m_ViewportWidth = 1280, m_ViewportHeight = 720;
    };
    
}
