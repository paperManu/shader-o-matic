#ifndef SHADEROMATIC_H
#define SHADEROMATIC_H

#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES

#include <ctime>
#include <iostream>
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "opencv2/opencv.hpp"
#include "boost/chrono/chrono.hpp"

using namespace std;

class shaderomatic
{
public:
    shaderomatic();
    void setImageFile(char* pFile);
    void setShaderFile(const char* pFileBasename);
    void setResolution(const int pWidth, const int pHeight);
    void setSwapInterval(int pSwap);
    void setWireframe(bool wire) {mWireframe = wire;}
    void init();

private:
    /***********/
    // Attributes
    int mSwapInterval;
    std::string mImageFile;
    std::string mVertexFile, mTessControlFile, mTessEvalFile, mGeometryFile, mFragmentFile;

    bool mIsRunning;

    boost::chrono::steady_clock::time_point mClockStart;
    float mTimePerFrame;
    cv::Mat mHUD;

    int mTextureWidth, mTextureHeight;
    int mWindowWidth, mWindowHeight;

    // GLFW
    GLFWwindow* mGlfwWindow {nullptr};

    // OpengL
    bool mShaderValid;
    bool mWireframe {false};
    bool mTessellate {false};

    GLuint mVertexArray;
    GLuint mVertexBuffer[2];
    GLuint mTexture[2];

    GLuint mVertexShader;
    GLuint mTessellationControlShader;
    GLuint mTessellationEvaluationShader;
    GLuint mGeometryShader;
    GLuint mFragmentShader;
    GLuint mShaderProgram;

    GLint mMVPMatLocation;
    GLint mMouseLocation;
    GLint mTimerLocation;
    GLint mResolutionLocation;
    GLint mTextureResLocation;
    GLint mHUDLocation;
    GLint mPassLocation;

    std::time_t mVertexChange;
    std::time_t mTessControlChange;
    std::time_t mTessEvalChange;
    std::time_t mGeometryChange;
    std::time_t mFragmentChange;
    std::time_t mImageChange;

    GLuint mFBO;
    GLuint mFBOTexture;

    // Methods
    void settings();

    static void glMsgCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);

    void prepareFBO();
    void prepareGeometry();
    void prepareTexture();
    bool compileShader();
    bool verifyShader(GLuint pShader);
    bool verifyProgram(GLuint pProgram);
    void draw();

    bool loadTexture(const char* pFilename, GLuint pTexture);
    bool updateTexture(const char* pFilename, GLuint pTexture);
    bool textureChanged();
    void prepareHUDTexture();

    char* readFile(const char* pFile);
    bool shaderChanged();
};

#endif // SHADEROMATIC_H
