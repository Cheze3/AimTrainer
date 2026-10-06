#include <glad/gl.h>      // must come before the GLFW include
#include <GLFW/glfw3.h>
#include <cstdio>
#include "camera.hpp"

struct InputState {
    double dx = 0, dy = 0;
    double lastX = 0, lastY = 0;
    bool first = true;
};

static void cursorCallback(GLFWwindow* w, double x, double y) {
    InputState* in = static_cast<InputState*>(glfwGetWindowUserPointer(w));
    if (in->first) { in->lastX = x; in->lastY = y; in->first = false; }
    in->dx += x - in->lastX;      // accumulate, never overwrite
    in->dy += y - in->lastY;
    in->lastX = x; in->lastY = y;
}

int main() {
    if (!glfwInit()) {
        std::fprintf(stderr, "glfwInit failed\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);   // required on macOS
#endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "AimTrainer", nullptr, nullptr);
    if (!window) {
        std::fprintf(stderr, "window creation failed\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);   // VSync off: we want to see the real frame rate

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::fprintf(stderr, "gladLoadGL failed\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    std::printf("OpenGL %s\n", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        InputState input;
    glfwSetWindowUserPointer(window, &input);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    else
        std::printf("raw mouse motion NOT supported on this system\n");
    input.first = true;
    glfwSetCursorPosCallback(window, cursorCallback);

    // Hardcoded until the settings file is wired in
    const double dpi = 800.0, cm360 = 35.0;
    const double dpc = degPerCount(cm360, dpi);
    Camera cam;

    double previous = glfwGetTime();
    double lastReport = previous;
    int frames = 0;

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        double dt = now - previous;   // unused for now, this is where it will live
        previous = now;
        (void)dt;

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        applyMouseDelta(cam, input.dx, input.dy, dpc);
        input.dx = input.dy = 0.0;

        int fbw = 0, fbh = 0;
        glfwGetFramebufferSize(window, &fbw, &fbh);   // pixels, not window units (Retina)
        glViewport(0, 0, fbw, fbh);

        glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        ++frames;
        if (now - lastReport >= 1.0) {
            std::printf("FPS: %d\n", frames);
            frames = 0;
            lastReport = now;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}