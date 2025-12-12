#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

#define POURING_DURATION 4.0f   // Pouring time in seconds
#define POURING_X_TARGET 0.19f  // X position where the cup should be centered for pouring
#define POURING_Y_TARGET -0.43f // Y position where the cup should be centered for pouring
#define CUP_START_X -0.6f       // Starting X position of the cup
#define CUP_START_Y -0.6f       // Starting Y position of the cup
#define POURING_DELAY 3.0f      // Delay before pouring starts after drink selection
#define CUP_SCALE_X 0.3f
#define CUP_SCALE_Y 0.65f
#define LID_START_X -0.6f       // Starting X position of the lid 
#define LID_START_Y -0.65f      // Starting Y position of the lid
#define LID_SCALE_X CUP_SCALE_X
#define LID_SCALE_Y 0.25f
#define LID_TOLERANCE 0.25f     // Distance tolerance for placing lid on cup
#define NUM_CIRCLE_SEGMENTS 64  //Segments used to draw circles/ellipses
#define PROGRESS_BAR_Y 0.5f     // Y position of the pouring progress bar

// Enum representing different states of the game
enum class GameState {
    MENU, POSITIONING_CUP, SELECTING_DRINK, POURING, DONE
};

// Enum representing different types of drinks
enum class DrinkType {
    NONE, ESPRESSO, HOT_CHOCOLATE, MILK, MATCHA
};

// Structure to hold character glyph information for text rendering
struct Character {
	unsigned int TextureID;     // ID handle of the glyph texture
    int Size[2];                // Width and height of glyph
	int Bearing[2];             // Offset from baseline to left/top of glyph
	unsigned int Advance;       // Offset to advance to next glyph
};

// Global state structure holding all relevant game data
struct GlobalState {
    int screenWidth = 800;
    int screenHeight = 800;

    GameState state = GameState::POSITIONING_CUP;
    DrinkType selectedDrink = DrinkType::NONE;

    float cupPos[2] = { CUP_START_X, CUP_START_Y };
    float liquidLevel = 0.0f;
    float currentDrinkColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    double pourStartTime = -1.0;
    bool cupCentered = false;
    bool isDraggingCup = false;
    double selectionTime = -1.0;

    float lidPos[2] = { LID_START_X, LID_START_Y };
    bool lidOnCup = false;
    bool isDraggingLid = false;

	// OpenGL resources
    unsigned int VAO_Quad;
    unsigned int VBO_Quad;
    unsigned int VAO_Circle = 0;
    unsigned int VBO_Circle = 0;

    unsigned int machineShader;
    unsigned int cupShader;
    unsigned int liquidShader;

    unsigned int textShader;
    unsigned int textVAO, textVBO;
    FT_Library ft;

	unsigned int backgroundTexture;
    unsigned int machineBodyTexture;
    unsigned int cupTexture;
    unsigned int cupWithLidTexture;
    unsigned int lidTexture;

    GLFWcursor* cursorNormal;
    GLFWcursor* cursorPressed;

	// Uniform locations for liquid shader
    GLint liquid_uPos_loc;
    GLint liquid_uCol_loc;
    GLint liquid_uLevel_loc;
    GLint liquid_uScaleX_loc;
    GLint liquid_uScaleY_loc;
};

extern GlobalState GState;

// Character map for text rendering
extern std::map<char, Character> Characters;

void getDrinkColor(DrinkType drink, float color[4]);