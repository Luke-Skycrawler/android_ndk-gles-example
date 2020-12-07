// Example app that draws a triangle. The triangle can be moved via touch or keyboard arrow keys.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "esUtil.h"
// #include "shader.h"
#include "glfm.h"
#define FILE_COMPAT_ANDROID_ACTIVITY glfmAndroidGetActivity()
#include "file_compat.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#ifndef GL_RED 
#define GL_RED 0x1903
#endif
#include <android/log.h>
#define LOG_TAG "FUCK"
#define  LOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
typedef struct {
    GLuint program;
    GLuint vertexBuffer;

    float lastTouchX;
    float lastTouchY;

    float offsetX;
    float offsetY;
} ExampleApp;

static void onFrame(GLFMDisplay *display, double frameTime);
static void onSurfaceCreated(GLFMDisplay *display, int width, int height);
static void onSurfaceDestroyed(GLFMDisplay *display);
static bool onTouch(GLFMDisplay *display, int touch, GLFMTouchPhase phase, double x, double y);
static bool onKey(GLFMDisplay *display, GLFMKey keyCode, GLFMKeyAction action, int modifiers);

// Main entry point
void glfmMain(GLFMDisplay *display) {
    ExampleApp *app = calloc(1, sizeof(ExampleApp));

    glfmSetDisplayConfig(display,
                         GLFMRenderingAPIOpenGLES2,
                         GLFMColorFormatRGBA8888,
                         GLFMDepthFormat24,
                         GLFMStencilFormat8,
                         GLFMMultisampleNone);
    glfmSetUserData(display, app);
    glfmSetSurfaceCreatedFunc(display, onSurfaceCreated);
    glfmSetSurfaceResizedFunc(display, onSurfaceCreated);
    glfmSetSurfaceDestroyedFunc(display, onSurfaceDestroyed);
    glfmSetMainLoopFunc(display, onFrame);
    glfmSetTouchFunc(display, onTouch);
    glfmSetKeyFunc(display, onKey);
}
bool fistMouse=true;
static float lightPos[3]={1.2f,1.0f,2.0f},cameraPosition[3]={0.0f,0.0f,3.0f};
static ESMatrix projection,view,model;
static bool onTouch(GLFMDisplay *display, int touch, GLFMTouchPhase phase, double x, double y) {
    if (phase == GLFMTouchPhaseHover) {
        return false;
    }
    ExampleApp *app = glfmGetUserData(display);
    if (phase != GLFMTouchPhaseBegan) {
        int width, height;
        glfmGetDisplaySize(display, &width, &height);
        app->offsetX += 2 * (x - app->lastTouchX) / width;
        app->offsetY -= 2 * (y - app->lastTouchY) / height;
    }
    app->lastTouchX = x;
    app->lastTouchY = y;
    float front[3];
    front[0]=cos(app->offsetX)*cos(app->offsetY);
    front[1]=sin(app->offsetY);
    front[2]=sin(app->offsetX)*cos(app->offsetY);
    for(int i=0;i<3;i++)front[i]+=cameraPosition[i];
    //esMatrixLookAt ( &view,cameraPosition[0],cameraPosition[1],cameraPosition[2] ,front[0], front[1], front[2],0.0f, 1.0f, 0.0f );
    return true;
    // if (firstMouse)
    // {
    //     app->lastTouchX = x;
    //     app->lastTouchY = y;
    //     firstMouse = false;
    // }

    // float xoffset = xpos - lastX;
    // float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    // lastX = xpos;
    // lastY = ypos;

    // camera.ProcessMouseMovement(xoffset, yoffset);
}

static bool onKey(GLFMDisplay *display, GLFMKey keyCode, GLFMKeyAction action, int modifiers) {
    bool handled = false;
    if (action == GLFMKeyActionPressed) {
        ExampleApp *app = glfmGetUserData(display);
        switch (keyCode) {
            case GLFMKeyLeft:
                app->offsetX -= 0.1f;
                handled = true;
                break;
            case GLFMKeyRight:
                app->offsetX += 0.1f;
                handled = true;
                break;
            case GLFMKeyUp:
                app->offsetY += 0.1f;
                handled = true;
                break;
            case GLFMKeyDown:
                app->offsetY -= 0.1f;
                handled = true;
                break;
            default:
                break;
        }
    }
    return handled;
}

static void onSurfaceCreated(GLFMDisplay *display, int width, int height) {
    glViewport(0, 0, width, height);

    GLFMRenderingAPI api = glfmGetRenderingAPI(display);
    LOG("Fuck, Hello from GLFM! Using OpenGL %s\n",
           api == GLFMRenderingAPIOpenGLES32 ? "ES 3.2" :
           api == GLFMRenderingAPIOpenGLES31 ? "ES 3.1" :
           api == GLFMRenderingAPIOpenGLES3 ? "ES 3.0" : "ES 2.0");
}

static void onSurfaceDestroyed(GLFMDisplay *display) {
    // When the surface is destroyed, all existing GL resources are no longer valid.
    ExampleApp *app = glfmGetUserData(display);
    app->program = 0;
    app->vertexBuffer = 0;
}

static GLuint compileShader(GLenum type, const char *shaderName) {
    char fullPath[PATH_MAX];
    fc_resdir(fullPath, sizeof(fullPath));
    strncat(fullPath, shaderName, sizeof(fullPath) - strlen(fullPath) - 1);

    // Get shader string
    char *shaderString = NULL;
    FILE *shaderFile = fopen(fullPath, "rb");
    if (shaderFile) {
        fseek(shaderFile, 0, SEEK_END);
        long length = ftell(shaderFile);
        fseek(shaderFile, 0, SEEK_SET);

        shaderString = malloc(length + 1);
        if (shaderString) {
            fread(shaderString, length, 1, shaderFile);
            shaderString[length] = 0;
        }
        fclose(shaderFile);
    }
    if (!shaderString) {
        LOG("fuck, Couldn't read file: %s\n", fullPath);
        return 0;
    }
    // Compile
    const char *constChaderString = shaderString;
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &constChaderString, NULL);
    glCompileShader(shader);
    free(shaderString);

    // Check compile status
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        LOG("fuck, Couldn't compile shader: %s\n", shaderName);
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = malloc(logLength);
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            if (log[0] != 0) {
                LOG("fuck, Shader log: %s\n", log);
            }
            free(log);
        }
        glDeleteShader(shader);
        shader = 0;
    }
    return shader;
}
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        LOG("Texture loaded");
    }
    else
    {
        LOG("Texture failed to load at path: ");// << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
#define setVec3v(name,vec) glUniform3fv(glGetUniformLocation(app->program,name), 1, vec); 
#define setVec3(name,x,y,z) glUniform3f(glGetUniformLocation(app->program, name), x, y, z); 
#define setMat4(name,mat) glUniformMatrix4fv(glGetUniformLocation(app->program, name), 1, GL_FALSE, &(mat.m[0][0]));
#define SCR_WIDTH 2244
#define SCR_HEIGHT 1080
// struct Shader{
//     unsigned int ID;
//     void setMat4(const char* name, float **mat)
//     {
//         glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, mat);
//     };
//     void setVec3(const char* name, float *value)
//     { 
//         glUniform3fv(glGetUniformLocation(ID, name), 1, value); 
//     };
// };
static void onFrame(GLFMDisplay *display, double frameTime) {
    static ExampleApp *app;
    static unsigned int diffuseMap,specularMap;
    static unsigned int VBO, cubeVAO;
    static unsigned int lightCubeVAO;
    static float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    app = glfmGetUserData(display);
    // static Shader lightingShader("1.color.vs", "1.color.fs");
    
    if (app->program == 0) {
        // app->program=ID;
        // GLuint vertShader = compileShader(GL_VERTEX_SHADER, "simple.vert");
        // GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, "simple.frag");
        GLuint vertShader = compileShader(GL_VERTEX_SHADER, "1.color.vs");
        GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, "1.color.fs");
        if (vertShader == 0 || fragShader == 0) {
            glfmSetMainLoopFunc(display, NULL);
            return;
        }
        app->program = glCreateProgram();

        glAttachShader(app->program, vertShader);
        glAttachShader(app->program, fragShader);

        // glBindAttribLocation(app->program, 0, "a_position");
        // glBindAttribLocation(app->program, 1, "a_color");

        glLinkProgram(app->program);

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        // first, configure the cube's VAO (and VBO)
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);
        // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
        glGenVertexArrays(1, &lightCubeVAO);
        glBindVertexArray(lightCubeVAO);

        // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        diffuseMap = loadTexture("container2.png");
        specularMap = loadTexture("container2_specular.png");
        // esMatrixLookAt ( &view,cameraPosition[0],cameraPosition[1],cameraPosition[2] ,0.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f );
        for (int i = 0; i < 4; i++)for (int j = 0; j < 4; j++) {
			model.m[i][j] = view.m[i][j] = projection.m[i][j] = i == j ? 1 : 0;
		}
    }

    // Draw background
        glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        glUseProgram(app->program);

        setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        setVec3v("lightPos",lightPos);
        setVec3v("viewPos",cameraPosition);
        // view/projection transformations
        // world transformation
        // esPerspective ( &projection, 45.0f, (float)SCR_WIDTH/(float)SCR_WIDTH, 0.1f, 100.0f );
        // ESMatrix view = camera.GetViewMatrix();
        // esMatrixLoadIdentity ( &model );

        // create view matrix transformation from the eye position

        // esMatrixMultiply ( &modelview, &model, &view );

        setMat4("projection", projection);
        setMat4("view", view);
        setMat4("model", model);



        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

    // if (app->vertexBuffer == 0) {
    //     glGenBuffers(1, &app->vertexBuffer);
    // }
    // glBindBuffer(GL_ARRAY_BUFFER, app->vertexBuffer);
    // const size_t stride = sizeof(GLfloat) * 6;
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(sizeof(GLfloat) * 3));

    // const GLfloat vertices[] = {
    //     // x,y,z, r,g,b
    //     app->offsetX + 0.0f, app->offsetY + 0.5f, 0.0,  1.0, 0.0, 0.0,
    //     app->offsetX - 0.5f, app->offsetY - 0.5f, 0.0,  0.0, 1.0, 0.0,
    //     app->offsetX + 0.5f, app->offsetY - 0.5f, 0.0,  0.0, 0.0, 1.0,
    // };

    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
}
