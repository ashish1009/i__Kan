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
    
    Ref<Scene>      Player::s_ActiveScene        = nullptr;
    Ref<Texture>    Player::s_Texture            = nullptr;
    Ref<SubTexture> Player::s_StandingSubtexComp = nullptr;
        
    float Player::s_RunningSpeed = 0.10;
    float Player::s_FallingSpeed = 0.50;

    std::function <void(Player*)> Player::s_StateFunc[(int32_t)State::None];

    // ******************************************************************************
    // Player Constructor
    // ******************************************************************************
    Player::Player(Ref<Scene> scene)
    {
        Init(scene, (m_Size == Size::Short), m_Color);
    
        IK_INFO("Mario Player Constructor called");
        m_Entity = s_ActiveScene->CreateEntity("Player 1");
        m_Entity.GetComponent<BoxCollider2DComponent>().IsRigid = true;
        m_Entity.AddComponent<SpriteRendererComponent>(s_StandingSubtexComp);
        
        SetCurrentTexture(Color::Classic);
        
        auto& position = m_Entity.GetComponent<TransformComponent>().Translation;
        position.x = 16.0f;
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
    void Player::Init(Ref<Scene> scene, bool isShort, Color color)
    {
        s_ActiveScene = scene;
        s_ActiveScene->SetEditingFlag(false);
        
        s_Texture = scene->AddTextureToScene("../../../Mario/assets/Resources/Graphics/Player.png");
        
        s_StateFunc[(int32_t)State::Falling]  = Player::Falling;
        s_StateFunc[(int32_t)State::Jumping]  = Player::Jumping;
        s_StateFunc[(int32_t)State::Standing] = Player::Standing;
        s_StateFunc[(int32_t)State::Firing]   = Player::Firing;
        s_StateFunc[(int32_t)State::Dying]    = Player::Dying;
        s_StateFunc[(int32_t)State::Sitting]  = Player::Sitting;
        s_StateFunc[(int32_t)State::Running]  = Player::Running;
        
        SetPlayerTextureForAllStates(isShort, color);
    }
    
    // ******************************************************************************
    // Update the texture for all state
    // ******************************************************************************
    void Player::SetPlayerTextureForAllStates(bool isShort, Color color)
    {
        s_StandingSubtexComp = SubTexture::CreateFromCoords(s_Texture, { 6.0f, (float)color + (isShort ? 0.0f : 1.0f) }, { 1.0f, (isShort ? 1.0f : 2.0f) } );
    }
    
    // ******************************************************************************
    // Update the player each frame
    // ******************************************************************************
    void Player::OnUpdate(Timestep ts)
    {
        m_EntityPosition = &m_Entity.GetComponent<TransformComponent>().Translation;
        m_EntitySize     = &m_Entity.GetComponent<TransformComponent>().Scale;

        {
            if (Input::IsKeyPressed(KeyCode::Right) && !s_ActiveScene->IsRightCollision(m_Entity, s_RunningSpeed))
                m_EntityPosition->x += s_RunningSpeed;
            if (Input::IsKeyPressed(KeyCode::Left) && !s_ActiveScene->IsLeftCollision(m_Entity, s_RunningSpeed))
                m_EntityPosition->x -= s_RunningSpeed;
        }
        
        s_StateFunc[(int32_t)m_State](this);
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
                ImTextureID myTexId = (ImTextureID)((size_t)s_Texture->GetRendererID());
                float myTexW = (float)s_Texture->GetWidth();
                float myTexH = (float)s_Texture->GetHeight();
                
                for (int32_t color = (int32_t)Color::Black_Grey; color < (int32_t)Color::Classic; color += 3)
                {
                    ImGui::PushID(color);
                    
                    glm::vec2 coords = { 0.0f, (float)color };
                    glm::vec2 uv1 = { (coords.x + 1) * 16.0f, coords.y * 16.0f };
                    glm::vec2 uv0 = { coords.x * 16.0f, (coords.y + 1) * 16.0f };
                    
                    if (ImGui::ImageButton(myTexId, ImVec2(32.0f, 32.0f), ImVec2(uv0.x / myTexW, uv0.y / myTexH), ImVec2(uv1.x / myTexW, uv1.y / myTexH), 0))
                    {
                        SetPlayerTextureForAllStates((m_Size == Size::Short), (Color)color);
                        SetCurrentTexture((Color)color);
                    }

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
        
        Ref<SubTexture> currSubtexture;
        switch (m_State)
        {
            case State::Falling:
                currSubtexture = s_StandingSubtexComp;
                break;
                
            default:
                break;
        }

        m_Entity.GetComponent<SpriteRendererComponent>().SubTexComp = currSubtexture;
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
    
    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void Player::Falling(Player* player)
    {
        if (!s_ActiveScene->IsBottomCollision(player->m_Entity, s_FallingSpeed))
            player->m_EntityPosition->y -= s_FallingSpeed;
    }
    
    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void Player::Jumping(Player* player)
    {
        
    }

    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void Player::Standing(Player* player)
    {
        
    }

    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void Player::Firing(Player* player)
    {
        
    }

    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void Player::Dying(Player* player)
    {
        
    }

    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void Player::Sitting(Player* player)
    {
        
    }

    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void Player::Running(Player* player)
    {
        
    }
    
}
