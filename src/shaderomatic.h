#ifndef SHADEROMATIC_H
#define SHADEROMATIC_H

#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU
#define GL3_PROTOTYPES

#include <ctime>
#include <iostream>
#include "GL/glfw.h"
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
    void init();

private:
    /***********/
    // Attributs
    /***********/
    // Paramètres
    int mSwapInterval;
    std::string mImageFile;
    std::string mVertexFile, mGeometryFile, mFragmentFile;

    bool mIsRunning;

    boost::chrono::steady_clock::time_point mClockStart; // Un chrono, comme son nom l'indique
    float mTimePerFrame; // Temps de rendu de la dernière image
    cv::Mat mHUD; // Image OpenCV pour stocker des infos

    int mTextureWidth, mTextureHeight;
    int mWindowWidth, mWindowHeight;

    // OpengL
    bool mShaderValid;

    GLuint mVertexArray;
    GLuint mVertexBuffer[2];
    GLuint mTexture[2];

    GLuint mVertexShader;
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
    std::time_t mGeometryChange;
    std::time_t mFragmentChange;
    std::time_t mImageChange;

    GLuint mFBO;
    GLuint mFBOTexture;

    /**********/
    // Méthodes
    /**********/
    void settings();

    void prepareFBO();
    void prepareGeometry(); // Soit 2 triangles ...
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
