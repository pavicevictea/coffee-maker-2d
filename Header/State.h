#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

#define POURING_DURATION 4.0f    
#define CUP_MOVEMENT_SPEED 0.01f 
#define POURING_X_TARGET 0.19f 
#define POURING_Y_TARGET -0.43f
#define CUP_START_X -0.6f
#define CUP_START_Y -0.6f
#define POURING_DELAY 3.0f
#define CUP_SCALE_X 0.3f
#define CUP_SCALE_Y 0.65f
#define LID_START_X -0.6f 
#define LID_START_Y -0.65f
#define LID_SCALE_X CUP_SCALE_X
#define LID_SCALE_Y 0.25f
#define LID_TOLERANCE 0.25f
#define NUM_CIRCLE_SEGMENTS 64
#define PROGRESS_BAR_Y 0.5f

enum class GameState {
    MENU, POSITIONING_CUP, SELECTING_DRINK, POURING, DONE
};

enum class DrinkType {
    NONE, ESPRESSO, HOT_CHOCOLATE, MILK, MATCHA
};

struct Character {
    unsigned int TextureID;
    int Size[2];
    int Bearing[2];
    unsigned int Advance;
};

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

    GLint liquid_uPos_loc;
    GLint liquid_uCol_loc;
    GLint liquid_uLevel_loc;
    GLint liquid_uScaleX_loc;
    GLint liquid_uScaleY_loc;
};

extern GlobalState GState;
extern std::map<char, Character> Characters;

void getDrinkColor(DrinkType drink, float color[4]);