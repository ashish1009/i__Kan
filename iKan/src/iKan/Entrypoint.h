// ******************************************************************************
// File         : Entrypoint.h
// Description  : Core Entry point defined in core. If this file is included in
//                client side then there must not be any other entry point in the
//                client code else entry point must be defined in client side.
// Project      : iKan
//
// Created by Ashish on 12/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

extern iKan::Scope<iKan::Application> iKan::CreateApplication();

/// Core Entry point (Main Function for the Core). Game loop calls from here
int main()
{
    // initializing spd logger
    iKan::Log::Init();
    IK_CORE_INFO("Initialized spd logger");

    // Application is created
    iKan::Scope<iKan::Application> app = iKan::CreateApplication();
    app->Run();
    return 0;
}
