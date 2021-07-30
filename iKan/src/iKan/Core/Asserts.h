// ******************************************************************************
// File         : Asserts.h
// Description  : API for trigger exceptions. NOTE  Name of this file is Assets.h,
//                if we rename this as Asset then it might replace with C++ Asset
//                file
// Project      : iKan : Core
//
// Created by Ashish on 30/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#define IK_EXPAND_VARGS(x) x

#ifdef IK_ENABLE_ASSERT

#define ASSERT(condition) assert(condition);

#define IK_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { IK_ERROR("Assertion Failed"); ASSERT(condition); } }
#define IK_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { IK_ERROR("Assertion Failed: {0}", __VA_ARGS__); ASSERT(condition); } }
#define IK_CORE_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { IK_CORE_ERROR("Assertion Failed"); ASSERT(condition); } }
#define IK_CORE_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { IK_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); ASSERT(condition); } }

#define IK_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro

#define IK_GET_ASSERT_MACRO(...) IK_EXPAND_VARGS(IK_ASSERT_RESOLVE(__VA_ARGS__, IK_ASSERT_MESSAGE, IK_ASSERT_NO_MESSAGE))
#define IK_GET_CORE_ASSERT_MACRO(...) IK_EXPAND_VARGS(IK_ASSERT_RESOLVE(__VA_ARGS__, IK_CORE_ASSERT_MESSAGE, IK_CORE_ASSERT_NO_MESSAGE))

#define IK_ASSERT(...) IK_EXPAND_VARGS( IK_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#define IK_CORE_ASSERT(...) IK_EXPAND_VARGS( IK_GET_CORE_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )

#else

#define IK_ASSERT(...)
#define IK_CORE_ASSERT(...)

#endif
