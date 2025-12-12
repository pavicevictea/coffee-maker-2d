#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void initInputCallbacks(GLFWwindow* window);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);