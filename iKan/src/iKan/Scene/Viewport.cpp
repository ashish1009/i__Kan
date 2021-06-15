// ******************************************************************************
// File         : Viewport.cpp
// Description  : stores all the information of Viewport
// Project      : iKan
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Viewport.h"
#include <iKan/Core/Application.h>
#include <iKan/Core/Input.h>
#include <iKan/Scene/Component.h>
#include <iKan/Editor/ScenePropertyGrid.h>
#include <iKan/Imgui/ImguiAPI.h>

namespace iKan {
    
    // ******************************************************************************
    // Update the view port bouds
    // ******************************************************************************
    void Viewport::UpdateBounds()
    {
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 minBound   = ImGui::GetWindowPos();
        
        minBound.x += Data.CursorPos.x;
        minBound.y += Data.CursorPos.y;

        ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
        Data.Bounds[0] = { minBound.x, minBound.y };
        Data.Bounds[1] = { maxBound.x, maxBound.y };
    }
    
    // ******************************************************************************
    // Update the Viewport
    // ******************************************************************************
    void Viewport::OnUpdateImGui()
    {
        // If viewport is not present then return
        if (!Flags.Present)
            return;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Viewport", &Flags.Present);
        {
            Data.CursorPos = ImGui::GetCursorPos();

            Data.Focused = ImGui::IsWindowFocused();
            Data.Hovered = ImGui::IsWindowHovered();
            Application::Get().GetImGuiLayer()->BlockEvents(!Data.Focused && !Data.Hovered);

            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            Data.Size = { viewportPanelSize.x, viewportPanelSize.y };

            size_t textureID = Data.FrameBuffer->GetColorAttachmentRendererID();
            ImGui::Image((void*)textureID, ImVec2{ Data.Size.x, Data.Size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

            UpdateBounds();
        }

        ImGui::End(); // ImGui::Begin("Viewport");
        ImGui::PopStyleVar(); // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    }

    // ******************************************************************************
    // Update mouse position of viewport
    // ******************************************************************************
    void Viewport::OnUpdate(Ref<Scene>& activeScene)
    {
        UpdateMousePos();
        UpdateHoveredEntity(activeScene);
    }
    
    // ******************************************************************************
    // Update mouse position of viewport
    // ******************************************************************************
    void Viewport::UpdateMousePos()
    {
        auto [mx, my] = ImGui::GetMousePos();
        mx -= Data.Bounds[0].x;
        my -= Data.Bounds[0].y;

        Data.Height = Data.Bounds[1].y - Data.Bounds[0].y;
        Data.Width  = Data.Bounds[1].x - Data.Bounds[0].x;

        my = Data.Height - my;

        Data.MousePosX = (int32_t)mx;
        Data.MousePosY = (int32_t)my;
    }

    // ******************************************************************************
    // Update the Hovered entity in the view port
    // ******************************************************************************
    void Viewport::UpdateHoveredEntity(Ref<Scene>& activeScene)
    {
        if (Data.MousePosX >= 0 && Data.MousePosY >= 0 && Data.MousePosX <= Data.Width && Data.MousePosY <= Data.Height )
        {
            int32_t ID = activeScene->GetEntityIdFromPixels(Data.MousePosX, Data.MousePosY);
            Data.HoveredEntity = (ID >= activeScene->GetNumEntities()) ? Entity() : Entity((entt::entity)ID, activeScene.get());
        }
    }

    // ******************************************************************************
    // Scene Editor Events
    // ******************************************************************************
    void Viewport::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(IK_BIND_EVENT_FN(Viewport::OnMouseButtonPressed));
    }

    // ******************************************************************************
    // Mouse button press event
    // ******************************************************************************
    bool Viewport::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == MouseCode::ButtonLeft && !Input::IsKeyPressed(KeyCode::LeftAlt))
        {
            if (Data.MousePosX >= 0 && Data.MousePosY >= 0 && Data.MousePosX <= Data.Width && Data.MousePosY <= Data.Height )
            {
                Data.SelectedEntity = Data.HoveredEntity;
            }
            else
            {
                Data.SelectedEntity = {};
            }
        }
        return false;
    }
    
    // ******************************************************************************
    // Reender Viewport Imgui pannel, if flag is true then render
    // ******************************************************************************
    void Viewport::OnImguiRenderer(Timestep ts)
    {
        OnUpdateImGui();

        // No Imgui renderer if flag is false
        if (!Flags.IsImguiPannel)
            return;

        ImGui::Begin("Viewport Properties", &Flags.IsImguiPannel);
        ImGui::PushID("Viewport Properties");

        ImGui::Columns(3);

        ImGui::SetColumnWidth(0, 80);
        ImGui::Text("Present : %d", Flags.Present);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 80);
        ImGui::Text("Focused : %d", Data.Focused);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(2, 80);
        ImGui::Text("Hovered : %d", Data.Hovered);
        ImGui::NextColumn();

        ImGui::Separator();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 130);
        ImGui::Text("Width x Height ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 130);
        ImGui::Text("%d x %d", (int32_t)Data.Width,  (int32_t)Data.Height);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 130);
        ImGui::Text("Size ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 130);
        ImGui::Text("%d x %d", (int32_t)Data.Size.x, (int32_t)Data.Size.y);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 130);
        ImGui::Text("Top Left Position ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 130);
        ImGui::Text("%d x %d", (int32_t)Data.Bounds[0].x, (int32_t)Data.Bounds[0].y);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 130);
        ImGui::Text("Bottom Right Position ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 130);
        ImGui::Text("%d x %d", (int32_t)Data.Bounds[1].x, (int32_t)Data.Bounds[1].y);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 130);
        ImGui::Text("Mouse Position ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 130);
        ImGui::Text("%d x %d", Data.MousePosX, Data.MousePosY);
        ImGui::NextColumn();

        ImGui::Columns(1);

        ImGui::Separator();

        std::string entityName = "NULL";
        if ((entt::entity)Data.HoveredEntity != entt::null)
        {
            entityName = Data.HoveredEntity.GetComponent<TagComponent>().Tag;

            ImGui::Text("Hovered Entity");
            PropertyGrid::String("Entity ID", (uint32_t)Data.HoveredEntity, 130.0f);
            PropertyGrid::String("Unique ID", (uint32_t)Data.HoveredEntity.GetComponent<IDComponent>().ID, 130.0f);
            PropertyGrid::String("Entity Name", entityName, 130.0f, 300.0f, "", false); // No need to add any Hint in non modifiable string
            ImGui::Separator();
        }

        ImGui::PopID();
        ImGui::End();

        RendererStats(ts);
    }

    // ******************************************************************************
    // Show the renderer stats
    // ******************************************************************************
    void Viewport::RendererStats(Timestep ts)
    {
        if (Flags.IsFrameRate)
        {
            ImGuiAPI::FrameRate(ts, &Flags.IsFrameRate);
        }

        if (Flags.IsRendererStats)
        {
            ImGuiAPI::RendererStats(&Flags.IsRendererStats);
        }

        if (Flags.IsVendorType)
        {
            ImGuiAPI::RendererVersion(&Flags.IsVendorType);
        }
    }

    // ******************************************************************************
    // View menu for view port
    // NOTE: this should be called between Imgui::BeginMenue and ImGui::EndMenue();
    // ******************************************************************************
    void Viewport::ViewMenu()
    {
        ImGui::Separator();

        if (ImGui::MenuItem("Frame Rate", nullptr, Flags.IsFrameRate))
        {
            Flags.IsFrameRate = !Flags.IsFrameRate;
        }

        if (ImGui::MenuItem("Render Stats", nullptr, Flags.IsRendererStats))
        {
            Flags.IsRendererStats = !Flags.IsRendererStats;
        }

        if (ImGui::MenuItem("Vendor Types", nullptr, Flags.IsVendorType))
        {
            Flags.IsVendorType = !Flags.IsVendorType;
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Renderer Viewport", nullptr, Flags.Present))
        {
            Flags.Present = !Flags.Present;
        }

        if (ImGui::MenuItem("Imgui", nullptr, Flags.IsImguiPannel))
        {
            Flags.IsImguiPannel = !Flags.IsImguiPannel;
        }

        ImGui::Separator();
    }

}
