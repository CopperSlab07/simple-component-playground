#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

#include "components_atlas.h"

#define BACKGROUND 0x181818FF

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
void draw_preview();
void draw_selection(Component *component, int thickness);
void draw_mode(UserState state);

int main(void){
    UserState state                 = USER_VIEW;
    float expected_zoom             = 0;
    float expected_scale            = 0;
    Vector2 position                = Vector2Zero();

    ComponentTemplate templates[3] = {
        [TYPE_DEFAULT] = (ComponentTemplate){0},
        [TYPE_BUTTON] = (ComponentTemplate){
            .type           = TYPE_BUTTON,
            .max_port_count = 1,
            .hitbox         = (Rectangle){0, 0, 25, 25},
            .texture_box    = (Rectangle){32, 0, 25, 25},
            .interactive    = INTERACTIVE_DYNAMIC
        },
        [TYPE_LED] = (ComponentTemplate){
            .type           = TYPE_LED,
            .max_port_count = 1,
            .hitbox         = (Rectangle){0, 0, 32, 32},
            .texture_box    = (Rectangle){0, 0, 32, 32},
            .interactive    = INTERACTIVE_DYNAMIC
        },
    };

    ComponentHandle current_handle      = {0};
    static Component *current_component = NULL;

    Camera2D camera = {
        .target = {0},
        .offset = {.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT / 2},
        .rotation = 0,
        .zoom = 1.0f,
    };
    
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Raylib on Windows!");
    SetTargetFPS(60);

    map = calloc(1, sizeof(*map));
    map_init("./assets/def_atlas.png");
    current_component                   = component_from_handle(current_handle);

    component_create((Rectangle){.x = 50,   .y = 50,    .width  = 16,   .height = 16},  TYPE_BUTTON,INTERACTIVE_DYNAMIC);
    map->components[1].data.button = (Button){
        .tint_off = GetColor(0xFF1818FF), 
        .tint_on = GetColor(0x18FF18FF)
    };
    component_apply_template(map->components[1].handle, templates[TYPE_BUTTON]);

    component_create((Rectangle){.x = 0,    .y = 0,     .width  = 1,    .height = 1},   TYPE_LED,   INTERACTIVE_STATIC);
    map->components[2].data.led = (LED){
        .tint_off   = GetColor(0x883838FF),
        .tint_on    = GetColor(0xFF3838FF)
    };
    component_apply_template(map->components[2].handle, templates[TYPE_LED]);

    component_create((Rectangle){.x = 50,   .y = 0,     .width  = 1,    .height = 1},   TYPE_LED,   INTERACTIVE_STATIC);
    map->components[3].data.led = (LED){
        .tint_off   = GetColor(0x388838FF),
        .tint_on    = GetColor(0x38FF38FF)
    };
    component_apply_template(map->components[3].handle, templates[TYPE_LED]);
    
    component_create((Rectangle){.x = 100,  .y = 0,     .width  = 1,    .height = 1},   TYPE_LED,   INTERACTIVE_STATIC);
    map->components[4].data.led = (LED){
        .tint_off   = GetColor(0x383888FF),
        .tint_on    = GetColor(0x3838FFFF)
    };
    component_apply_template(map->components[4].handle, templates[TYPE_LED]);
    
    component_create((Rectangle){.x = 150,  .y = 0,     .width  = 1,    .height = 1},   TYPE_LED,   INTERACTIVE_STATIC);
    map->components[5].data.led = (LED){
        .tint_off   = GetColor(0x888838FF),
        .tint_on    = GetColor(0xFFFF38FF)
    };
    component_apply_template(map->components[5].handle, templates[TYPE_LED]);
    
    component_create((Rectangle){.x = 200,  .y = 0,     .width  = 1,    .height = 1},   TYPE_LED,   INTERACTIVE_STATIC);
    map->components[6].data.led = (LED){
        .tint_off   = GetColor(0x883888FF),
        .tint_on    = GetColor(0xFF38FFFF)
    };
    component_apply_template(map->components[6].handle, templates[TYPE_LED]);
    
    component_create((Rectangle){.x = 250,  .y = 0,     .width  = 1,    .height = 1},   TYPE_LED,   INTERACTIVE_STATIC);
    map->components[7].data.led = (LED){
        .tint_off   = GetColor(0x388888FF),
        .tint_on    = GetColor(0x38FFFFFF)
    };
    component_apply_template(map->components[7].handle, templates[TYPE_LED]);
    
    component_create((Rectangle){.x = 300,  .y = 0,     .width  = 1,    .height = 1},   TYPE_LED,   INTERACTIVE_STATIC);
    map->components[8].data.led = (LED){
        .tint_off   = GetColor(0x383838FF),
        .tint_on    = GetColor(0xFFFFFFFF)
    };
    component_apply_template(map->components[8].handle, templates[TYPE_LED]);

    digital_ports_init(&map->components[1]);
    map->components[1].ports[0].offset.y = 0;

    for(uint32_t i = 2; i < 9; i++){
        //component_set_texture(  map->components[i].handle, &led_texture);
        //component_scale_hitbox( map->components[i].handle, (float)map->components[i].texture->width, (float)map->components[i].texture->height);
        digital_ports_init(&map->components[i]);
        connect_digital_ports(&map->components[i].ports[0], &map->components[1].ports[0]);
    }

    while(!WindowShouldClose()){
        current_handle = map_update(camera);
        if(IsKeyPressed(KEY_C)){
            state++;
            state %= 3;
        }
        switch(state){
            case USER_VIEW: {
                expected_zoom = GetMouseWheelMove();
                if(IsKeyDown(KEY_LEFT_SHIFT) == true){
                    expected_zoom /= 5;
                }

                if(camera.zoom + expected_zoom >= 1){
                    camera.zoom += expected_zoom;
                }

                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                    camera.target = Vector2Add(camera.target, Vector2Negate(Vector2Divide(GetMouseDelta(), (Vector2){.x = camera.zoom, .y = camera.zoom})));
                }

                if(IsKeyPressed(KEY_ZERO)){
                    camera.target   = Vector2Zero();
                    camera.zoom     = 1;
                    camera.rotation = 0;
                }

                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    current_component    = component_from_handle(current_handle);
                    switch(current_component->type){
                        case TYPE_BUTTON: {
                            current_component->ports[0].out = !current_component->ports[0].out;
                        }
                        break;

                        default: {}
                        break;
                    }
                }
            }
            break;

            case USER_MOVE: {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    current_component    = component_from_handle(current_handle);
                    //check_button_collision = 1;
                }
                
                //else if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)){
                //    check_button_collision = 0;                
                //    position = Vector2Zero();
                //}
                
                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                    position = Vector2Add(position, Vector2Divide(GetMouseDelta(), Vector2Scale(Vector2One(), camera.zoom)));
                    current_component->hitbox.x += position.x;
                    current_component->hitbox.y += position.y;
                    position = Vector2Zero();
                }
            }
            break;

            case USER_SCALE: {
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    current_component    = component_from_handle(current_handle);
                } 

                expected_scale = GetMouseWheelMove();
                if(IsKeyDown(KEY_LEFT_SHIFT) == true){
                    expected_scale /= 5;
                }

                if(current_component->hitbox.width + expected_scale >= current_component->hitbox.width / 4
                    && current_component->hitbox.height + expected_scale >= current_component->hitbox.height / 4){
                    current_component->hitbox.width  += expected_scale;
                    current_component->hitbox.height += expected_scale;
                    //digital_port_update(&current_component->ports[0], (Vector2){.x = current_component->hitbox.width / 2, .y = 0});
                }
            }
            break;

            default: break;
        }

        BeginDrawing();
        ClearBackground(GetColor(BACKGROUND));
            BeginMode2D(camera);
            draw_selection(current_component, 4);
            map_draw();
            EndMode2D();
        DrawFPS(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 30);
        draw_mode(state);
        EndDrawing();
    }
    CloseWindow();
    //fprintf(stdout, "Count: %d\n", map->components_count);
    free(map);
    return 0;
}

void draw_selection(Component *component, int thickness){
    if(component->handle.index == 0) return;
    DrawRectangleLinesEx((Rectangle){component->hitbox.x - thickness / 2, component->hitbox.y - thickness / 2, component->hitbox.width + thickness, component->hitbox.height + thickness}, thickness, WHITE);
}

void draw_mode(UserState state){
    switch(state){
        case USER_VIEW: {
            DrawText("VIEW MODE", 10, 10, 20, GREEN);
        }
        break;

        case USER_MOVE: {
            DrawText("MOVE MODE", 10, 10, 20, BLUE);
        }
        break;

        case USER_SCALE: {
            DrawText("SCALE MODE", 10, 10, 20, YELLOW);
        }
        break;
    }
}
