#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    Shader ();
    Shader (const char *vert_source, const char *frag_source);
    void use ();
    void destroy ();
    GLuint get_attrib_loc (const char *name);
    void set_uniform_3f (const char *name, float x, float y, float z);
    void set_uniform_3fv (const char *name, glm::vec3 &vec);
    void set_uniform_mat4fv (const char *name, glm::mat4 &matrix);

protected:
    GLuint compile_shader (const char *src, int type);

    GLuint vert_shader;
    GLuint frag_shader;
    GLuint shader_prog;
};

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
    float fov;
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    Shader shader;
    SDL_Window   *window;
    SDL_GLContext glContext;
    SDL_Event     e;
    GLuint VAO;
    GLuint VBO;
};
