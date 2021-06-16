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

    static std::filesystem::path s_AssetsPath = "../../../../../../../ashish";

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
            static ImVec2 initPos = ImGui::GetCursorPos();

            if (ImGui::ArrowButton("Back", ImGuiDir(0)))
            {
                if (m_CurrentDir != std::filesystem::path(s_AssetsPath))
                {
                    m_CurrentDir = m_CurrentDir.parent_path();
                }
            }

            int32_t i = 0;
            for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDir))
            {
                Ref<Texture> iconTexture;
                static bool isDirectory = false;

                const auto& path            = directoryEntry.path();
                auto relativePath           = std::filesystem::relative(path, s_AssetsPath);
                std::string filenameString  = relativePath.filename().string();

                if (directoryEntry.is_directory())
                {
                    iconTexture = m_FolderIcon;
                    isDirectory = true;
                }
                else
                {
                    iconTexture = m_FileIcon;
                    isDirectory = false;
                }

                static float iconSize = 64.0f;
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((iconSize + 30.0f) * i), initPos.y + 30.0f));
                ImTextureID myTexId = (ImTextureID)((size_t)iconTexture->GetRendererID());
                ImGui::PushID(i);
                if (ImGui::ImageButton(myTexId, ImVec2(iconSize, iconSize), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
                {
                    if (isDirectory)
                        m_CurrentDir /= path.filename();
                }

                static float wrapWidth = 90.0f;
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((iconSize + 30.0f) * i), initPos.y + iconSize + 10.0f + 30.0f));
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);
                ImGui::Text(filenameString.c_str(), wrapWidth);
                ImGui::PopTextWrapPos();

                ImGui::PopID();
                i++;
            }
        }
        ImGui::End(); // ImGui::Begin("Scene Hierarchy", pIsOpen);
    }


}
