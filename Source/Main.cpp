#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Header/Util.h"
#include "../Header/State.h"
#include "../Header/Input.h"
#include "../Header/Draw.h"

#define FPS_LIMIT 75.0
#define FRAME_TIME (1.0 / FPS_LIMIT)

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
        glClear(GL_COLOR_BUFFER_BIT);
        // Render entire scene
        drawScene(frameStartTime);

        // Display rendered frame
        glfwSwapBuffers(window);

        // Frame limiter to maintain FPS_LIMIT
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