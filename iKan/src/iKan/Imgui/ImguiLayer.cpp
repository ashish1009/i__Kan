// ******************************************************************************
// File         : ImguiLayer.cpp
// Project      : iKan : Core
// Description  : Class for Imgui Layer
//
// Created on   : 24/04/21.
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
        
        // Store the Bold font as well
        io.Fonts->AddFontFromFileTTF("../../Engine/src/iKan/Editor/Fonts/OpenSans/OpenSans-ExtraBold.ttf", 15.0f);
        
        // Default font is Regular
        io.FontDefault = io.Fonts->AddFontFromFileTTF("../../Engine/src/iKan/Editor/Fonts/OpenSans/OpenSans-Regular.ttf", 15.0f);
                
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