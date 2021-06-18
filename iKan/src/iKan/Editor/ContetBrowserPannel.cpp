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

    // ******************************************************************************
    // ContentBrowserPannel constructor
    // ******************************************************************************
    ContentBrowserPannel::ContentBrowserPannel(const std::string& rootPath)
    : m_RootPath(rootPath), m_CurrentDir(rootPath)
    {
        m_PathHierarchy.emplace_back(m_CurrentDir);
    }

    // ******************************************************************************
    // ContentBrowserPannel imgui renderer
    // ******************************************************************************
    void ContentBrowserPannel::OnImguiender(bool* pIsOpen)
    { 
        // no rendering if flag is false
        if (!IsContentBrowserPannel)
            return;

        ImGui::Begin("Content Browser ", pIsOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

        TitleIcon();

        // Side Menue should be called before MainArea
        SideMenu();
        ImGui::SameLine();

        MainArea();

        ImGui::End(); // ImGui::Begin("Scene Hierarchy", pIsOpen);
    }

    // ******************************************************************************
    // Render the Title Icons for COntent browser
    // ******************************************************************************
    void ContentBrowserPannel::TitleIcon()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("Title Area", ImVec2(ImGui::GetWindowContentRegionWidth(), 40.0f), true, ImGuiWindowFlags_NoScrollbar);

        ImGui::Columns(3);
        ImGui::SetColumnWidth(0, 80);
        if (PropertyGrid::ImageButton("Back", m_Back->GetRendererID(), ImVec2(16.0f, 16.0f)))
        {
            if (m_CurrentDir != std::filesystem::path(m_RootPath))
            {
                if (m_PrevDir.back() == m_CurrentDir.parent_path())
                {
                    m_PathHierarchy.pop_back();
                }
                else
                {
                    for (auto prevIt = m_PrevDir.begin(); prevIt != m_PrevDir.end(); prevIt++)
                    {
                        if (*prevIt == m_CurrentDir)
                        {
                            m_PathHierarchy.insert(m_PathHierarchy.end(), ++prevIt, m_PrevDir.end());
                            break;
                        }
                    }
                }
                m_ForwardDir.emplace_back(m_CurrentDir);
                m_CurrentDir = m_PrevDir.back();
                m_PrevDir.pop_back();
            }
        }
        ImGui::SameLine();
        if (PropertyGrid::ImageButton("Forward", m_Forward->GetRendererID(), ImVec2(16.0f, 16.0f)))
        {
            if (!m_ForwardDir.empty())
            {
                m_PrevDir.emplace_back(m_CurrentDir);
                m_CurrentDir = m_ForwardDir.back();
                m_PathHierarchy.emplace_back(m_CurrentDir);
                m_ForwardDir.pop_back();
            }
        }

        ImGui::SameLine();
        ImGui::NextColumn();
        ImGui::SetColumnWidth(1, 200);

        m_Filter.Draw("", 150.0f);
        ImGui::SameLine();
        PropertyGrid::ImageButton("Search", m_Search->GetRendererID(), ImVec2(16.0f, 16.0f));

        ImGui::NextColumn();
        ImGui::SetColumnWidth(1, 200);
        size_t i = 0;
        for (auto path : m_PathHierarchy)
        {
            i++;
            ImGui::SameLine();
            ImGui::Text("\\");
            ImGui::SameLine();
            if (ImGui::Button(path.filename().c_str()))
            {
                if (path != m_CurrentDir)
                    m_PrevDir.emplace_back(m_CurrentDir);

                m_CurrentDir = path;
                m_PathHierarchy.erase(m_PathHierarchy.begin() + i, m_PathHierarchy.end());
            }
        }
        ImGui::NextColumn();
        ImGui::Columns(1);

        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    // ******************************************************************************
    // Render the Main for COntent browser
    // ******************************************************************************
    void ContentBrowserPannel::MainArea()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("MainArea", ImVec2(ImGui::GetWindowContentRegionWidth() * (1.0f - s_SideChildWidth), ImGui::GetWindowHeight() - s_WindowYOffset), true, ImGuiWindowFlags_HorizontalScrollbar);

        int32_t pushId = 0;
        static ImVec2 initPos = ImGui::GetCursorPos();
        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDir))
        {
            const auto& path            = directoryEntry.path();
            auto relativePath           = std::filesystem::relative(path, m_RootPath);
            std::string filenameString  = relativePath.filename().string();

            if (m_Filter.PassFilter(filenameString.c_str()))
            {
                Ref<Texture> iconTexture;
                static bool isDirectory = false;
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
                if (PropertyGrid::ImageButton(pushId, iconTexture->GetRendererID(), ImVec2(iconSize, iconSize)))
                {
                    if (isDirectory)
                    {
                        m_PrevDir.emplace_back(m_CurrentDir);
                        m_CurrentDir /= path.filename();
                        m_PathHierarchy.emplace_back(m_CurrentDir);
                    }
                }

                static float wrapWidth = 90.0f;
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ((iconSize + 30.0f) * pushId), initPos.y + iconSize + 10.0f + 30.0f));
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrapWidth);
                ImGui::Text(filenameString.c_str(), wrapWidth);
                ImGui::PopTextWrapPos();

                pushId++;
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    // ******************************************************************************
    // Render the Main for COntent browser
    // ******************************************************************************
    void ContentBrowserPannel::SideMenu()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::BeginChild("SideArea", ImVec2(ImGui::GetWindowContentRegionWidth() * s_SideChildWidth, ImGui::GetWindowHeight() - s_WindowYOffset), true, ImGuiWindowFlags_HorizontalScrollbar);

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDir))
        {
            const auto& path            = directoryEntry.path();
            auto relativePath           = std::filesystem::relative(path, m_RootPath);
            std::string filenameString  = relativePath.filename().string();

        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }


}
