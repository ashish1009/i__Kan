// ******************************************************************************
// File         : Camera.h
// Project      : iKan : Renderer
// Description  : Base camera class
//
// Created on   : 28/04/21.
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
