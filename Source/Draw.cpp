#define _USE_MATH_DEFINES
#include "../Header/Draw.h"
#include "../Header/State.h"
#include "../Header/Shaders.h" 
#include "../Header/Util.h" 
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

std::map<char, Character> Characters;

extern int endProgram(const char* message);
extern unsigned int loadImageToTexture(const char* path);
extern GLFWcursor* loadImageToCursor(const char* path);

void createOrthographicMatrix(float left, float right, float bottom, float top, float matrix[16]) {
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);

    float near_val = -1.0f;
    float far_val = 1.0f;

    matrix[0] = 2.0f / (right - left); matrix[4] = 0.0f;               matrix[8] = 0.0f;               matrix[12] = tx;
    matrix[1] = 0.0f;               matrix[5] = 2.0f / (top - bottom);  matrix[9] = 0.0f;               matrix[13] = ty;
    matrix[2] = 0.0f;               matrix[6] = 0.0f;               matrix[10] = -2.0f / (far_val - near_val); matrix[14] = 0.0f;
    matrix[3] = 0.0f;               matrix[7] = 0.0f;               matrix[11] = 0.0f;              matrix[15] = 1.0f;
}

float getTextWidth(std::string text, float scale) {
    float width = 0.0f;
    for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        if (Characters.find(*c) == Characters.end()) continue;
        Character ch = Characters[*c];
        width += (ch.Advance >> 6) * scale;
    }
    return width;
}

void setupTextRendering(GLFWwindow* window) {
    if (FT_Init_FreeType(&GState.ft)) {
        endProgram(std::string("ERROR::FREETYPE: Could not init FreeType Library"));
        return;
    }

    FT_Face fontFace;
    if (FT_New_Face(GState.ft, "Resources/ariali.ttf", 0, &fontFace)) {
        endProgram(std::string("ERROR::FREETYPE: Failed to load font (Proverite putanju Resources/ariali.ttf)"));
        return;
    }

    FT_Set_Pixel_Sizes(fontFace, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(fontFace, c, FT_LOAD_RENDER)) {
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
            fontFace->glyph->bitmap.width,
            fontFace->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE,
            fontFace->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character;
        character.TextureID = texture;
        character.Size[0] = fontFace->glyph->bitmap.width;
        character.Size[1] = fontFace->glyph->bitmap.rows;
        character.Bearing[0] = fontFace->glyph->bitmap_left;
        character.Bearing[1] = fontFace->glyph->bitmap_top;
        character.Advance = (unsigned int)fontFace->glyph->advance.x;

        Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(fontFace);
    FT_Done_FreeType(GState.ft);

    glGenVertexArrays(1, &GState.textVAO);
    glGenBuffers(1, &GState.textVBO);
    glBindVertexArray(GState.textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, GState.textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RenderText(unsigned int shader, std::string text, float x, float y, float scale, const float color[3]) {
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color[0], color[1], color[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(GState.textVAO);

    GLFWwindow* window = glfwGetCurrentContext();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float projectionMatrix[16];
    createOrthographicMatrix(0.0f, (float)width, 0.0f, (float)height, projectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, projectionMatrix);

    for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        if (Characters.find(*c) == Characters.end()) continue;

        Character ch = Characters[*c];

        float xpos = x + ch.Bearing[0] * scale;
        float ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

        float w = ch.Size[0] * scale;
        float h = ch.Size[1] * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, GState.textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

float quadVertices[] = {
    -0.5f,  0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f,
     0.5f, -0.5f, 1.0f, 0.0f,
     0.5f,  0.5f, 1.0f, 1.0f,
};

void setupVAOs() {
    glGenVertexArrays(1, &GState.VAO_Quad);
    glGenBuffers(1, &GState.VBO_Quad);
    glBindVertexArray(GState.VAO_Quad);
    glBindBuffer(GL_ARRAY_BUFFER, GState.VBO_Quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &GState.VAO_Circle);
    glGenBuffers(1, &GState.VBO_Circle);
    glBindVertexArray(GState.VAO_Circle);
    glBindBuffer(GL_ARRAY_BUFFER, GState.VBO_Circle);
    glBufferData(GL_ARRAY_BUFFER, (NUM_CIRCLE_SEGMENTS + 2) * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void loadTexturesAndCursors(GLFWwindow* window) {
    GState.backgroundTexture = loadImageToTexture("Resources/background.jpg");
    GState.machineBodyTexture = loadImageToTexture("Resources/coffee-machine.png");
    GState.cupTexture = loadImageToTexture("Resources/empty-cup.png");
    GState.cupWithLidTexture = loadImageToTexture("Resources/cup.png");
    GState.lidTexture = loadImageToTexture("Resources/lid.png");

    glBindTexture(GL_TEXTURE_2D, GState.backgroundTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GState.machineBodyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GState.cupTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GState.cupWithLidTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GState.lidTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    GState.cursorNormal = loadImageToCursor("Resources/coffee-bean-cursor.png");
    GState.cursorPressed = loadImageToCursor("Resources/spoon-cursor.png");

    glfwSetCursor(window, GState.cursorNormal);
}

void initDrawingResources(GLFWwindow* window) {
    initShaders();
    setupVAOs();
    loadTexturesAndCursors(window);
    setupTextRendering(window);
}

void drawTexturedQuad(unsigned int shaderProgram, unsigned int textureID, float x, float y, float scaleX, float scaleY) {
    glUseProgram(shaderProgram);

    GLint uPosScale_loc = glGetUniformLocation(shaderProgram, "uPosScale");
    GLint uTex0_loc = glGetUniformLocation(shaderProgram, "uTex0");

    glUniform4f(uPosScale_loc, x, y, scaleX, scaleY);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(uTex0_loc, 0);

    glBindVertexArray(GState.VAO_Quad);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void drawTexturedQuad(unsigned int shaderProgram, unsigned int textureID, float x, float y, float scale) {
    drawTexturedQuad(shaderProgram, textureID, x, y, scale, scale);
}

void drawColoredQuad(const float color[4], float x, float y, float scaleX, float scaleY, float liquidLevel) {
    glUseProgram(GState.liquidShader);

    glUniform2f(GState.liquid_uPos_loc, x, y);
    glUniform4f(GState.liquid_uCol_loc, color[0], color[1], color[2], color[3]);
    glUniform1f(GState.liquid_uLevel_loc, liquidLevel);
    glUniform1f(GState.liquid_uScaleX_loc, scaleX);
    glUniform1f(GState.liquid_uScaleY_loc, scaleY);

    glBindVertexArray(GState.VAO_Quad);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glUseProgram(0);
}

void drawColoredEllipse(unsigned int shaderProgram, const float color[4], float x, float y, float radiusX, float radiusY) {
    glUseProgram(shaderProgram);

    glUniform2f(glGetUniformLocation(shaderProgram, "uPos"), x, y);
    glUniform4f(glGetUniformLocation(shaderProgram, "uCol"), color[0], color[1], color[2], color[3]);
    glUniform1f(glGetUniformLocation(shaderProgram, "uLevel"), 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "uScaleX"), 1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "uScaleY"), 1.0f);

    std::vector<float> vertices;
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    for (int i = 0; i <= NUM_CIRCLE_SEGMENTS; i++) {
        float angle = 2.0f * M_PI * (float)i / NUM_CIRCLE_SEGMENTS;
        vertices.push_back(radiusX * cosf(angle));
        vertices.push_back(radiusY * sinf(angle));
    }

    glBindVertexArray(GState.VAO_Circle);
    glBindBuffer(GL_ARRAY_BUFFER, GState.VBO_Circle);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 2);

    glBindVertexArray(0);
    glUseProgram(0);
}

void drawCupAndLiquid() {
    float cupScaleX = CUP_SCALE_X; 
    float cupScaleY = CUP_SCALE_Y;
    unsigned int cupTex = GState.cupTexture;
    float finalCupScaleY = cupScaleY;

    const float SCALE_FACTOR = 0.86f;
    const float Y_OFFSET = (CUP_SCALE_Y * (1.0f - SCALE_FACTOR)) / 2.0f;

    if (GState.state == GameState::DONE && GState.lidOnCup) {
        cupTex = GState.cupWithLidTexture;
        cupScaleX = CUP_SCALE_X * SCALE_FACTOR;
        finalCupScaleY = CUP_SCALE_Y * SCALE_FACTOR;
    }

    float drawCupY = GState.cupPos[1];
    if (GState.state == GameState::DONE && GState.lidOnCup) {
        drawCupY += Y_OFFSET;
    }

    drawTexturedQuad(GState.cupShader, cupTex, GState.cupPos[0], GState.cupPos[1], cupScaleX, finalCupScaleY);

    if (GState.state == GameState::DONE && GState.liquidLevel >= 1.0f && !GState.lidOnCup) {
        float liquidTopX = GState.cupPos[0] + 0.013f;
        float liquidTopY = GState.cupPos[1] + (CUP_SCALE_Y * 0.5f) - 0.181f;
        float liquidRadiusX = CUP_SCALE_X * 0.274f;
        float liquidRadiusY = 0.027f;
        drawColoredEllipse(GState.liquidShader, GState.currentDrinkColor, liquidTopX, liquidTopY, liquidRadiusX, liquidRadiusY);
    }

    if (GState.state == GameState::DONE && !GState.lidOnCup) {
        drawTexturedQuad(GState.cupShader, GState.lidTexture, GState.lidPos[0], GState.lidPos[1], LID_SCALE_X, LID_SCALE_Y);
    }
}

void drawStudentInfo() {
    GLFWwindow* window = glfwGetCurrentContext();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float bgX = -0.75f;
    float bgY = 0.85f; 
    float bgWidth = 0.45f;
    float bgHeight = 0.25f;
    float bgColor[4] = { 1.0f, 1.0f, 1.0f, 0.5f };

    drawColoredQuad(bgColor, bgX, bgY, bgWidth, bgHeight, 1.0f);

    float nameScale = 0.9f;
    float indexScale = 0.8f;
    const float blackColor[3] = { 0.0f, 0.0f, 0.0f };

    std::string nameText = "Tea Pavicevic";
    float nameWidth = getTextWidth(nameText, nameScale);
    std::string indexText = "RA 146/2022";
    float indexWidth = getTextWidth(indexText, indexScale);

    float centerPixelX = (bgX + 1.0f) * 0.5f * width;
    float centerPixelY = (bgY + 1.0f) * 0.5f * height;
    float namePixelX = centerPixelX - (nameWidth / 2.0f);
    float namePixelY = centerPixelY + 5.0f;
    float indexPixelX = centerPixelX - (indexWidth / 2.0f);
    float indexPixelY = centerPixelY - 29.0f;

    RenderText(GState.textShader, nameText, namePixelX, namePixelY, nameScale, blackColor);
    RenderText(GState.textShader, indexText, indexPixelX, indexPixelY, indexScale, blackColor);
}

void drawDrinkButtons() {
    GLFWwindow* window = glfwGetCurrentContext();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float col1X = 0.1f;
    float col2X = 0.3f;
    float row1Y = 0.65f;
    float row2Y = 0.50f;

    float buttonWidth = 0.18f;
    float buttonHeight = 0.13f;
    float borderThickness = 0.005f;

    std::string drinkNames[] = { "ESPRESSO", "HOT CHOCOLATE", "MILK", "MATCHA" };
    float positions[4][2] = {
        {col1X, row1Y},
        {col2X, row1Y},
        {col1X, row2Y},
        {col2X, row2Y}
    };
    const float blackColor[3] = { 0.0f, 0.0f, 0.0f };
    float borderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float backColor[4] = { 0.3922f, 0.4039f, 0.4392f, 1.0f };

    for (int i = 0; i < 4; ++i) {
        float btnX = positions[i][0];
        float btnY = positions[i][1];

        drawColoredQuad(borderColor, btnX, btnY,
            buttonWidth + borderThickness, buttonHeight + borderThickness, 1.0f);
        drawColoredQuad(backColor, btnX, btnY,
            buttonWidth, buttonHeight, 1.0f);

        float textScale = 0.35f;
        float textWidth = getTextWidth(drinkNames[i], textScale);

        float textNDC_X = btnX;
        float textNDC_Y = btnY;
        float centerPixelX = (textNDC_X + 1.0f) * 0.5f * width;
        float centerPixelY = (textNDC_Y + 1.0f) * 0.5f * height;
        float textPixelX = centerPixelX - (textWidth / 2.0f);
        float textPixelY = centerPixelY - 10.0f;

        RenderText(GState.textShader, drinkNames[i], textPixelX, textPixelY, textScale, blackColor);
    }
}

void drawProgressBar() {
    float barX = POURING_X_TARGET;
    float barY = PROGRESS_BAR_Y;
    float barWidth = 0.43f;
    float barHeight = 0.04f;

    float blackColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    drawColoredQuad(blackColor, barX, barY, barWidth, barHeight, 1.0f);

    if (GState.liquidLevel > 0.0f && GState.state == GameState::POURING) {
        float filledWidth = GState.liquidLevel * barWidth;
        float filledX = barX - (barWidth - filledWidth) / 2.0f;
        float barColor[4] = { 0.3922f, 0.4039f, 0.4392f, 1.0f };
        drawColoredQuad(barColor, filledX, barY, filledWidth, barHeight * 0.9f, 1.0f);
    }

    if (GState.state == GameState::DONE) {
        float barColor[4] = { 0.3922f, 0.4039f, 0.4392f, 1.0f };
        drawColoredQuad(barColor, barX, barY, barWidth, barHeight * 0.9f, 1.0f);

        const float blackColor[3] = { 0.0f, 0.0f, 0.0f };
        GLFWwindow* window = glfwGetCurrentContext();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        float textNDC_Y = barY + barHeight / 2.0f + 0.08f;
        float textPixelY = (textNDC_Y + 1.0f) * 0.5f * height;

        float barCenterPixelX = (barX + 1.0f) * 0.5f * width;
        float textScale = 0.5f;

        if (!GState.lidOnCup) {
            std::string text1 = "POUR DONE!";
            std::string text2 = "DRAG LID ON THE CUP";

            float width1 = getTextWidth(text1, textScale);
            float width2 = getTextWidth(text2, textScale);

            float pixelX1 = barCenterPixelX - (width1 / 2.0f);
            RenderText(GState.textShader, text1, pixelX1, textPixelY, textScale, blackColor);

            float pixelX2 = barCenterPixelX - (width2 / 2.0f);
            RenderText(GState.textShader, text2, pixelX2, textPixelY - 30.0f, textScale, blackColor);
        }
        else {
            std::string text = "CUP READY! TAKE IT";
            float widthText = getTextWidth(text, textScale);

            float pixelX = barCenterPixelX - (widthText / 2.0f);
            RenderText(GState.textShader, text, pixelX, textPixelY - 15.0f, textScale, blackColor);
        }
    }
}

void drawPlaceCupMessage() {
    GLFWwindow* window = glfwGetCurrentContext();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float centerX = POURING_X_TARGET;
    float messageY = POURING_Y_TARGET;

    float pixelX = (centerX + 1.0f) * 0.5f * width - 150.0f;
    float pixelY = (messageY + 1.95f) * 0.5f * height;

    const float blackColor[3] = { 0.0f, 0.0f, 0.0f };

    RenderText(GState.textShader, "DRAG CUP TO THE TARGET", pixelX, pixelY, 0.5f, blackColor);
}

void drawSelectDrinkMessage(double currentTime) {
    GLFWwindow* window = glfwGetCurrentContext();
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float centerX = POURING_X_TARGET;
    float messageY = POURING_Y_TARGET;

    float pixelX = (centerX + 1.0f) * 0.5f * width - 150.0f;
    float pixelY = (messageY + 1.8f) * 0.5f * height;

    const float blackColor[3] = { 0.0f, 0.0f, 0.0f };
    double remaining = POURING_DELAY - (currentTime - GState.selectionTime);

    if (remaining > 0) {
        std::string text = "STARTING POUR IN: " + std::to_string((int)std::ceil(remaining)) + "s";
        RenderText(GState.textShader, text, pixelX, pixelY, 0.6f, blackColor);
    }
}

void drawTargetCircle() {
    float cupScaleY = CUP_SCALE_Y;
    float circleColor[4] = { 0.7412f, 0.3647f, 0.1176f, 0.4f };
    float targetX = POURING_X_TARGET;
    float targetY = POURING_Y_TARGET - cupScaleY / 2.0f + 0.03f;

    float targetRadiusX = CUP_SCALE_X * 0.4f;
    float targetRadiusY = 0.03f;

    drawColoredEllipse(GState.liquidShader, circleColor, targetX + 0.01f, targetY + 0.1f, targetRadiusX, targetRadiusY);
}

void drawPouringStream() {
    if (GState.liquidLevel > 0.0f && GState.liquidLevel < 1.0f) {
        float streamX = POURING_X_TARGET + 0.01f;
        float cupTopY = POURING_Y_TARGET + CUP_SCALE_Y / 2.0f - 0.208f;
        float machineBottomY = -0.193f;

        glUseProgram(GState.liquidShader);

        float streamCenterY = (machineBottomY + cupTopY) / 2.0f;
        glUniform2f(GState.liquid_uPos_loc, streamX, streamCenterY);
        glUniform4f(GState.liquid_uCol_loc, GState.currentDrinkColor[0],
            GState.currentDrinkColor[1], GState.currentDrinkColor[2],
            GState.currentDrinkColor[3]);
        glUniform1f(GState.liquid_uLevel_loc, 1.0f);
        glUniform1f(GState.liquid_uScaleX_loc, 0.015f);
        glUniform1f(GState.liquid_uScaleY_loc, machineBottomY - cupTopY);

        glBindVertexArray(GState.VAO_Quad);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glUseProgram(0);
    }
}

void drawScene(double currentTime) {
    glDisable(GL_BLEND);

    drawTexturedQuad(GState.machineShader, GState.backgroundTexture, 0.0f, 0.0f, 2.5f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawTexturedQuad(GState.machineShader, GState.machineBodyTexture, 0.2f, 0.0f, 1.5f, 2.2f);

    drawStudentInfo();

    if (GState.state == GameState::POSITIONING_CUP) {
        drawTargetCircle();
    }

    if (GState.state == GameState::SELECTING_DRINK) {
        if (currentTime - GState.selectionTime >= POURING_DELAY) {
            GState.state = GameState::POURING;
            GState.pourStartTime = currentTime;
        }
    }
    else if (GState.state == GameState::POURING) {
        double elapsedTime = currentTime - GState.pourStartTime;
        GState.liquidLevel = std::min(1.0f, (float)(elapsedTime / POURING_DURATION));

        if (GState.liquidLevel >= 1.0f) {
            GState.state = GameState::DONE;
            GState.liquidLevel = 1.0f;
        }
    }

    drawCupAndLiquid();

    if (GState.state == GameState::POSITIONING_CUP) {
        drawPlaceCupMessage();
    }
    else if (GState.state == GameState::MENU) {
        drawDrinkButtons();
    }
    else if (GState.state == GameState::SELECTING_DRINK) {
        drawDrinkButtons();
        drawSelectDrinkMessage(currentTime);
    }

    if (GState.state == GameState::POURING && GState.liquidLevel > 0.0f && GState.liquidLevel < 1.0f) {
        drawPouringStream();
    }

    if (GState.state == GameState::POURING || GState.state == GameState::DONE) {
        drawProgressBar();
    }

    glDisable(GL_BLEND);
}

void cleanupDrawingResources() {
    glDeleteVertexArrays(1, &GState.VAO_Quad);
    glDeleteBuffers(1, &GState.VBO_Quad);

    glDeleteVertexArrays(1, &GState.VAO_Circle);
    glDeleteBuffers(1, &GState.VBO_Circle);

    glDeleteVertexArrays(1, &GState.textVAO);
    glDeleteBuffers(1, &GState.textVBO);

    glfwDestroyCursor(GState.cursorNormal);
    glfwDestroyCursor(GState.cursorPressed);

    cleanupShaders();
}