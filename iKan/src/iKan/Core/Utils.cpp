// ******************************************************************************
// File         : Utils.cpp
// Description  : Utils API for common functions
// Project      : iKan : Core
//
// Created by Ashish on 25/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Utils.h"

namespace iKan {

    // ******************************************************************************
    // Extract file Name from path
    // ******************************************************************************
    std::string Utils::GetNameFromFilePath(const std::string& path)
    {
        auto lastSlash = path.find_last_of("/\\");
        lastSlash      = lastSlash == std::string::npos ? 0 : lastSlash + 1;

        auto lastDot = path.find_last_of('.');
        auto count   = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;

        return path.substr(lastSlash, count);
    }

    // ******************************************************************************
    // Extract file extention from path
    // ******************************************************************************
    std::string Utils::GetExtensionFromFilePath(const std::string& path)
    {
        auto lastDot = path.find_last_of('.') + 1;
        auto count   = lastDot - std::string::npos;

        return path.substr(lastDot, count);
    }

}
