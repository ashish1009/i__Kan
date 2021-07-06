// ******************************************************************************
// File         : Math.h
// Description  : AABB Bound Class
// Projectt     : iKan : Core : Event
//
// Created by Ashish on 17/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan {

    struct AABB
    {
        glm::vec3 Min, Max;

        AABB()
        : Min(0.0f), Max(0.0f) {}

        AABB(const glm::vec3& min, const glm::vec3& max)
        : Min(min), Max(max) {}

    };

}
