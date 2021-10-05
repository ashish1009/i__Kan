// ******************************************************************************
// File         : Background.cpp
// Description  : Storing all entity for Background rendering
// Project      : Mario
//
//  Created by Ashish on 07/05/21.
//  Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Background.h"
#include "MarioLayer.h"

namespace Mario {

    // ******************************************************************************
    // Scene String. each char represnet a entity described below
    // ******************************************************************************
    /********************************************************************************
     0 : END OF LINE TO GET NUMBER OF ELEMENT IN A ROW
     G : Ground
     | : Castel Brick
     o : Castel Gate
     u : castel Gate Domb
     . : Castel Domb
     l : Castel Windlow Left
     r : Castel Window Right
     S : Steps
     - : Bridge
     ! : Pipe Base
     Y : Pipe Head
     X : Bricks
     B : Bonus
     ( : Cloud (
     ^ : Cloud ^
     ) : Cloud )
     < : Grass <
     v : Grass v
     > : Grass >
     { : Forest {
     } : Forest }
     * : Forest *
     1 : Forest 1
     2 : Forest 2
     3 : Forest 3
     ********************************************************************************/
    // TODO: For now making number of row even to make position of y integral
    static std::string s_MapTiles =
    "                                                                                                                                                                                                                                                                                                                            0"
    "                                                                                                                                                                                                                                                                                                                            0"
    "                                  (^)                                                                  (^)                                                                                           (^^^)                                                                         (^^)                                     0"
    "       (^)                                              (^^^)                                                                                  (^^)                                                                                           (^)                                                                  .        0"
    "                                                                                                                                                                                                                                                                                                                  ...       0"
    "                                                                                                                                                                                                                                                                                                                  |u|       0"
    "                                                                                                                                                                                                                                                                                                                  |o|       0"
    "                                                                                                                                                                                                                                                                                                                .......     0"
    "                                               B                  XXXX                             B            XBX                                                                 B                                                                 B                                                         |u|r|u|     0"
    "                                                                                                                                                                                                                                                                                                                |o|||o|     0"
    "                                                                                                                                                                                                                                                                                                              ...........   0"
    "                                                                                                                                                                                                                                                                                                              |||||||||||   0"
    "                                                                                                                                                                                                                                                                                                              |l|u|r|u|l|   0"
    "      XBXBX                                  XBXBX              XBXXXXBX                           S           XXXXX               B                                           B    B    B                                                          XBXBX                                                     |||o|||o|||   0"
    "                                                                                Y                 SS                                                                                                                          S  S                                                       S                  ............... 0"
    "                                                                                !                SSS                                                   S                                                 Y                   SS  SS                                                     SS                  ||||||||||||||| 0"
    "                                                          Y                     !               SSSS                               Y                  SS        *                                        !                  SSS  SSS       *          Y                                SSS                  |u||u||u||u||u| 0"
    "  <v>          S                           <v>      S     !       S  S          !     <vv>     SSSSS                    <vvv>      !          <v>    SSS       {1}                                       !                 SSSS  SSSS     {1}         !         <v>                   SSSS         <v>      |o||o||o||o||0| 0"
    "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG--------------------GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGG--------------------------GGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG0"
    "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG                    GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGG                          GGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG0"
    ;
    
    // ******************************************************************************
    // Copy the tile Scene and replace the following for their behaviour
    // ******************************************************************************
    /********************************************************************************
     c : Single Coin
     C : Multiple Coin
     F : Fire Flower
     M : Mushroom
     S : Star
     - : Nothing
     | : Go inside
     
     ********************************************************************************/
    static std::string s_MapTilesBehaviour =
    "                                                                                                                                                                                                                                                                                                                            0"
    "                                                                                                                                                                                                                                                                                                                            0"
    "                                  (^)                                                                  (^)                                                                                           (^^^)                                                                         (^^)                                     0"
    "       (^)                                              (^^^)                                                                                  (^^)                                                                                           (^)                                                                  .        0"
    "                                                                                                                                                                                                                                                                                                                  ...       0"
    "                                                                                                                                                                                                                                                                                                                  |u|       0"
    "                                                                                                                                                                                                                                                                                                                  |o|       0"
    "                                                                                                                                                                                                                                                                                                                .......     0"
    "                                               F                  --S-                             c            -C-                                                                 F                                                                 c                                                         |u|r|u|     0"
    "                                                                                                                                                                                                                                                                                                                |o|||o|     0"
    "                                                                                                                                                                                                                                                                                                              ...........   0"
    "                                                                                                                                                                                                                                                                                                              |||||||||||   0"
    "                                                                                                                                                                                                                                                                                                              |l|u|r|u|l|   0"
    "      -M-c-                                  -c-c-              -c----c-                           S           XXXXX               c                                           c    c    c                                                          -c-c-                                                     |||o|||o|||   0"
    "                                                                                Y                 SS                                                                                                                          S  S                                                       S                  ............... 0"
    "                                                                                !                SSS                                                   S                                                 Y                   SS  SS                                                     SS                  ||||||||||||||| 0"
    "                                                          Y                     !               SSSS                               Y                  SS        *                                        !                  SSS  SSS       *          Y                                SSS                  |u||u||u||u||u| 0"
    "  <v>          S                           <v>      S     !       S  S          !     <vv>     SSSSS                    <vvv>      !          <v>    SSS       {1}                                       !                 SSSS  SSSS     {1}         !         <v>                   SSSS         <v>      |o||o||o||o||0| 0"
    "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG--------------------GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGG--------------------------GGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG0"
    "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG                    GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGG                          GGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG  GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG0"
    ;


    // ******************************************************************************
    // Get entity name from Char code
    // ******************************************************************************
    static std::string GetEntityNameFromChar(char type)
    {
        switch(type)
        {
            case 'G' : return "Ground"; break;

            case '|' : return "Castel Brick";        break;
            case 'o' : return "Castel Gate";         break;
            case 'u' : return "castel Gate Domb";    break;
            case '.' : return "Castel Domb";         break;
            case 'l' : return "Castel Windlow Left"; break;
            case 'r' : return "Castel Window Right"; break;

            case 'S' : return "Steps";     break;
            case '-' : return "Bridge";    break;
            case '!' : return "Pipe Base"; break;
            case 'Y' : return "Pipe";      break;
            case 'X' : return "Bricks";    break;
            case 'B' : return "Bonus";     break;

            case '<' : return "Grass <";   break;
            case 'v' : return "Grass v";   break;
            case '>' : return "Grass >>";  break;

            case '(' : return "Cloud (";    break;
            case '^' : return "Cloud ^";    break;
            case ')' : return "Cloud )>";   break;

            case '}' : return "Grass }";    break;
            case '{' : return "Grass {";    break;
            case '*' : return "Grass *";    break;
            case '1' : return "Grass 1";    break;
            case '2' : return "Grass 2";    break;
            case '3' : return "Grass 3";    break;
        }
        IK_ASSERT(false, "Invalid Type");
        return "";
    }

    // ******************************************************************************
    // Chnage the subtexture for each tiles in vector
    // ******************************************************************************
    static void ChangeEntitiesSubtex(const std::vector<Entity>& entityVector, const Ref<SubTexture>& subTex)
    {
        for (auto entity : entityVector)
            if (auto &subTexComp = entity.GetComponent<SpriteRendererComponent>().SubTexComp)
                subTexComp = subTex;
    }
    
    // ******************************************************************************
    // Check it tile is having property
    // ******************************************************************************
    static bool IsPropertyTyle(char ch)
    {
        return ch == 'X' || ch == 'B' || ch == 'Y';
    }

    // ******************************************************************************
    // Background data storage
    // ******************************************************************************
    struct BgData
    {
        // ******************************************************************************
        // Tile wrapper from background
        // ******************************************************************************
        struct Tile
        {
            bool IsRigid = true;
            Ref<iKan::SubTexture> SubTexture;
        };

        // Texture to store tile sprite sheet
        Ref<Texture> Texture;

        // Map of vector of subtexture to the Char of their corresponding.
        // Sotring Subtextures of same tile with different color in a vector.
        std::unordered_map<char, std::vector<Ref<SubTexture>>> SubTextureVectorMap;

        // Map of tile (property of tile that stores the subtexture also)
        // to the Char of their corresponding. This map stores the Tile information
        // that will be rendered in background
        // NOTE: s_Data.SubTextureVectorMap is just storing all the colors. These are not
        // getting rendered at all
        std::unordered_map<char, Tile> TileMap;

        // Map of vector of Entity to the Char of their corresponding
        // Storing Entities of samee tiles with different positions in a vector
        std::unordered_map<char, std::vector<Entity>> EntityVectorMap;
    };
    static BgData s_Data;

    glm::vec4 Background::s_BgColor = { 0.3f, 0.4f, 0.6f, 1.0f };

    // ******************************************************************************
    // Initialize the background tiles and creat entities
    // ******************************************************************************
    void Background::CreateEntities(Ref<Scene>& scene)
    {
        auto& sceneData = scene->GetDataRef();
        std::unordered_map<std::string, Ref<Texture>> textureMap = sceneData.TextureMap;

        // Texture tile
        s_Data.Texture = scene->AddTextureToScene("../../../Mario/assets/Resources/Graphics/MarioTile.png");

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
                if (char tileType = s_MapTiles[x + y * mapWidth]; s_Data.TileMap.find(tileType) != s_Data.TileMap.end())
                {
                    auto entity = s_Data.EntityVectorMap[tileType].emplace_back(scene->CreateEntity(GetEntityNameFromChar(tileType)));

                    if (s_Data.TileMap[tileType].IsRigid)
                    {
                        entity.GetComponent<BoxCollider2DComponent>().IsRigid = true;
                    }
                    entity.GetComponent<SceneHierarchyPannelProp>().IsProp = false;

                    auto spriteEntity = entity.AddComponent<SpriteRendererComponent>(s_Data.TileMap[tileType].SubTexture);
                    auto spriteSize   = spriteEntity.SubTexComp->GetSpriteSize();

                    auto& tc = entity.GetComponent<TransformComponent>();

                    tc.Translation = { x, (mapHeight / 2.0f) - y, 0.0f };
                    tc.Scale       = { spriteSize.x, spriteSize.y , 0.0f};
                    
                    // Add Behavious component
                    {
                        if (IsPropertyTyle(tileType))
                            entity.AddComponent<ItemPropertyComponent>(ItemPropertyComponent::GetPropertyFromChar(tileType));
                    }

                    //
//                    entity.AddComponent<NativeScriptComponent>().Bind<ItemScript>(scene, "ItemScript");

                } //if (char tileType = s_MapTiles[x + y * mapWidth]; s_TextureMap.find(tileType) != s_TextureMap.end())
            } // for (uint32_t x = 0; x < mapWidth; x++)
        } // for (uint32_t y = 0; y < mapHeight; y++)
    }

    // ******************************************************************************
    // Imgui update each frame for background
    // ******************************************************************************
    void Background::ImGuiRenderer()
    {
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool tileOpened = ImGui::TreeNodeEx((void*)1234567, flags, "Tile");
        if (tileOpened)
        {
            ImgButtons('G'); // Ground
            ImgButtons('X'); // Bricks
            ImgButtons('B'); // Bonus
            ImgButtons('S'); // Steps
            ImgButtons('-'); // Bridge
            ImgButtons('Y'); // Pipe
            ImgButtons('v'); // Grass
            ImgButtons('^'); // Clouds
            ImgButtons('{'); // Forest

            ImGui::TreePop();
        }
    }

    // ******************************************************************************
    // Imgui interface for chnaging tiles image using buttons
    // ******************************************************************************
    void Background::ImgButtons(const char name)
    {
        if (ImGui::TreeNode(GetEntityNameFromChar(name).c_str()))
        {
            ImTextureID myTexId = (ImTextureID)((size_t)s_Data.Texture->GetRendererID());
            float myTexW = (float)s_Data.Texture->GetWidth();
            float myTexH = (float)s_Data.Texture->GetHeight();

            int32_t i = 0;
            for (auto subTex : s_Data.SubTextureVectorMap[name])
            {
                ImGui::PushID(i);

                auto coords = subTex->GetCoords();
                glm::vec2 uv1 = { (coords.x + 1) * 16.0f, coords.y * 16.0f };
                glm::vec2 uv0 = { coords.x * 16.0f, (coords.y + 1) * 16.0f };
                if (ImGui::ImageButton(myTexId, ImVec2(32.0f, 32.0f), ImVec2(uv0.x / myTexW, uv0.y / myTexH), ImVec2(uv1.x / myTexW, uv1.y / myTexH), 0))
                {
                    ChangeEntitiesSubtex(s_Data.EntityVectorMap[name], subTex);

                    if (name == 'Y')
                    {
                        ChangeEntitiesSubtex(s_Data.EntityVectorMap['!'], s_Data.SubTextureVectorMap['!'][i]);
                    }

                    else if (name == 'v')
                    {
                        ChangeEntitiesSubtex(s_Data.EntityVectorMap['<'], s_Data.SubTextureVectorMap['<'][i]);
                        ChangeEntitiesSubtex(s_Data.EntityVectorMap['>'], s_Data.SubTextureVectorMap['>'][i]);
                    }

                    else if (name == '^')
                    {
                        ChangeEntitiesSubtex(s_Data.EntityVectorMap[')'], s_Data.SubTextureVectorMap[')'][i]);
                        ChangeEntitiesSubtex(s_Data.EntityVectorMap['('], s_Data.SubTextureVectorMap['('][i]);
                    }

                    else if (name == '{')
                    {
                        ChangeEntitiesSubtex(s_Data.EntityVectorMap['*'], s_Data.SubTextureVectorMap['*'][i]);
                        ChangeEntitiesSubtex(s_Data.EntityVectorMap['}'], s_Data.SubTextureVectorMap['}'][i]);
                        ChangeEntitiesSubtex(s_Data.EntityVectorMap['1'], s_Data.SubTextureVectorMap['1'][i]);
                        ChangeEntitiesSubtex(s_Data.EntityVectorMap['2'], s_Data.SubTextureVectorMap['2'][i]);
                        ChangeEntitiesSubtex(s_Data.EntityVectorMap['3'], s_Data.SubTextureVectorMap['3'][i]);
                    }
                }
                ImGui::PopID();
                ImGui::SameLine();
                i++;
            }
            ImGui::NewLine();
            ImGui::TreePop();
        }
    }

    // ******************************************************************************
    // Initialize the background tiles
    // ******************************************************************************
    void Background::Init()
    {
        IK_INFO("Initializing the Background tiles");
        // Ground SubTexutres
        {
            s_Data.SubTextureVectorMap['G'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 27.0f })); // Brown
            s_Data.SubTextureVectorMap['G'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 25.0f })); // Blue
            s_Data.SubTextureVectorMap['G'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 23.0f })); // Grey
            s_Data.SubTextureVectorMap['G'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 21.0f })); // Green

            s_Data.TileMap['G'].SubTexture = s_Data.SubTextureVectorMap['G'][0];
        }

        // Bricks SubTextures
        {
            s_Data.SubTextureVectorMap['X'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 17.0f, 27.0f })); // Brown;
            s_Data.SubTextureVectorMap['X'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 17.0f, 25.0f })); // Blue;
            s_Data.SubTextureVectorMap['X'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 17.0f, 23.0f })); // Grey;
            s_Data.SubTextureVectorMap['X'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 17.0f, 21.0f })); // green;

            s_Data.TileMap['X'].SubTexture = s_Data.SubTextureVectorMap['X'][0];
        }

        // Bonus SubTextures
        {
            // Unused bonus (from 0 -3)
            s_Data.SubTextureVectorMap['B'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 24.0f, 27.0f })); // Brown
            s_Data.SubTextureVectorMap['B'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 24.0f, 25.0f })); // Blue
            s_Data.SubTextureVectorMap['B'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 24.0f, 23.0f })); // Grey
            s_Data.SubTextureVectorMap['B'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 24.0f, 21.0f })); // Green

            // USed bonus (from 4 -7)
            s_Data.SubTextureVectorMap['B'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 27.0f, 27.0f })); // Brown
            s_Data.SubTextureVectorMap['B'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 27.0f, 25.0f })); // Blue
            s_Data.SubTextureVectorMap['B'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 27.0f, 23.0f })); // Grey
            s_Data.SubTextureVectorMap['B'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 27.0f, 21.0f })); // Green

            s_Data.TileMap['B'].SubTexture = s_Data.SubTextureVectorMap['B'][0];
        }

        // Steps SubTextures
        {
            s_Data.SubTextureVectorMap['S'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 26.0f })); // Brown
            s_Data.SubTextureVectorMap['S'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 24.0f })); // Blue
            s_Data.SubTextureVectorMap['S'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 22.0f })); // Grey
            s_Data.SubTextureVectorMap['S'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 20.0f })); // Green

            s_Data.TileMap['S'].SubTexture = s_Data.SubTextureVectorMap['S'][0];
        }

        // Bridge SubTexture
        {
            s_Data.SubTextureVectorMap['-'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 15.0f, 18.5f })); // Green
            s_Data.SubTextureVectorMap['-'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 15.0f, 14.5f })); // Orange
            s_Data.SubTextureVectorMap['-'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 15.0f, 10.5f })); // Grey
            s_Data.SubTextureVectorMap['-'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 15.0f,  8.5f })); // Pink

            s_Data.TileMap['-'].SubTexture = s_Data.SubTextureVectorMap['-'][0];
        }

        // Pipes
        {
            // Heads
            s_Data.SubTextureVectorMap['Y'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 19.0f }, { 2.0f, 1.0f })); // Green
            s_Data.SubTextureVectorMap['Y'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 15.0f }, { 2.0f, 1.0f })); // Orange
            s_Data.SubTextureVectorMap['Y'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 13.0f }, { 2.0f, 1.0f })); // Grey
            s_Data.SubTextureVectorMap['Y'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f,  9.0f }, { 2.0f, 1.0f })); // Pink
            s_Data.TileMap['Y'].SubTexture = s_Data.SubTextureVectorMap['Y'][0];

            // Base
            s_Data.SubTextureVectorMap['!'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 18.0f }, { 2.0f, 1.0f })); // Green
            s_Data.SubTextureVectorMap['!'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 14.0f }, { 2.0f, 1.0f })); // Orange
            s_Data.SubTextureVectorMap['!'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 12.0f }, { 2.0f, 1.0f })); // Grey
            s_Data.SubTextureVectorMap['!'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f,  8.0f }, { 2.0f, 1.0f })); // Pink

            s_Data.TileMap['!'].SubTexture = s_Data.SubTextureVectorMap['!'][0];
        }

        // Grass
        {
            s_Data.SubTextureVectorMap['<'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 11.0f, 18.0f })); // Green
            s_Data.SubTextureVectorMap['<'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 11.0f, 14.0f })); // Orange
            s_Data.SubTextureVectorMap['<'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 11.0f, 12.0f })); // Grey
            s_Data.SubTextureVectorMap['<'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 11.0f,  8.0f })); // Pink

            s_Data.TileMap['<'].SubTexture = s_Data.SubTextureVectorMap['<'][0];
            s_Data.TileMap['<'].IsRigid = false;

            s_Data.SubTextureVectorMap['v'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 12.0f, 18.0f })); // Green
            s_Data.SubTextureVectorMap['v'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 12.0f, 14.0f })); // Orange
            s_Data.SubTextureVectorMap['v'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 12.0f, 12.0f })); // Grey
            s_Data.SubTextureVectorMap['v'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 12.0f,  8.0f })); // Pink

            s_Data.TileMap['v'].SubTexture = s_Data.SubTextureVectorMap['v'][0];
            s_Data.TileMap['v'].IsRigid = false;

            s_Data.SubTextureVectorMap['>'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 13.0f, 18.0f })); // Green
            s_Data.SubTextureVectorMap['>'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 13.0f, 14.0f })); // Orange
            s_Data.SubTextureVectorMap['>'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 13.0f, 12.0f })); // Grey
            s_Data.SubTextureVectorMap['>'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 13.0f, 8.0f  })); // Pink

            s_Data.TileMap['>'].SubTexture = s_Data.SubTextureVectorMap['>'][0];
            s_Data.TileMap['>'].IsRigid = false;
        }

        // Forest Grass
        {
            s_Data.SubTextureVectorMap['{'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 8.0f, 19.0f })); // Green
            s_Data.SubTextureVectorMap['{'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 8.0f, 15.0f })); // Orange
            s_Data.SubTextureVectorMap['{'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 8.0f, 13.0f })); // Grey
            s_Data.SubTextureVectorMap['{'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 8.0f,  9.0f })); // Pink

            s_Data.TileMap['{'].SubTexture = s_Data.SubTextureVectorMap['{'][0];
            s_Data.TileMap['{'].IsRigid = false;

            s_Data.SubTextureVectorMap['}'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 10.0f, 19.0f })); // Green
            s_Data.SubTextureVectorMap['}'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 10.0f, 15.0f })); // Orange
            s_Data.SubTextureVectorMap['}'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 10.0f, 13.0f })); // Grey
            s_Data.SubTextureVectorMap['}'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 10.0f,  9.0f })); // Pink

            s_Data.TileMap['}'].SubTexture = s_Data.SubTextureVectorMap['}'][0];
            s_Data.TileMap['}'].IsRigid = false;


            s_Data.SubTextureVectorMap['*'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 9.0f, 19.0f })); // Green
            s_Data.SubTextureVectorMap['*'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 9.0f, 15.0f })); // Orange
            s_Data.SubTextureVectorMap['*'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 9.0f, 13.0f })); // Grey
            s_Data.SubTextureVectorMap['*'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 9.0f,  9.0f })); // Pink

            s_Data.TileMap['*'].SubTexture = s_Data.SubTextureVectorMap['*'][0];
            s_Data.TileMap['*'].IsRigid = false;

            s_Data.SubTextureVectorMap['1'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 8.0f, 18.0f })); // Green
            s_Data.SubTextureVectorMap['1'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 8.0f, 14.0f })); // Orange
            s_Data.SubTextureVectorMap['1'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 8.0f, 12.0f })); // Grey
            s_Data.SubTextureVectorMap['1'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 8.0f,  8.0f })); // Pink

            s_Data.TileMap['1'].SubTexture = s_Data.SubTextureVectorMap['1'][0];
            s_Data.TileMap['1'].IsRigid = false;

            s_Data.SubTextureVectorMap['2'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 9.0f, 18.0f })); // Green
            s_Data.SubTextureVectorMap['2'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 9.0f, 14.0f })); // Orange
            s_Data.SubTextureVectorMap['2'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 9.0f, 12.0f })); // Grey
            s_Data.SubTextureVectorMap['2'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 9.0f,  8.0f })); // Pink

            s_Data.TileMap['2'].SubTexture = s_Data.SubTextureVectorMap['2'][0];
            s_Data.TileMap['2'].IsRigid = false;

            s_Data.SubTextureVectorMap['3'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 10.0f, 18.0f })); // Green
            s_Data.SubTextureVectorMap['3'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 10.0f, 14.0f })); // Orange
            s_Data.SubTextureVectorMap['3'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 10.0f, 12.0f })); // Grey
            s_Data.SubTextureVectorMap['3'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 10.0f,  8.0f })); // Pink

            s_Data.TileMap['3'].SubTexture = s_Data.SubTextureVectorMap['3'][0];
            s_Data.TileMap['3'].IsRigid = false;
        }

        // Cloud
        {
            s_Data.SubTextureVectorMap['('].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 0.0f }, { 1.0f, 2.0f })); // Red
            s_Data.SubTextureVectorMap['('].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 2.0f }, { 1.0f, 2.0f })); // Red
            s_Data.SubTextureVectorMap['('].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 0.0f }, { 1.0f, 2.0f })); // Grey
            s_Data.SubTextureVectorMap['('].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 0.0f, 2.0f }, { 1.0f, 2.0f })); // Blue

            s_Data.TileMap['('].SubTexture = s_Data.SubTextureVectorMap['('][0];
            s_Data.TileMap['('].IsRigid = false;

            s_Data.SubTextureVectorMap['^'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 1.0f, 0.0f }, { 1.0f, 2.0f })); // Red
            s_Data.SubTextureVectorMap['^'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 1.0f, 2.0f }, { 1.0f, 2.0f })); // Red
            s_Data.SubTextureVectorMap['^'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 1.0f, 0.0f }, { 1.0f, 2.0f })); // Grey
            s_Data.SubTextureVectorMap['^'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 1.0f, 2.0f }, { 1.0f, 2.0f })); // Blue

            s_Data.TileMap['^'].SubTexture = s_Data.SubTextureVectorMap['^'][0];
            s_Data.TileMap['^'].IsRigid = false;

            s_Data.SubTextureVectorMap[')'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 2.0f, 0.0f }, { 1.0f, 2.0f })); // Red
            s_Data.SubTextureVectorMap[')'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 2.0f, 2.0f }, { 1.0f, 2.0f })); // Red
            s_Data.SubTextureVectorMap[')'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 2.0f, 0.0f }, { 1.0f, 2.0f })); // Grey
            s_Data.SubTextureVectorMap[')'].emplace_back(SubTexture::CreateFromCoords(s_Data.Texture, { 2.0f, 2.0f }, { 1.0f, 2.0f })); // Blue

            s_Data.TileMap[')'].SubTexture = s_Data.SubTextureVectorMap[')'][0];
            s_Data.TileMap[')'].IsRigid = false;
        }

        // Castel
        {
            s_Data.TileMap['.'].SubTexture = SubTexture::CreateFromCoords(s_Data.Texture, { 19.0f, 25.0f });
            s_Data.TileMap['.'].IsRigid = false;

            s_Data.TileMap['u'].SubTexture = SubTexture::CreateFromCoords(s_Data.Texture, { 20.0f, 24.0f });
            s_Data.TileMap['u'].IsRigid = false;

            s_Data.TileMap['o'].SubTexture = SubTexture::CreateFromCoords(s_Data.Texture, { 21.0f, 24.0f });
            s_Data.TileMap['o'].IsRigid = false;

            s_Data.TileMap['|'].SubTexture = SubTexture::CreateFromCoords(s_Data.Texture, { 21.0f, 25.0f });
            s_Data.TileMap['|'].IsRigid = false;

            s_Data.TileMap['l'].SubTexture = SubTexture::CreateFromCoords(s_Data.Texture, { 20.0f, 25.0f });
            s_Data.TileMap['l'].IsRigid = false;

            s_Data.TileMap['r'].SubTexture = SubTexture::CreateFromCoords(s_Data.Texture, { 22.0f, 25.0f });
            s_Data.TileMap['r'].IsRigid = false;
        }
    }
    
    void ItemScript::OnCollision(Entity &colloidedEntity)
    {
        if (m_Entity.HasComponent<ItemPropertyComponent>())
        {
            m_ActiveScene->DestroyEntity(m_Entity);
        }
    }

}
