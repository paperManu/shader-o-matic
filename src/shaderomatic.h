/*
 * Copyright (C) 2013 Emmanuel Durand
 *
 * This file is part of Shader-0-matic.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * blobserver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with blobserver.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SHADEROMATIC_H
#define SHADEROMATIC_H

#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES

#include <atomic>
#include <ctime>
#include <iostream>
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "opencv2/opencv.hpp"
#include "boost/chrono/chrono.hpp"

/*************/
class shaderomatic
{
public:
    shaderomatic();
    void setImageFile(std::string file) {mImageFile = file;}
    void setObjectFile(std::string file) {mObjectFile = file;}
    void setShaderFile(std::string file);
    void setResolution(const int pWidth, const int pHeight);
    void setSwapInterval(int pSwap);
    void setWireframe(bool wire) {mWireframe = wire;}
    void setCull(int value) {mCullFace = value;}
    void init();

private:
    /***********/
    // Attributes
    int mSwapInterval {0};
    int mCullFace {0};
    std::string mImageFile {""};
    std::string mObjectFile {""};
    std::string mVertexFile, mTessControlFile, mTessEvalFile, mGeometryFile, mFragmentFile;

    bool mIsRunning;

    boost::chrono::steady_clock::time_point mClockStart;
    float mTimePerFrame;
    cv::Mat mHUD;

    int mTextureWidth, mTextureHeight;
    int mWindowWidth, mWindowHeight;

    // GLFW
    GLFWwindow* mGlfwWindow {nullptr};
    static std::atomic<double> mScrollValue;

    // OpengL
    bool mShaderValid;
    bool mWireframe {false};
    bool mTessellate {false};

    GLuint mScreenVertexArray;
    GLuint mScreenVertexBuffer[2];
    GLuint mObjectVertexArray;
    GLuint mObjectVertexBuffer[2];
    GLuint mTexture[2];

    int mObjectVertexNumber {6};

    GLuint mVertexShader;
    GLuint mTessellationControlShader;
    GLuint mTessellationEvaluationShader;
    GLuint mGeometryShader;
    GLuint mFragmentShader;
    GLuint mShaderProgram;

    GLint mMVPMatLocation;
    GLint mMouseLocation;
    GLint mMouseScrollLocation;
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
    GLuint mFBODepthTexture;

    // Methods
    void settings();

    static void glMsgCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);
    static void scrollCallback(GLFWwindow*, double, double);

    void prepareFBO();
    bool prepareScreenGeometry();
    bool prepareObjectGeometry();
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
