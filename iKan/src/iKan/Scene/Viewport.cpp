// ******************************************************************************
// File         : Viewport.cpp
// Project      : iKan
// Description  : stores all the information of Viewport
//
//  Created by Ashish . on 02/05/21.
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
    
    
}
