// ******************************************************************************
// File         : Layerstack.h
// Description  : Layer stack class to store all the layers in a stack
// Project      : iKan : Core
//
// Created on   : 13/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Core/Layer.h>

namespace iKan {
    
    // ******************************************************************************
    // Layer stack class to store all the layers in a stack. Layers can be pushed and
    // popped in any location
    // ******************************************************************************
    class Application;
    class Layerstack
    {
    public:
        Layerstack() = default;
        ~Layerstack();
        
        void PushLayer(Ref<Layer> layer);
        void PopLayer(Ref<Layer> layer);
        
        uint32_t GetNumInserted() const { return m_LayerInsertIndex; }
        
    private:
        void PushOverlay(Ref<Layer> overlay);
        void PopOverlay(Ref<Layer> overlay);

        // APIs for iterators
        std::vector<Ref<Layer>>::iterator begin() { return m_Layers.begin(); }
        std::vector<Ref<Layer>>::iterator end() { return m_Layers.end(); }
        std::vector<Ref<Layer>>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
        std::vector<Ref<Layer>>::reverse_iterator rend() { return m_Layers.rend(); }
        
        std::vector<Ref<Layer>>::const_iterator begin() const { return m_Layers.begin(); }
        std::vector<Ref<Layer>>::const_iterator end() const { return m_Layers.end(); }
        std::vector<Ref<Layer>>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
        std::vector<Ref<Layer>>::const_reverse_iterator rend() const { return m_Layers.rend(); }
        
    private:
        uint32_t m_LayerInsertIndex = 0;
        
        std::vector<Ref<Layer>> m_Layers;
        
        // as Push overlay and Pop overlay are only for core layers, and core layers
        // are maintained by Application only 
        friend class Application;
    };
}
