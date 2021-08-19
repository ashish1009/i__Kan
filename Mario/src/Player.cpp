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
                
        m_Entity = scene->CreateEntity("Player 1");
        m_Entity.GetComponent<BoxCollider2DComponent>().IsRigid = true;
        m_Entity.AddComponent<SpriteRendererComponent>(m_StandingSubtexComp);
        
        SetCurrentTexture(Color::Classic);
        
        auto& position = m_Entity.GetComponent<TransformComponent>().Translation;
        position.x = -10;
    }
    
    // ******************************************************************************
    // Update the player each frame
    // ******************************************************************************
    void Player::OnUpdate(Timestep ts)
    {
        m_EntityPosition = &m_Entity.GetComponent<TransformComponent>().Translation;
        m_EntitySize     = &m_Entity.GetComponent<TransformComponent>().Scale;

        {
            IK_ASSERT(m_EntityPosition, "Entity Posiiton not pointing to any memory");
            if (Input::IsKeyPressed(KeyCode::Right) && !m_ActiveScene->IsRightCollision(m_Entity, m_RunningSpeed))
                m_EntityPosition->x += m_RunningSpeed;
            if (Input::IsKeyPressed(KeyCode::Left) && !m_ActiveScene->IsLeftCollision(m_Entity, m_RunningSpeed))
                m_EntityPosition->x -= m_RunningSpeed;
            
            // TODO: Temp
            if (Input::IsKeyPressed(KeyCode::Up) && !m_ActiveScene->IsTopCollision(m_Entity, m_RunningSpeed))
                m_EntityPosition->y += m_RunningSpeed;
            if (Input::IsKeyPressed(KeyCode::Down) && !m_ActiveScene->IsBottomCollision(m_Entity, m_RunningSpeed))
                m_EntityPosition->y -= m_RunningSpeed;
        }
    }
    
    // ******************************************************************************
    // Imgui renderer for Imgui
    // ******************************************************************************
    void Player::ImguiRenderer()
    {
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth;
        const auto& playerId = m_Entity.GetComponent<IDComponent>().ID;
        bool tileOpened = ImGui::TreeNodeEx((void*)(uint64_t)playerId, flags, "Player");
        if (tileOpened)
        {
            static UUID uuid;
            if (ImGui::TreeNodeEx((void*)(uint64_t)uuid, flags, "Color"))
            {
                ImTextureID myTexId = (ImTextureID)((size_t)m_Texture->GetRendererID());
                float myTexW = (float)m_Texture->GetWidth();
                float myTexH = (float)m_Texture->GetHeight();
                
                for (int32_t color = (int32_t)Color::Black_Grey; color < (int32_t)Color::Classic; color += 3)
                {
                    ImGui::PushID(color);
                    
                    glm::vec2 coords = { 0.0f, (float)color };
                    glm::vec2 uv1 = { (coords.x + 1) * 16.0f, coords.y * 16.0f };
                    glm::vec2 uv0 = { coords.x * 16.0f, (coords.y + 1) * 16.0f };
                    
                    if (ImGui::ImageButton(myTexId, ImVec2(32.0f, 32.0f), ImVec2(uv0.x / myTexW, uv0.y / myTexH), ImVec2(uv1.x / myTexW, uv1.y / myTexH), 0))
                        SetCurrentTexture((Color)color);

                    ImGui::PopID();
                    ImGui::SameLine();
                }
                ImGui::NewLine();
                ImGui::TreePop();
            }
            ImGui::TreePop();
            
            ImGui::Separator();
            static bool tall = false;
            PropertyGrid::CheckBox("Tall", tall, 100.0f);
            tall == true ? ChangeSize(Size::Tall) : ChangeSize(Size::Short);
        }
    }
    
    // ******************************************************************************
    // Set the current Texture
    // ******************************************************************************
    void Player::SetCurrentTexture(Color color)
    {
        m_Color = Color(color);
        bool isShort = m_Size == Size::Short;
        m_Entity.GetComponent<SpriteRendererComponent>().SubTexComp = SubTexture::CreateFromCoords(m_Texture, { 0.0f, (float)color + (isShort ? 0.0f : 1.0f) }, { 1.0f, (isShort ? 1.0f : 2.0f) } );
    }
    
    // ******************************************************************************
    // Change the size of Player
    // ******************************************************************************
    void Player::ChangeSize(Size size)
    {
        IK_ASSERT(m_EntitySize, "Entity Size is not pointing to any memory");
        m_Size = size;
        m_EntitySize->y = (m_Size == Size::Tall ? 2.0f : 1.0f);
        
        SetCurrentTexture(m_Color);
    }

}
