// Example app that draws a triangle. The triangle can be moved via touch or keyboard arrow keys.
// #define __ANDROID__
#define GLFM_INCLUDE_ES3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "esUtil.h"
// #include "shader.h"
#include <android/log.h>
#define LOG_TAG "FUCK"
#define  LOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#include "camera.h"
#include "glfm.h"
#include "mesh.h"
#include "model.h"
// #pragma comment(lib, "libassimp.so")
#define FILE_COMPAT_ANDROID_ACTIVITY glfmAndroidGetActivity()
#include "file_compat.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// #ifndef GL_RED 
// #define GL_RED 0x1903
// #endif
#define SCR_WIDTH 2244
#define SCR_HEIGHT 1080
Camera camera(glm::vec3(0.0f,0.0f,0.0f));
float lastFrame=0.0f,deltaTime=0.0f;
static bool edge=false;

static glm::mat4 init(1.0f);
static glm::vec3 lightPos(1.2f,1.0f,2.0f);
/*
#include <fstream>
#include <vector>
struct Vertex{
    Vertex(glm::vec3 pos,glm::vec3(norm)):Position(pos),Normal(norm){}
    glm::vec3 Position;
    glm::vec3 Normal;
};
class Model{
    public:
        Model(const char* filename){load_obj(filename);setupMesh();}
        std::vector<Vertex> vertices;
        std::vector<glm::vec3> points;
        std::vector<glm::vec3> faces;
        std::vector<glm::vec3> normals;
        const void show();
    private:
        void setupMesh();
        void load_obj(const char* filename);
        glm::vec3 Normal(int i);
        unsigned int vbo,ebo,vao;
};
void Model::setupMesh(){
        // create buffers/arrays
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int), &faces[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // // vertex texture coords
        // glEnableVertexAttribArray(2);	
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // // vertex tangent
        // glEnableVertexAttribArray(3);
        // glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // // vertex bitangent
        // glEnableVertexAttribArray(4);
        // glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        // glBindVertexArray(0);
    }
const void Model::show(){
        // draw mesh
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
}
glm::vec3 Model::Normal(int i){
    // normal.push
    glm::vec3 v1(points[faces[i][0]]-points[faces[i][1]]),
        v2(points[faces[i][0]]-points[faces[i][2]]);
    return glm::normalize(glm::cross(v1,v2));
}
void Model::load_obj(const char* filename){
    char c;
	std::ifstream fin(filename);
	int temppoints, tempfaces;
	char tempchars[100];

	fin >> c;
	while (c != 'v') {
		fin.getline(tempchars, 100);
		fin >> c;
	}

	while(c == 'v') {
        float x,y,z;
		fin >> x >> y >> z;	// 读入顶点坐标
		points.push_back(glm::vec3(x,y,z));				// 添加新顶点
		fin >> c;
	}
	while (c != 'f') {
		fin.getline(tempchars, 100);
		fin >> c;
	}
	while (c == 'f') {
        int pt1,pt2,pt3;
		fin >> pt1 >> pt2 >> pt3;			// 读入三角形顶点标号
		if (pt1 *pt2*pt3==0) break;
		faces.push_back(glm::vec3(pt1,pt2,pt3));								// 添加新三角形
		while (c != 'f' && c != 'g') {						// 忽略其他字母表示的含义
			fin.getline(tempchars, 100);
			fin >> c;
		}
		if (c == 'g') 
			break;
	}
	fin.close();
	for (int i = 0; i < faces.size(); i++) {				// 设置三角形法向量
        glm::vec3 tmp = Normal(i);
        normals.push_back(tmp);
        for(int j=0;j<3;j++)
            vertices.push_back(Vertex(points[faces[i][j]],tmp));
	}
}*/

static void setInitModelMatrix(float x,float y){
    init=glm::rotate(glm::mat4(1.0f),x,camera.Up)*glm::rotate(glm::mat4(1.0f),-y,camera.Right)*init;    
}
void setLightPos(float x,float y){
    
    lightPos+=glm::length(lightPos-camera.Position)*(camera.Up*y+camera.Right*x)*2.0f/(float)SCR_WIDTH;
}
extern "C"{
typedef struct {
    int program[2];
    int vertexBuffer;

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
    ExampleApp *app = (ExampleApp*) calloc(1, sizeof(ExampleApp));

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
    // glfmSetMultitouchEnabled(display,true);
    glfmSetTouchFunc(display, onTouch);
    glfmSetKeyFunc(display, onKey);
}
// bool fistMouse=true;

static bool onTouch(GLFMDisplay *display, int touch, GLFMTouchPhase phase, double x, double y) {
    if (phase == GLFMTouchPhaseHover) {
        return false;
    }
    static const float leeway=SCR_WIDTH/50.0f;
    ExampleApp *app = (ExampleApp*) glfmGetUserData(display);
    static float centerx,centery,lastTouch;
    if (phase != GLFMTouchPhaseBegan) {
        int width, height;
        glfmGetDisplaySize(display, &width, &height);
        app->offsetX = 2 * (x - app->lastTouchX) / width;
        app->offsetY = 2 * (y - app->lastTouchY) / height;
    }
    else{
        app->lastTouchX = x;
        app->lastTouchY = y;
        if(lastFrame-lastTouch>0.02&&lastFrame-lastTouch<0.15&&fabs(centerx-x)<leeway&&fabs(centery-y)<leeway){
            // double click
            LOG("double click: edge=%d\n",edge);
            edge=!edge;
            lastTouch=0.0f;
        }
        else lastTouch=lastFrame;
        centerx=x;
        centery=y;
        return true;
    }
    float xoffset = x - app->lastTouchX;
    float yoffset = app->lastTouchY - y; // reversed since y-coordinates go from bottom to top
    static const float RADIUS=0.05f*SCR_WIDTH;
    if(x>SCR_WIDTH/2.0F){
        if(!edge)camera.ProcessMouseMovement(xoffset, yoffset);
        else setInitModelMatrix(xoffset*0.01f,yoffset*0.01f);
        // else setInitModelMatrix(app->offsetX,app->offsetY);
    }
    else if(!edge)camera.ProcessKeyboard(deltaTime,(x-centerx)/RADIUS,-(y-centery)/RADIUS);
    else setLightPos(xoffset,yoffset);
    app->lastTouchX = x;
    app->lastTouchY = y;
    return true;
}

static bool onKey(GLFMDisplay *display, GLFMKey keyCode, GLFMKeyAction action, int modifiers) {
    bool handled = false;
    if (action == GLFMKeyActionPressed) {
        ExampleApp *app = (ExampleApp*) glfmGetUserData(display);
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
    ExampleApp *app = (ExampleApp*) glfmGetUserData(display);
    app->program[0] = 0;
    app->vertexBuffer = 0;
}

static GLuint compileShader(GLenum type, const char *shaderName) {
    char fullPath[PATH_MAX];
    fc_resdir(fullPath, sizeof(fullPath));
    strncat(fullPath, shaderName, sizeof(fullPath) - strlen(fullPath) - 1);
    LOG("shader: %s",fullPath);
    // Get shader string
    char *shaderString = NULL;
    FILE *shaderFile = fopen(fullPath, "rb");
    if (shaderFile) {
        fseek(shaderFile, 0, SEEK_END);
        long length = ftell(shaderFile);
        fseek(shaderFile, 0, SEEK_SET);

        shaderString = (char*)
        malloc(length + 1);
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
            GLchar *log = (GLchar*)malloc(logLength);
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
}
#define setVec3v(id,name,vec) glUniform3fv(glGetUniformLocation(id,name), 1, &vec[0]); 
#define setVec3(id,name,x,y,z) glUniform3f(glGetUniformLocation(id, name), x, y, z); 
#define setMat4(id,name,mat) glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &(mat[0][0]));
#define setInt(ID,name,value) glUniform1i(glGetUniformLocation(ID, name), value); 
#define setFloat(ID,name,value) glUniform1f(glGetUniformLocation(ID, name), value); 

int createShader(const char* vert_path,const char* frag_path){
    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vert_path);
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, frag_path);
    if (vertShader == 0 || fragShader == 0) {
        return 0;
    }
    int ID= glCreateProgram();
    glAttachShader(ID, vertShader);
    glAttachShader(ID, fragShader);
    glLinkProgram(ID);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    return ID;
}
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
    static Model temple("nanosuit/nanosuit.obj");
    app = (ExampleApp*) glfmGetUserData(display);
    // static Shader lightingShader("1.color.vs", "1.color.fs");
    deltaTime = frameTime/3.0f - lastFrame;
    lastFrame = frameTime/3.0f;
    static int simpleshader;
    if (app->program[0] == 0) {
        // app->program=ID;
        // GLuint vertShader = compileShader(GL_VERTEX_SHADER, "simple.vert");
        // GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, "simple.frag");
        app->program[0]=createShader("1.color.vs","1.color.fs");
        app->program[1]=createShader("1.light_cube.vs","1.light_cube.fs");
        simpleshader=createShader("1.color.vs","simple.fs");
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        diffuseMap = loadTexture("container2.png");
        specularMap = loadTexture("container2_specular.png");
        glUseProgram(app->program[0]);
        setInt(app->program[0],"material.diffuse", 0);
        setInt(app->program[0],"material.specular", 1);
        setFloat(app->program[0],"material.shininess",64);
    }

    // Draw background
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
        glStencilFunc(GL_ALWAYS,1,0XFF);
        glStencilMask(0XFF);
        glClearColor(0.05f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        glUseProgram(app->program[0]);

        setVec3(app->program[0],"objectColor", 1.0f, 0.5f, 0.31f);
        setVec3(app->program[0],"lightColor",  1.0f, 1.0f, 1.0f);
        setVec3v(app->program[0],"lightPos",lightPos);
        setVec3v(app->program[0],"viewPos",camera.Position);
        static glm::mat4 projection,view,model;
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        // model = glm::mat4(1.0f);
        model=init;
        setMat4(app->program[0],"projection", projection);
        setMat4(app->program[0],"view", view);
        setMat4(app->program[0],"model", model);



        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        static const float scale=1.02f;
        static glm::mat4 tmpmodel;
        tmpmodel=glm::scale(model,glm::vec3(scale,scale,scale));
        if(edge){
            glStencilFunc(GL_NOTEQUAL,1,0XFF);
            // glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            glUseProgram(simpleshader);
            setMat4(simpleshader,"projection",projection);
            setMat4(simpleshader,"view",view);
            setMat4(simpleshader,"model",tmpmodel);
            glDrawArrays(GL_TRIANGLES,0,36);
            glStencilMask(0xFF);
            glEnable(GL_DEPTH_TEST);  
            glStencilFunc(GL_ALWAYS,1,0XFF);
        }
        model=glm::mat4(1.0f);
        glUseProgram(app->program[0]);
        glm::vec3 box2Pos(0.3,0.0,1.2);
        // lightingShader.use();
        model = glm::translate(model,box2Pos);
        setMat4(app->program[0],"model",model);
        glDrawArrays(GL_TRIANGLES,0,36);
        model = glm::translate(model,box2Pos);
        model = glm::translate(model,glm::vec3(0.0f,-0.5f,0.0f));
        model = glm::scale(model,glm::vec3(0.1f));
        setMat4(app->program[0],"model",model);
        temple.Draw(app->program[0]);
        // also draw the lamp object
        glUseProgram(app->program[1]);
        setMat4(app->program[1],"projection", projection);
        setMat4(app->program[1],"view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        setMat4(app->program[1],"model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // temple.show();
}
