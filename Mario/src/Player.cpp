// ******************************************************************************
// File         : Player.cpp
// Descrition   : Player Class info
// Project      : Mario
//
// Created by Ashish on 10/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Player.h"

namespace Mario {

    // ******************************************************************************
    // Player Constructor
    // ******************************************************************************
    Player::Player(Ref<Scene> scene)
    : m_ActiveScene(scene)
    {
        IK_INFO("Mario Player Constructor called");
        Init(scene);
    }

    // ******************************************************************************
    // Player Destructor
    // ******************************************************************************
    Player::~Player()
    {
        IK_WARN("Mario Player Destructor called");
    }

    // ******************************************************************************
    // Initialize the player
    // ******************************************************************************
    void Player::Init(Ref<Scene> scene)
    {
        m_Texture = scene->AddTextureToScene("../../../Mario/assets/Resources/Graphics/Player.png");
        
        m_StandingSubtexComp = SubTexture::CreateFromCoords(m_Texture, { 0.0f, 0.0f });
        
        m_Entity = scene->CreateEntity("Player 1");
        m_Entity.GetComponent<BoxCollider2DComponent>().IsRigid = true;
        m_Entity.AddComponent<SpriteRendererComponent>(m_StandingSubtexComp);
        
        auto& position = m_Entity.GetComponent<TransformComponent>().Translation;
        position.x = -10;
    }
    
    // ******************************************************************************
    // Update the player each frame
    // ******************************************************************************
    void Player::OnUpdate(Timestep ts)
    {
        auto& position = m_Entity.GetComponent<TransformComponent>().Translation;
        auto& size     = m_Entity.GetComponent<TransformComponent>().Scale;

        {
            if (Input::IsKeyPressed(KeyCode::Right) && !m_ActiveScene->IsRightCollision(m_Entity, m_RunningSpeed))
                position.x += m_RunningSpeed;
            if (Input::IsKeyPressed(KeyCode::Left) && !m_ActiveScene->IsLeftCollision(m_Entity, m_RunningSpeed))
                position.x -= m_RunningSpeed;
            
            if (Input::IsKeyPressed(KeyCode::Up) && !m_ActiveScene->IsTopCollision(m_Entity, m_RunningSpeed))
                position.y += m_RunningSpeed;
            if (Input::IsKeyPressed(KeyCode::Down) && !m_ActiveScene->IsBottomCollision(m_Entity, m_RunningSpeed))
                position.y -= m_RunningSpeed;
        }
        
        // Get the Position and size of Player entity each frame
        m_EntityPosition = position;
        m_EntitySize     = size;
    }
    
    void Player::ImguiRenderer()
    {
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_SpanAvailWidth;
        static UUID uuid;
        bool tileOpened = ImGui::TreeNodeEx((void*)(uint64_t)uuid, flags, "Player");
        if (tileOpened)
        {
            ImGui::Text("Player");
            ImGui::TreePop();
        }
    }

}
