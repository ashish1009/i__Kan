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
        ContentBrowserPannel();

        void OnImguiender(bool* pIsOpen = nullptr);

    public:
        bool IsContentBrowserPannel = true;

    private:
        std::filesystem::path    m_CurrentDir;
        std::vector<std::string> m_PathHierarchy;

        Ref<Texture> m_FolderIcon = Texture::Create("../../../Editor/assets/resources/ContentBrowser/Folder.png");
        Ref<Texture> m_FileIcon   = Texture::Create("../../../Editor/assets/resources/ContentBrowser/File.png");
        Ref<Texture> m_JpgIcon    = Texture::Create("../../../Editor/assets/resources/ContentBrowser/Jpg.png");
        Ref<Texture> m_PngIcon    = Texture::Create("../../../Editor/assets/resources/ContentBrowser/Png.png");
        Ref<Texture> m_Cpp        = Texture::Create("../../../Editor/assets/resources/ContentBrowser/cpp.png");
        Ref<Texture> m_C          = Texture::Create("../../../Editor/assets/resources/ContentBrowser/c.png");
        Ref<Texture> m_H          = Texture::Create("../../../Editor/assets/resources/ContentBrowser/h.png");
    };

}
