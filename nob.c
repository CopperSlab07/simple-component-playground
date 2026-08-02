#define NOB_IMPLEMENTATION
#include "vendor/nob.h"

void build_common_windows(Nob_Cmd *cmd, const char *input, const char *output){
    nob_cmd_append(cmd, "gcc", "-std=c11", "-Wall", "-Wextra", "-ggdb");
    nob_cmd_append(cmd, "-I./vendor", "-I./vendor/raylib-6.0_win64_mingw-w64/include"); //change the path of raylib's includes here
    nob_cmd_append(cmd, "-o", output, input);
    nob_cmd_append(cmd, "-L./vendor/raylib-6.0_win64_mingw-w64/lib", "-lraylib", "-lgdi32", "-lwinmm"); //change the path of raylib's libs here
}

void build_common_linux(Nob_Cmd *cmd, const char *input, const char *output){
    nob_cmd_append(cmd, "gcc", "-std=c11", "-Wall", "-Wextra", "-ggdb");
    nob_cmd_append(cmd, "-I./vendor", "-I./vendor/raylib-6.0_linux_amd64/include"); //change the path of raylib's includes here
    nob_cmd_append(cmd, "-o", output, input);
    nob_cmd_append(cmd, "-L./vendor/raylib-6.0_linux_amd64/lib", "-lm", "-lX11", "-Wl,-rpath=./vendor/raylib-6.0_linux_amd64/lib", "-lraylib"); //change the path of raylib's libs here
}

int main(int argc, char **argv){
    NOB_GO_REBUILD_URSELF(argc, argv);
    Nob_Cmd cmd = {0};
    //build_common_windows(&cmd, "src/main.c", "bin/run");
    //if(!nob_cmd_run(&cmd)) return -1;
    build_common_windows(&cmd, "src/main_atlas.c", "bin/run_atlas");
    if(!nob_cmd_run(&cmd)) return -1;
    //build_common_windows(&cmd, "src/todo.c", "bin/todo.exe");
    //if(!nob_cmd_run(&cmd)) return -1;
    return 0;
}
