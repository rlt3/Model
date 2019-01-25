#pragma once
#define GLM_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
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

enum CameraMode {
    FPS, ARCBALL
};

class Camera {
public:
    Camera ();
    Camera (int screen_x, int screen_y);
    Camera (int screen_x, int screen_y, CameraMode mode);

    /* using x, y position to orient where one looks in 3D-space */
    void look (int x, int y);

    /* zooming in and out of where one looks in 3D-space */
    void zoom (int increment);

    /* move camera across the 3D space along a 2D plane */
    void move (CameraDir dir, float delta);

    /* projection matrix (where field of view is setup) */
    glm::mat4 projection ();

    /* the view the camera has in 3D space */
    glm::mat4 view ();

    /* set the current mode of the camera */
    void set_mode (CameraMode mode);

    glm::vec3 pos ();
    void set_pos (glm::vec3 pos, float angle);

protected:
    void fps_look (int x, int y);
    void arc_look (int x, int y);

    void fps_move (CameraDir dir, float delta);
    void arc_move (CameraDir dir, float delta);

    glm::mat4 fps_view ();
    glm::mat4 arc_view ();

    CameraMode mode;

    int screen_x;
    int screen_y;

    float yaw;
    float pitch;
    float fov;
    float zoomf;

    glm::vec4 target;
    glm::vec4 position; /* coordinates of the camera in the 3d plane */
    glm::vec3 front; /* where the camera is looking in 3d space */
    glm::vec3 up; /* a normal pointing up to normalize movement on 2d plane */
    glm::vec4 lefthand; /* vector for left-handed coordinate system */
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
