// ******************************************************************************
// File         : Component.h
// Project      : iKan : Scene
// Description  : Structures of various components
//
// Created on   : 29/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Core/UUID.h>

namespace iKan {
    
    // ******************************************************************************
    // Stores the Universal unique ID for component
    // ******************************************************************************
    struct IDComponent
    {
        UUID ID = 0;
        
        ~IDComponent() = default;
        
        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
        IDComponent(const UUID& Id)
        : ID(Id)
        {
             
        }
    };
    
    // ******************************************************************************
    // Stores the Tag or Title of component
    // ******************************************************************************
    struct TagComponent
    {
        std::string Tag = "Default Entity";
        
        ~TagComponent() = default;
        
        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
        : Tag(tag)
        {
            
        }
    };
    
}
