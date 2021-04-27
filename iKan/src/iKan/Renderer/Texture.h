// ******************************************************************************
// File         : Texture.h
// Project      : i-Kan : Renderer
// Description  : Interface for Texture class
//
// Created on   : 27/04/21.
// ******************************************************************************

#pragma once

namespace iKan {
    
    // ******************************************************************************
    // Interface for storing Texture
    // ******************************************************************************
    class Texture
    {
    public:
        virtual ~Texture() = default;
        
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void Unbind() const = 0;
        
        virtual uint32_t GetRendererID() const = 0;
        
        virtual std::string GetfilePath() const = 0;
        
        virtual bool operator==(const Texture& other) const = 0;
        
        static Ref<Texture> Create(const std::string& path);
        static Ref<Texture> Create(uint32_t width, uint32_t height, void* data, uint32_t size);
    };
    
    // ******************************************************************************
    // Implementation for storung Subtexture for tiles
    // ******************************************************************************
    class SubTexture
    {
    public:
        SubTexture(const Ref<Texture>& texture, const glm::vec2& min, const glm::vec2& max, const glm::vec2& spriteSize = {1.0f, 1.0f}, const glm::vec2& cellSize = {16.0f, 16.0f}, const glm::vec2& coords = {0.0f, 0.0f});
        
        const Ref<Texture> GetTeture() const { return m_Texture; }
        const glm::vec2* GetTexCoord() const { return m_TexCoord; }
        const glm::vec2& GetSpriteSize() const { return m_SpriteSize; }
        const glm::vec2& GetCellSize() const { return m_CellSize; }
        const glm::vec2& GetCoords() const { return m_Coords; }
        
        // By Default Tile size is 16 x 16
        static Ref<SubTexture> CreateFromCoords(const Ref<Texture>& texture, const glm::vec2& coords, const glm::vec2& spriteSize = {1.0f, 1.0f}, const glm::vec2& cellSize = {16.0f, 16.0f});
        
    private:
        Ref<Texture> m_Texture;
        glm::vec2 m_TexCoord[4];
        glm::vec2 m_SpriteSize;
        glm::vec2 m_CellSize;
        glm::vec2 m_Coords;
    };

    // ******************************************************************************
    // Interface for storing cubemaps
    // ******************************************************************************
    class CubeMapTexture
    {
    public:
        virtual ~CubeMapTexture() = default;
        virtual void Bind(uint32_t slot = 0) const = 0;
        
        static Ref<CubeMapTexture> Create(std::vector<std::string> paths);
        static Ref<CubeMapTexture> Create(const std::string& paths);
        
    private:
        uint32_t m_RendererId;
    };
}
