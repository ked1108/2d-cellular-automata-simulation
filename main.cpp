#include <iostream>
#include "cellular_automata.h"
#include "cell.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include <filesystem>

#include "raygui.h"

typedef enum {CONFIG=0, SIM} SCREENS;

// #define OFFWHITE (Color){0xc3, 0xc3, 0xc3, 255}

int n, r, it;
int main()
{
    const int screenWidth = 500;
    const int screenHeight = 500;
    InitWindow(screenWidth, screenHeight, "Simulate 2D Cellular Automata");
    SetTargetFPS(60);

    Camera2D camera = { 0 };
    Vector2 center = {screenWidth/2.0f, screenHeight/2.0f};
    camera.target = center;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SCREENS curr = CONFIG;
    cellular_automata* CA = nullptr;
    const int cell_size = 30;

    std::string directory;

    bool sizeEditMode = false;
    bool ruleEditMode = false;
    bool checked = false;
    float X, Y;

    const float posY = 350.0f;
    const float posX =  screenWidth/2.0f - 50.0f;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {


        if(curr==SIM && CA) {
            if(IsKeyPressed(KEY_SPACE)) {
                ++it;
                if(checked) {
                    CA->step(directory+"/"+std::to_string(it)+"_"+"iteration.csv");
                } else {
                    CA->step();
                }
            }
            if(IsKeyDown(KEY_W)) center.y -= 4;
            if(IsKeyDown(KEY_A)) center.x -= 4;
            if(IsKeyDown(KEY_S)) center.y += 4;
            if(IsKeyDown(KEY_D)) center.x += 4;
            if(IsKeyDown(KEY_Q)) camera.zoom -= 0.1f;
            if(IsKeyDown(KEY_E)) camera.zoom += 0.1f;
            camera.zoom += ((float)GetMouseWheelMove()*0.05f);
            if (camera.zoom < 0.1f) camera.zoom = 0.05f;
            if (IsKeyPressed(KEY_R))
            {
                camera.zoom = 1.0f;
                camera.target = {screenWidth/2.0, screenHeight/2.0};
            }
            camera.target = center;
        }


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        switch (curr) {
            case CONFIG:
                // ClearBackground(LIGHTGRAY);

            if(GuiValueBox((Rectangle){posX, posY, 100.0f, 20.0f}, "Size:    ", &n, 0.0f, 10.0f, sizeEditMode)) sizeEditMode = !sizeEditMode;
            if(GuiValueBox((Rectangle){posX, posY + 30, 100.0f, 20.0f}, "Rule:    ", &r, 0.0f, 511.0f, ruleEditMode)) ruleEditMode = !ruleEditMode;
            GuiCheckBox((Rectangle){posX-40.0f, posY+60, 20.0f, 20.0f}, "Generate Image Output Files", &checked);
            Y = screenWidth/2.0f - cell_size*n/2.0f - 50.0f;
            X = screenWidth/2.0f - cell_size*n/2.0f;

            for(int i = 0; i <= n; ++i) {
                DrawLineV((Vector2){X+(i*cell_size), Y }, (Vector2){X+(i*cell_size), Y+(n*cell_size)}, BLUE);
                DrawLineV((Vector2){X, Y+(i*cell_size)}, (Vector2){X+(n*cell_size), Y+(i*cell_size)}, BLUE);
            }
            if(GuiButton((Rectangle){posX, posY +90, 80.0f, 20.0f}, "Generate")) {
                it = 0;
                directory = "rule"+std::to_string(r);
                if(checked) std::filesystem::create_directory(directory);
                CA = new cellular_automata(r, n);

                curr = SIM;
            };
            break;
            case SIM:
            if(CA) {
                n = CA->get_size();
                Y = screenWidth/2.0f - cell_size*n/2.0f;
                X = screenWidth/2.0f - cell_size*n/2.0f;
                BeginMode2D(camera);
                DrawRectangle(X, Y, cell_size*n, cell_size*n, SKYBLUE);
                for(int i = 0; i <= n; ++i) {
                    DrawLineV((Vector2){X+(i*cell_size), Y }, (Vector2){X+(i*cell_size), Y+(n*cell_size)}, BLUE);
                    DrawLineV((Vector2){X, Y+(i*cell_size)}, (Vector2){X+(n*cell_size), Y+(i*cell_size)}, BLUE);
                }

                std::vector<cell> grid = CA->get_grid();
                for(int i = 0; i < n; ++i) {
                    for (int j = 0; j < n; ++j) {
                        if(grid[CA->get_pos(j, i)].state) {
                            DrawRectangle(X+(j*cell_size), Y+(i*cell_size), cell_size, cell_size, RED);
                        }
                    }
                }
                EndMode2D();
                std::string text = "Iteration:    "+std::to_string(it);
                DrawRectangle( 10, 10, 250, 113, Fade(SKYBLUE, 0.5f));
                DrawRectangleLines( 10, 10, 250, 113, BLUE);

                DrawText(text.c_str(), 20, 20, 10, BLACK);
                DrawText("- W/A/S/D to Move Around The Grid", 40, 40, 10, DARKGRAY);
                DrawText("- Mouse Wheel or Q/E to Zoom in-out", 40, 60, 10, DARKGRAY);
                DrawText("- SPACE to move to the next state", 40, 80, 10, DARKGRAY);
                DrawText("- R to reset Zoom and Rotation", 40, 100, 10, DARKGRAY);

            }

            break;
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}