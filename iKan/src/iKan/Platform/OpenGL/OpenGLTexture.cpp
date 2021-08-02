// ******************************************************************************
// File         : OpenGLTexture.cpp
// Description  : Implementation of Open GL Texture
// Project      : iKan : Platform
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "OpenGLTexture.h"
#include <iKan/Renderer/Renderer.h>
#include <stb_image.h>

namespace iKan {
 
    // ******************************************************************************
    // Open GL Texture Constructor
    // ******************************************************************************
    OpenGLTexture::OpenGLTexture(const std::string& path)
    : m_Filepath(path)
    {
        IK_CORE_INFO("Creating Open GL Texture from file : {0}", path.c_str());

        stbi_set_flip_vertically_on_load(1);
        
        int32_t height, width, channel;
        
        stbi_uc* data = nullptr;
        data = stbi_load(path.c_str(), &width, &height, &channel, 0);
        if (!data)
            IK_CORE_CRITICAL("Failed to load stbi Image {0}", path.c_str());

        else
        {
            m_Uploaded = true;

            m_Width   = width;
            m_Height  = height;
            m_Channel = channel;

            if (4 == m_Channel)
            {
                m_InternalFormat = GL_RGBA8;
                m_DataFormat     = GL_RGBA;
            }
            else if (3 == channel)
            {
                m_InternalFormat = GL_RGB8;
                m_DataFormat     = GL_RGB;
            }
            else
            {
                IK_CORE_ASSERT(false, "Invalid Format ");
            }

            glGenTextures(1, &m_RendererId);
            glBindTexture(GL_TEXTURE_2D, m_RendererId);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);

            if (data)
                stbi_image_free(data);
        }
    }
    
    // ******************************************************************************
    // Open GL Texture Constructor with white texture
    // ******************************************************************************
    OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, void* data, uint32_t size)
    : m_Width(width), m_Height(height), m_InternalFormat(GL_RGBA8), m_DataFormat(GL_RGBA), m_Size(size), m_Data(data)
    {
        Renderer::Submit([this]()
                         {
            IK_CORE_INFO("Creating Open GL Texture with white data ");

            glGenTextures(1, &m_RendererId);
            glBindTexture(GL_TEXTURE_2D, m_RendererId);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            uint16_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
            IK_CORE_ASSERT((m_Size == m_Width * m_Height * bpp), "Data must be entire texture");
            glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, (stbi_uc*)m_Data);
        });
    }
    
    // ******************************************************************************
    // Open GL Destructor
    // ******************************************************************************
    OpenGLTexture::~OpenGLTexture()
    {
        IK_CORE_WARN("Destroying Open GL Texture");

        glDeleteTextures(1, &m_RendererId);
    }
    
    // ******************************************************************************
    // Bind Open GL Destructor
    // ******************************************************************************
    void OpenGLTexture::Bind(uint32_t slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererId);
    }
    
    // ******************************************************************************
    // Unbind Open GL Destructor
    // ******************************************************************************
    void OpenGLTexture::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // ******************************************************************************
    // Open GL Cubemap Constructor
    // ******************************************************************************
    OpenGLCubeMapTexture::OpenGLCubeMapTexture(std::vector<std::string> paths)
    {
        IK_CORE_INFO("Creating Open GL Cubemap from paths");

        stbi_set_flip_vertically_on_load(0);
        
        glGenTextures(1, &m_RendererId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);
        
        int32_t width, height, nrChannels;
        int32_t i = 0;
        for (auto path : paths)
        {
            IK_CORE_INFO("{0}", path.c_str());

            uint8_t *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
            GLenum internalFormat = GL_RGB8, dataFormat = GL_RGB;

            if (4 == nrChannels)
            {
                internalFormat = GL_RGBA8;
                dataFormat     = GL_RGBA;
            }
            else if (3 == nrChannels)
            {
                internalFormat = GL_RGB8;
                dataFormat     = GL_RGB;
            }
            else
            {
                IK_CORE_ASSERT(false, "Invalid Format ");
            }
            
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                IK_CORE_ERROR("Cubemap texture failed to load at path: {0}",  path);
                stbi_image_free(data);
            }
            i++;
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    
    // ******************************************************************************
    // Open GL Cubemap Constructor
    // ******************************************************************************
    OpenGLCubeMapTexture::OpenGLCubeMapTexture(const std::string& path)
    {
        IK_CORE_INFO("Constructing Open GL Cubemaps from path : {0}", path.c_str());

        int32_t width, height, channels;
        stbi_set_flip_vertically_on_load(false);
        uint8_t* imageData = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);
        
        if (!imageData)
        {
            IK_CORE_ERROR("Cubemap texture failed to load at path: {0}",  path);
            stbi_image_free(imageData);
        }
        
        uint32_t faceWidth  = width / 4;
        uint32_t faceHeight = height / 3;

        IK_CORE_ASSERT((faceWidth == faceHeight), "Non-square faces!");
        
        std::array<uint8_t*, 6> faces;
        for (size_t i = 0; i < faces.size(); i++)
        {
            faces[i] = new uint8_t[faceWidth * faceHeight * 3]; // 3 BPP
        }
        
        int32_t faceIndex = 0;
        
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t y = 0; y < faceHeight; y++)
            {
                size_t yOffset = y + faceHeight;
                for (size_t x = 0; x < faceWidth; x++)
                {
                    size_t xOffset = x + i * faceWidth;
                    faces[faceIndex][(x + y * faceWidth) * 3 + 0] = imageData[(xOffset + yOffset * width) * 3 + 0];
                    faces[faceIndex][(x + y * faceWidth) * 3 + 1] = imageData[(xOffset + yOffset * width) * 3 + 1];
                    faces[faceIndex][(x + y * faceWidth) * 3 + 2] = imageData[(xOffset + yOffset * width) * 3 + 2];
                }
            }
            faceIndex++;
        }
        
        for (size_t i = 0; i < 3; i++)
        {
            // Skip the middle one
            if (i == 1)
            {
                continue;
            }
            
            for (size_t y = 0; y < faceHeight; y++)
            {
                size_t yOffset = y + i * faceHeight;
                for (size_t x = 0; x < faceWidth; x++)
                {
                    size_t xOffset = x + faceWidth;
                    faces[faceIndex][(x + y * faceWidth) * 3 + 0] = imageData[(xOffset + yOffset * width) * 3 + 0];
                    faces[faceIndex][(x + y * faceWidth) * 3 + 1] = imageData[(xOffset + yOffset * width) * 3 + 1];
                    faces[faceIndex][(x + y * faceWidth) * 3 + 2] = imageData[(xOffset + yOffset * width) * 3 + 2];
                }
            }
            faceIndex++;
        }
        
        glGenTextures(1, &m_RendererId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);
            
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        auto format = GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[2]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[0]);
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[4]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[5]);
            
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[1]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[3]);
        
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        
        glBindTexture(GL_TEXTURE_2D, 0);
            
        for (size_t i = 0; i < faces.size(); i++)
        {
            delete[] faces[i];
        }
        
        stbi_image_free(imageData);
    }

    // ******************************************************************************
    // Open GL Cubemap Destructor
    // ******************************************************************************
    OpenGLCubeMapTexture::~OpenGLCubeMapTexture()
    {
        IK_CORE_WARN("Destroying OpenGL Cubemap");

        glDeleteTextures(1, &m_RendererId);
    }
    
    // ******************************************************************************
    // Bind Open GL Cubemap
    // ******************************************************************************
    void OpenGLCubeMapTexture::Bind(uint32_t slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererId);
    }


}
    
