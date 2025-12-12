#include "../Header/Shaders.h"
#include "../Header/State.h"
#include "../Header/Util.h" 
#include <iostream>

extern unsigned int createShader(const char* vertexPath, const char* fragmentPath);

void initShaders() {
    GState.machineShader = createShader("Shaders/MachineBody.vert", "Shaders/MachineBody.frag");
    GState.cupShader = createShader("Shaders/Cup.vert", "Shaders/Cup.frag");
    GState.liquidShader = createShader("Shaders/Liquid.vert", "Shaders/Liquid.frag");
    GState.textShader = createShader("Shaders/Text.vert", "Shaders/Text.frag");

    if (GState.machineShader == 0 || GState.cupShader == 0 ||  GState.liquidShader == 0 || GState.textShader == 0) {
        std::cerr << "Greska: Nisu uspeli da se kreiraju svi sejderi." << std::endl;
        return;
    }

    glUseProgram(GState.liquidShader);
    GState.liquid_uPos_loc = glGetUniformLocation(GState.liquidShader, "uPos");
    GState.liquid_uCol_loc = glGetUniformLocation(GState.liquidShader, "uCol");
    GState.liquid_uLevel_loc = glGetUniformLocation(GState.liquidShader, "uLevel");
    GState.liquid_uScaleX_loc = glGetUniformLocation(GState.liquidShader, "uScaleX");
    GState.liquid_uScaleY_loc = glGetUniformLocation(GState.liquidShader, "uScaleY");

    glUseProgram(0);
}

void cleanupShaders() {
    glDeleteProgram(GState.machineShader);
    glDeleteProgram(GState.cupShader);
    glDeleteProgram(GState.liquidShader);

    if (GState.textShader != 0) {
        glDeleteProgram(GState.textShader);
    }
}