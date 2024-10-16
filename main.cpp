#include "src/cellular_automata.h"
#include "src/cell.h"

#include <filesystem>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"



#define CELL_SIZE 30

std::array<Color, 10> cols =
			{WHITE, BLACK, RED, BLUE, GREEN, YELLOW, ORANGE, PINK, MAGENTA, BROWN};

typedef enum {CONFIG=0, GRID, SIM} SCREEN;

typedef struct State {
	int cols;
	int rows;
	int z;
	int rule;
	int it;
	float screenWidth;
	float screenHeight;
	bool gen_coeffs;
	Vector2 center;
	Camera2D camera;
	SCREEN curr;
} State; 

void DrawConfigScene(void);
void DrawSimScene(void);
void DrawGridScene(void);

void InitState(void);
void UpdateState(void);
void HandleInputEvents(void);
int get_pos(int x, int y, int size);
void takeScreenshot(const std::string filename);

State state;
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
	while (!WindowShouldClose())
	{

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
	CloseWindow();
	return 0;
}

void DrawSimScene(void) {

	float X, Y, Yend, Ystart, Xend, Xstart;

	Size n = CA->get_size();
	Y = GetScreenHeight()/2.0f - CELL_SIZE*n.y/2.0f;
	X = GetScreenWidth()/2.0f - CELL_SIZE*n.x/2.0f;

	BeginMode2D(state.camera);
	DrawRectangle(X, Y, CELL_SIZE*n.x, CELL_SIZE*n.y, SKYBLUE);

	std::vector<cell> grid = CA->get_grid();

	#pragma omp parallel for
	for(int i = 0; i < n.y; ++i) {
		for (int j = 0; j < n.x; ++j) {
			DrawRectangle(X+(j*CELL_SIZE), Y+(i*CELL_SIZE), 
				 			CELL_SIZE, CELL_SIZE, cols[grid[CA->get_pos(j, i)].state]);
		}
	}

	if(toggleGrid) {
		for(int i = 0; i <= n.x; ++i)
			DrawLineV((Vector2){X+(i*CELL_SIZE), Y }, 
			 			(Vector2){X+(i*CELL_SIZE), Y+(n.y*CELL_SIZE)}, BLUE);

		for(int i = 0; i <= n.y; ++i)
			DrawLineV((Vector2){X, Y+(i*CELL_SIZE)}, 
			 			(Vector2){X+(n.x*CELL_SIZE), Y+(i*CELL_SIZE)}, BLUE);
	}


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
	}
	if(GuiButton((Rectangle){state.screenWidth - 190, 90, 80.0f, 20.0f}, "Reset")) {
		state.curr = CONFIG;
		free(CA);
		CA=nullptr;
		grid.clear();
		n.x = 2;
		n.y = 2;
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
					&state.rule, 0, pow(state.z, 9)-1, ruleEditMode)) 
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

void InitState(void) {
	state.cols 					= 2;
	state.rows 					= 2;
	state.z 					= 2;
	state.rule					= 0;
	state.it					= 0;
	state.gen_coeffs 			= false;
	state.screenWidth 			= 700.0f;
	state.screenHeight 			= 700.0f;
	state.center 				= {state.screenWidth/2.0f, state.screenHeight/2.0f};

	state.camera.target 		= state.center;
	state.camera.offset 		= (Vector2){ state.screenWidth/2.0f, state.screenHeight/2.0f };
	state.camera.rotation 		= 0.0f;
	state.camera.zoom 			= 1.0f;

	state.curr 					= CONFIG;

	InitWindow(state.screenWidth, state.screenHeight, "Simulate 2D Cellular Automata");
}

void UpdateState(void) {
	if(state.screenWidth != GetScreenWidth()) {
		state.screenWidth = GetScreenWidth();
		state.center.x = state.screenWidth/2.0f;
	}
	if(state.screenHeight != GetScreenHeight()) {
		state.screenHeight = GetScreenHeight();
		state.center.y = state.screenHeight/2.0f;
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
		}

		if(IsKeyDown(KEY_W)) state.center.y -= speed;
		if(IsKeyDown(KEY_A)) state.center.x -= speed;
		if(IsKeyDown(KEY_S)) state.center.y += speed;
		if(IsKeyDown(KEY_D)) state.center.x += speed;

		if(IsKeyDown(KEY_PAGE_UP)) speed += 1;
		if(IsKeyDown(KEY_PAGE_DOWN)) speed = (speed <= 4) ? 4 : --speed;


		if(IsKeyDown(KEY_Q)) state.camera.zoom -= 0.1f;
		if(IsKeyDown(KEY_E)) state.camera.zoom += 0.1f;

		state.camera.zoom += GetMouseWheelMove()*0.05f;
		if (state.camera.zoom < 0.1f) state.camera.zoom = 0.05f;
		if (IsKeyPressed(KEY_R))
		{
			state.camera.zoom = 1.0f;
			state.camera.target = {state.screenWidth/2.0f, state.screenHeight/2.0f};
		}
		state.camera.target = state.center;

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

void takeScreenshot(const std::string filename) {
	Size n = CA->get_size();
	Image output = GenImageColor(n.x*CELL_SIZE, n.y*CELL_SIZE, WHITE);
	std::vector<cell> grid = CA->get_grid();

	#pragma omp parallel for
	for(int i = 0; i < n.y; ++i) {
		for (int j = 0; j < n.x; ++j) {
			ImageDrawRectangle(&output, (j*CELL_SIZE), (i*CELL_SIZE), 
					  			CELL_SIZE, CELL_SIZE,
					  			cols[grid[CA->get_pos(j, i)].state]);
		}
	}

	ExportImage(output, filename.c_str());
}

