#pragma once
#define GLM_SWIZZLE
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

    /* using mouse's relative position to look in 3D-space */
    void mouselook (float xrel, float yrel);

    void arcball (float xrel, float yrel);

    /* move camera across the 3D space along a 2D plane */
    void move (CameraDir dir, float delta);

    /* projection matrix (where field of view is setup) */
    glm::mat4 projection ();
    /* the view the camera has in 3D space */
    glm::mat4 view ();
    glm::vec3 pos ();

    void set_pos (glm::vec3 pos, float angle);

protected:
    int screen_x;
    int screen_y;

    float yaw;
    float pitch;
    float fov;

    glm::vec4 target;
    glm::vec3 l_position; /* coordinates of the camera in the 3d plane */
    glm::vec4 position; /* coordinates of the camera in the 3d plane */
    glm::vec3 front; /* where the camera is looking in 3d space */
    glm::vec3 up; /* a normal pointing up to normalize movement on 2d plane */
    glm::vec3 right; /* a normal pointing up to normalize movement on 2d plane */
};

class Window {
public:
    Window ();
    ~Window ();

    unsigned long get_ticks ();

    bool should_close ();

    void handle_input ();

    void camera_pos (float x, float y, float z, float angle);

    /* Give info about what to draw and where */
    void draw_cube (float x, float y, float z);
    
    /* Clear the window, draw the internal objects, and flip */
    void render ();

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
