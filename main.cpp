#include "main.h"
#include <filesystem>

#include "raylib.h"
#include "src/cellular_automata.h"
#include "src/cell.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

State state;
RenderTexture2D tex;
cellular_automata* CA;

std::vector<cell> image;

bool sizeXEditMode = false;
bool sizeYEditMode = false;
bool ruleEditMode = false;
bool baseEditMode = false;
bool toggleGrid = true;

int main() {

	SetTraceLogLevel(LOG_ERROR);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitState();
	SetTargetFPS(60);


	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		UpdateState();
		HandleInputEvents();

		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		switch (state.curr) {
			case CONFIG:
				DrawConfigScene();
				break;
			case GRID:
				DrawGridScene();
				break;
			case SIM:
				DrawSimScene();
				break;
		}
		EndDrawing();
	}
	UnloadTexture(tex.texture);
	CloseWindow();
	return 0;
}

void DrawSimScene(void) {

	float X, Y, Yend, Ystart, Xend, Xstart;

	Size n = CA->get_size();
	Y = GetScreenHeight()/2.0f - CELL_SIZE*n.y/2.0f;
	X = GetScreenWidth()/2.0f - CELL_SIZE*n.x/2.0f;

	BeginMode2D(state.camera);
	DrawTextureRec(tex.texture, (Rectangle){0, 0, (float)tex.texture.width, (float)-tex.texture.height}, (Vector2){X, Y}, WHITE);
	EndMode2D();
	std::string text = "Iteration:    "+std::to_string(state.it);
	DrawRectangle( 10, 10, 250, 133, Fade(SKYBLUE, 0.5f));
	DrawRectangleLines( 10, 10, 250, 133, BLUE);

	DrawText(text.c_str(), 20, 20, 10, BLACK);
	DrawText("- W/A/S/D to Move Around The Grid", 40, 40, 10, DARKGRAY);
	DrawText("- Mouse Wheel or Q/E to Zoom in-out", 40, 60, 10, DARKGRAY);
	DrawText("- SPACE to move to the next state", 40, 80, 10, DARKGRAY);
	DrawText("- R to reset Zoom and Rotation", 40, 100, 10, DARKGRAY);
	DrawText("- P to take Screenshot", 40, 120, 10, DARKGRAY);

	DrawRectangle( state.screenWidth - 200, 10, 190, 130, Fade(SKYBLUE, 0.5f));
	DrawRectangleLines( state.screenWidth - 200, 10, 190, 130, BLUE);
	GuiCheckBox((Rectangle){ state.screenWidth - 190, 20, 20.0f, 20.0f}, 
						 "Toggle Grid Lines", &toggleGrid);
	if(GuiButton((Rectangle){state.screenWidth - 190, 60, 80.0f, 20.0f}, "Undo")) {
		CA->undo_step();
		--state.it;
		DrawTex();
	}
	if(GuiButton((Rectangle){state.screenWidth - 190, 90, 80.0f, 20.0f}, "Reset")) {
		state.curr = CONFIG;
		delete CA;
		n.x = 2;
		n.y = 2;
		UnloadTexture(tex.texture);
	}
}


void DrawGridScene(void) {
	float X, Y, Yend, Ystart, Xend, Xstart;

	float posY = 135.0f;
	float posX =  state.screenWidth/2.0f - 50.0f;

	Y = GetScreenHeight()/2.0f - CELL_SIZE*state.rows/2.0f;
	X = GetScreenWidth()/2.0f - CELL_SIZE*state.cols/2.0f;

	Xstart = X;
	Xend = X + CELL_SIZE*state.cols;

	Ystart = Y;
	Yend = Y + CELL_SIZE*state.rows;

	DrawRectangle(X, Y, CELL_SIZE*state.cols, CELL_SIZE*state.rows, SKYBLUE);

	for(int i = 0; i < state.rows; ++i) {
		for(int j = 0; j < state.cols; ++j) {
			DrawRectangle(X+j*CELL_SIZE, Y+i*CELL_SIZE, 
								 CELL_SIZE, CELL_SIZE,
								 cols[image[get_pos(j, i, state.cols)].state]);
		}
	}
	for(int i = 0; i <= state.cols; ++i)
		DrawLineV((Vector2){X+(i*CELL_SIZE), Y }, 
						(Vector2){X+(i*CELL_SIZE), Y+(state.rows*CELL_SIZE)}, BLUE);

	for(int i = 0; i <= state.rows; ++i)
		DrawLineV((Vector2){X, Y+(i*CELL_SIZE)}, 
						(Vector2){X+(state.cols*CELL_SIZE), Y+(i*CELL_SIZE)}, BLUE);



	Vector2 mousepos = GetMousePosition();
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && 
		mousepos.x > Xstart && mousepos.x < Xend &&
		mousepos.y > Ystart && mousepos.y < Yend) 
	{
		mousepos.x -= Xstart;
		mousepos.y -= Ystart;

		int j = floor(mousepos.x/CELL_SIZE);
		int i = floor(mousepos.y/CELL_SIZE);
		int pos = get_pos(j, i, state.cols);

		image[pos].state = (image[pos].state + 1) % state.z;

		if(state.gen_coeffs) {
			image[pos].pixel["B"+std::to_string(i+1)+std::to_string(j+1)] = image[pos].state;
		}

	} 

	if(GuiButton((Rectangle){posX, Yend +100.0f, 80.0f, 20.0f}, "Start")) {
		state.it = 0;
		CA = new cellular_automata(state.rule, state.cols, state.rows, state.z, image);
		if(state.gen_coeffs) {
			std::string directory = "rule"+std::to_string(state.rule);
			CA->export_image(directory+"/"+std::to_string(state.it)+"_iteration.csv");
		}
		DrawTex();
		state.curr = SIM;
	}

}

void DrawConfigScene(void) {

	float posY = 135.0f;
	float posX =  state.screenWidth/2.0f - 50.0f;

	if(GuiValueBox((Rectangle){posX, posY, 100.0f, 20.0f}, 
								"Rows:    ", 
								&state.rows, 1, 100, sizeYEditMode)) 		
	{
		sizeYEditMode = !sizeYEditMode;
	}
	if(GuiValueBox((Rectangle){posX, posY+30, 100.0f, 20.0f},
								"Columns:    ", 
								&state.cols, 1, 100, sizeXEditMode)) 	
	{
		sizeXEditMode = !sizeXEditMode;
	}
	if(GuiValueBox((Rectangle){posX, posY + 60, 100.0f, 20.0f},
								"Z:    ", 
								&state.z, 2, 10, baseEditMode)) 			
	{
		baseEditMode = !baseEditMode;
	}
	if(GuiValueBox((Rectangle){posX, posY + 90, 100.0f, 20.0f}, 
								"Rule:    ", 
								&state.rule, 0, max_val(state.z), ruleEditMode)) 
	{
		ruleEditMode = !ruleEditMode;
	}

	GuiCheckBox((Rectangle){posX-40.0f, posY+ 120, 20.0f, 20.0f}, 
						 "Generate Coefficient Files", &state.gen_coeffs);



	if(GuiButton((Rectangle){posX, posY +150, 80.0f, 20.0f}, "Generate")) {
		if(state.gen_coeffs) {
			std::string directory = "rule"+std::to_string(state.rule);
			std::filesystem::create_directory(directory);
			for(int i = 1; i <= state.cols; ++i) {
				for (int j = 1; j <= state.rows; ++j) {
					image.emplace_back(0, state.z, "B"+std::to_string(j)+std::to_string(i));
				}
			}
		} else {
			image = std::vector<cell>(state.cols*state.rows, cell(0, state.z));
		}

		state.curr = GRID;
	}
}

void DrawTex() {
	Size n = CA->get_size();
	std::vector<cell> grid = CA->get_grid();
	UnloadRenderTexture(tex);
	tex = LoadRenderTexture(n.x*CELL_SIZE+2, n.y*CELL_SIZE+2);
	BeginTextureMode(tex);
	DrawRectangle(0, 0, CELL_SIZE*n.x, CELL_SIZE*n.y, SKYBLUE);


	#pragma omp parallel for
	for(int i = 0; i < n.y; ++i) {
		for (int j = 0; j < n.x; ++j) {
			DrawRectangle((j*CELL_SIZE), (i*CELL_SIZE), 
								 CELL_SIZE, CELL_SIZE, cols[grid[CA->get_pos(j, i)].state]);
		}
	}

	if(toggleGrid) {
		for(int i = 0; i <= n.x; ++i)
			DrawLineEx((Vector2){(float)(i*CELL_SIZE), 0 }, 
						 (Vector2){(float)(i*CELL_SIZE), (float)(n.y*CELL_SIZE)}, 2.0f, BLUE);

		for(int i = 0; i <= n.y; ++i)
			DrawLineEx((Vector2){0, (float)(i*CELL_SIZE)}, 
						 (Vector2){(float)(n.x*CELL_SIZE), (float)(i*CELL_SIZE)}, 2.0f, BLUE);
	}
	EndTextureMode();
}

void InitState(void) {
	state.cols 							= 2;
	state.rows 							= 2;
	state.z 								= 2;
	state.rule							= 0;
	state.it								= 0;
	state.gen_coeffs 				= false;
	state.screenWidth 			= 700.0f;
	state.screenHeight 			= 700.0f;
	state.center 						= {state.screenWidth/2.0f, state.screenHeight/2.0f};
	state.camera 						= {0};
	state.camera.target 		= state.center;
	state.camera.offset 		= state.center;
	state.camera.rotation 	= 0.0f;
	state.camera.zoom 			= 1.0f;

	state.curr 							= CONFIG;

	InitWindow(state.screenWidth, state.screenHeight, "Simulate 2D Cellular Automata");
}

void UpdateState(void) {
	if(IsWindowResized()) {
		state.screenWidth = GetScreenWidth();
		state.center.x = state.screenWidth/2.0f;
		state.screenHeight = GetScreenHeight();
		state.center.y = state.screenHeight/2.0f;

		state.camera.target = state.center;
	}
}

void HandleInputEvents(void) {
	int speed = 4;
	std::string directory = "rule"+std::to_string(state.rule);
	if(state.curr==SIM && CA) {
		if(IsKeyPressed(KEY_SPACE)) {
			++state.it;
			if(state.gen_coeffs) {
				CA->step(directory+"/"+std::to_string(state.it)+"_iteration.csv");
			} else {
				CA->step();
			}
			DrawTex();
		}

		if(IsKeyDown(KEY_W)) state.center.y += speed;
		if(IsKeyDown(KEY_A)) state.center.x += speed;
		if(IsKeyDown(KEY_S)) state.center.y -= speed;
		if(IsKeyDown(KEY_D)) state.center.x -= speed;

		if(IsKeyDown(KEY_PAGE_UP)) speed += 1;
		if(IsKeyDown(KEY_PAGE_DOWN)) speed = (speed <= 4) ? 4 : --speed;


		if(IsKeyDown(KEY_Q)) state.camera.zoom -= 0.1f;
		if(IsKeyDown(KEY_E)) state.camera.zoom += 0.1f;

		state.camera.zoom += GetMouseWheelMove()*0.05f;
		if (state.camera.zoom < 0.1f) state.camera.zoom = 0.05f;
		if (IsKeyPressed(KEY_R))
		{
			state.camera.zoom = 1.0f;
			state.camera.offset = {state.screenWidth/2.0f, state.screenHeight/2.0f};
		}
		state.camera.offset = state.center;

		if(IsKeyPressed(KEY_P)) {
			std::filesystem::create_directory(directory);
			std::string filename = directory+"/"+std::to_string(state.rule)+"_"+std::to_string(state.it)+"_image.png";
			takeScreenshot(filename);
		}
	}

}

int get_pos(int x, int y, int size) {
	return x+size*y;
}

inline int max_val(int z) {
	int val = 0;
	for(int i = 0; i < 9; i++) {
		val += pow(z, i);
	}
	return val;
}

void takeScreenshot(const std::string filename) {
	Image output = LoadImageFromTexture(tex.texture);
	ImageFlipVertical(&output);
	ExportImage(output, filename.c_str());
}

