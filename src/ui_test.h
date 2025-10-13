/**지나치게 복잡해진 ui소스코드 리팩토링이 목적
 * ui_test는 임시명임. 나중에 뭐든 바꿔야 함.
 */
#pragma once
#include <string>
#include <vector>
#include "physics.h"

// UI State management
enum class CoordinateMode {
    CARTESIAN,
    POLAR
};

enum class EditMode {
    NONE,
    VELOCITY,
    MASS,
    POSITION,
    ADD_BODY,
    DELETE_CONFIRM
};

enum class EditMode {
    NONE,
    VELOCITY,
    MASS,
    POSITION,
    ADD_BODY,
    DELETE_CONFIRM
};

struct uiConfig{
    
};

class window{ //ui 창
    private:
        int width;
        int height;
        int r, g, b;
};

class textbox{
    private:
        int width;
        int height;
        bool inputable; //창에서 입력 가능한지 여부 결정.
};



