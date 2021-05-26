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

    std::string Utils::GetNameFromFilePath(const std::string& path)
    {
        auto lastSlash = path.find_last_of("/\\");
        lastSlash      = lastSlash == std::string::npos ? 0 : lastSlash + 1;

        auto lastDot = path.find_last_of('.');
        auto count   = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;

        return path.substr(lastSlash, count);
    }

}
