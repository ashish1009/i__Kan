// ******************************************************************************
// File         : Entrypoint.h
// Description  : Core Entry point defined in core. If this file is included in
//               client side then there must not be any other entry point in the
//               client code else entry point must be defined in client side.
// Project      : iKan
//
// Created on   : 12/04/21.
// ******************************************************************************

#pragma once

extern std::unique_ptr<iKan::Application> iKan::CreateApplication();

// ******************************************************************************
// Core Entry point (Main Function for the Core). Game loop calls from here
// ******************************************************************************
int main()
{
    // Application is created
    std::unique_ptr<iKan::Application> app = iKan::CreateApplication();
    return 0;
}
