// ******************************************************************************
// File         : SceneHierarchyPannel.cpp
// Project      : iKan
// Description  : Class for having property pannel of each entity
//
// Created      : on 02/05/21.
// ******************************************************************************

#include "SceneHierarchyPannel.h"

namespace iKan {
    
    // ******************************************************************************
    // Scene hirarchy pannel constructot
    // ******************************************************************************
    SceneHeirarchyPannel::SceneHeirarchyPannel(const Ref<Scene>& context)
    {
        IK_CORE_INFO("Scene hierarchy Pannel created");
        SetContext(context);
    }
    
    // ******************************************************************************
    // Scene hirarchy pannel Context is updated
    // ******************************************************************************
    void SceneHeirarchyPannel::SetContext(const Ref<Scene>& context)
    {
        IK_CORE_INFO("Context Updated in Scene Hierarchy pannel");
        m_Context = context;
    }
    
    // ******************************************************************************
    // Scene hirarchy pannel Imgui renderer
    // ******************************************************************************
    void SceneHeirarchyPannel::OnImguiender(bool* pIsOpen)
    {
        ImGui::Begin("Scene Hierarchy", pIsOpen);
        {
            
        }
        ImGui::End(); // ImGui::Begin("Scene Hierarchy", pIsOpen);
    }
    
}
