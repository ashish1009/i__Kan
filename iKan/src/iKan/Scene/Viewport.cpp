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

namespace iKan {
    
    // ******************************************************************************
    // Update the view port bouds
    // ******************************************************************************
    void Viewport::UpdateBounds()
    {
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 minBound   = ImGui::GetWindowPos();
        
        minBound.x += CursorPos.x;
        minBound.y += CursorPos.y;

        ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
        Bounds[0] = { minBound.x, minBound.y };
        Bounds[1] = { maxBound.x, maxBound.y };
    }
    
    // ******************************************************************************
    // Update the Viewport
    // ******************************************************************************
    void Viewport::OnUpdate()
    {
        CursorPos = ImGui::GetCursorPos();

        Focused = ImGui::IsWindowFocused();
        Hovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!Focused && !Hovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        Size = { viewportPanelSize.x, viewportPanelSize.y };
        
        size_t textureID = FrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2{ Size.x, Size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    }
    
    // ******************************************************************************
    // Update mouse position of viewport
    // ******************************************************************************
    void Viewport::UpdateMousePos()
    {
        auto [mx, my] = ImGui::GetMousePos();
        mx -= Bounds[0].x;
        my -= Bounds[0].y;

        Height = Bounds[1].y - Bounds[0].y;
        Width  = Bounds[1].x - Bounds[0].x;

        my = Height - my;

        MousePosX = (int32_t)mx;
        MousePosY = (int32_t)my;
    }
    
    // ******************************************************************************
    // Reender Viewport Imgui pannel, if flag is true then render
    // ******************************************************************************
    void Viewport::OnImguiRenderer()
    {
        ImGui::Begin("Viewport Properties", &IsImguiPannel);
        ImGui::PushID("Viewport Properties");

        ImGui::Columns(3);

        ImGui::SetColumnWidth(0, 80);
        ImGui::Text("Present : %d", Present);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 80);
        ImGui::Text("Focused : %d", Focused);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(2, 80);
        ImGui::Text("Hovered : %d", Hovered);
        ImGui::NextColumn();

        ImGui::Separator();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 120);
        ImGui::Text("Width x Height ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 120);
        ImGui::Text("%d x %d", (int32_t)Width,  (int32_t)Height);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 130);
        ImGui::Text("Size ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 130);
        ImGui::Text("%d x %d", (int32_t)Size.x, (int32_t)Size.y);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 130);
        ImGui::Text("Top Left Position ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 130);
        ImGui::Text("%d x %d", (int32_t)Bounds[0].x, (int32_t)Bounds[0].y);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 130);
        ImGui::Text("Bottom Right Position ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 130);
        ImGui::Text("%d x %d", (int32_t)Bounds[1].x, (int32_t)Bounds[1].y);
        ImGui::NextColumn();

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, 130);
        ImGui::Text("Mouse Position ");
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 130);
        ImGui::Text("%d x %d", MousePosX, MousePosY);
        ImGui::NextColumn();

        ImGui::Columns(1);

        ImGui::PopID();
        ImGui::End();
    }

}
