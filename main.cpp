#include "raylib.h"

struct AnimData 
{
    Rectangle rec{};
    Vector2 pos{};
    int frame{};
    float updateTime{};
    float runningTime{};
};

// update animation frames
void updateAnimData(AnimData& data, float deltaTime, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime) 
    {
        data.runningTime = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        ++data.frame;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
}

// checks if scarfy is on the ground
bool isOnGround(AnimData& data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

void setVolume(Music& music, float& volume) 
{
    if (IsKeyPressed(KEY_V))
    {
        if (volume > 0.1) 
            SetMusicVolume(music, volume -= 0.1);
    }
    if (IsKeyPressed(KEY_B))
    {
        if (volume < 1.0)
            SetMusicVolume(music, volume += 0.1);
    }
}

void checkIfPaused(Music& music, bool& pause)
{
    if (IsKeyPressed(KEY_P))
    {
        pause = !pause;
        if (pause) 
            PauseMusicStream(music);
        else
            ResumeMusicStream(music);
    }
}

int main() 
{
    // Window Specs
    int windowDimensions[2] {512, 380};
    InitWindow(windowDimensions[0], windowDimensions[1],"Dapper Dasher");
    SetTargetFPS(60);

    // Acceleration due to gravity (pixels/s)/s
    const int Gravity{1'000};

    Texture2D scarfy {LoadTexture("textures/scarfy.png")};
    Texture2D nebula {LoadTexture("textures/12_nebula_spritesheet.png")};
    Texture2D background {LoadTexture("textures/far-buildings.png")};
    Texture2D midground {LoadTexture("textures/back-buildings.png")};
    Texture2D foreground {LoadTexture("textures/foreground.png")};
    float bgPosX{};
    float mgPosX{};
    float fgPosX{};
    // loadMusic
    InitAudioDevice();
    Music music {LoadMusicStream("music/snakebeat.mp3")};
    PlayMusicStream(music);
    float volume{0.2};
    SetMusicVolume(music,volume);
    bool pause{false};


    // nebulae array
    const int maxNebulae{20};
    AnimData nebulae[maxNebulae] {};

    // initialize each element in the nebulae array
    for (int i = 0; i < maxNebulae; ++i)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8.f;
        nebulae[i].rec.height = nebula.height/8.f;
        nebulae[i].pos.x = windowDimensions[0] + i*300;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8.f;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.f;
        nebulae[i].updateTime = 1.f/12.f;
    }

    float finishLine{nebulae[maxNebulae-1].pos.x};

    // AnimData for scarfy
    AnimData scarfyData {
        {0, 0, scarfy.width/6.f, scarfy.height/1.f},
        {windowDimensions[0]/2 - scarfyData.rec.width/2, windowDimensions[1] - scarfyData.rec.height},
        0, 
        1.0/12.0, 
        0.f
    };

    // nebula X velocity (pixels/second)
    int nebVelocity{-200};
    // scarfy velocity (pixels/second)
    int scarfyVelocity{0};
    // Jump velocity (pixels/second)
    const int jumpVelocity{-600};

    // Prevent Air Jumping
    bool isInAir{false};

    // collision check
    bool collision{false};


    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(BLACK);
        const float deltaTime {GetFrameTime()};
        // Game Logic Begins

        UpdateMusicStream(music);
        checkIfPaused(music, pause);
        setVolume(music, volume);

        // update scarfy animation frame
        if (!isInAir)
            updateAnimData(scarfyData, deltaTime, 5);

        // update animation frame for all nebula
        for (int i = 0; i < maxNebulae; ++i)
            updateAnimData(nebulae[i], deltaTime, 7);

        // Scarfy Ground Check
        if (isOnGround(scarfyData, windowDimensions[1]))
        {
            scarfyVelocity = 0;                                                  // reset velocity
            scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;      // reset scarfy ground position
            isInAir = false;
        }
        else                                                                     // scarfy is in the air
        {
            scarfyVelocity += Gravity * deltaTime; 
            isInAir = true;
        }

        // Scarfy Jump Action
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            scarfyVelocity = jumpVelocity;
        }


        // update scarfy position
        scarfyData.pos.y += scarfyVelocity * deltaTime;

        // draw the bacgkround
        bgPosX -= 20 * deltaTime;
        if (bgPosX <= -background.width*2)
            bgPosX = 0.0;
        Vector2 bg1Pos{bgPosX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{bgPosX + background.width*2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        // draw the midground
        mgPosX -= 40 * deltaTime;
        if (mgPosX <= -midground.width*2)
            mgPosX = 0.0;
        Vector2 mg1Pos{mgPosX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgPosX + midground.width*2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        // draw the foreground
        fgPosX -= 80 * deltaTime;
        if (fgPosX <= -foreground.width*2)
            fgPosX = 0.0;
        Vector2 fg1Pos{fgPosX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgPosX + foreground.width*2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        
        // // update each nebulae position
        for (auto i = 0; i < maxNebulae; ++i)
        {
            nebulae[i].pos.x += nebVelocity * deltaTime;  
        }

        // update position of finishLine
        finishLine = nebulae[maxNebulae-1].pos.x;

        // collision detection
        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec {
                nebula.pos.x + pad, 
                nebula.pos.y + pad, 
                nebula.rec.width - 2*pad, 
                nebula.rec.height - 2*pad
            };
            Rectangle scarfyRec {
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }

        if (!collision)
        {
            // draw the nebula
            for (auto i = 0; i < maxNebulae; ++i)
            {
                if (i % 2 == 0) 
                    DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
                else 
                    DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, RED);
            }
            // draw scarfy if !collision
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);

            if (scarfyData.pos.x >= finishLine)
            {
                // Win the game
                DrawText("YOU WIN!", windowDimensions[0]/2 - 90, windowDimensions[1]/2 - 20, 40, YELLOW);
            } 
        }
        else
        {
            // lose the game
            DrawText("GAME OVER", windowDimensions[0]/2 - 120, windowDimensions[1]/2 - 20, 40, YELLOW);
        }

        DrawText(FormatText("Velocity: %i", scarfyVelocity), 20, 20, 20, RED);
        // DrawText(FormatText("DeltaTime: %i", deltaTime), 20, 40, 20, YELLOW);
        // DrawText(FormatText("UpdateTime: %i", scarfyData.updateTime), 20, 60, 20, GREEN);
        // DrawText(FormatText("RunningTime: %i", scarfyData.runningTime), 20, 80, 20, BLUE);

        // Game Logic Ends
        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    UnloadMusicStream(music);
    CloseWindow();
}