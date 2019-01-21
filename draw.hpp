#pragma once
#include <vector>
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
    void set_uniform_3fv (const char *name, glm::vec3 vec);
    void set_uniform_mat4fv (const char *name, glm::mat4 matrix);

protected:
    GLuint compile_shader (const char *src, int type);

    GLuint vert_shader;
    GLuint frag_shader;
    GLuint shader_prog;
};

enum CameraDir {
    FORWARD, BACKWARD, RIGHT, LEFT
};

class Camera {
public:
    Camera ();

    Camera (int screen_x, int screen_y);

    void mouselook (float xrel, float yrel);
    void move (CameraDir dir, float delta);

    glm::mat4 projection ();
    glm::mat4 view ();
    glm::vec3 pos ();

protected:
    int screen_x;
    int screen_y;

    float yaw;
    float pitch;
    float fov;

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
};

class Window {
public:
    Window ();
    ~Window ();

    bool should_close ();

    /* Give info about what to draw and where */
    void draw_cube (float x, float y, float z);
    
    /* Clear the window, draw the internal objects, and flip */
    int render ();

protected:
    bool should_quit;

    unsigned long delta_time;
    unsigned long last_frame;

    std::vector<glm::vec3> object_positions;

    Camera camera;
    Shader shader;
    SDL_Window *window;
    SDL_GLContext glContext;
    SDL_Event e;
    GLuint VAO;
    GLuint VBO;
};
