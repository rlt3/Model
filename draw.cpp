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
    "in vec4 vertex;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(vertex.xyz, 1.0);\n"
    "}";

static const GLchar* fragment_source =
    "#version 130\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}";

static float vertices[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f
};

// world space positions of our cubes
static glm::vec3 cubePositions[] = {
	glm::vec3( 0.0f,  0.0f,  0.0f),
	glm::vec3( 2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3( 2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3( 1.3f, -2.0f, -2.5f),
	glm::vec3( 1.5f,  2.0f, -2.5f),
	glm::vec3( 1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

inline int
compile_shader (const char *src, int type)
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

Window::Window ()
{
    int maxlength, status;

    this->deltaTime = 0.0f;
    this->lastFrame = 0.0f;

    this->fov = 45.0f;
    this->cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
    this->cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    this->cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

    /* 
     * yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a
     * direction vector pointing to the right so we initially rotate a bit to
     * the left.
     */
    this->yaw   = -90.0f;
    this->pitch =  0.0f;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL Failed to init: %s\n", SDL_GetError());
        exit(1);
    }

    this->window = SDL_CreateWindow("Model", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        800, 600, 
        SDL_WINDOW_OPENGL);

    if (!this->window) {
        fprintf(stderr, "Window could not be created: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    /* 
     * Mix OpenGL and SDL draw calls at your own peril.
     */
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

    glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

    this->vert_shader = compile_shader(vertex_source, GL_VERTEX_SHADER);
    this->frag_shader = compile_shader(fragment_source, GL_FRAGMENT_SHADER);
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

	/* use the shaders compiled above */
    glUseProgram(this->shader_prog);
    /* Setup the buffer and attribute buffers so we can set values */
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /* reserve size of vertices buffer */
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint posAttrib = glGetAttribLocation(shader_prog, "vertex");
    /* 3 float items per vertex, with a stride of 3 floats between vertices */
    glVertexAttribPointer(posAttrib, 3, 
            GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    if (SDL_GL_SetSwapInterval(1) < 0)
        fprintf(stderr, "Warning: SwapInterval could not be set: %s\n", 
                SDL_GetError());

    SDL_SetRelativeMouseMode(SDL_TRUE);
}

Window::~Window()
{
    glDeleteShader(this->vert_shader);
    glDeleteShader(this->frag_shader);
    glDeleteProgram(this->shader_prog);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->VAO);
    SDL_GL_DeleteContext(this->glContext);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void
Window::mouselook (float xrel, float yrel)
{

    static const float sensitivity = 0.1f;
    yaw += xrel * sensitivity;
    /* negate yrel to make up go up and down go down */ 
    pitch += -yrel * sensitivity;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int
Window::render ()
{
    unsigned long currentFrame = SDL_GetTicks();
    this->deltaTime = currentFrame - this->lastFrame;
    this->lastFrame = currentFrame;
    
    float cameraSpeed = 5.0 * ((float)this->deltaTime * 0.001);
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            return 0;

        case SDL_MOUSEMOTION:
            mouselook(e.motion.xrel, e.motion.yrel);
            break;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    /* Quit */
    if (state[SDL_SCANCODE_ESCAPE]) {
        return 0;
    }

    /* Movement */
    if (state[SDL_SCANCODE_W]) {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (state[SDL_SCANCODE_S]) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (state[SDL_SCANCODE_A]) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (state[SDL_SCANCODE_D]) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(this->shader_prog, "projection"),
            1, GL_FALSE, &projection[0][0]);

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(this->shader_prog, "view"),
            1, GL_FALSE, &view[0][0]);

    for (int i = 0; i < 10; i++) {
        // make sure to initialize matrix to identity matrix first
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        glUniformMatrix4fv(glGetUniformLocation(this->shader_prog, "model"),
                1, GL_FALSE, &model[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    SDL_GL_SwapWindow(window);

    return 1;
}
