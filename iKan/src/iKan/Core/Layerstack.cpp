// ******************************************************************************
// File         : Layerstack.cpp
// Description  : Implementation of Layer stack class to store all the layers in
//                a stack
// Project      : iKan : Core
//
// Created by Ashish on 13/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Layerstack.h"

namespace iKan {
    
    // ******************************************************************************
    // layer stack destructor
    // ******************************************************************************
    Layerstack::~Layerstack()
    {
        IK_CORE_WARN("Destroying Layerstack instance and deleting all the layers inside");
        for (Ref<Layer> layer : m_Layers)
        {
            layer->OnDetach();
        }
    }
    
    // ******************************************************************************
    // Push the layer into layer stack
    // ******************************************************************************
    void Layerstack::PushLayer(Ref<Layer> layer)
    {
        IK_CORE_INFO("Pushing the Client layer: '{0}', into stack at {1}", layer->GetName().c_str(), m_LayerInsertIndex);

        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
        layer->OnAttach();
    }
    
    // ******************************************************************************
    // Pop the layer from end
    // ******************************************************************************
    void Layerstack::PopLayer(Ref<Layer> layer)
    {
        IK_CORE_WARN("Pop the Client layer: '{0}', from the stack ", layer->GetName().c_str());

        auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
        if (it != m_Layers.begin() + m_LayerInsertIndex)
        {
            layer->OnDetach();
            m_Layers.erase(it);
            m_LayerInsertIndex--;
        }
    }
    
    // ******************************************************************************
    // Push the layer into layer stack. These layers are Core layers that will be
    // handled by core only
    // ******************************************************************************
    void Layerstack::PushOverlay(Ref<Layer> overlay)
    {
        IK_CORE_INFO("Pushing the Core layer: '{0}', into stack at the end ", overlay->GetName().c_str());

        m_Layers.emplace_back(overlay);
        overlay->OnAttach();
    }
    
    // ******************************************************************************
    // Pop the layer from stack. This will delete only core layers, will not touch
    // any client layers
    // ******************************************************************************
    void Layerstack::PopOverlay(Ref<Layer> overlay)
    {
        IK_CORE_WARN("Pop the Core layer: '{0}', from the stack ", overlay->GetName().c_str());

        auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
        if (it != m_Layers.end())
        {
            overlay->OnDetach();
            m_Layers.erase(it);
        }
    }
    
}


