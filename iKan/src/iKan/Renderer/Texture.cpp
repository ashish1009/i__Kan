// ******************************************************************************
// File         : Texture.cpp
// Project      : i-Kan : Renderer
// Description  : Instance creating for Texture class
//
// Created on   : 27/04/21.
// ******************************************************************************

#include <iKan/Renderer/Texture.h>
#include <iKan/Renderer/Renderer.h>
#include <iKan/Platform/OpenGL/OpenGLTexture.h>

namespace iKan {
    
    // ******************************************************************************
    // Creating Texture
    // ******************************************************************************
    Ref<Texture> Texture::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::None   : IK_CORE_ASSERT(false, "Render API not Supporting");
            case Renderer::API::OpenGL : return CreateRef<OpenGLTexture>(path); break;
        }
        IK_CORE_ASSERT(false, "Invalid Render API ");
        return nullptr;
    }
    
    // ******************************************************************************
    // Creating Texture
    // ******************************************************************************
    Ref<Texture> Texture::Create(uint32_t width, uint32_t height, void* data, uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::None   : IK_CORE_ASSERT(false, "Render API not Supporting");
            case Renderer::API::OpenGL : return CreateRef<OpenGLTexture>(width, height, data, size); break;
        }
        IK_CORE_ASSERT(false, "Invalid Render API ");
        return nullptr;
    }
    
    // ******************************************************************************
    // Creating SubTexture
    // ******************************************************************************
    SubTexture::SubTexture(const Ref<Texture>& texture, const glm::vec2& min, const glm::vec2& max, const glm::vec2& spriteSize, const glm::vec2& cellSize, const glm::vec2& coords)
    : m_Texture(texture), m_SpriteSize(spriteSize), m_CellSize(cellSize), m_Coords(coords)
    {
        m_TexCoord[0] = {min.x, min.y};
        m_TexCoord[1] = {max.x, min.y};
        m_TexCoord[2] = {max.x, max.y};
        m_TexCoord[3] = {min.x, max.y};
    }
    
    // ******************************************************************************
    // Creating SubTexture
    // ******************************************************************************
    Ref<SubTexture> SubTexture::CreateFromCoords(const Ref<Texture>& texture, const glm::vec2& coords, const glm::vec2& spriteSize, const glm::vec2& cellSize)
    {
        glm::vec2 min = { (coords.x * cellSize.x) / texture->GetWidth(), (coords.y * cellSize.y) / texture->GetHeight() };
        glm::vec2 max = { ((coords.x + spriteSize.x) * cellSize.x) / texture->GetWidth(), ((coords.y + spriteSize.y) * cellSize.y) / texture->GetHeight() };
        return CreateRef<SubTexture>(texture, min, max, spriteSize, cellSize, coords);
    }
    
    // ******************************************************************************
    // Creating Cubemap
    // ******************************************************************************
    Ref<CubeMapTexture> CubeMapTexture::Create(std::vector<std::string> paths)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::None   : IK_CORE_ASSERT(false, "Render API not Supporting");
            case Renderer::API::OpenGL : return CreateRef<OpenGLCubeMapTexture>(paths); break;
        }
        IK_CORE_ASSERT(false, "Invalid Render API ");
        return nullptr;
    }
    
    // ******************************************************************************
    // Creating Cubemap
    // ******************************************************************************
    Ref<CubeMapTexture> CubeMapTexture::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::None   : IK_CORE_ASSERT(false, "Render API not Supporting");
            case Renderer::API::OpenGL : return CreateRef<OpenGLCubeMapTexture>(path); break;
        }
        IK_CORE_ASSERT(false, "Invalid Render API ");
        return nullptr;
    }
    
}
