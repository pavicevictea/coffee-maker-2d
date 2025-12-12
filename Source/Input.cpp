#include "../Header/Input.h"
#include "../Header/State.h"
#include <iostream>
#include <cmath>
#include <algorithm>

// Handles keyboard input
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// Handles mouse movement
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float ndcX = (float)(xpos / width) * 2.0f - 1.0f;
    float ndcY = 1.0f - (float)(ypos / height) * 2.0f;

    if (GState.state == GameState::POSITIONING_CUP && GState.isDraggingCup) {
        GState.cupPos[0] = ndcX;
        GState.cupPos[1] = ndcY;
    }
    else if (GState.state == GameState::DONE && GState.isDraggingLid) {
        GState.lidPos[0] = ndcX;
        GState.lidPos[1] = ndcY;
    }
}

// Handles mouse button presses and releases
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float mouseNDC[2];
    mouseNDC[0] = (float)(xpos / width) * 2.0f - 1.0f;
    mouseNDC[1] = 1.0f - (float)(ypos / height) * 2.0f;

    float cupSizeY = CUP_SCALE_Y;
    float cupSizeX = CUP_SCALE_X;
    float tolerance = 0.15f;
    float lidSizeX = LID_SCALE_X;
    float lidSizeY = LID_SCALE_Y;

    // Mouse press handling
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwSetCursor(window, GState.cursorPressed); // change cursor look

		// Menu drink selection handling
        if (GState.state == GameState::MENU) {
            float col1X = 0.1f;
            float col2X = 0.3f;
            float row1Y = 0.65f;
            float row2Y = 0.50f;
            float buttonWidth = 0.18f;
            float buttonHeight = 0.13f;

            float positions[4][2] = {
                {col1X, row1Y},
                {col2X, row1Y},
                {col1X, row2Y},
                {col2X, row2Y}
            };

            for (int i = 0; i < 4; ++i) {
                float btnX = positions[i][0];
                float btnY = positions[i][1];

                if (mouseNDC[0] < btnX + buttonWidth / 2.0f &&
                    mouseNDC[0] > btnX - buttonWidth / 2.0f &&
                    mouseNDC[1] < btnY + buttonHeight / 2.0f &&
                    mouseNDC[1] > btnY - buttonHeight / 2.0f) {

                    GState.selectedDrink = (DrinkType)(i + 1);
                    getDrinkColor(GState.selectedDrink, GState.currentDrinkColor);
                    GState.state = GameState::SELECTING_DRINK;
                    GState.selectionTime = glfwGetTime();
                    GState.cupCentered = true;

                    GState.cupPos[0] = POURING_X_TARGET;
                    GState.cupPos[1] = POURING_Y_TARGET;
                    break;
                }
            }
        }
		// Drag cup if in positioning state
        else if (GState.state == GameState::POSITIONING_CUP) {

            if (mouseNDC[0] < GState.cupPos[0] + cupSizeX / 2.0f &&
                mouseNDC[0] > GState.cupPos[0] - cupSizeX / 2.0f &&
                mouseNDC[1] < GState.cupPos[1] + cupSizeY / 2.0f &&
                mouseNDC[1] > GState.cupPos[1] - cupSizeY / 2.0f) {

                GState.isDraggingCup = true;
            }
        }
		// Drag lid if in done state and lid is not on cup
        else if (GState.state == GameState::DONE) {
            if (!GState.lidOnCup) {
                if (mouseNDC[0] < GState.lidPos[0] + lidSizeX / 2.0f &&
                    mouseNDC[0] > GState.lidPos[0] - lidSizeX / 2.0f &&
                    mouseNDC[1] < GState.lidPos[1] + lidSizeY / 2.0f &&
                    mouseNDC[1] > GState.lidPos[1] - lidSizeY / 2.0f) {

                    GState.isDraggingLid = true;
                }
            }
			// Reset cup and lid positions 
            else {
                if (mouseNDC[0] < GState.cupPos[0] + cupSizeX / 2.0f &&
                    mouseNDC[0] > GState.cupPos[0] - cupSizeX / 2.0f &&
                    mouseNDC[1] < GState.cupPos[1] + cupSizeY / 2.0f && 
                    mouseNDC[1] > GState.cupPos[1] - cupSizeY / 2.0f) {

                    GState.state = GameState::POSITIONING_CUP;
                    GState.selectedDrink = DrinkType::NONE;
                    GState.liquidLevel = 0.0f;
                    GState.pourStartTime = -1.0;
                    GState.cupPos[0] = CUP_START_X;
                    GState.cupPos[1] = CUP_START_Y; 
                    GState.cupCentered = false;

                    GState.lidOnCup = false;
                    GState.lidPos[0] = LID_START_X;
                    GState.lidPos[1] = LID_START_Y;
                }
            }
        }
    }
	// Mouse release handling
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		glfwSetCursor(window, GState.cursorNormal); // change cursor look back
        
        // Drop cup and snap to target if close enough
        if (GState.isDraggingCup && GState.state == GameState::POSITIONING_CUP) {
            GState.isDraggingCup = false;

            float targetX = POURING_X_TARGET;
            float targetY = POURING_Y_TARGET;

            if (std::abs(GState.cupPos[0] - targetX) < tolerance &&
                std::abs(GState.cupPos[1] - targetY) < tolerance) {

                GState.cupPos[0] = targetX;
                GState.cupPos[1] = targetY;
                GState.cupCentered = true;
                GState.state = GameState::MENU;
            }
            else {
                GState.cupPos[0] = CUP_START_X;
                GState.cupPos[1] = CUP_START_Y;
                GState.cupCentered = false;
            }
        }
		// Drop lid and snap to cup if close enough
        else if (GState.isDraggingLid && GState.state == GameState::DONE) {
            GState.isDraggingLid = false;

            float targetX = GState.cupPos[0];
            float targetY = GState.cupPos[1];

            if (std::abs(GState.lidPos[0] - targetX) < LID_TOLERANCE &&
                std::abs(GState.lidPos[1] - targetY) < LID_TOLERANCE) {

                GState.lidOnCup = true;
                GState.lidPos[0] = targetX;
                GState.lidPos[1] = targetY;

            }
            else {
                GState.lidPos[0] = LID_START_X;
                GState.lidPos[1] = LID_START_Y;
            }
        }
        else if (GState.isDraggingCup) {
            GState.isDraggingCup = false;
        }
    }
}

void initInputCallbacks(GLFWwindow* window) {
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
}