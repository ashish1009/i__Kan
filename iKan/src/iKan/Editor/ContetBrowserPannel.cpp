// ******************************************************************************
// File         : ContentBrowserPannel.h
// Description  : Class for having property pannel of each entity
// Project      : iKan : Editor
//
// Created by Ashish on 16/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "ContetBrowserPannel.h"

namespace iKan {

    static std::filesystem::path s_AssetsPath = "../../../Mario/assets";

    // ******************************************************************************
    // ContentBrowserPannel constructor
    // ******************************************************************************
    ContentBrowserPannel::ContentBrowserPannel()
    : m_CurrentDir(s_AssetsPath)
    {
        
    }

    // ******************************************************************************
    // ContentBrowserPannel imgui renderer
    // ******************************************************************************
    void ContentBrowserPannel::OnImguiender(bool* pIsOpen)
    { 
        // no rendering if flag is false
        if (!IsContentBrowserPannel)
            return;

        ImGui::Begin("Content Browser ", pIsOpen);
        {
            if (m_CurrentDir != std::filesystem::path(s_AssetsPath))
            {
                if (ImGui::Button("<"))
                {
                    m_CurrentDir = m_CurrentDir.parent_path();
                }
            }

            for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDir))
            {
                const auto& path            = directoryEntry.path();
                auto relativePath           = std::filesystem::relative(path, s_AssetsPath);
                std::string filenameString  = relativePath.filename().string();

                if (directoryEntry.is_directory())
                {
                    if (ImGui::Button(filenameString.c_str()))
                    {
                        m_CurrentDir /= path.filename();
                    }
                }
                else
                {
                    if (ImGui::Button(filenameString.c_str()))
                    {

                    }
                }
            }
        }
        ImGui::End(); // ImGui::Begin("Scene Hierarchy", pIsOpen);
    }


}
