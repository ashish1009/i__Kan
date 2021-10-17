// ******************************************************************************
// File         : Math.h
// Project      : iKan : Core
// Description  : Math Functions
//
// Created by Ashish on 17/10/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <glm/glm.hpp>

namespace iKan::Math {
    
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);
    
}

