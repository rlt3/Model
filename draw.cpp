#define GL_GLEXT_PROTOTYPES 1
#include <cstdlib>
#include "draw.hpp"

void
checkGLError()
{
    GLenum err = glGetError();
    const GLubyte* errstring;
    if (err != GL_NO_ERROR) {
        errstring = gluErrorString(err);
        fprintf(stderr, "ERROR: %s\n", errstring);
    }
}

static const GLchar* vertex_source =
    "#version 130\n"
    "in vec3 vertex;\n"
    "in vec3 norm;\n"
	"out vec3 FragPos;\n"
	"out vec3 Normal;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   FragPos = vec3(model * vec4(vertex, 1.0));\n"
    "   Normal = norm;\n"
    "   gl_Position = projection * view * model * vec4(vertex.xyz, 1.0);\n"
    "}";

static const GLchar* fragment_source =
    "#version 130\n"
    "out vec4 FragColor;\n"
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec3 objectColor;\n"
    "void main()\n"
    "{\n"
    "   float ambientStrength = 0.1;\n"
    "   vec3 ambient = ambientStrength * lightColor;\n"
    "\n"
    "   vec3 norm = normalize(Normal);\n"
    "   vec3 lightDir = normalize(lightPos - FragPos);\n"
    "   float diff = max(dot(norm, lightDir), 0.0);\n"
    "   vec3 diffuse = diff * lightColor;\n"
    "\n"
    "   vec3 result = (ambient + diffuse) * objectColor;\n"
    "   FragColor = vec4(result, 1.0);\n"
    "}";

static const float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

Shader::Shader ()
    : vert_shader(0)
    , frag_shader(0)
    , shader_prog(0)
{ }

Shader::Shader (const char *vert_source, const char *frag_source)
{
    int status, maxlength;

    this->vert_shader = compile_shader(vert_source, GL_VERTEX_SHADER);
    this->frag_shader = compile_shader(frag_source, GL_FRAGMENT_SHADER);
    this->shader_prog = glCreateProgram();

    glAttachShader(shader_prog, vert_shader);
    glAttachShader(shader_prog, frag_shader);
    glLinkProgram(shader_prog);

    /* check for linking errors */
    glGetProgramiv(shader_prog, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        glGetProgramiv(shader_prog, GL_INFO_LOG_LENGTH, &maxlength);
        if (maxlength > 0) {
            char buffer[maxlength];
            glGetProgramInfoLog(shader_prog, maxlength, NULL, buffer);
            fprintf(stderr, "OpenGL shader failed to link: %s\n", buffer);
            exit(1);
        }
    }
}

void
Shader::use ()
{
    if (this->shader_prog == 0) {
        fprintf(stderr, "Shader not initialized or may have been destroyed.\n");
        exit(1);
    }
    glUseProgram(this->shader_prog);
}

void
Shader::destroy ()
{
    glDeleteShader(this->vert_shader);
    glDeleteShader(this->frag_shader);
    glDeleteProgram(this->shader_prog);
    this->vert_shader = 0;
    this->frag_shader = 0;
    this->shader_prog = 0;
}

GLuint
Shader::get_attrib_loc (const char *name)
{
    return glGetAttribLocation(shader_prog, name);
}

void
Shader::set_uniform_3f (const char *name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(this->shader_prog, name), x, y, z);
}

void
Shader::set_uniform_3fv (const char *name, glm::vec3 vec)
{
    glUniform3fv(glGetUniformLocation(this->shader_prog, name), 1, &vec[0]);
}

void
Shader::set_uniform_mat4fv (const char *name, glm::mat4 matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(this->shader_prog, name),
            1, GL_FALSE, &matrix[0][0]);
}

GLuint
Shader::compile_shader (const char *src, int type)
{
    char buffer[512];
    int status, shader_id;
    shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, &src, NULL);
    glCompileShader(shader_id);
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(shader_id, 512, NULL, buffer);
        fprintf(stderr, "Shader failed to compile: %s\n", buffer);
        exit(1);
    }
    return shader_id;
}

Camera::Camera ()
    : screen_x(0)
    , screen_y(0)
    , mode(FPS)
    , fps_yaw(0)
    , fps_pitch(0)
    , arc_yaw(0)
    , arc_pitch(0)
    , fov(0)
    , zoomf(0)
    , target(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
    , fps_position(glm::vec4(0.0f, 0.0f, 3.0f, 1.0f))
    , arc_position(glm::vec4(0.0f, 0.0f, 3.0f, 1.0f))
    , front(glm::vec3(0.0f, 0.0f, -1.0f))
    , up(glm::vec3(0.0f, 1.0f, 0.0f))
    , lefthand(glm::vec4(0.f, 0.f, -1.f, 1.f))
{ }

Camera::Camera (int screen_x, int screen_y)
    : screen_x(screen_x)
    , screen_y(screen_y)
    , mode(FPS)
    , fps_yaw(180.f)
    , fps_pitch(0)
    , arc_yaw(0)
    , arc_pitch(0)
    , fov(45.0f)
    , zoomf(0)
    , target(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
    , fps_position(glm::vec4(0.0f, 0.0f, 3.0f, 1.0f))
    , arc_position(glm::vec4(0.0f, 0.0f, 3.0f, 1.0f))
    , front(glm::vec3(0.0f, 0.0f, -1.0f))
    , up(glm::vec3(0.0f, 1.0f, 0.0f))
    , lefthand(glm::vec4(0.f, 0.f, -1.f, 1.f))
{
    zoomf = -250.f;
}

Camera::Camera (int screen_x, int screen_y, CameraMode mode)
    : screen_x(screen_x)
    , screen_y(screen_y)
    , mode(mode)
    , fps_yaw(180.f)
    , fps_pitch(0)
    , arc_yaw(0)
    , arc_pitch(0)
    , fov(45.0f)
    , zoomf(0)
    , target(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
    , fps_position(glm::vec4(0.0f, 0.0f, 3.0f, 1.0f))
    , arc_position(glm::vec4(0.0f, 0.0f, 3.0f, 1.0f))
    , front(glm::vec3(0.0f, 0.0f, -1.0f))
    , up(glm::vec3(0.0f, 1.0f, 0.0f))
    , lefthand(glm::vec4(0.f, 0.f, -1.f, 1.f))
{
    zoomf = -250.f;
}

/* zooming in and out of where one looks in 3D-space */
void
Camera::zoom (int increment)
{
    static const float speed = 5.f;
    this->zoomf = glm::clamp(this->zoomf + speed * increment, -1000.f, 1000.f);
}

/* using x, y position to orient where one looks in 3D-space */
void
Camera::look (int x, int y)
{
    switch (mode) {
        case FPS: fps_look(x, y); break;
        case ARCBALL: arc_look(x, y); break;
    }
}

/* move camera across the 3D space along a 2D plane */
void
Camera::move (CameraDir dir, float delta)
{
    switch (mode) {
        case FPS: fps_move(dir, delta); break;
        case ARCBALL: arc_move(dir, delta); break;
    }
}

glm::mat4
Camera::view ()
{
    switch (mode) {
        case FPS: return fps_view();
        default:
        case ARCBALL: return arc_view();
    }
}

void
Camera::fps_look (int x, int y)
{
    static const float sensitivity = 0.1f;
    glm::vec3 f;

    fps_yaw = fmodf(fps_yaw + (x * sensitivity), 360.f);
    fps_pitch = glm::clamp(fps_pitch + -(y * sensitivity), -90.f, 90.f);

    f.x = cos(glm::radians(fps_yaw)) * cos(glm::radians(fps_pitch));
    f.y = sin(glm::radians(fps_pitch));
    f.z = sin(glm::radians(fps_yaw)) * cos(glm::radians(fps_pitch));
    this->front = glm::normalize(f);
}

void
Camera::arc_look (int x, int y)
{
    static const float sensitivity = 0.1f;
    arc_yaw = fmodf(arc_yaw + (x * sensitivity), 360.f);
    arc_pitch = glm::clamp(arc_pitch + -(y * sensitivity), -90.f, 90.f);
}

/*
 * Pans camera left and right, keeping height, by moving the target around
 * the camera arcs.
 */
void
Camera::arc_move (CameraDir dir, float delta)
{
    float speed = 50.0 * delta;

    glm::vec4 right, forward;

    right = glm::normalize(target - arc_position);
    right = glm::vec4(glm::cross(glm::vec3(right), up), 1.f);
    right.y = 0;
    right = glm::normalize(right);

    forward = glm::normalize(target - arc_position);
    forward.y = 0;
    forward = glm::normalize(forward);

    switch (dir) {
        case FORWARD:
            target += forward * speed;
            break;
        case BACKWARD:
            target -= forward * speed;
            break;
        case RIGHT:
            target += right * speed;
            break;
        case LEFT:
            target -= right * speed;
            break;
    }
}

/*
 * Moves the cameara itself and uses the front to translate its motion into
 * 3D spaces. It's not restricted to a plane like the above.
 */
void
Camera::fps_move (CameraDir dir, float delta)
{
    float speed = 50.0 * delta;
    switch (dir) {
        case FORWARD:
            fps_position += speed * glm::vec4(front, 1.f);
            break;
        case BACKWARD:
            fps_position -= speed * glm::vec4(front, 1.f);
            break;
        case LEFT:
            fps_position -= glm::vec4(glm::normalize(glm::cross(front, up)) * speed, 1.f);
            break;
        case RIGHT:
            fps_position += glm::vec4(glm::normalize(glm::cross(front, up)) * speed, 1.f);
            break;
    }
}

glm::mat4
Camera::fps_view ()
{
    glm::vec3 pos(fps_position);
    return glm::lookAt(pos, pos + front, up);
}

/*
 * The arcball view of the world space.
 */
glm::mat4
Camera::arc_view ()
{
    glm::vec4 pos;
    /* the order of this matters and yes, `lefthand' is required */
    pos = glm::yawPitchRoll(
            -glm::radians(arc_yaw - 90.f),
             glm::radians(arc_pitch), 0.f) * lefthand;
    pos *= zoomf;
    pos += target;
    arc_position = pos;
    return glm::lookAt(glm::vec3(arc_position), glm::vec3(target), up);
}

glm::mat4
Camera::projection ()
{
    return glm::perspective(glm::radians(fov),
            (float)screen_x / (float)screen_y, 0.1f, 1000.0f);
}

void
Camera::set_mode (CameraMode mode)
{
    this->mode = mode;
}

glm::vec3
Camera::pos ()
{
    switch (mode) {
        case FPS: return fps_position;
        default:
        case ARCBALL: return arc_position;
    }
}

void
Camera::lookat (glm::vec3 pos)
{
    this->fps_position = glm::vec4(pos.x - zoomf, pos.y, pos.z, 1.0f);
    this->target = glm::vec4(pos.x, pos.y, pos.z, 1.0f);
    fps_look(0,0);
}

Window::Window ()
    : should_quit(false)
    , delta_time(0.0f)
    , last_frame(0.0f)
{
    SDL_DisplayMode display;
    GLuint vertex_id, norm_id;

    object_positions.reserve(50);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL Failed to init: %s\n", SDL_GetError());
        exit(1);
    }

    this->window = SDL_CreateWindow("Model", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        800, 600, /* these won't be used if FULLSCREEN is given below */
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (!this->window) {
        fprintf(stderr, "Window could not be created: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    /* Get screen width and height since we're in fullscreen */
    SDL_GetCurrentDisplayMode(0, &display);
    this->camera = Camera(display.w, display.h, FPS);

    this->glContext = SDL_GL_CreateContext(window);
    if (!this->glContext) {
        printf("Could not create OpenGL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

    this->shader = Shader(vertex_source, fragment_source);
    this->shader.use();

    /* Setup the buffer and attribute buffers so we can set values */
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /* reserve size of vertices buffer */
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* setup vertices attribute, width of 3 in span of 6 elements */
    vertex_id = this->shader.get_attrib_loc("vertex");
    glVertexAttribPointer(vertex_id, 3,
                GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(vertex_id);

    /* setup normal attribute, width of 3, 3 elements into span of 6 elements */
    norm_id = this->shader.get_attrib_loc("norm");
    glVertexAttribPointer(norm_id, 3, GL_FLOAT,
                GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(norm_id);

    if (SDL_GL_SetSwapInterval(1) < 0)
        fprintf(stderr, "Warning: SwapInterval could not be set: %s\n", 
                SDL_GetError());

    /*
     * TODO: rewrite vertices in winding to use this for performance
     * glEnable(GL_CULL_FACE);
     */
}

Window::~Window()
{
    this->shader.destroy();
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->VAO);
    SDL_GL_DeleteContext(this->glContext);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

bool
Window::should_close ()
{
    return this->should_quit;
}

void
Window::lookat (float x, float y, float z)
{
    camera.lookat(glm::vec3(x, y, z));
}

void
Window::draw_cube (float x, float y, float z)
{
    object_positions.push_back(glm::vec3(x, y, z));
}

unsigned long
Window::get_ticks ()
{
    return SDL_GetTicks();
}

glm::vec3
get_coords_from_click (float x, float y,
                       int width, int height,
                       glm::mat4 view,
                       glm::mat4 proj)
{
    float depth;
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    glm::vec4 window(0.f, 0.f, width, height);
    glm::vec3 mouse(x, height - 1.f - y, depth);
    return glm::round(glm::unProject(mouse, view, proj, window));
}

void
Window::handle_input ()
{
    const Uint8 *state;
    unsigned long current_frame;
    float delta;

    current_frame = SDL_GetTicks();
    this->delta_time = current_frame - this->last_frame;
    this->last_frame = current_frame;
    /* time is in miliseconds, dividing by one-thousandth gets delta as float */
    delta = ((float)this->delta_time * 0.001);

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            should_quit = true;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                /* place object */
            }
            if (e.button.button == SDL_BUTTON_RIGHT) {
                /* remove object */
            }
            break;

        case SDL_MOUSEWHEEL:
            camera.zoom(e.wheel.y);
            break;

        case SDL_MOUSEMOTION:
            placeholder = get_coords_from_click(e.button.x, e.button.y,
                                            camera.screen_x, camera.screen_y,
                                            camera.view(),
                                            camera.projection());
            if (e.button.button == SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
                camera.look(e.motion.xrel, e.motion.yrel);
            }
            break;
        }
    }

    state = SDL_GetKeyboardState(NULL);

    SDL_SetRelativeMouseMode((SDL_bool) state[SDL_SCANCODE_LSHIFT]);

    /* Quit */
    if (state[SDL_SCANCODE_ESCAPE]) {
        should_quit = true;
    }

    /* Movement */
    if (state[SDL_SCANCODE_W]) {
        camera.move(FORWARD, delta);
    }
    if (state[SDL_SCANCODE_S]) {
        camera.move(BACKWARD, delta);
    }
    if (state[SDL_SCANCODE_A]) {
        camera.move(LEFT, delta);
    }
    if (state[SDL_SCANCODE_D]) {
        camera.move(RIGHT, delta);
    }
    if (state[SDL_SCANCODE_I]) {
        camera.set_mode(FPS);
    }
    if (state[SDL_SCANCODE_O]) {
        camera.set_mode(ARCBALL);
    }
}

void
Window::render ()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    this->shader.set_uniform_3f("objectColor", 1.0f, 0.5f, 0.31f);
    this->shader.set_uniform_3f("lightColor", 1.0f, 0.5f, 0.31f);

    this->shader.set_uniform_mat4fv("projection", this->camera.projection());
    this->shader.set_uniform_3fv("lightPos", this->camera.pos());
    this->shader.set_uniform_mat4fv("view", this->camera.view());

    for (auto &loc : object_positions) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, loc);
        //model = glm::rotate(model, (float)(SDL_GetTicks() * 0.001), glm::vec3(0, 0, 1));
        this->shader.set_uniform_mat4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    object_positions.clear();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, placeholder);
    this->shader.set_uniform_mat4fv("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    SDL_GL_SwapWindow(window);
}
