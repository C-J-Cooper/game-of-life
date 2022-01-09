#include "window.h"

#include <stdio.h>
#include <stdlib.h>

// Global window
struct Window window;

static void framebufferSizeCallback(GLFWwindow *handle, int width, int height) {
    glViewport(0, 0, width, height);
    window.size_x = width;
    window.size_y = height;
}

static void cursorPosCallback(GLFWwindow *handle, double xp, double yp) {
    window.mouse.pos_x = xp;
    window.mouse.pos_y = yp;
}

static void mouseButtonCallback(GLFWwindow *handle, int button, int action, int mods) {
    if (button < 0)
        return;

    fprintf(stderr, "window::mouseButtonCallback: mouse pos x = %f, mouse pos y = %f\n", window.mouse.pos_x, window.mouse.pos_y);

    switch (action) {
        case GLFW_PRESS:
            window.mouse.buttons[button].pressed = 1;
            break;
        case GLFW_RELEASE:
            window.mouse.buttons[button].pressed = 0;
            break;
        default:
            break;
    }
}

static void keyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods) {
    if (key < 0)
        return;

    fprintf(stderr, "window::keyboardCallback: \n");

    switch (action) {
        case GLFW_PRESS:
            window.keyboard.keys[key].pressed = 1;
            break;
        case GLFW_RELEASE:
            window.keyboard.keys[key].pressed = 0;
            break;
        default:
            break;
    }
}

int windowInit() {
    fprintf(stderr, "window::windowInit: \n");
    
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialise GLEW.\n");
        return 0;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA (anti-aliasing)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Use OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window handle
    window.fps.ticks_per_sec = 30.0f;
    window.fps.last_tick = timeNow();
    window.size_x = 1024;
    window.size_y = 768;
    window.handle = glfwCreateWindow(window.size_x, window.size_y, "Game of Life", NULL, NULL);
    if (!window.handle) {
        fprintf(stderr, "Failed to create GLFW window.\n");
        return 0;
    }
    glfwMakeContextCurrent(window.handle);

    // Load the GLFW functions for this OS
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialise GLAD.\n");
        return 0;
    }

    // Finish setting up the window
    glViewport(0, 0, 1024, 768);
    glfwSetFramebufferSizeCallback(window.handle, framebufferSizeCallback);
    glfwSetCursorPosCallback(window.handle, cursorPosCallback);
    glfwSetKeyCallback(window.handle, keyCallback);
    glfwSetMouseButtonCallback(window.handle, mouseButtonCallback);

    return 1;
}

void windowCleanup() {
    fprintf(stderr, "window::windowCleanup: \n");

    glfwTerminate();
}

void windowProcessInput() {
    fprintf(stderr, "window::processInput:\n");
    if (glfwGetKey(window.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.handle, 1);
}

void windowLoop(struct Renderer *renderer, struct World *world) {
    fprintf(stderr, "window::windowLoop: \n");

    while (!glfwWindowShouldClose(window.handle)) {
        windowProcessInput();

        renderClear(renderer);
        renderWorld(renderer, world);

        glfwSwapBuffers(window.handle);
        sleepTillNextTick(&window.fps);
        glfwPollEvents();
    }

}



