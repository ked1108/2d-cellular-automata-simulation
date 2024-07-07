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

int n = 2, b = 2, it;
int r;

int main()
{
    const int screenWidth = 1000;
    const int screenHeight = 1000;
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

    bool sizeEditMode = false;
    bool ruleEditMode = false;
    bool baseEditMode = false;
    bool checked = false;
    float X, Y, Yend, Ystart, Xend, Xstart;
    Vector2 mousepos;

    const float posY = 135.0f;
    const float posX =  screenWidth/2.0f - 50.0f;

    std::vector<cell> image;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {

        if(curr==SIM && CA) {
            if(IsKeyPressed(KEY_SPACE)) {
                ++it;
                if(checked) {
                    CA->step(directory+"/"+std::to_string(it)+"_iteration.csv");
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

            if(GuiValueBox((Rectangle){posX, posY, 100.0f, 20.0f}, "Size:    ", &n, 2, 10, sizeEditMode)) sizeEditMode = !sizeEditMode;
            if(GuiValueBox((Rectangle){posX, posY + 30, 100.0f, 20.0f}, "Base:    ", &b, 2, 10, baseEditMode)) baseEditMode = !baseEditMode;
            if(GuiValueBox((Rectangle){posX, posY + 60, 100.0f, 20.0f}, "Rule:    ", &r, 0, pow(b, 9)-1, ruleEditMode)) ruleEditMode = !ruleEditMode;
            GuiCheckBox((Rectangle){posX-40.0f, posY+90, 20.0f, 20.0f}, "Generate Image Output Files", &checked);


            if(GuiButton((Rectangle){posX, posY +120, 80.0f, 20.0f}, "Generate")) {
                if(checked) {
                    directory = "rule"+std::to_string(r);
                    std::filesystem::create_directory(directory);
                    for(int i = 1; i <= n; ++i) {
                        for (int j = 1; j <= n; ++j) {
                            image.emplace_back(0, b, "A"+std::to_string(j)+std::to_string(i));
                        }
                    }
                } else {
                    image = std::vector<cell>(n*n, cell(0, b));
                }

                curr = GRID;
            }
            break;
            case GRID:
                Y = screenWidth/2.0f - cell_size*n/2.0f;
                X = screenWidth/2.0f - cell_size*n/2.0f;

                DrawRectangle(X, Y, cell_size*n, cell_size*n, SKYBLUE);

                for(int i = 0; i <= n; ++i) {
                    DrawLineV((Vector2){X+(i*cell_size), Y }, (Vector2){X+(i*cell_size), Y+(n*cell_size)}, BLUE);
                    DrawLineV((Vector2){X, Y+(i*cell_size)}, (Vector2){X+(n*cell_size), Y+(i*cell_size)}, BLUE);
                }

                for(int i = 0; i < n; ++i) {
                    for(int j = 0; j < n; ++j) {
                        DrawRectangle(X+j*cell_size, Y+i*cell_size, cell_size, cell_size,cols[image[get_pos(j, i, n)].state]);
                    }
                }

                Ystart = screenWidth/2.0f - cell_size*n/2.0f;
                Xstart = screenWidth/2.0f - cell_size*n/2.0f;
                Xend = screenWidth/2.0f + cell_size*n/2.0f;
                Yend = screenWidth/2.0f + cell_size*n/2.0f;

                mousepos = GetMousePosition();
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mousepos.x > Xstart && mousepos.x < Xend && mousepos.y > Ystart && mousepos.y < Yend) {
                    mousepos.x -= Xstart;
                    mousepos.y -= Ystart;

                    int x = floor(mousepos.x/cell_size);
                    int y = floor(mousepos.y/cell_size);
                    int pos = get_pos(x, y, n);

                    image[pos].state = ((image[pos].state + 1) % b + b) % b; //UPDATED TO THE POSITIVE MOD

                    // ADDED SUPPORT FOR MULTIPLE STATES IN PIXELS
                    auto node = image[pos].pixel.extract(image[pos].pixel.begin()->first);
                    node.key()[0] = image[pos].state + 'A';
                    image[pos].pixel.insert(std::move(node));
                }

                if(GuiButton((Rectangle){posX, Yend +100.0f, 80.0f, 20.0f}, "Start")) {
                    it = 0;
                    CA = new cellular_automata(r, n, b, image);
                    if(checked) CA->export_image(directory+"/"+std::to_string(it)+"_iteration.csv");
                    curr = SIM;
                }

                break;
            case SIM:
            if(CA) {
                n = CA->get_size();
                Y = screenWidth/2.0f - cell_size*n/2.0f;
                X = screenWidth/2.0f - cell_size*n/2.0f;
                BeginMode2D(camera);
                DrawRectangle(X, Y, cell_size*n, cell_size*n, SKYBLUE);
                for(int i = 0; i <= n; ++i) {
                    DrawLineV((Vector2){X+(i*cell_size), Y }, (Vector2){X+(i*cell_size), Y+(n*cell_size)}, BLUE); // NOLINT(*-narrowing-conversions)
                    DrawLineV((Vector2){X, Y+(i*cell_size)}, (Vector2){X+(n*cell_size), Y+(i*cell_size)}, BLUE);
                }

                std::vector<cell> grid = CA->get_grid();
                for(int i = 0; i < n; ++i) {
                    for (int j = 0; j < n; ++j) {

                        DrawRectangle(X+(j*cell_size), Y+(i*cell_size), cell_size, cell_size, cols[grid[CA->get_pos(j, i)].state]);

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
int get_pos(int x, int y, int size) {
    return x+size*y;
}


