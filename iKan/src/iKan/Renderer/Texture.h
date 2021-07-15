// ******************************************************************************
// File         : Texture.h
// Description  : Interface for Texture class
// Project      : i-Kan : Renderer
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
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

        virtual bool Uploaded() const = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void Unbind() const = 0;
        
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
        
        const Ref<Texture> GetTexture() const { return m_Texture; }
        const glm::vec2* GetTexCoord() const { return m_TexCoord; }

        void UpdateCoords(const glm::vec2& coords) { m_Coords = coords; UpdateTexcoords(); }
        void UpdateSpriteSize(const glm::vec2& spriteSize) { m_SpriteSize = spriteSize; }
        void UpdateCellSize(const glm::vec2& cellSize) { m_CellSize = cellSize; }

        glm::vec2& GetSpriteSize() { return m_SpriteSize; }
        glm::vec2& GetCellSize() { return m_CellSize; }
        glm::vec2& GetCoords() { return m_Coords; }

        // By Default Tile size is 16 x 16
        static Ref<SubTexture> CreateFromCoords(const Ref<Texture>& texture, const glm::vec2& coords, const glm::vec2& spriteSize = {1.0f, 1.0f}, const glm::vec2& cellSize = {16.0f, 16.0f});

    private:
        void UpdateTexcoords();

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
    class TextureCube
    {
    public:
        virtual ~TextureCube() = default;
        virtual void Bind(uint32_t slot = 0) const = 0;
        
        static Ref<TextureCube> Create(std::vector<std::string> paths);
        static Ref<TextureCube> Create(const std::string& paths);
        
    private:
        uint32_t m_RendererId;
    };
}
