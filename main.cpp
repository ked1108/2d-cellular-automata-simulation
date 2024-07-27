#include <iostream>
#include "src/cellular_automata.h"
#include "src/cell.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include <filesystem>

#include "raygui.h"

typedef enum {CONFIG=0, GRID, SIM} SCREENS;

std::vector<Color> cols = {VIOLET, PURPLE, DARKBLUE, BLUE, SKYBLUE, DARKGREEN, GREEN, YELLOW, ORANGE, RED};

int get_pos(int x, int y, int size);

int x = 2, y = 2, b = 2, it;
int r;

int main()
{
    SetTraceLogLevel(LOG_ERROR);
    float screenWidth = 700;
    float screenHeight = 700;
    int speed = 4;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Simulate 2D Cellular Automata");

    SetTargetFPS(60);

    Camera2D camera = { 0 };
    Vector2 center = {screenWidth/2.0f, screenHeight/2.0f};
    camera.target = center;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SCREENS curr = CONFIG;
    cellular_automata* CA;
    const int cell_size = 30;

    std::string directory;

    bool sizeXEditMode = false;
    bool sizeYEditMode = false;
    bool ruleEditMode = false;
    bool baseEditMode = false;
    bool checked = false;
    bool toggleGrid = true;
    float X, Y, Yend, Ystart, Xend, Xstart;
    Vector2 mousepos;

    float posY = 135.0f;
    float posX =  screenWidth/2.0f - 50.0f;

    std::vector<cell> image;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        if(screenWidth != GetScreenWidth()) {
            screenWidth = GetScreenWidth();
            center.x = screenWidth/2.0f;
        }
        if(screenHeight != GetScreenHeight()) {
            screenHeight = GetScreenHeight();
            center.y = screenHeight/2.0f;
        }

        posY = 135.0f;
        posX =  screenWidth/2.0f - 50.0f;

        if(curr==SIM && CA) {
            if(IsKeyPressed(KEY_SPACE)) {
                ++it;
                if(checked) {
                    CA->step(directory+"/"+std::to_string(it)+"_iteration.csv");
                } else {
                    CA->step();
                }
            }

            if(IsKeyDown(KEY_W)) center.y -= speed;
            if(IsKeyDown(KEY_A)) center.x -= speed;
            if(IsKeyDown(KEY_S)) center.y += speed;
            if(IsKeyDown(KEY_D)) center.x += speed;

            if(IsKeyDown(KEY_PAGE_UP)) speed += 1;
            if(IsKeyDown(KEY_PAGE_DOWN)) speed = (speed <= 4) ? 4 : --speed;


            if(IsKeyDown(KEY_Q)) camera.zoom -= 0.1f;
            if(IsKeyDown(KEY_E)) camera.zoom += 0.1f;

            camera.zoom += GetMouseWheelMove()*0.05f;
            if (camera.zoom < 0.1f) camera.zoom = 0.05f;
            if (IsKeyPressed(KEY_R))
            {
                camera.zoom = 1.0f;
                camera.target = {screenWidth/2.0f, screenHeight/2.0f};
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

            if(GuiValueBox((Rectangle){posX, posY, 100.0f, 20.0f}, "Rows:    ", &y, 2, 10, sizeYEditMode)) sizeYEditMode = !sizeYEditMode;
            if(GuiValueBox((Rectangle){posX, posY+30, 100.0f, 20.0f}, "Columns:    ", &x, 2, 10, sizeXEditMode)) sizeXEditMode = !sizeXEditMode;
            if(GuiValueBox((Rectangle){posX, posY + 60, 100.0f, 20.0f}, "Z:    ", &b, 2, 10, baseEditMode)) baseEditMode = !baseEditMode;
            if(GuiValueBox((Rectangle){posX, posY + 90, 100.0f, 20.0f}, "Rule:    ", &r, 0, pow(b, 9)-1, ruleEditMode)) ruleEditMode = !ruleEditMode;
            GuiCheckBox((Rectangle){posX-40.0f, posY+ 120, 20.0f, 20.0f}, "Generate Image Output Files", &checked);


            if(GuiButton((Rectangle){posX, posY +150, 80.0f, 20.0f}, "Generate")) {
                if(checked) {
                    directory = "rule"+std::to_string(r);
                    std::filesystem::create_directory(directory);
                    for(int i = 1; i <= y; ++i) {
                        for (int j = 1; j <= x; ++j) {
                            image.emplace_back(0, b, "B"+std::to_string(j)+std::to_string(i));
                        }
                    }
                } else {
                    image = std::vector<cell>(x*y, cell(0, b));
                }

                curr = GRID;
            }
            break;
            case GRID:
                Y = GetScreenHeight()/2.0f - cell_size*y/2.0f;
                X = GetScreenWidth()/2.0f - cell_size*x/2.0f;

                DrawRectangle(X, Y, cell_size*x, cell_size*y, SKYBLUE);

                for(int i = 0; i < y; ++i) {
                    for(int j = 0; j < x; ++j) {
                        DrawRectangle(X+j*cell_size, Y+i*cell_size, cell_size, cell_size,cols[image[get_pos(j, i, x)].state]);
                    }
                }
                for(int i = 0; i <= x; ++i)
                    DrawLineV((Vector2){X+(i*cell_size), Y }, (Vector2){X+(i*cell_size), Y+(y*cell_size)}, BLUE);

                for(int i = 0; i <= y; ++i)
                    DrawLineV((Vector2){X, Y+(i*cell_size)}, (Vector2){X+(x*cell_size), Y+(i*cell_size)}, BLUE);


                Ystart = screenHeight/2.0f - cell_size*y/2.0f;
                Xstart = screenWidth/2.0f - cell_size*x/2.0f;
                Xend = screenWidth/2.0f + cell_size*x/2.0f;
                Yend = screenHeight/2.0f + cell_size*y/2.0f;

                mousepos = GetMousePosition();
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mousepos.x > Xstart && mousepos.x < Xend && mousepos.y > Ystart && mousepos.y < Yend) {
                    mousepos.x -= Xstart;
                    mousepos.y -= Ystart;

                    int j = floor(mousepos.x/cell_size);
                    int i = floor(mousepos.y/cell_size);
                    int pos = get_pos(j, i, x);

                    image[pos].state = ((image[pos].state + 1) % b + b) % b;

                    if(checked) {
                        image[pos].pixel["B"+std::to_string(j+1)+std::to_string(i+1)] = image[pos].state;
                    }
                }

                if(GuiButton((Rectangle){posX, Yend +100.0f, 80.0f, 20.0f}, "Start")) {
                    it = 0;
                    CA = new cellular_automata(r, x, y, b, image);
                    if(checked) CA->export_image(directory+"/"+std::to_string(it)+"_iteration.csv");
                    curr = SIM;
                }

                break;
            case SIM:
            if(CA) {
                Size n = CA->get_size();
                Y = GetScreenHeight()/2.0f - cell_size*n.y/2.0f;
                X = GetScreenWidth()/2.0f - cell_size*n.x/2.0f;

                BeginMode2D(camera);
                DrawRectangle(X, Y, cell_size*n.x, cell_size*n.y, SKYBLUE);

                std::vector<cell> grid = CA->get_grid();
                for(int i = 0; i < n.y; ++i) {
                    for (int j = 0; j < n.x; ++j) {
                        DrawRectangle(X+(j*cell_size), Y+(i*cell_size), cell_size, cell_size, cols[grid[CA->get_pos(j, i)].state]);
                    }
                }

                if (toggleGrid) {
                    for(int i = 0; i <= n.x; ++i)
                        DrawLineV((Vector2){X+(i*cell_size), Y }, (Vector2){X+(i*cell_size), Y+(n.y*cell_size)}, BLUE);

                    for(int i = 0; i <= n.y; ++i)
                        DrawLineV((Vector2){X, Y+(i*cell_size)}, (Vector2){X+(n.x*cell_size), Y+(i*cell_size)}, BLUE);
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

                DrawRectangle( screenWidth - 200, 10, 190, 130, Fade(SKYBLUE, 0.5f));
                DrawRectangleLines( screenWidth - 200, 10, 190, 130, BLUE);
                GuiCheckBox((Rectangle){ screenWidth - 190, 20, 20.0f, 20.0f}, "Toggle Grid Lines", &toggleGrid);
                if(GuiButton((Rectangle){screenWidth - 190, 60, 80.0f, 20.0f}, "Undo")) {
                    CA->undo_step();
                }
                if(GuiButton((Rectangle){screenWidth - 190, 90, 80.0f, 20.0f}, "Reset")) {
                    curr = CONFIG;
                    CA = nullptr;
                    grid.clear();
                    n.x = 2;
                    n.y = 2;
                }
            }

            break;
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
int get_pos(int x, int y, int size) {
    return x+size*y;
}


