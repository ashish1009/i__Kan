// ******************************************************************************
// File         : SceneCamera.cpp
// Description  : Class for scene camera API
// Project      : iKan : Scene
//
// Created by Ashish on 01/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include <iKan/Scene/SceneCamera.h>

namespace iKan {
    
    // ******************************************************************************
    // Constructor
    // ******************************************************************************
    SceneCamera::SceneCamera()
    {
        IK_CORE_INFO("Scene camera is constructed");
        RecalculateProjection();
    }
    
    // ******************************************************************************
    // Set Orthpgraphic
    // ******************************************************************************
    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_ProjectionType   = ProjectionType::Orthographic;
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar  = farClip;
        RecalculateProjection();
    }

    // ******************************************************************************
    // Set Perspective
    // ******************************************************************************
    void SceneCamera::SetPerspective(float fov, float nearClip, float farClip)
    {
        m_ProjectionType  = ProjectionType::Perspective;
        m_PerspectiveFOV  = fov;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar  = farClip;
        RecalculateProjection();
    }

    // ******************************************************************************
    // Set View port
    // ******************************************************************************
    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = (float)width / (float)height;
        RecalculateProjection();
    }

    // ******************************************************************************
    // Recalculate
    // ******************************************************************************
    void SceneCamera::RecalculateProjection()
    {
        if (m_ProjectionType == ProjectionType::Perspective)
        {
            m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
        }
        else if (m_ProjectionType == ProjectionType::Orthographic)
        {
            float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoBottom = -m_OrthographicSize * 0.5f;
            float orthoTop = m_OrthographicSize * 0.5f;
            
            m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
        }
    }
}
