// ******************************************************************************
// File         : ContentBrowserPannel.h
// Description  : Class for having property pannel of each entity
// Project      : iKan : Editor
//
// Created by Ashish on 16/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <filesystem>
#include <iKan/Renderer/Texture.h>

namespace iKan {

    // ******************************************************************************
    // Scene Hierarchy Pannel class
    // ******************************************************************************
    class ContentBrowserPannel
    {
    public:
        ContentBrowserPannel(const std::string& rootPath);

        void OnImguiender(bool* pIsOpen = nullptr);

    private:
        void TitleIcon();
        void MainArea();
        void SideMenu();

        void Back();
        void Forward();
        void Home();
        void PathHistory();
        void Search();

    public:
        bool IsContentBrowserPannel = true;

    private:
        static constexpr float s_SideChildWidth = 0.25;
        static constexpr float s_WindowYOffset  = 82.0f;

        std::filesystem::path m_CurrentDir;
        std::filesystem::path m_RootPath;

        std::vector<std::filesystem::path> m_PrevDir;
        std::vector<std::filesystem::path> m_ForwardDir;
        std::vector<std::filesystem::path> m_PathHierarchy;

        ImGuiTextFilter m_Filter;

        struct TileIconTexture
        {
            Ref<Texture> FolderIcon = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/Folder.png");
            Ref<Texture> FileIcon   = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/File.png");
            Ref<Texture> JpgIcon    = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/Jpg.png");
            Ref<Texture> PngIcon    = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/Png.png");

            Ref<Texture> Cpp        = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/cpp.png");
            Ref<Texture> C          = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/c.png");
            Ref<Texture> H          = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/h.png");

            Ref<Texture> Back       = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/Back.png");
            Ref<Texture> Forward    = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/Forward.png");
            Ref<Texture> Search     = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/Search.png");

            Ref<Texture> Home       = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/ContentBrowser/Home.png");
        };
        TileIconTexture m_TileIconTexture;
    };

}
