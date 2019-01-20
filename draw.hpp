#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Window {
public:
    Window ();
    ~Window ();

    int render ();

protected:
    void mouselook (float xpos, float ypos);

    unsigned long deltaTime;
    unsigned long lastFrame;

    bool firstMouse;
    float yaw;
    float pitch;
    float lastX;
    float lastY;

    float fov;
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    SDL_Window   *window;
    SDL_GLContext glContext;
    SDL_Event     e;
    GLuint VAO;
    GLuint VBO;
    GLuint shader_prog;
    GLuint vert_shader;
    GLuint frag_shader;
};
