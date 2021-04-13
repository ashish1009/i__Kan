// ******************************************************************************
// File         : MacWindow.cpp
// Description  : Mac Window Class
// Project      : iKan : Platform
//
// Created on   : 13/04/21.
// ******************************************************************************

#include "MacWindow.h"

namespace iKan {
    
    // ******************************************************************************
    // Constructor of MAC Window
    // ******************************************************************************
    MacWindow::MacWindow(const Window::Property& prop)
    {
        IK_CORE_INFO("Constructing the MAC Window : {0} ({1} x {2})", prop.Title.c_str(), prop.Width, prop.Height);

        Init(prop);
    }
    
    // ******************************************************************************
    // Destrucot of MAC Window
    // ******************************************************************************
    MacWindow::~MacWindow()
    {
        IK_CORE_WARN("Destroying MAC Window : {0} ({1} x {2})", m_Data.Title.c_str(), m_Data.Width, m_Data.Height);

        Shutdown();
    }
    
    // ******************************************************************************
    // Initializing of MAC Window
    // ******************************************************************************
    void MacWindow::Init(const Window::Property &prop)
    {
        
    }
    
    // ******************************************************************************
    // Shutting down MAC Window
    // ******************************************************************************
    void MacWindow::Shutdown()
    {
        IK_CORE_WARN("Shutting down MAC Window : {0} ({1} x {2})", m_Data.Title.c_str(), m_Data.Width, m_Data.Height);
    }
    
    // ******************************************************************************
    // Constructor of MAC Window
    // ******************************************************************************
    void MacWindow::Update()
    {

    }
    

}
