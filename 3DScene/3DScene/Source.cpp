#include <iostream>             // cout, cerr
#include <cstdlib>              // EXIT_FAILURE
#include <GL/glew.h>            // GLEW library
#include <GLFW/glfw3.h>         // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h" // Camera class

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "3D Scene"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vaoPlane, vaoPyr, vaoCube, vaoRec, vaoRec2, vaoRec3;       // Handle for the vertex array object
        GLuint vboPlane, vboPyr, vboCube, vboRec, vboRec2, vboRec3;       // Handle for the vertex buffer object
        GLuint nVertices;                                                 // Number of indices of the mesh
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMeshPlane;
    GLMesh gMeshPyr;
    GLMesh gMeshCube;
    GLMesh gMeshRec;
    GLMesh gMeshRec2;
    GLMesh gMeshRec3;
    // Shader program
    GLuint gProgramId;

    // camera
    Camera gCamera(glm::vec3(0.0f, 0.0f, 10.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMeshPlane(GLMesh& meshPlane);
void UCreateMeshPyr(GLMesh& meshPyr);
void UCreateMeshCube(GLMesh& meshCube);
void UCreateMeshRec(GLMesh& meshRec);
void UCreateMeshRec2(GLMesh& meshRec2);
void UCreateMeshRec3(GLMesh& meshRec3);
void UDestroyMeshPlane(GLMesh& meshPlane);
void UDestroyMeshPyr(GLMesh& meshPyr);
void UDestroyMeshCube(GLMesh& meshCube);
void UDestroyMeshRec(GLMesh& meshRec);
void UDestroyMeshRec2(GLMesh& meshRec2);
void UDestroyMeshRec3(GLMesh& meshRec3);
void URenderPlane();
void URenderPyr();
void URenderCube();
void URenderRec();
void URenderRec2();
void URenderRec3();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
    layout(location = 1) in vec4 color;  // Color data from Vertex Attrib Pointer 1

    out vec4 vertexColor; // variable to transfer color data to the fragment shader

    //Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
        vertexColor = color; // references incoming color data
    }
);


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec4 vertexColor; // Variable to hold incoming color data from vertex shader

    out vec4 fragmentColor;

    void main()
    {
        fragmentColor = vec4(vertexColor);
    }
);


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMeshPlane(gMeshPlane); // Calls the function to create the Vertex Buffer Object
    UCreateMeshPyr(gMeshPyr); 
    UCreateMeshCube(gMeshCube);
    UCreateMeshRec(gMeshRec);
    UCreateMeshRec2(gMeshRec2);
    UCreateMeshRec3(gMeshRec3);

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URenderPlane();
        URenderPyr();
        URenderCube();
        URenderRec();
        URenderRec2();
        URenderRec3();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMeshPlane(gMeshPlane);
    UDestroyMeshPyr(gMeshPyr);
    UDestroyMeshCube(gMeshCube);
    UDestroyMeshRec(gMeshRec);
    UDestroyMeshRec2(gMeshRec2);
    UDestroyMeshRec3(gMeshRec3);

    // Release shader program
    UDestroyShaderProgram(gProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


void URenderPlane()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f));
    // 2. Rotates shape by 15 degrees in the x axis
    glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMeshPlane.vaoPlane);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMeshPlane.nVertices);

}

// Pencil Tip
// ----------
void URenderPyr()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(0.25f, 0.5f,0.25f));
    // 2. Rotates shape by 15 degrees in the x axis
    glm::mat4 rotation = glm::rotate(45.5f, glm::vec3(1.0f, 0.0f, 0.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(-2.5f, -3.86f, 2.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMeshPyr.vaoPyr);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMeshPyr.nVertices);

}

// Rubix Cube
// ----------
void URenderCube()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
    // 2. Rotates shape by 15 degrees in the y axis
    glm::mat4 rotation = glm::rotate(10.0f, glm::vec3(0.0, 1.0f, 0.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(2.5f, -3.5f, -1.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMeshCube.vaoCube);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMeshCube.nVertices);
}

// IPad
// ----
void URenderRec()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(3.0f, 0.5f, 5.0f));
    // 2. Rotates shape by 15 degrees in the y axis
    glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, -3.9f, 0.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMeshRec.vaoRec);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMeshRec.nVertices);
}

// Pencil body
// -----------
void URenderRec2()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(0.25f, 0.5f, 3.0f));
    // 2. Rotates shape by 15 degrees in the y axis
    glm::mat4 rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(-2.5f, -3.88f, 0.25f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMeshRec2.vaoRec2);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMeshRec2.nVertices);
}

// Airpods
// -------
void URenderRec3()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(0.65f, 0.65f, 1.2f));
    // 2. Rotates shape by 15 degrees in the y axis
    glm::mat4 rotation = glm::rotate(10.0f, glm::vec3(0.0, 1.0f, 0.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(2.5f, -3.84f, 0.78f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    // Creates a perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Activate the VBOs contained within the mesh's VAO
    glBindVertexArray(gMeshRec3.vaoRec3);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMeshRec3.nVertices);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}


void UCreateMeshPlane(GLMesh& meshPlane)
{
    // Vertex data
    GLfloat verts[] = {
        // Vertex Positions    // Colors (r,g,b,a)
        -2.0f, -2.0f, -2.0f,   0.59f, 0.29f, 0.0f, 1.0f,
         2.0f, -2.0f, -2.0f,   0.59f, 0.29f, 0.0f, 1.0f,
         2.0f, -2.0f,  2.0f,   0.59f, 0.29f, 0.0f, 1.0f,

         2.0f, -2.0f,  2.0f,   0.59f, 0.29f, 0.0f, 1.0f,
        -2.0f, -2.0f,  2.0f,   0.59f, 0.29f, 0.0f, 1.0f,
        -2.0f, -2.0f, -2.0f,   0.59f, 0.29f, 0.0f, 1.0f,

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    meshPlane.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor));

    glGenVertexArrays(1, &meshPlane.vaoPlane); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(meshPlane.vaoPlane);

    // Create VBO
    glGenBuffers(1, &meshPlane.vboPlane);
    glBindBuffer(GL_ARRAY_BUFFER, meshPlane.vboPlane); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}

// Implements the UCreateMesh function
// Pencil Tip
// -----------------------------------
void UCreateMeshPyr(GLMesh& meshPyr)
{
    // Vertex data
    GLfloat verts[] = {
        // Vertex Positions    // Colors (r,g,b,a)
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
         0.0f,  0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
         0.0f,  0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
         0.0f,  0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,

         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
         0.0f,  0.5f,  0.0f,   1.0f, 1.0f, 1.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f, 1.0f,

         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f, 1.0f,
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    meshPyr.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor));

    glGenVertexArrays(1, &meshPyr.vaoPyr); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(meshPyr.vaoPyr);

    // Create VBO
    glGenBuffers(1, &meshPyr.vboPyr);
    glBindBuffer(GL_ARRAY_BUFFER, meshPyr.vboPyr); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}

// Implements the UCreateMesh function
// Rubix Cube
// -----------------------------------
void UCreateMeshCube(GLMesh& meshCube)
{
    // Vertex data
    GLfloat verts[] = {
        // Vertex Positions    // Colors (r,g,b,a)
        //Back Face        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, // green
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,

        //Front Face       
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, // blue
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,

        //Left Face        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 1.0f, // purple
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 1.0f,

        //Right Face       
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, // red
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,

        //Bottom Face      
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, // white
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

        //Top Face         
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, // yellow
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    meshCube.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor));

    glGenVertexArrays(1, &meshCube.vaoCube); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(meshCube.vaoCube);

    // Create VBO
    glGenBuffers(1, &meshCube.vboCube);
    glBindBuffer(GL_ARRAY_BUFFER, meshCube.vboCube); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}

// Implements the UCreateMesh function
// Ipad
// -----------------------------------
void UCreateMeshRec(GLMesh& meshRec)
{
    // Vertex data
    GLfloat verts[] = {
        // Vertex Positions    // Colors (r,g,b,a)
        //Back Face        
        -0.5f, -0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f, -0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f,  0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f,  0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
        -0.5f,  0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
        -0.5f, -0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,

        //Front Face       
        -0.5f, -0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f, -0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f,  0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f,  0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
        -0.5f,  0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
        -0.5f, -0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,

        //Left Face        
        -0.5f,  0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
        -0.5f,  0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
        -0.5f, -0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
        -0.5f, -0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
        -0.5f, -0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
        -0.5f,  0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,

        //Right Face       
         0.5f,  0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f,  0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f, -0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f, -0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f, -0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         0.5f,  0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,

         //Bottom Face      
         -0.5f, -0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
          0.5f, -0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
          0.5f, -0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
          0.5f, -0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         -0.5f, -0.25f,  0.5f,  0.33f, 0.33f, 0.33f, 1.0f,
         -0.5f, -0.25f, -0.5f,  0.33f, 0.33f, 0.33f, 1.0f,

         //Top Face         
         -0.5f,  0.25f, -0.5f,  0.0f, 0.0f, 0.0f, 1.0f,
          0.5f,  0.25f, -0.5f,  0.0f, 0.0f, 0.0f, 1.0f,
          0.5f,  0.25f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,
          0.5f,  0.25f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,
         -0.5f,  0.25f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f,
         -0.5f,  0.25f, -0.5f,  0.0f, 0.0f, 0.0f, 1.0f

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    meshRec.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor));

    glGenVertexArrays(1, &meshRec.vaoRec); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(meshRec.vaoRec);

    // Create VBO
    glGenBuffers(1, &meshRec.vboRec);
    glBindBuffer(GL_ARRAY_BUFFER, meshRec.vboRec); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}


// Implements the UCreateMesh function
// Pencil Body
// -----------------------------------
void UCreateMeshRec2(GLMesh& meshRec2)
{
    // Vertex data
    GLfloat verts[] = {
        // Vertex Positions    // Colors (r,g,b,a)
        //Back Face        
        -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

        //Front Face       
        -0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

        //Left Face        
        -0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

        //Right Face       
         0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

         //Bottom Face      
         -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         -0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

         //Top Face         
         -0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         -0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         -0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    meshRec2.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor));

    glGenVertexArrays(1, &meshRec2.vaoRec2); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(meshRec2.vaoRec2);

    // Create VBO
    glGenBuffers(1, &meshRec2.vboRec2);
    glBindBuffer(GL_ARRAY_BUFFER, meshRec2.vboRec2); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}


// Implements the UCreateMesh function
// Airpods
// -----------------------------------
void UCreateMeshRec3(GLMesh& meshRec3)
{
    // Vertex data
    GLfloat verts[] = {
        // Vertex Positions    // Colors (r,g,b,a)
        //Back Face        
        -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

        //Front Face       
        -0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

        //Left Face        
        -0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

        //Right Face       
         0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

         //Bottom Face      
         -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         -0.5f, -0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         -0.5f, -0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,

         //Top Face         
         -0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
          0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         -0.5f,  0.25f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,
         -0.5f,  0.25f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f

    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 4;

    meshRec3.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerColor));

    glGenVertexArrays(1, &meshRec3.vaoRec3); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(meshRec3.vaoRec3);

    // Create VBO
    glGenBuffers(1, &meshRec3.vboRec3);
    glBindBuffer(GL_ARRAY_BUFFER, meshRec3.vboRec3); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
}

void UDestroyMeshPlane(GLMesh& meshPlane)
{
    glDeleteVertexArrays(1, &meshPlane.vaoPlane);
    glDeleteBuffers(1, &meshPlane.vboPlane);
}

void UDestroyMeshPyr(GLMesh& meshPyr)
{
    glDeleteVertexArrays(1, &meshPyr.vaoPyr);
    glDeleteBuffers(1, &meshPyr.vboPyr);
}


void UDestroyMeshCube(GLMesh& meshCube)
{
    glDeleteVertexArrays(1, &meshCube.vaoCube);
    glDeleteBuffers(1, &meshCube.vboCube);
}

void UDestroyMeshRec(GLMesh& meshRec)
{
    glDeleteVertexArrays(1, &meshRec.vaoRec);
    glDeleteBuffers(1, &meshRec.vboRec);
}


void UDestroyMeshRec2(GLMesh& meshRec2)
{
    glDeleteVertexArrays(1, &meshRec2.vaoRec2);
    glDeleteBuffers(1, &meshRec2.vboRec2);
}

void UDestroyMeshRec3(GLMesh& meshRec3)
{
    glDeleteVertexArrays(1, &meshRec3.vaoRec3);
    glDeleteBuffers(1, &meshRec3.vboRec3);
}

// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}

// Implements destroy shader program
void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // If w key pressed camera forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    // If s key pressed camera backwards
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    // If a key pressed camera left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    // If d key pressed camera right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    // If q key pressed camera up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    // If e key pressed camera down
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}