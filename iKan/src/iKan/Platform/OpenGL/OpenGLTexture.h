// ******************************************************************************
// File         : OpenGLTexture.h
// Project      : iKan : Platform
// Description  : Implementation of Open GL Texture
//
// Created on   : 27/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/Texture.h>
#include <glad/glad.h>

namespace iKan {
    
    // ******************************************************************************
    // Implementation for OpenGL Texture
    // ******************************************************************************
    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture(const std::string& path);
        OpenGLTexture(uint32_t width, uint32_t height, void* data, uint32_t size);
        
        virtual ~OpenGLTexture();
        
        virtual uint32_t GetWidth() const override { return m_Width;  }
        virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererId; }

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void Unbind() const override;

        virtual std::string GetfilePath() const override { return m_Filepath; }

        virtual bool operator==(const Texture& other) const override
        {
            return m_RendererId == ((OpenGLTexture&)other).m_RendererId;
        }
        
    private:
        uint32_t m_RendererId;
        uint32_t m_Width, m_Height, m_Channel;
        GLenum m_InternalFormat = GL_RGBA8, m_DataFormat = GL_RGBA;
        
        std::string m_Filepath;
    };
    
    // ******************************************************************************
    // Implementation for OpenGL Cubemap
    // ******************************************************************************
    class OpenGLCubeMapTexture : public CubeMapTexture
    {
    public:
        OpenGLCubeMapTexture(std::vector<std::string> paths);
        OpenGLCubeMapTexture(const std::string& path);
        
        virtual ~OpenGLCubeMapTexture();
        virtual void Bind(uint32_t slot = 0) const override;
        
    private:
        uint32_t m_RendererId;
    };
    
}
