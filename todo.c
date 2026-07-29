#include <stdint.h>
#include <stdbool.h>
#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

int main(void){
    const char *title = "oh fuck no";
    const char *message = "oh hell no";
    
    const char *buttons = "";
    int32_t button_count = 1;
    bool secret = false;

    char active[120] = {0};
    int32_t active_length = 120;

    InitWindow(1280, 720, "TODO?");
    SetTargetFPS(60);
    
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    int32_t padding = GuiGetStyle(DEFAULT, TEXT_PADDING);
    GuiSetStyle(DEFAULT, TEXT_PADDING, padding + 5);
    GuiLabel((Rectangle){10, 10, (float)MeasureText("TODO?", 30), 100}, "TODO?");

    while(!WindowShouldClose()){
        BeginDrawing();

        ClearBackground(GetColor(0x383838FF));
        GuiTextInputBox((Rectangle){150, 10, 600, 300}, title, message, active, 120, buttons, &button_count, &secret);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
