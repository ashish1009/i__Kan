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
    };

}
