// ******************************************************************************
// File         : SceneCamera.h
// Project      : iKan : Scene
// Description  : Class for scene camera API
//
// Created on   : 01/05/21.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/Camera.h>

namespace iKan {
    
    // ******************************************************************************
    // Scene Camera
    // ******************************************************************************
    class SceneCamera : public Camera
    {
    public:
        enum class ProjectionType { Perspective = 0, Orthographic = 1};
        
    public:
        SceneCamera();
        virtual ~SceneCamera() = default;
        
        void SetOrthographic(float size, float nearClip, float farClip);
        void SetPerspective(float fov, float nearClip, float farClip);
        
        void SetViewportSize(uint32_t width, uint32_t height);
        
        float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
        float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
        float GetPerspectiveFarClip() const { return m_PerspectiveFar; }

        void SetPerspectiveFOV(float fov) { m_PerspectiveFOV = fov; RecalculateProjection(); }
        void SetPerspectiveNearClip(float near) { m_PerspectiveNear = near; RecalculateProjection(); }
        void SetPerspectiveFarClip(float far) { m_PerspectiveFar = far; RecalculateProjection(); }
        
        float GetOrthographicSize() const { return m_OrthographicSize; }
        float GetOrthographicNearClip() const { return m_OrthographicNear; }
        float GetOrthographicFarClip() const { return m_OrthographicFar; }

        void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
        void SetOrthographicNearClip(float near) { m_OrthographicNear = near; RecalculateProjection(); }
        void SetOrthographicFarClip(float far) { m_OrthographicFar = far; RecalculateProjection(); }
        
        ProjectionType GetProjectionType() const { return m_ProjectionType; }
        void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection();}
        
    private:
        void RecalculateProjection();
        
    private:
        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f;
        float m_OrthographicFar  = 1.0f;
        
        float m_PerspectiveFOV  = glm::radians(45.0f);
        float m_PerspectiveNear = 0.01f;
        float m_PerspectiveFar  = 1000.0f;
        
        float m_AspectRatio = 16.0f / 9.0f;
        
        ProjectionType m_ProjectionType = ProjectionType::Perspective;
    };
    
}
