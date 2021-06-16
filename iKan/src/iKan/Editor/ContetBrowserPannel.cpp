// ******************************************************************************
// File         : ContentBrowserPannel.h
// Description  : Class for having property pannel of each entity
// Project      : iKan : Editor
//
// Created by Ashish on 16/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "ContetBrowserPannel.h"
#include <iKan/Editor/ScenePropertyGrid.h>

namespace iKan {

    static std::filesystem::path s_AssetsPath = "../../../../../../../ashish";

    // ******************************************************************************
    // ContentBrowserPannel constructor
    // ******************************************************************************
    ContentBrowserPannel::ContentBrowserPannel()
    : m_CurrentDir(s_AssetsPath)
    {
        m_PathHierarchy.emplace_back(m_CurrentDir.filename());
    }

    // ******************************************************************************
    // ContentBrowserPannel imgui renderer
    // ******************************************************************************
    void ContentBrowserPannel::OnImguiender(bool* pIsOpen)
    { 
        // no rendering if flag is false
        if (!IsContentBrowserPannel)
            return;

        ImGui::Begin("Content Browser ", pIsOpen, ImGuiWindowFlags_NoResize);
        {
            ImGui::BeginChild("##Content Browser", ImVec2(0, ImGui::GetWindowHeight() - 30.0f), false, ImGuiWindowFlags_HorizontalScrollbar);
            static ImVec2 initPos = ImGui::GetCursorPos();

            if (ImGui::ArrowButton("Back", ImGuiDir(0)))
            {
                if (m_CurrentDir != std::filesystem::path(s_AssetsPath))
                {
                    m_CurrentDir = m_CurrentDir.parent_path();
                    m_PathHierarchy.pop_back();
                }
            }
            ImGui::SameLine();
            static std::string search;
            PropertyGrid::String("", search, 30.0f, 200.0f, " Search ...", true);

            for (auto fileName : m_PathHierarchy)
            {
                ImGui::SameLine();
                ImGui::Text(" \\ %s", fileName.c_str());
            }

            ImGui::Separator();

            int32_t pushId = 0;
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
                    if (".png" == relativePath.extension())         iconTexture = m_PngIcon;
                    else if (".jpg" == relativePath.extension())    iconTexture = m_JpgIcon;
                    else if (".cpp" == relativePath.extension())    iconTexture = m_Cpp;
                    else if (".h" == relativePath.extension())      iconTexture = m_H;
                    else if (".c" == relativePath.extension())      iconTexture = m_C;
                    else                                            iconTexture = m_FileIcon;

                    isDirectory = false;
                }

                static float iconSize = 64.0f;
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((iconSize + 30.0f) * pushId), initPos.y + 30.0f));
                ImTextureID myTexId = (ImTextureID)((size_t)iconTexture->GetRendererID());
                ImGui::PushID(pushId);
                if (ImGui::ImageButton(myTexId, ImVec2(iconSize, iconSize), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)))
                {
                    if (isDirectory)
                    {
                        m_CurrentDir /= path.filename();
                        m_PathHierarchy.emplace_back(m_CurrentDir.filename());
                    }
                }

                static float wrapWidth = 90.0f;
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((iconSize + 30.0f) * pushId), initPos.y + iconSize + 10.0f + 30.0f));
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);
                ImGui::Text(filenameString.c_str(), wrapWidth);
                ImGui::PopTextWrapPos();

                ImGui::PopID();
                pushId++;
            }

            ImGui::EndChild();
        }
        ImGui::End(); // ImGui::Begin("Scene Hierarchy", pIsOpen);
    }


}
