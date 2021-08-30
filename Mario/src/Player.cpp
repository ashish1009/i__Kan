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
    
    Ref<SubTexture> Player::s_RunningSubtexComp[Player::MAX_RUNNING_IMG];
        
    float Player::s_RunningSpeed = 0.10;
    float Player::s_FallingSpeed = 0.50;

    std::function <void(Player*)> Player::s_StateFunc[Player::MAX_STATES];

    // ******************************************************************************
    // Player Constructor
    // ******************************************************************************
    Player::Player(Ref<Scene> scene)
    {
        Player::Init(scene, (m_Size == Size::Short), m_Color);
    
        IK_INFO("Mario Player Constructor called");
        m_Entity = s_ActiveScene->CreateEntity("Player 1");
        m_Entity.GetComponent<BoxCollider2DComponent>().IsRigid = true;
        m_Entity.AddComponent<SpriteRendererComponent>(s_StandingSubtexComp);
        
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
        
        s_StateFunc[Player::GetFirstSetBit((int32_t)State::Falling)]  = Player::Falling;
        s_StateFunc[Player::GetFirstSetBit((int32_t)State::Jumping)]  = Player::Jumping;
        s_StateFunc[Player::GetFirstSetBit((int32_t)State::Standing)] = Player::Standing;
        s_StateFunc[Player::GetFirstSetBit((int32_t)State::Firing)]   = Player::Firing;
        s_StateFunc[Player::GetFirstSetBit((int32_t)State::Dying)]    = Player::Dying;
        s_StateFunc[Player::GetFirstSetBit((int32_t)State::Sitting)]  = Player::Sitting;
        s_StateFunc[Player::GetFirstSetBit((int32_t)State::Running)]  = Player::Running;
        
        SetPlayerTextureForAllStates(isShort, color);
    }
    
    // ******************************************************************************
    // Spaercial function to get the fist set bit returns -1 if value is 0
    // 0 means Most LSB and So on...
    // ******************************************************************************
    int32_t Player::GetFirstSetBit(uint32_t value)
    {
        for (int32_t pos = 0; value >> pos; pos++)
            if (value & BIT(pos))
                return pos;
        
        return -1;
    }
    
    // ******************************************************************************
    // Update the texture for all state
    // ******************************************************************************
    void Player::SetPlayerTextureForAllStates(bool isShort, Color color)
    {
        s_StandingSubtexComp = SubTexture::CreateFromCoords(s_Texture, { 6.0f, (float)color + (isShort ? 0.0f : 1.0f) }, { 1.0f, (isShort ? 1.0f : 2.0f) } );
        
        for (uint32_t imgIdx = 0; imgIdx < Player::MAX_RUNNING_IMG; imgIdx++)
            s_RunningSubtexComp[imgIdx] = SubTexture::CreateFromCoords(s_Texture, { imgIdx, (float)color + (isShort ? 0.0f : 1.0f) }, { 1.0f, (isShort ? 1.0f : 2.0f) } );
    }
    
    // ******************************************************************************
    // Update the player each frame
    // ******************************************************************************
    void Player::OnUpdate(Timestep ts)
    {
        // By default player should be falling until it colloid with obstacle
        SetState(State::Falling);
        
        // TODO: Move them to Init : Some issues found
        m_EntityPosition   = &m_Entity.GetComponent<TransformComponent>().Translation;
        m_EntitySize       = &m_Entity.GetComponent<TransformComponent>().Scale;
        m_EntitySubtexture = &m_Entity.GetComponent<SpriteRendererComponent>().SubTexComp;
    
        {
            if (Input::IsKeyPressed(KeyCode::Right) && !s_ActiveScene->IsRightCollision(m_Entity, s_RunningSpeed))
            {
                SetState(State::Running);
                m_EntitySize->x = 1.0f;
                m_EntityPosition->x += s_RunningSpeed;
                m_Direction = Direction::Right;
            }
            if (Input::IsKeyPressed(KeyCode::Left) && !s_ActiveScene->IsLeftCollision(m_Entity, s_RunningSpeed))
            {
                SetState(State::Running);
                m_EntitySize->x = -1.0f;
                m_EntityPosition->x -= s_RunningSpeed;
                m_Direction = Direction::Left;
            }
        }
        
        for (int32_t stateIds = 0; stateIds < MAX_STATES; stateIds++)
        {
            int32_t fncIdx = Player::GetFirstSetBit(m_State & BIT(stateIds));
            if (fncIdx >= 0)
                s_StateFunc[fncIdx](this);
        }
    }
    
    
    // ******************************************************************************
    // Player Event handler
    // ******************************************************************************
    void Player::OnEvent(iKan::Event &event)
    {
        EventDispatcher dispather(event);
        dispather.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(Player::OnkeyPressed));
        dispather.Dispatch<KeyReleasedEvent>(IK_BIND_EVENT_FN(Player::OnKeyReleased));
    }
    
    // ******************************************************************************
    // Player Key pressed event handler
    // ******************************************************************************
    bool Player::OnkeyPressed(KeyPressedEvent& event)
    {
        return false;
    }

    // ******************************************************************************
    // Player Key released event handler
    // ******************************************************************************
    bool Player::OnKeyReleased(KeyReleasedEvent& event)
    {
        if (event.GetKeyCode() == KeyCode::Left || event.GetKeyCode() == KeyCode::Right)
        {
            ClearState(State::Running);
            m_RunningImgIdx = 0;
        }

        return false;
    }
    
    // ******************************************************************************
    // Change the size of Player
    // ******************************************************************************
    void Player::ChangeSize(Size size)
    {
        // TODO: Add Animation/
        IK_ASSERT(m_EntitySize, "Entity Size is not pointing to any memory");
        m_Size = size;
        
        // TODO: 1 is just random to avoid overlap. Need to fix this later
        m_EntityPosition->y += 1.0f;
        m_EntitySize->y = (m_Size == Size::Tall ? 2.0f : 1.0f);

        SetPlayerTextureForAllStates((m_Size == Size::Short), m_Color);
    }
    
    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void Player::Falling(Player* player)
    {
        if (!s_ActiveScene->IsBottomCollision(player->m_Entity, s_FallingSpeed))
        {
            player->m_EntityPosition->y -= s_FallingSpeed;
        }
        else
        {
            player->ClearState(State::Falling);
            player->SetState(State::Standing);
        }
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
        *player->m_EntitySubtexture = s_StandingSubtexComp;
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
        player->ClearState(State::Standing);
        *player->m_EntitySubtexture = s_RunningSubtexComp[(player->m_RunningImgIdx++ / 4) % MAX_RUNNING_IMG];
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
                        m_Color = (Color)color;
                        SetPlayerTextureForAllStates((m_Size == Size::Short), (Color)color);
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
            if (PropertyGrid::CheckBox("Tall", tall, 100.0f))
                tall == true ? ChangeSize(Size::Tall) : ChangeSize(Size::Short);
            
            ImGui::Text("State: %d \n Falling :%d \n Jumping: %d \n Standing: %d \n Firing: %d \n Dying: %d \n Sitting: %d \n Running: %d",
                        m_State,
                        m_State & (uint32_t)State::Falling, m_State & (uint32_t)State::Jumping, m_State & (uint32_t)State::Standing, m_State & (uint32_t)State::Firing,
                        m_State & (uint32_t)State::Dying, m_State & (uint32_t)State::Sitting, m_State & (uint32_t)State::Running);
        }
    }


}
