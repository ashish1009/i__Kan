// ******************************************************************************
// File         : ImguiAPI.cpp
// Project      : iKan : Imgui
// Description  : Contains API that would be used for Imgui interface
//
// Created on   : 02/05/21.
// ******************************************************************************

#include "ImguiAPI.h"
#include <iKan/Renderer/RendererAPI.h>
#include <iKan/Renderer/RendererStats.h>

namespace iKan {
    
    // ******************************************************************************
    // Starts Window Docking Docking
    // ******************************************************************************
    void ImGuiAPI::StartDcocking()
    {
        // Note: Switch this to true to enable dockspace
        static bool               dockspaceOpen              = false;
        static bool               opt_fullscreen_persistant  = true;
        bool                      opt_fullscreen             = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags            = ImGuiDockNodeFlags_None;
        
        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;// | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        
        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();
        
        if (opt_fullscreen)
        {
            ImGui::PopStyleVar(2);
        }
        
        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
    }
    
    // ******************************************************************************
    // Ends window docking
    // ******************************************************************************
    void ImGuiAPI::EndDcocking()
    {
        ImGui::End();
    }
    
    // ******************************************************************************
    // Renderer Version
    // ******************************************************************************
    void ImGuiAPI::RendererVersion(bool *pIsOpen)
    {
        ImGui::Begin("Renderer Version", pIsOpen);
        auto& caps = RendererAPI::GetCapabilities();

        ImGui::PushID("Renderer Version");
        
        ImGui::Columns(3);

        ImGui::SetColumnWidth(0, 110);
        ImGui::Text("Vendor : %s", caps.Vendor.c_str());
        ImGui::NextColumn();
        
        ImGui::SetColumnWidth(1, 220);
        ImGui::Text("Renderer : %s", caps.Renderer.c_str());
        ImGui::NextColumn();

        ImGui::SetColumnWidth(2, 150);
        ImGui::Text("Version : %s", caps.Version.c_str());
        ImGui::NextColumn();

        ImGui::Columns(1);
        
        ImGui::PopID();
        
        ImGui::End();
    }
    
    // ******************************************************************************
    // Frame Rate
    // ******************************************************************************
    void ImGuiAPI::FrameRate(bool *pIsOpen)
    {
        ImGui::Begin("Frame Rate", pIsOpen);
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    
    // ******************************************************************************
    // Renderer Stats
    // ******************************************************************************
    void ImGuiAPI::RendererStats(bool *pIsOpen)
    {
        ImGui::Begin("Renderer Stats", pIsOpen);

        ImGui::PushID("Renderer Stats");

        ImGui::Columns(4);

        ImGui::SetColumnWidth(0, 120);
        ImGui::Text("Draw Calls : %d", RendererStatistics::DrawCalls);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 140);
        ImGui::Text("Vertex Counts : %d", RendererStatistics::VertexCount);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(2, 140);
        ImGui::Text("Index Counts : %d", RendererStatistics::IndexCount);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(3, 120);
        ImGui::Text("Texture Counts : %d", RendererStatistics::TextureCount);

        ImGui::Columns(1);

        ImGui::PopID();

        ImGui::End();
    }
    
}
