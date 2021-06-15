// ******************************************************************************
// File         : StartScreen.h
// Description  : Storing all entity for Start Sceen rendering
// Project      : Mario
//
//  Created by Ashish on 15/06/21.
//  Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "StartScreen.h"

namespace Mario {

    Ref<Texture> StartScreen::s_Texture = nullptr;
    std::unordered_map<char, Ref<SubTexture>> StartScreen::s_TileMap;

    static std::string s_MapTiles =
//              I__K A N
    "aggggggggggggggggggggggggggggggggb0"
    "e          I  JJ  yzG            h0"
    "e          w  JlyzJJJ            h0"
    "e          w  JJJvJCH            h0"
    "e          N  NNJJNOP            h0"
    "e            yuzMwMzIyz          h0"
    "e            JxxJvJlwww          h0"
    "e            JJJNNJtwCD          h0"
    "cffffffffffffnnnffnnnEFffffffffffd0"
    ;

    // ******************************************************************************
    // Initialize the background tiles and creat entities
    // ******************************************************************************
    void StartScreen::CreateEntities(Ref<Scene>& scene)
    {
        auto& sceneData = scene->GetDataRef();
        std::unordered_map<std::string, Ref<Texture>> textureMap = sceneData.TextureMap;

        // Texture tile
        s_Texture = scene->AddTextureToScene("../../../Mario/assets/Resources/Graphics/StartScreen/StartScreenTile.png");

        // Initialise the tiles and map
        Init();

        // Extract the map width. MAP Width should be same for each New line string
        size_t mapWidth    = s_MapTiles.find_first_of('0') + 1;
        uint32_t mapHeight = static_cast<uint32_t>(strlen(s_MapTiles.c_str())) / mapWidth;

        // Creating entity for each tiles
        IK_INFO("Creating Entity for each tiles");
        for (uint32_t y = 0; y < mapHeight; y++)
        {
            for (uint32_t x = 0; x < mapWidth; x++)
            {
                if (char tileType = s_MapTiles[x + y * mapWidth]; s_TileMap.find(tileType) != s_TileMap.end())
                {
                    auto entity = scene->CreateEntity();
                    entity.GetComponent<SceneHierarchyPannelProp>().IsProp = false;

                    auto spriteEntity = entity.AddComponent<SpriteRendererComponent>(s_TileMap[tileType]);
                    auto spriteSize   = spriteEntity.SubTexComp->GetSpriteSize();

                    auto& tc = entity.GetComponent<TransformComponent>();

                    tc.Translation = { x, (mapHeight / 2.0f) - y, 0.0f };
                    tc.Scale       = { spriteSize.x, spriteSize.y , 0.0f};
                } //if (char tileType = s_MapTiles[x + y * mapWidth]; s_TextureMap.find(tileType) != s_TextureMap.end())
            } // for (uint32_t x = 0; x < mapWidth; x++)
        } // for (uint32_t y = 0; y < mapHeight; y++)
    }

    // ******************************************************************************
    // Initialize the start screen tiles
    // ******************************************************************************
    void StartScreen::Init()
    {
        for (size_t i = 0; i < 10; i++)
            s_TileMap['a' + (char)i] = SubTexture::CreateFromCoords(s_Texture, { float(i), 0.0f }, { 1.0f, 1.0f }, { 8.0f, 8.0f });

        s_TileMap['N'] = SubTexture::CreateFromCoords(s_Texture, { 11.0f, 0.0f }, { 1.0f, 1.0f }, { 8.0f, 8.0f });
        s_TileMap['O'] = SubTexture::CreateFromCoords(s_Texture, { 12.0f, 0.0f }, { 1.0f, 1.0f }, { 8.0f, 8.0f });
        s_TileMap['P'] = SubTexture::CreateFromCoords(s_Texture, { 13.0f, 0.0f }, { 1.0f, 1.0f }, { 8.0f, 8.0f });

        for (size_t i = 0; i < 13; i++)
            s_TileMap['l' + (char)i] = SubTexture::CreateFromCoords(s_Texture, { float(i), 1.0f }, { 1.0f, 1.0f }, { 8.0f, 8.0f });

        for (size_t i = 0; i < 2; i++)
            s_TileMap['y' + (char)i] = SubTexture::CreateFromCoords(s_Texture, { float(i), 2.0f }, { 1.0f, 1.0f }, { 8.0f, 8.0f });

        for (size_t i = 0; i < 13; i++)
            s_TileMap['A' + (char)i] = SubTexture::CreateFromCoords(s_Texture, { float(i + 2), 2.0f }, { 1.0f, 1.0f }, { 8.0f, 8.0f });

        s_TileMap[' '] = s_TileMap['j'];

    }

}
