// ******************************************************************************
// File         : Player.cpp
// Descrition   : Player Class info
// Project      : Mario
//
// Created by Ashish on 10/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "PlayerController.h"

namespace Mario {
    
    Ref<Scene>      PlayerController::s_ActiveScene        = nullptr;
    Ref<Texture>    PlayerController::s_Texture            = nullptr;
    Ref<SubTexture> PlayerController::s_StandingSubtexComp = nullptr;
    
    Ref<SubTexture> PlayerController::s_RunningSubtexComp[PlayerController::MAX_RUNNING_IMG];
        
    float PlayerController::s_RunningSpeed = 0.10;
    float PlayerController::s_FallingSpeed = 0.50;
    float PlayerController::s_JumpingSpeed = 0.25;

    std::function <void(PlayerController*)> PlayerController::s_StateFunc[PlayerController::MAX_STATES];

    // ******************************************************************************
    // Player Constructor
    // ******************************************************************************
    PlayerController::PlayerController(Ref<Scene>& scene)
    : ScriptableEntity(scene)
    {
        PlayerController::Init(scene, (m_Size == Size::Short), m_Color);
    
        // Creating Player Entity
        IK_INFO("Mario Player Constructor called");
    }

    // ******************************************************************************
    // Player Destructor
    // ******************************************************************************
    PlayerController::~PlayerController()
    {
        IK_WARN("Mario Player Destructor called");
    }
    
    // ******************************************************************************
    // Create the Player Script Instance
    // ******************************************************************************
    void PlayerController::OnCreate()
    {
        m_Created = true;
        
        m_EntityPosition   = &m_Entity.GetComponent<TransformComponent>().Translation;
        m_EntitySize       = &m_Entity.GetComponent<TransformComponent>().Scale;
        m_EntitySubtexture = &m_Entity.GetComponent<SpriteRendererComponent>().SubTexComp;
        
        auto cameraEntity = s_ActiveScene->GetMainCameraEntity();
        m_CameraRefPos    = &cameraEntity.GetComponent<TransformComponent>().Translation.x;
        
        // TODO: Temp Delete later
        m_EditorCameraRefPos = &s_ActiveScene->GetEditorCameraEntity().GetComponent<TransformComponent>().Translation.x;
    }
    
    // ******************************************************************************
    // Initialize the player
    // ******************************************************************************
    void PlayerController::Init(Ref<Scene> scene, bool isShort, Color color)
    {
        // Setting Scene
        s_ActiveScene = scene;
        
        // Setting base texture
        s_Texture = scene->AddTextureToScene("../../../Mario/assets/Resources/Graphics/Player.png");
        
        // Setting all state callback function
        s_StateFunc[PlayerController::GetFirstSetBit((int32_t)State::Falling)]  = PlayerController::Falling;
        s_StateFunc[PlayerController::GetFirstSetBit((int32_t)State::Jumping)]  = PlayerController::Jumping;
        s_StateFunc[PlayerController::GetFirstSetBit((int32_t)State::Standing)] = PlayerController::Standing;
        s_StateFunc[PlayerController::GetFirstSetBit((int32_t)State::Firing)]   = PlayerController::Firing;
        s_StateFunc[PlayerController::GetFirstSetBit((int32_t)State::Dying)]    = PlayerController::Dying;
        s_StateFunc[PlayerController::GetFirstSetBit((int32_t)State::Sitting)]  = PlayerController::Sitting;
        s_StateFunc[PlayerController::GetFirstSetBit((int32_t)State::Running)]  = PlayerController::Running;
        
        // Set all the textures absed on size and color
        SetPlayerTextureForAllStates(isShort, color);
    }
    
    // ******************************************************************************
    // Spaercial function to get the fist set bit returns -1 if value is 0
    // 0 means Most LSB and So on...
    // ******************************************************************************
    int32_t PlayerController::GetFirstSetBit(uint32_t value)
    {
        for (int32_t pos = 0; value >> pos; pos++)
            if (value & BIT(pos))
                return pos;
        
        return -1;
    }
    
    // ******************************************************************************
    // Update the texture for all state
    // ******************************************************************************
    void PlayerController::SetPlayerTextureForAllStates(bool isShort, Color color)
    {
        s_StandingSubtexComp = SubTexture::CreateFromCoords(s_Texture, { 6.0f, (float)color + (isShort ? 0.0f : 1.0f) }, { 1.0f, (isShort ? 1.0f : 2.0f) } );
        
        for (uint32_t imgIdx = 0; imgIdx < PlayerController::MAX_RUNNING_IMG; imgIdx++)
            s_RunningSubtexComp[imgIdx] = SubTexture::CreateFromCoords(s_Texture, { imgIdx, (float)color + (isShort ? 0.0f : 1.0f) }, { 1.0f, (isShort ? 1.0f : 2.0f) } );
    }
    
    // ******************************************************************************
    // Update the player each frame
    // ******************************************************************************
    void PlayerController::OnUpdate(Timestep ts)
    {
        // By default player should be falling until it colloid with obstacle. If it is jumping then avoid the state of Falling
        (IsState(State::Jumping)) ? ClearState(State::Falling) : SetState(State::Falling);
        
        // TODO: Move them to Init : Some issues found so adding if statement to hit this for just once
        static bool temp = true;
        if (temp)
        {
//            m_EntityPosition   = &m_Entity.GetComponent<TransformComponent>().Translation;
//            m_EntitySize       = &m_Entity.GetComponent<TransformComponent>().Scale;
//            m_EntitySubtexture = &m_Entity.GetComponent<SpriteRendererComponent>().SubTexComp;
//
//            auto cameraEntity = s_ActiveScene->GetMainCameraEntity();
//            m_CameraRefPos    = &cameraEntity.GetComponent<TransformComponent>().Translation.x;
//
//            // TODO: Temp Delete later
//            m_EditorCameraRefPos = &s_ActiveScene->GetEditorCameraEntity().GetComponent<TransformComponent>().Translation.x;
            
            temp = true;
        }
        
        // TODO: Move to Some other function or may be Scrip Component
        {
            if (Input::IsKeyPressed(KeyCode::Right) && !s_ActiveScene->IsRightCollision(m_Entity, s_RunningSpeed))
            {
                SetState(State::Running);
                m_EntitySize->x = 1.0f;
                m_EntityPosition->x += s_RunningSpeed;
                *m_CameraRefPos += s_RunningSpeed;
                *m_EditorCameraRefPos += s_RunningSpeed;
                m_Direction = Direction::Right;
            }
            if (Input::IsKeyPressed(KeyCode::Left) && !s_ActiveScene->IsLeftCollision(m_Entity, s_RunningSpeed))
            {
                SetState(State::Running);
                m_EntitySize->x = -1.0f;
                m_EntityPosition->x -= s_RunningSpeed;
                *m_CameraRefPos -= s_RunningSpeed;
                *m_EditorCameraRefPos -= s_RunningSpeed;
                m_Direction = Direction::Left;
            }
        }
        
        // Check if player fallen to death
        {
            if (m_EntityPosition->y <= -9.0f)
            {
                m_Life--;
                // Save scene automatically then open that sceen on death
                
                m_EntityPosition->x = 0.0f;
                m_EntityPosition->y = 0.0f;
                Viewport::Get().SaveSceneAs("../../../Mario/assets/Scene/Mario.iKan");
                Viewport::Get().OpenScene("../../../Mario/assets/Scene/Mario.iKan");
            }
        }
        
        for (int32_t stateIds = 0; stateIds < MAX_STATES; stateIds++)
        {
            int32_t fncIdx = PlayerController::GetFirstSetBit(m_State & BIT(stateIds));
            if (fncIdx >= 0)
                s_StateFunc[fncIdx](this);
        }
    }
    
    
    // ******************************************************************************
    // Player Event handler
    // ******************************************************************************
    void PlayerController::OnEvent(iKan::Event &event)
    {
        EventDispatcher dispather(event);
        dispather.Dispatch<KeyPressedEvent>(IK_BIND_EVENT_FN(PlayerController::OnkeyPressed));
        dispather.Dispatch<KeyReleasedEvent>(IK_BIND_EVENT_FN(PlayerController::OnKeyReleased));
    }
    
    // ******************************************************************************
    // Player Key pressed event handler
    // ******************************************************************************
    bool PlayerController::OnkeyPressed(KeyPressedEvent& event)
    {
        if (event.GetKeyCode() == KeyCode::Z && (!IsState(State::Falling) && !IsState(State::Jumping)))
        {
            m_StartingJumpingPosing = m_EntityPosition->y;
            
            ClearState(State::Standing);
            ClearState(State::Falling);
            SetState(State::Jumping);
        }
        return false;
    }

    // ******************************************************************************
    // Player Key released event handler
    // ******************************************************************************
    bool PlayerController::OnKeyReleased(KeyReleasedEvent& event)
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
    void PlayerController::ChangeSize(Size size)
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
    void PlayerController::Falling(PlayerController* player)
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
    void PlayerController::Jumping(PlayerController* player)
    {
        if (!s_ActiveScene->IsTopCollision(player->m_Entity, s_JumpingSpeed) && (player->m_EntityPosition->y - player->m_StartingJumpingPosing < MAX_JUMPING_HEIGHT))
        {
            player->m_EntityPosition->y += s_JumpingSpeed;
        }
        else
        {
            player->ClearState(State::Jumping);
            player->SetState(State::Falling);
        }
    }

    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void PlayerController::Standing(PlayerController* player)
    {
        *player->m_EntitySubtexture = s_StandingSubtexComp;
    }

    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void PlayerController::Firing(PlayerController* player)
    {
        
    }

    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void PlayerController::Dying(PlayerController* player)
    {
        
    }

    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void PlayerController::Sitting(PlayerController* player)
    {
        
    }

    // ******************************************************************************
    // Player function for falling
    // ******************************************************************************
    void PlayerController::Running(PlayerController* player)
    {
        player->ClearState(State::Standing);
        
        if (!(player->IsState(State::Jumping)))
            *player->m_EntitySubtexture = s_RunningSubtexComp[(player->m_RunningImgIdx++ / 4) % MAX_RUNNING_IMG];
    }
    
    // ******************************************************************************
    // Imgui renderer for Imgui
    // ******************************************************************************
    void PlayerController::ImguiRenderer()
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
            
            ImGui::Text("%f", m_StartingJumpingPosing);
        }
    }
    
    // ******************************************************************************
    // Collision Callback
    // ******************************************************************************
    void PlayerController::OnCollision(Entity& colloidedEntity)
    {
        
    }

}
