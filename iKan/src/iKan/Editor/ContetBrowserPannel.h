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
        std::filesystem::path m_CurrentDir;

        Ref<Texture> m_FolderIcon = Texture::Create("../../../Editor/assets/resources/ContentBrowser/Folder.png");
        Ref<Texture> m_FileIcon   = Texture::Create("../../../Editor/assets/resources/ContentBrowser/File.png");

    };

}
