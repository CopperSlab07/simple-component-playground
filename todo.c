#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define DEFAULT_TEXT_SIZE 20

typedef enum CellType {
    CELL_NORM,
    CELL_FATSO,
    CELL_TALL,
    CELL_TINY
} CellType;

typedef struct CellHandle {
    uint32_t index, id;
} CellHandle;

typedef struct CellConfig {
    Rectangle hitbox;
    Color color;
} CellConfig;

typedef struct Cell {
    CellHandle handle;
    CellConfig config;
} Cell;

#define MAX_CELLS 64
#define DEFAULT_COLOR WHITE
#define DEFAULT_SIZE  20
typedef struct Map {
    Cell cells[MAX_CELLS];
    size_t cells_count, last_cell_id;
} Map;
static Map *map;

Cell *cell_from_handle(CellHandle handle){
    if(handle.index == 0 || handle.index > map->cells_count){
        return &map->cells[0];
    }
    //what?
    Cell *result = &map->cells[handle.index];
    if(result->handle.id != handle.id){
        return &map->cells[0];
    }
    return result;
}

Cell *cell_create_ex(Rectangle hitbox, Color color){
    if(map->cells_count >= MAX_CELLS){
        fprintf(stderr, "Ran out of space for cells!\n");
        //exit(1);
        return &map->cells[0];
    }

    Cell *result            = &map->cells[map->cells_count];
    result->handle.index    = map->cells_count++;
    result->handle.id       = map->last_cell_id++;
    result->config.hitbox   = hitbox;
    result->config.color    = color;

    return result;
}

Cell *cell_create_with_template(CellConfig config){
    Cell *result = cell_create_ex(config.hitbox, config.color);
    return result;
}

void cell_destroy(CellHandle handle){
    Cell *result = cell_from_handle(handle);
    if(result == NULL || result->handle.index == 0){
        fprintf(stderr, "Warning: attempt to delete invalid cell\n");
        exit(1);
    }

    if(handle.index == map->cells_count - 1){
        memset(result, 0, sizeof(*result));
        map->cells_count--;
        return;
    }

    Cell *last_cell = &map->cells[map->cells_count];
    last_cell->handle.index = handle.index;
    memmove(result, last_cell, sizeof(*result));
    map->cells_count--;
}

void map_init(void){
    cell_create_ex((Rectangle){0}, BLANK);
};

void map_draw(void){
    for(size_t i = 1; i < map->cells_count; i++){
        DrawRectangleRec(map->cells[i].config.hitbox, map->cells[i].config.color);
    }
}

void cell_apply_template(Cell *cell, CellConfig template){
    cell->config = template;
}

void draw_outline(Cell *cell, float thickness){
    DrawRectangleLines(
        cell->config.hitbox.x - (thickness / 2),
        cell->config.hitbox.y - (thickness / 2),
        cell->config.hitbox.width + thickness,
        cell->config.hitbox.height + thickness,
        DEFAULT_COLOR
    );
}

int main(void){
    CellConfig templates[4] = {
        [CELL_NORM]     = (CellConfig){(Rectangle){0, 0, 50, 50}, WHITE},
        [CELL_FATSO]    = (CellConfig){(Rectangle){0, 0, 70, 30}, RED},
        [CELL_TALL]     = (CellConfig){(Rectangle){0, 0, 30, 70}, GREEN},
        [CELL_TINY]     = (CellConfig){(Rectangle){0, 0, 25, 25}, BLUE}
    };

    map = calloc(1, sizeof(*map));
    map_init();
    Cell *current_cell          = &map->cells[0];
    CellType current_template   = CELL_NORM;
    Vector2 mouse_pos           = Vector2Zero();
    float mouse_wheel_delta     = 0;
    int32_t add_state = 0, del_state = 0;

    srand(time(NULL));
    rand();
    rand();

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TODO?");
    SetTargetFPS(60);
    
    GuiLoadStyle("vendor/style_genesis.rgs");
    GuiSetStyle(DEFAULT, TEXT_SIZE, DEFAULT_TEXT_SIZE);

    while(!WindowShouldClose()){
        mouse_pos = GetMousePosition();
        mouse_wheel_delta = GetMouseWheelMove();
        if(mouse_wheel_delta > 0.5){
            //DrawRectangle(0, 0, 200, 200, RED);
            current_template += 1;
            current_template %= 4;
        }
        else if(mouse_wheel_delta < -0.5){
            //DrawRectangle(0, 0, 200, 200, RED);
            current_template -= 1;
            current_template %= 4;
        }

        //This is for, y'know, getting rid of stuff like a LIFO stack.
        if((IsKeyPressed(KEY_N) || IsKeyPressedRepeat(KEY_N) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) && map->cells_count < MAX_CELLS){
            if(
                   mouse_pos.x >= 0 
                && mouse_pos.y >= 0 
                && mouse_pos.x + DEFAULT_SIZE <= WINDOW_WIDTH 
                && mouse_pos.y + DEFAULT_SIZE <= WINDOW_HEIGHT
            ){
                //current_cell = cell_create_ex((Rectangle){mouse_pos.x, mouse_pos.y, 20, 20}, (Color){rand(), rand(), rand(), 0xFF});
                current_cell = cell_create_with_template(templates[current_template]);
                current_cell->config.hitbox.x = mouse_pos.x;
                current_cell->config.hitbox.y = mouse_pos.y;
            }
        }
        if((IsKeyPressed(KEY_D) || IsKeyPressedRepeat(KEY_D) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) && map->cells_count > 1){
            if(current_cell == &map->cells[0]) current_cell = &map->cells[map->cells_count - 1];
            cell_destroy(current_cell->handle);
            current_cell = &map->cells[map->cells_count - 1];
        }
        
        BeginDrawing();

        ClearBackground(GetColor(0x383838FF));
        DrawRectangleRec(
            (Rectangle){
                mouse_pos.x - (templates[current_template].hitbox.width / 2),
                mouse_pos.y - (templates[current_template].hitbox.height / 2),
                templates[current_template].hitbox.width,
                templates[current_template].hitbox.height
            },
            templates[current_template].color
        );
        /*
        DrawRectangleLinesEx(
            (Rectangle){
                mouse_pos.x - (templates[current_template].hitbox.width / 2) - 2,
                mouse_pos.y - (templates[current_template].hitbox.height / 2) - 2,
                templates[current_template].hitbox.width + 4,
                templates[current_template].hitbox.height + 4
            },
            4,
            templates[current_template].color
        );
        */
        draw_outline(current_cell, 4);
        map_draw();

        if(map->cells_count < MAX_CELLS){
            add_state = GuiButton((Rectangle){20, 20, (float)MeasureText("ADD RANDOM", DEFAULT_TEXT_SIZE), 50}, "ADD RANDOM");
        }
        if(map->cells_count > 1){
            del_state = GuiButton((Rectangle){20, 75, (float)MeasureText("ADD RANDOM", DEFAULT_TEXT_SIZE), 50}, "DELETE");
        }

        if(add_state == RESULT_PRESSED){
            //current_cell = cell_create_ex((Rectangle){fabs((float)(rand() % (WINDOW_WIDTH - DEFAULT_SIZE))), fabs((float)(rand() % (WINDOW_HEIGHT - DEFAULT_SIZE))), 20, 20}, (Color){abs(rand()) % 255, abs(rand()) % 255, abs(rand()) % 255, 0xFF});
            current_cell = cell_create_with_template(templates[abs(rand()) % 4]);
            current_cell->config.hitbox.x = fabs((float)(rand() % (WINDOW_WIDTH - DEFAULT_SIZE)));
            current_cell->config.hitbox.y = fabs((float)(rand() % (WINDOW_HEIGHT - DEFAULT_SIZE)));
            add_state = 0;
        }
        if(del_state == RESULT_PRESSED && map->cells_count > 1){
            if(current_cell == &map->cells[0]) current_cell = &map->cells[map->cells_count - 1];
            cell_destroy(current_cell->handle);
            current_cell = &map->cells[map->cells_count - 1];
            del_state = 0;
        }

        EndDrawing();
    }

    CloseWindow();
    free(map);
    return 0;
}
