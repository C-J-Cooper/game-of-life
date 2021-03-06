#include "renderer.h"
#include "world.h"
#include "window.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int init(struct Renderer **renderer, struct World **world, enum ColorScheme cs);
void cleanup(struct Renderer *renderer, struct World *world);
void printUsage();
void printControls();

// Global GLFW window
extern struct Window window;

int main(int argc, char *argv[]) {

    int load_file = 0;
    char load_file_path[256];
    int save_file = 0;
    char save_file_path[256];
    enum ColorScheme color_scheme = Terminal;

    int opt;
    while ((opt = getopt(argc, argv, "l:s:c:")) != -1) {
        switch (opt) {
            case 'l':
                load_file = 1;
                strcpy(load_file_path, optarg);
                break;
            case 's':
                save_file = 1;
                strcpy(save_file_path, optarg);
                break;
            case 'c':
                if (strcmp(optarg, "terminal") == 0)
                    color_scheme = Terminal;
                else if (strcmp(optarg, "light") == 0)
                    color_scheme = Light;
                else if (strcmp(optarg, "grayscale") == 0)
                    color_scheme = Grayscale;

                break;
            case ':':
                fprintf(stderr, "Option needs a value\n");
                printUsage();
                return 1;
                break;
            default:
                fprintf(stderr, "Unrecognised command line option.\n");
                printUsage();
                return 1;
        }
    }

    struct Renderer *renderer = NULL;
    struct World *world = NULL;
    if (!init(&renderer, &world, color_scheme)) {
        cleanup(renderer, world);
        return 1;
    }
    

    if (load_file && !worldLoadFromFile(world, load_file_path)) {
        fprintf(stderr, "Failed to load world file %s\n", load_file_path);
        cleanup(renderer, world);
        return 1;
    }
    rendererRecenter(renderer, world);
    if (!rendererGrowWorldToFillView(renderer, world)) {
        cleanup(renderer, world);
        return 1;
    }
    printControls();

    // Main Loop
    windowLoop(renderer, world);

    if (save_file)
        worldSaveToFile(world, save_file_path);

    cleanup(renderer, world);

    return 0;
}

int init(struct Renderer **renderer, struct World **world, enum ColorScheme color_scheme) {

    if (!windowInit()) {
        fprintf(stderr, "Failed to setup window.\n");
        return 0;
    }

    *renderer = rendererCreate(color_scheme);
    if (!(*renderer)) {
        fprintf(stderr, "Failed to create the renderer.\n");
        return 0;
    }

    *world = worldCreate(color_scheme);
    if (!(*world)) {
        fprintf(stderr, "Failed to create world.\n");
        return 0;
    }

    return 1;
}

void cleanup(struct Renderer *renderer, struct World *world) {
    worldDestroy(world);
    rendererDestroy(renderer);
    windowCleanup();
}

void printUsage() {
    fprintf(stderr, "./game_of_life -l load_file_path -s save_file_path -c [terminal, light, grayscale]");
}

void printControls() {
    fprintf(stderr, "  Move      = arrows keys or wasd\n");
    fprintf(stderr, "  Zoom      = scroll wheel\n");
    fprintf(stderr, "  Edit mode = e or space\n");
    fprintf(stderr, "  Edit cell = left mouse when in edit mode\n");
    fprintf(stderr, "  Speed up  = shift + w or shift + up arrow\n");
    fprintf(stderr, "  Slow down = shift + s or shift + down arrow\n");
}
