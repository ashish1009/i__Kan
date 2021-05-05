// ******************************************************************************
// File         : Camera.h
// Description  : Base camera class
// Project      : iKan : Renderer
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan {
    
    class Camera
    {
    public:
        Camera() = default;
        Camera(const glm::mat4& projection)
        : m_Projection(projection)
        {
            
        }
        virtual ~Camera() = default;
        const glm::mat4& GetProjection() const { return m_Projection; }
        // TODO: add API like SetPerspective, Set Ortho ......
        
    protected:
        glm::mat4 m_Projection = glm::mat4(1.0f);
    };
    
}
