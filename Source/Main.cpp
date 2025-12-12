#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Header/Util.h"
#include "../Header/State.h"
#include "../Header/Input.h"
#include "../Header/Draw.h"

#define FPS_LIMIT 75.0
#define FRAME_TIME (1.0 / FPS_LIMIT)

void updateLogic(double currentTime) {

    if (GState.state == GameState::POSITIONING_CUP) {
        GLFWwindow* window = glfwGetCurrentContext();
        float deltaX = 0.0f;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            deltaX -= CUP_MOVEMENT_SPEED;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            deltaX += CUP_MOVEMENT_SPEED;
        }

        if (deltaX != 0.0f) {
            GState.cupPos[0] += deltaX;
            GState.cupPos[0] = std::max(-0.8f, std::min(0.8f, GState.cupPos[0]));

            if (std::abs(GState.cupPos[0] - POURING_X_TARGET) < 0.05f) {
                GState.cupCentered = true;
            }
            else {
                GState.cupCentered = false;
            }
        }
    }

    if (GState.state == GameState::POURING) {
        double elapsed = currentTime - GState.pourStartTime;

        GState.liquidLevel = (float)std::min(1.0, elapsed / POURING_DURATION);

        if (GState.liquidLevel >= 1.0f) {
            GState.state = GameState::DONE;
        }
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Aparat za Kafu - Projekat", monitor, NULL); 
    if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");
    glfwMakeContextCurrent(window);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    initInputCallbacks(window);
    initDrawingResources(window);

    double frameStartTime, frameEndTime, sleepTime;

    while (!glfwWindowShouldClose(window))
    {
        frameStartTime = glfwGetTime();
        glfwPollEvents();
        updateLogic(frameStartTime);
        glClear(GL_COLOR_BUFFER_BIT);
        drawScene(frameStartTime);


        glfwSwapBuffers(window);
        frameEndTime = glfwGetTime();
        double frameDuration = frameEndTime - frameStartTime;
        sleepTime = FRAME_TIME - frameDuration;

        if (sleepTime > 0.0) {
            double waitUntil = frameEndTime + sleepTime;
            while (glfwGetTime() < waitUntil) {
            }
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}