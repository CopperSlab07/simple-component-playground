#ifndef COMPONENTS_ATLAS_H
#define COMPONENTS_ATLAS_H

//:enums
typedef enum UserState {
    USER_VIEW,
    USER_MOVE,
    USER_SCALE
} UserState;

typedef enum PortError {
    PORT_DEFAULT,
    PORT_NOT_FOUND,
    PORT_INVALID_VALUE
} PortError;

typedef enum DigitalPortIO {
    DIGITAL_IN_OUT,
    DIGITAL_IN_ONLY,
    DIGITAL_OUT_ONLY
} DigitalPortIO;

typedef enum ComponentType {
    TYPE_DEFAULT,
    TYPE_BUTTON,
    TYPE_LED
} ComponentType;

typedef enum Interactive {
    INTERACTIVE_STATIC,
    INTERACTIVE_HALF_DYNAMIC,
    INTERACTIVE_DYNAMIC
} Interactive;

//:handles
typedef struct ComponentHandle {
    uint32_t index, id;   
} ComponentHandle;

typedef struct DigitalPortHandle {
    ComponentHandle handler;
    uint32_t index, id;
} DigitalPortHandle;

//:port
typedef struct DigitalPort DigitalPort;
struct DigitalPort {
    DigitalPortHandle handle;
    DigitalPortIO mode;
    Vector2 offset; //offset by scale, compared with component's width and height

    DigitalPort *in;
    int8_t out;
}; //how?

//:types
typedef struct LED {
    //DigitalPort port;
    uint32_t max_port_count;
    Color tint_off;
    Color tint_on;
} LED;

typedef struct Button {
    //DigitalPort port;
    uint32_t max_port_count;
    Color tint_on;
    Color tint_off;
} Button; //for testing purposes, only a rectangle that can change color for now

//:Component
typedef union ComponentData {
    LED led;
    Button button;
} ComponentData;

typedef struct ComponentTemplate {
    ComponentType type;
    //uint32_t digital_ports_count;
    uint32_t max_port_count;
    Rectangle hitbox;
    Rectangle texture_box;
    Interactive interactive;
} ComponentTemplate;

#define MAX_PORTS 4
typedef struct Component {
    ComponentType type;
    ComponentData data;
 
    ComponentHandle handle;
    DigitalPort ports[MAX_PORTS];
    uint32_t digital_ports_count;
    uint32_t last_port_id;
    Rectangle hitbox;
    Rectangle texture_box;
    Interactive interactive;
} Component;

//:map
#define MAX_COMPONENTS 16
typedef struct Map {
    Component components[MAX_COMPONENTS];
    uint32_t components_count, last_component_id;
    Texture2D atlas;
} Map;
static Map *map;

//:functions
void draw_component(Component *component){
    Color tint = WHITE;
    switch(component->type){
        case TYPE_BUTTON: {
            switch(component->ports[component->last_port_id - 1].out){
                case 1: {
                    tint = component->data.button.tint_on;
                } 
                break;

                case 0: {
                    tint = component->data.button.tint_off;
                }
                break;
            }
        }
        break;

        case TYPE_LED: {
            switch(component->ports[component->last_port_id - 1].in->out){
                case 1: {
                     tint = component->data.led.tint_on;
                } 
                break;

                case 0: {
                     tint = component->data.led.tint_off;
                }
                break;
            }
        }
        break;

        case TYPE_DEFAULT: {

        }
        break;
    }
    /*if(component->texture->id == map->base_texture.id){
        DrawTexturePro(
            *component->texture, 
            GetShapesTextureRectangle(), 
            component->hitbox, 
            Vector2Zero(), 
            0, 
            tint
        );
    }*/
    //else {
        DrawTexturePro(
            map->atlas,
            component->texture_box,
            component->hitbox,
            Vector2Zero(),
            0, 
            tint
        );
    //}
}

Component *component_from_handle(ComponentHandle handle){
    if(handle.index == 0 || handle.index > map->components_count){
        return &map->components[0];
    }
    //what?
    Component *result = &map->components[handle.index];
    if(result->handle.id != handle.id){
        return &map->components[0];
    }
    return result;
}

Component *component_create(Rectangle hitbox, ComponentType type, int interactable){
    if(map->components_count >= MAX_COMPONENTS){
        fprintf(stderr, "Ran out of space for components!\n");
        exit(1);
    }

    Component *result       = &map->components[map->components_count];
    result->handle.index    = map->components_count++;
    result->handle.id       = map->last_component_id++;
    if(result->handle.index == 0){
        result->data        = (ComponentData){0};
        //result->data.led.port.in    = &map->components[0].data.led.port;
    }
    else {
        result->data                = map->components[0].data;
    }
    result->hitbox          = hitbox;
    result->type            = type;
    result->interactive     = interactable;
    result->texture_box     = (Rectangle){0};

    return result;
}

void component_apply_template(ComponentHandle handle, ComponentTemplate temp){
    Component *result       = component_from_handle(handle);
    result->type            = temp.type;
    result->hitbox.width    = temp.hitbox.width;
    result->hitbox.height   = temp.hitbox.height;
    result->texture_box     = temp.texture_box;
    result->interactive     = temp.interactive;

    switch(result->type){
        case TYPE_BUTTON: {
            result->data.button.max_port_count = temp.max_port_count;
        }
        break;

        case TYPE_LED: {
            result->data.led.max_port_count = temp.max_port_count;
        }
        break;

        default: {} break;
    }
}

void component_set_texture(ComponentHandle handle, Rectangle bounds){
    Component *component = component_from_handle(handle);
    if(component == NULL || component->handle.index == 0){
        fprintf(stderr, "Warning: attempt to set texture of invalid component\n");
        return;
    }
    component->texture_box = bounds;
};

void component_set_hitbox(ComponentHandle handle, Rectangle rectangle){
    Component *component = component_from_handle(handle);
    if(component == NULL || component->handle.index == 0){
        fprintf(stderr, "Warning: attempt to set hitbox of invalid component\n");
        return;
    }
    component->hitbox = rectangle;
}

void component_scale_hitbox(ComponentHandle handle, float scale_x, float scale_y){
    if(scale_x <= 0 || scale_y <= 0){
        fprintf(stderr, "Cannot scale BELOW 0!\n");
        return;
    }

    Component *component = component_from_handle(handle);
    if(component == NULL || component->handle.index == 0){
        fprintf(stderr, "Warning: attempt to set hitbox of invalid component\n");
        return;
    }

    component->hitbox = (Rectangle){
        .x      = component->hitbox.x,
        .y      = component->hitbox.y,
        .width  = component->hitbox.width * scale_x,
        .height = component->hitbox.height * scale_y
    };
}

void component_destroy(ComponentHandle handle){
    Component *component = component_from_handle(handle);
    if(component == NULL || component->handle.index == 0){
        fprintf(stderr, "Warning: attempt to delete invalid component\n");
        exit(1);
    }

    if(handle.index == map->components_count - 1){
        memset(component, 0, sizeof(*component));
        map->components_count--;
        return;
    }

    Component *last_component = &map->components[map->components_count];
    last_component->handle.index = handle.index;
    memmove(component, last_component, sizeof(*component));
    map->components_count--;
}

void digital_ports_init(Component *component){
    if(component->digital_ports_count >= MAX_PORTS){
        fprintf(stderr, "Ran out of space for extra ports!\n");
        //exit(1);
        return;
    }
 
    uint32_t max_port_count = 0;
    switch(component->type){
        case TYPE_BUTTON: {
            max_port_count = component->data.button.max_port_count;
        }
        break;
        
        case TYPE_LED: {
            max_port_count = component->data.led.max_port_count;
        }
        break;

        default: {} break;
    }

    if(max_port_count > MAX_PORTS){
        fprintf(stderr, "Invalid amount of max ports!\n");
        return;
    }

    for(uint32_t i = 0; i < max_port_count; i++){
        component->ports[i].handle = (DigitalPortHandle){
            .handler    = component->handle,
            .index      = component->digital_ports_count++,
            .id         = component->last_port_id++
        };
        component->ports[i].offset = (Vector2){
            .x = 0.5,
            .y = 1
        };
        component->ports[i].in = &map->components[0].ports[0];
    }
}

void digital_ports_deinit(Component *component){
    fprintf(stdout, "TODO: ports deinit\n");
}

void map_init(const char *atlas_path){
    Texture2D tempo = LoadTexture(atlas_path);
    if(!IsTextureValid(tempo)){
        fprintf(stderr, "Invalid texture path!\n");
        map->atlas = GetShapesTexture();
        UnloadTexture(tempo);
        exit(1);
    }
    map->atlas = LoadTexture(atlas_path);
    UnloadTexture(tempo);
    component_create((Rectangle){0}, TYPE_DEFAULT, INTERACTIVE_STATIC);
}

ComponentHandle map_update(Camera2D camera){
    Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
    for(uint32_t i = 0; i < map->components_count; i++){
        if(map->components[i].hitbox.x > mouse_pos.x || map->components[i].hitbox.y > mouse_pos.y) continue;
        if(map->components[i].hitbox.x + map->components[i].hitbox.width > mouse_pos.x && map->components[i].hitbox.y + map->components[i].hitbox.height > mouse_pos.y) return map->components[i].handle;
    }
    return map->components[0].handle;
};
/*
void digital_port_update(DigitalPort *port, Vector2 offset){
    port->offset = offset;
}
*/
void connect_digital_ports(DigitalPort *input, DigitalPort *output){
    input->in   = output;
} //connect input port of a component to an output port of a component, not necessarily different components

void draw_connection(DigitalPort *port, Color color){
    if(port->in == &map->components[0].ports[0]){
        return;
    }
    Vector2 end_point = Vector2Zero();
    Vector2 begin_point = Vector2Zero();
    end_point = Vector2Add(
        (Vector2){map->components[port->in->handle.handler.index].hitbox.x, map->components[port->in->handle.handler.index].hitbox.y}, 
        Vector2Multiply(
            (Vector2){map->components[port->in->handle.handler.index].hitbox.width, 
                      map->components[port->in->handle.handler.index].hitbox.height}, 
            port->in->offset));

    begin_point = Vector2Add((Vector2){map->components[port->handle.handler.index].hitbox.x, map->components[port->handle.handler.index].hitbox.y}, 
        Vector2Multiply(
            (Vector2){map->components[port->handle.handler.index].hitbox.width, 
                      map->components[port->handle.handler.index].hitbox.height}, 
            port->offset));
    Color result_color = color;

    switch(map->components[port->handle.handler.index].type){
        case TYPE_BUTTON: {} break;

        case TYPE_LED: {
            switch(port->in->out){
                case 1: {
                    result_color = map->components[port->handle.handler.index].data.led.tint_on;
                }
                break;

                case 0: {
                    result_color = map->components[port->handle.handler.index].data.led.tint_off;
                }
                break;
            }
        }
        break;

        default: {} break;
    }

    DrawLineEx(
        begin_point,
        end_point,
        2,
        result_color
    );
}

void map_draw(void){
    Color connection_color = BLACK;
    for(uint32_t i = 1; i < map->components_count; i++){
        draw_component(&map->components[i]);
    }

    for(uint32_t i = 1; i < map->components_count; i++){
        if(map->components[i].digital_ports_count == 0) continue;
        for(uint32_t j = 0; j < map->components[i].digital_ports_count; j++){
            draw_connection(&map->components[i].ports[j], connection_color);
            //fprintf(stdout, "TODO: many ports\n");
        }
    }
}

#endif
