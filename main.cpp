#include <iostream>
#include "cellular_automata.h"
#include "cell.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef enum {CONFIG=0, SIM} SCREENS;

// #define OFFWHITE (Color){0xc3, 0xc3, 0xc3, 255}

float n_f, r_f, it_f;
int n, r, it;

int main()
{
    const int screenWidth = 1000;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "raygui - controls test suite");
    SetTargetFPS(60);

    SCREENS curr = CONFIG;
    cellular_automata* CA = nullptr;
    const int cell_size = 30;

    while (!WindowShouldClose())
    {

        if(curr==SIM && CA && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            CA->step();
            std::cout << "NEXT STEP" << std::endl;
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        switch (curr) {
            case CONFIG:
                // ClearBackground(LIGHTGRAY);

                GuiSliderBar((Rectangle){screenWidth/2.0f-75.0f, 135.0f, 200.0f, 20.0f}, "Size:", NULL, &n_f, 0.0f, 10.0f);
                GuiLabel((Rectangle){screenWidth/2.0f+20.0f, 135.0f, 20, 20}, std::to_string((int)n_f).c_str());

                GuiSliderBar((Rectangle){screenWidth/2.0f-75.0f, 185.0f, 200.0f, 20.0f}, "Rule:", NULL, &r_f, 1.0f, 511.0f);
                GuiLabel((Rectangle){screenWidth/2.0f+20.0f, 185.0f, 20, 20}, std::to_string((int)r_f).c_str());

                GuiSliderBar((Rectangle){screenWidth/2.0f-75.0f, 235.0f, 200.0f, 20.0f}, "Iterations:", NULL, &it_f, 0.0f, 100.0f);
                GuiLabel((Rectangle){screenWidth/2.0f+20.0f, 235.0f, 20, 20}, std::to_string((int)it_f).c_str());

                if(GuiButton((Rectangle){screenWidth/2.0, 285, 80.0f, 20.0f}, "Generate")) {
                    n  = static_cast<int>(n_f);
                    r  = static_cast<int>(r_f);
                    it = static_cast<int>(it_f);
                    CA = new cellular_automata(r, n);

                    curr = SIM;
                };
                break;
            case SIM:
                if(CA) {
                    n = CA->get_size();
                    const float X = screenWidth/2.0f - cell_size*n/2.0f;
                    const float Y = screenWidth/2.0f - cell_size*n/2.0f;
                    // DrawRectangle(X, Y, cell_size*n, cell_size*n, SKYBLUE);
                    for(int i = 0; i <= n; ++i) {
                        DrawLineV((Vector2){X+(i*cell_size), Y }, (Vector2){X+(i*cell_size), Y+(n*cell_size)}, BLUE);
                        DrawLineV((Vector2){X, Y+(i*cell_size)}, (Vector2){X+(n*cell_size), Y+(i*cell_size)}, BLUE);
                    }

                    std::vector<cell> grid = CA->get_grid();
                    for(int i = 0; i < n; ++i) {
                        for (int j = 0; j < n; ++j) {
                            if(grid[CA->get_pos(j, i)].state) {
                                DrawRectangle(X+(j*cell_size), Y+(i*cell_size), cell_size, cell_size, RED);
                                // std::cout << X+(j*cell_size) << ", " << Y+(i*cell_size);
                            }
                        }
                    }
                }

                break;
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}