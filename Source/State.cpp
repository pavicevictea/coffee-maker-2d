#include "../Header/State.h"
#include <algorithm> 

GlobalState GState;

// Sets RGBA color array based on the drink type
void getDrinkColor(DrinkType drink, float color[4]) {
    switch (drink) {
    case DrinkType::ESPRESSO:
        color[0] = 0.1765f; color[1] = 0.1137f; color[2] = 0.0510f; color[3] = 1.0f;
        break;
    case DrinkType::HOT_CHOCOLATE:
        color[0] = 0.5176f; color[1] = 0.2863f; color[2] = 0.1686f; color[3] = 1.0f;
        break;
    case DrinkType::MILK: 
        color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
        break;
    case DrinkType::MATCHA: 
        color[0] = 0.4706f; color[1] = 0.4980f; color[2] = 0.1333f; color[3] = 1.0f;
        break;
    default:
        color[0] = 0.0f; color[1] = 0.0f; color[2] = 0.0f; color[3] = 0.0f;
        break;
    }
}