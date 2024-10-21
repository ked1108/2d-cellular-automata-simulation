#include "raylib.h"
#include <array>
#include <string>

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



