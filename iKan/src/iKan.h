// ******************************************************************************
// File         : iKan.h
// Description  : Core Headers needed by Client
// Project      : iKan
//
// Created by Ashish on 12/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

// Core files
#include <iKan/Core/Application.h>
#include <iKan/Core/KeyCode.h>
#include <iKan/Core/MouseCode.h>
#include <iKan/Core/Input.h>
#include <iKan/Core/Window.h>
#include <iKan/Core/Layer.h>
#include <iKan/Core/TimeStep.h>
#include <iKan/Core/Events/Event.h>
#include <iKan/Core/Events/ApplicationEvent.h>
#include <iKan/Core/Events/KeyEvent.h>
#include <iKan/Core/Events/MouseEvent.h>

// Renderer files
#include <iKan/Renderer/Renderer.h>
#include <iKan/Renderer/RendererStats.h>
#include <iKan/Renderer/Buffers.h>
#include <iKan/Renderer/VertexArray.h>
#include <iKan/Renderer/FrameBuffer.h>
#include <iKan/Renderer/Shader.h>
#include <iKan/Renderer/Texture.h>
#include <iKan/Renderer/Camera.h>

// Editor files
#include <iKan/Editor/EditorCamera.h>
#include <iKan/Editor/SceneHierarchyPannel.h>
#include <iKan/Editor/ScenePropertyGrid.h>

// Scene files
#include <iKan/Scene/Scene.h>
#include <iKan/Scene/Entity.h>
#include <iKan/Scene/Component.h>
#include <iKan/Scene/Viewport.h>
#include <iKan/Scene/SceneSerializer.h>
#include <iKan/Scene/SceneSerializer.h>

// Imgui Files
#include <iKan/Imgui/ImguiAPI.h>
