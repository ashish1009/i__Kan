// ******************************************************************************
// File         : Layer.h
// Description  : Layer Base Class to manage Client layers
// Project      : iKan : Core
//
// Created by Ashish on 13/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/TimeStep.h>
#include <iKan/Core/Events/Event.h>

namespace iKan {
    
    // ******************************************************************************
    // Base class for Layer. Differet layes can be created in an application. These
    // layers can be created in core or Client, all layers will be derived from this
    // base class. Base functions are empty as these functions should be defined in
    // Derived Layers according to application requirement
    // ******************************************************************************
    class Layer
    {
    public:
        Layer(const std::string& name = "Unknown")
        : m_Name(name) {}
        
        virtual ~Layer() = default;
        
        virtual void OnAttach() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnImguiRender() {}
        virtual void OnDetach() {}
        virtual void OnEvent(Event& event) {}
        
        const std::string& GetName() const { return m_Name; }

    private:
        std::string m_Name = "Unknown";
    };

}
