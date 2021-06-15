// ******************************************************************************
// File         : ImguiLayer.cpp
// Description  : Class for Imgui Layer
// Project      : iKan : Core
//
// Created by Ashish on 24/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "ImguiLayer.h"
#include <iKan/Core/Application.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

namespace iKan {
    
    // ******************************************************************************
    // Attach ImGuiLayer
    // ******************************************************************************
    void ImguiLayer::OnAttach()
    {
        IK_CORE_INFO("Imgui layer is attached to application");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        SetFont("../../../iKan/src/iKan/Editor/Fonts/OpenSans/OpenSans-Regular.ttf");

        /* Setup Dear ImGui style */
        ImGui::StyleColorsDark();
        
        /* When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones. */
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        
        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        /* Setup Platform/Renderer bindings */
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    // ******************************************************************************
    // Set the imgui Font
    // ******************************************************************************
    void ImguiLayer::SetFont(const std::string &path)
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // Store the Bold font as well
        io.Fonts->AddFontFromFileTTF("../../../iKan/src/iKan/Editor/Fonts/OpenSans/OpenSans-ExtraBold.ttf", 15.0f);

        // Default font is Regular
        io.FontDefault = io.Fonts->AddFontFromFileTTF(path.c_str(), 15.0f);

        IK_CORE_INFO("Imgui Font chnaged to {0}", path.c_str());
    }

    // ******************************************************************************
    // Detatch ImGuiLayer
    // ******************************************************************************
    void ImguiLayer::OnDetach()
    {
        IK_CORE_WARN("Imgui layer is detached to application");

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    
    // ******************************************************************************
    // Event handler in ImGuiLayer
    // ******************************************************************************
    void ImguiLayer::OnEvent(Event& event)
    {
        if (m_BlockEvents)
        {
            ImGuiIO& io = ImGui::GetIO();

            event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
    }
    
    // ******************************************************************************
    // Begin ImGuiLayer
    // ******************************************************************************
    void ImguiLayer::Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    
    // ******************************************************************************
    // Ends ImGuiLayer
    // ******************************************************************************
    void ImguiLayer::End()
    {
        ImGuiIO& io      = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize   = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    
}
