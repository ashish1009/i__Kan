// ******************************************************************************
// File         : MarioLayer.cpp
// Description  : Client (Mario) Main Layer to be added in iKan Application
// Project      : Mario
//
// Created by Ashish on 05/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan.h>

using namespace iKan;

namespace Mario {
    
    // ******************************************************************************
    // Component for Mario Item in BG
    // ******************************************************************************
    struct ItemPropertyComponent
    {
        enum class Property
        {
            SingleCoin, MultipleCoin, FireFlower, Mushroom, Star, Nothing, GoInside
        };
        
        Property ItemProeprty;
        
        // Onlty for muliple coins // TODO: may be moved later to some other place
        int32_t Count = 0;
        
        ItemPropertyComponent(Property property = Property::Nothing)
        : ItemProeprty(property), Count(10) {}
        
        // ******************************************************************************
        // Get the property type from char tile
        // ******************************************************************************
        static Property GetPropertyFromChar(char ch)
        {
            switch(ch)
            {
                case 'c' : return Property::SingleCoin;
                case 'C' : return Property::MultipleCoin;
                case 'F' : return Property::FireFlower;
                case 'M' : return Property::Mushroom;
                case 'S' : return Property::Star;
                case '-' : return Property::Nothing;
                case '|' : return Property::GoInside;
            }
            return Property::Nothing;
        }
        
        // ******************************************************************************
        // get proeprty in string
        // ******************************************************************************
        std::string GetPropertyString()
        {
            switch(ItemProeprty)
            {
                case Property::SingleCoin      : return "SingleCoin";
                case Property::MultipleCoin    : return "MultipleCoin";
                case Property::FireFlower      : return "FireFlower";
                case Property::Mushroom        : return "Mushroom";
                case Property::Star            : return "Star";
                case Property::Nothing         : return "Nothing";
                case Property::GoInside        : return "GoInside";
            }
            return "Property::Nothing";
        }

    };
    
    // ******************************************************************************
    // Client layer for Mario (Main layer) derived from iKan Layer
    // ******************************************************************************
    class MarioLayer : public Layer
    {
    public:
        MarioLayer();
        virtual ~MarioLayer();
        
        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& event) override;
        virtual void OnUpdate(Timestep ts) override;
        virtual void OnImguiRender(Timestep ts) override;

    private:
        void ShowMenu();
        
    private:
        bool m_IsSetting = true;

        Viewport& m_Viewport = Viewport::Create("../../../../../../../iKan.");
        
        Entity m_PrimaryCameraEntity;
        Entity m_EditorCameraEntity;
        Entity m_PlayerEntity;
        
        Ref<Scene> m_ActiveScene;
    };

}
