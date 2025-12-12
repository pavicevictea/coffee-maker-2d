#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../Header/State.h"
#include <string>
#include <map>

void createOrthographicMatrix(float left, float right, float bottom, float top, float matrix[16]);
void initDrawingResources(GLFWwindow* window);
void cleanupDrawingResources();
void setupVAOs();
void loadTexturesAndCursors(GLFWwindow* window);
void setupTextRendering(GLFWwindow* window);
void drawTexturedQuad(unsigned int shaderProgram, unsigned int textureID, float x, float y, float scaleX, float scaleY);
void drawTexturedQuad(unsigned int shaderProgram, unsigned int textureID, float x, float y, float scale); 
void drawColoredQuad(const float color[4], float x, float y, float scaleX, float scaleY, float liquidLevel);
void drawColoredEllipse(unsigned int shaderProgram, const float color[4], float x, float y, float radiusX, float radiusY);
float getTextWidth(std::string text, float scale);
void RenderText(unsigned int shader, std::string text, float x, float y, float scale, const float color[3]);
void drawScene(double currentTime);
void drawCupAndLiquid();
void drawStudentInfo();
void drawDrinkButtons();
void drawProgressBar();
void drawPlaceCupMessage();
void drawSelectDrinkMessage(double currentTime);
void drawTargetCircle();
void drawPouringStream();