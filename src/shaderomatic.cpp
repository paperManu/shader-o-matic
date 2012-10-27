#include "shaderomatic.h"

#include <iostream>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "boost/filesystem.hpp"
#include "boost/lexical_cast.hpp"

using namespace boost::chrono;

char gDefaultVertShader[] =
    "#version 150 core\n"
    "\n"
    "in vec4 vVertex;\n"
    "in vec2 vTexCoord;\n"
    "\n"
    "uniform mat4 vMVP;\n"
    "uniform vec2 vMouse;\n"
    "\n"
    "smooth out vec2 finalTexCoord;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position.xyz = (vMVP*vVertex).xyz;\n"
    "\n"
    "    finalTexCoord = vTexCoord;\n"
    "}\n";

char gDefaultFragShader[] =
    "#version 150 core\n"
    "\n"
    "uniform sampler2D vTexMap;\n"
    "uniform sampler2D vHUDMap;\n"
    "uniform sampler2D vFBOMap;\n"
    "\n"
    "uniform vec2 vMouse;\n"
    "uniform float vTimer;\n"
    "uniform vec2 vResolution;\n"
    "uniform int vPass;\n"
    "\n"
    "in vec2 finalTexCoord;\n"
    "\n"
    "out vec4 fragColor;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    float lHUDScale = vResolution.y/32.f;\n"
    "    fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "    fragColor += texture(vHUDMap, vec2(finalTexCoord.s, finalTexCoord.t*lHUDScale));\n"
    "}\n";

/********************************/
shaderomatic::shaderomatic()
    :mIsRunning(false),
      mShaderValid(false),
      mTimePerFrame(0.f),
      mSwapInterval(1)
{
    // Nom de fichiers par défaut
    mImageFile = "texture.png";
    mVertexFile = "shader.vert";
    mGeometryFile = "shader.geom";
    mFragmentFile = "shader.frag";

    // On initialise la texture pour le HUD
    mHUD = cv::Mat::zeros(32, 640, CV_8UC3);
}

/********************************/
void shaderomatic::init()
{
    // On prépare OpenGL
    if(!glfwInit())
    {
        cerr << "Failed to create gl context." << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    settings();
    if(!glfwOpenWindow(640, 480, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
    {
        cerr << "Failed to create gl window." << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowTitle("shader-O-matic");

    glfwSwapInterval(mSwapInterval);

    glClearColor(0.f, 0.f, 0.f, 1.f);

    // On récupère les heures de modifs des shaders
    shaderChanged();

    // Préparation de la géométrie (un plan ...) et de la texture
    prepareGeometry();
    prepareTexture();

    // et quelques infos
    glfwGetWindowSize(&mWindowWidth, &mWindowHeight);
    glViewport(0, 0, mWindowWidth, mWindowHeight);

    // Préparation du FBO, pour faire le rendu en 2 passes
    // A faire après le chargement de la texture, pour des histoires de résolution
    prepareFBO();

    // Compilation du shader
    mShaderValid = compileShader();

    // On récupère le temps courant
    mClockStart = steady_clock::now();

    // Initialisation du timer pour connaître le temps par frame
    steady_clock::time_point lTimerFPS;

    // Boucle principale
    mIsRunning = true;
    while(mIsRunning)
    {
        // Début du rendu de la frame
        lTimerFPS = steady_clock::now();

        // On vérifie que la taille de la fenêtre n'a pas changé
        int lWidth, lHeight;
        glfwGetWindowSize(&lWidth, &lHeight);
        if(lWidth != mWindowWidth || lHeight != mWindowHeight)
        {
           mWindowWidth = lWidth;
           mWindowHeight = lHeight;

           // On adapte la taille du viewport opengl
           glViewport(0, 0, lWidth, lHeight);

           // On change la taille du hud
           mHUD = cv::Mat::zeros(32, lWidth, CV_8UC3);
           prepareHUDTexture();

           // On change la taille du FBO
           glBindTexture(GL_TEXTURE_2D, mFBOTexture);
           glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, lWidth, lHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        }

        // Rendu
        draw();
        if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
        {
            mIsRunning = false;
        }

        // Calcul du temps nécessaire pour cette frame
        mTimePerFrame = duration<float>((steady_clock::now() - lTimerFPS)).count();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

/********************************/
void shaderomatic::settings()
{
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

/********************************/
void shaderomatic::setImageFile(char* pFile)
{
    mImageFile = pFile;
}

/********************************/
void shaderomatic::setShaderFile(const char* pFileBasename)
{
    mVertexFile = pFileBasename;
    mVertexFile += ".vert";

    mGeometryFile = pFileBasename;
    mGeometryFile += ".geom";

    mFragmentFile = pFileBasename;
    mFragmentFile += ".frag";
}

/********************************/
void shaderomatic::setSwapInterval(int pSwap)
{
    mSwapInterval = std::max(0, pSwap);
}

/********************************/
void shaderomatic::prepareGeometry()
{
    // Géométrie de l'arrière-plan
    GLfloat lPoints[] = {-1.f, -1.f, 0.f, 1.f,
                         -1.f, 1.f, 0.f, 1.f,
                         1.f, 1.f, 0.f, 1.f,
                         1.f, 1.f, 0.f, 1.f,
                         1.f, -1.f, 0.f, 1.f,
                         -1.f, -1.f, 0.f, 1.f};

    GLfloat lTex[] = {0.f, 0.f,
                      0.f, 1.f,
                      1.f, 1.f,
                      1.f, 1.f,
                      1.f, 0.f,
                      0.f, 0.f};

    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    // Coordonnées des vertices
    glGenBuffers(2, mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, 6*4*sizeof(float), lPoints, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Coordonnées de texture
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer[1]);
    glBufferData(GL_ARRAY_BUFFER, 6*2*sizeof(float), lTex, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

/********************************/
void shaderomatic::prepareTexture()
{
    // Préparation des textures du fond et du HUD
    glGenTextures(2, mTexture);
    if(!loadTexture(mImageFile.c_str(), mTexture[0]))
        exit(EXIT_FAILURE);

    prepareHUDTexture();
}

/********************************/
void shaderomatic::prepareFBO()
{
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    // Texture as a color buffer
    glGenTextures(1, &mFBOTexture);
    glBindTexture(GL_TEXTURE_2D, mFBOTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWindowWidth, mWindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFBOTexture, 0);

    GLenum lFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(lFBOStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        cerr << "Error while preparing the FBO." << endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/********************************/
bool shaderomatic::compileShader()
{
    GLchar* lSrc;
    bool lResult;

    // Vertex shader
    mVertexShader = glCreateShader(GL_VERTEX_SHADER);
    lSrc = readFile(mVertexFile.c_str());
    bool isPresent = true;
    if(lSrc == NULL)
    {
        lSrc = gDefaultVertShader;
        isPresent = false;
    }

    glShaderSource(mVertexShader, 1, (const GLchar**)&lSrc, 0);
    glCompileShader(mVertexShader);
    lResult = verifyShader(mVertexShader);
    if(!lResult)
    {
        return false;
    }

    if(isPresent)
        free(lSrc);

    // Geometry shader
    mGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    lSrc = readFile(mGeometryFile.c_str());
    isPresent = true;
    if(lSrc == NULL)
        isPresent = false;
    else
    {
        glShaderSource(mGeometryShader, 1, (const GLchar**)&lSrc, 0);
        glCompileShader(mGeometryShader);
        lResult = verifyShader(mGeometryShader);
        if(lResult)
            return false;
    }

    // Fragment shader
    mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    lSrc = readFile(mFragmentFile.c_str());
    isPresent = true;
    if(lSrc == NULL)
    {
        lSrc = gDefaultFragShader;
        isPresent = false;
    }

    glShaderSource(mFragmentShader, 1, (const GLchar**)&lSrc, 0);
    glCompileShader(mFragmentShader);
    lResult = verifyShader(mFragmentShader);
    if(!lResult)
    {
        return false;
    }

    if(isPresent)
        free(lSrc);

    // Création du programme
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, mVertexShader);
    glAttachShader(mShaderProgram, mFragmentShader);
    glBindAttribLocation(mShaderProgram, 0, "vVertex");
    glBindAttribLocation(mShaderProgram, 1, "vTexCoord");
    glLinkProgram(mShaderProgram);
    lResult = verifyProgram(mShaderProgram);
    if(!lResult)
    {
        return false;
    }

    glUseProgram(mShaderProgram);

    // Préparation de la texture de fond et du HUD
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture[0]);
    GLint lTextureUniform = glGetUniformLocation(mShaderProgram, "vTexMap");
    glUniform1i(lTextureUniform, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTexture[1]);
    mHUDLocation = glGetUniformLocation(mShaderProgram, "vHUDMap");
    glUniform1i(mHUDLocation, 1);

    // Ainsi que de la texture liée au FBO
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mFBOTexture);
    lTextureUniform = glGetUniformLocation(mShaderProgram, "vFBOMap");
    glUniform1i(lTextureUniform, 2);

    // Position de la souris, matrice de transformation, timer, résolution, et passe ...
    mMouseLocation = glGetUniformLocation(mShaderProgram, "vMouse");
    mMVPMatLocation = glGetUniformLocation(mShaderProgram, "vMVP");
    mTimerLocation = glGetUniformLocation(mShaderProgram, "vTimer");
    mResolutionLocation = glGetUniformLocation(mShaderProgram, "vResolution");
    mTextureResLocation = glGetUniformLocation(mShaderProgram, "vTexResolution");
    mPassLocation = glGetUniformLocation(mShaderProgram, "vPass");

    return true;
}

/********************************/
void shaderomatic::draw()
{
    // On vérifie que les shaders n'ont pas changé
    if(shaderChanged())
        mShaderValid = compileShader();

    if(textureChanged())
        updateTexture(mImageFile.c_str(), mTexture[0]);

    // Rendu de la fenêtre
    if(mShaderValid)
    {
        // Rendu du HUD
        string lHUDText = string("Fps: ") + boost::lexical_cast<string>((int)(1.f/mTimePerFrame));
        lHUDText += string(" (") + boost::lexical_cast<string>(mTimePerFrame*1000) + string(" msec per frame)");

        mHUD = cv::Mat::zeros(mHUD.size(), mHUD.type());
        cv::putText(mHUD, lHUDText, cv::Point(0,28), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0, 255, 0));
        cv::Mat lMatBuffer;
        cv::flip(mHUD, lMatBuffer,0);

        glGetError();
        glBindTexture(GL_TEXTURE_2D, mTexture[1]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, lMatBuffer.cols, lMatBuffer.rows, GL_BGR, GL_UNSIGNED_BYTE, lMatBuffer.data);
        glBindTexture(GL_TEXTURE_2D, 0);

        GLenum lError = glGetError();
        if(lError)
        {
            cerr << "Error while updating HUD." << endl;
        }

        // Mouse position
        int lMouseX, lMouseY;
        glfwGetMousePos(&lMouseX, &lMouseY);
        float lMouse[2];
        lMouse[0] = max(0.f, min((float)mWindowWidth-1.f, (float)lMouseX));
        lMouse[1] = (float)mWindowHeight-1.f - max(0.f, min((float)mWindowHeight-1.f, (float)lMouseY));
        glUniform2fv(mMouseLocation, 1, (GLfloat*)lMouse);

        // Timer
        duration<float> lTimer = steady_clock::now() - mClockStart;
        glUniform1f(mTimerLocation, (float)lTimer.count());

        // Resolution
        float lRes[2];
        lRes[0] = (float)mWindowWidth;
        lRes[1] = (float)mWindowHeight;
        glUniform2fv(mResolutionLocation, 1, (GLfloat*)lRes);

        lRes[0] = (float)mTextureWidth;
        lRes[1] = (float)mTextureHeight;
        glUniform2fv(mTextureResLocation, 1, (GLfloat*)lRes);

        // Rendering
        glm::mat4 lProjMatrix = glm::ortho(-1.f, 1.f, -1.f, 1.f);
        glUniformMatrix4fv(mMVPMatLocation, 1, GL_FALSE, glm::value_ptr(lProjMatrix));

        // Première passe : rendu dans le FBO
        glUniform1i(mPassLocation, (GLint)0);
        GLenum lFBOBuf[] = {GL_COLOR_ATTACHMENT0};
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glDrawBuffers(1, lFBOBuf);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, mFBOTexture);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Seconde passe : rendu dans le back buffer
        glUniform1i(mPassLocation, (GLint)1);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GLenum lBackbuffer[] = {GL_BACK};

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mFBOTexture);

        glDrawBuffers(1, lBackbuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers();
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers();
    }
}

/********************************/
bool shaderomatic::loadTexture(const char *pFilename, GLuint pTexture)
{
    cout << "Loading texture " << pFilename << endl;

    cv::Mat lMatTexture, lBufferTexture;
    lMatTexture = cv::imread(pFilename);

    if(lMatTexture.rows == 0 || lMatTexture.cols == 0)
    {
        cerr << "Failed to load texture." << endl;
        cerr << "Using a black texture instead." << endl;
        lMatTexture = cv::Mat::zeros(512, 512, CV_8UC3);
    }
    else
    {
        mImageChange = boost::filesystem3::last_write_time(mImageFile.c_str());
    }

    cv::flip(lMatTexture, lBufferTexture, 0);

    glGetError();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, lBufferTexture.cols, lBufferTexture.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, lBufferTexture.data);

    glGenerateMipmap(GL_TEXTURE_2D);

    GLenum lError = glGetError();
    if(lError)
    {
        cerr << "Error while loading texture." << endl;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    glfwSetWindowSize(lMatTexture.cols, lMatTexture.rows);
    mTextureWidth = lMatTexture.cols;
    mTextureHeight = lMatTexture.rows;

    mWindowWidth = lMatTexture.cols;
    mWindowHeight = lMatTexture.rows;

    return true;
}

/***************************/
bool shaderomatic::updateTexture(const char* pFilename, GLuint pTexture)
{
    cout << "Reloading texture " << pFilename << endl;

    cv::Mat lMatTexture, lBufferTexture;
    lMatTexture = cv::imread(pFilename);

    if(lMatTexture.rows == 0 || lMatTexture.cols == 0)
    {
        cerr << "Failed to load texture." << endl;
        cerr << "Using a black texture instead." << endl;
        lMatTexture = cv::Mat::zeros(512, 512, CV_8UC3);
    }

    cv::flip(lMatTexture, lBufferTexture, 0);

    glGetError();
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, pTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, lBufferTexture.cols, lBufferTexture.rows, GL_BGR, GL_UNSIGNED_BYTE, lBufferTexture.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);


    GLenum lError = glGetError();
    if(lError)
    {
        cerr << "Error while updating texture" << endl;
    }

    return true;
}

/***************************/
void shaderomatic::prepareHUDTexture()
{
    mHUD = cv::Mat::zeros(32, mWindowWidth, CV_8UC3);

    glGetError();
    glBindTexture(GL_TEXTURE_2D, mTexture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mHUD.cols, mHUD.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);

    GLenum lError = glGetError();
    if(lError)
    {
        cerr << "Error while updating HUD texture." << endl;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

/***************************/
char* shaderomatic::readFile(const char* pFile)
{
    long lLength;
    char* lBuffer;

    cout << pFile << endl;

    FILE *lFile = fopen(pFile, "rb");
    if(!lFile)
    {
        cout << "Unable to find specified file: " << pFile << endl;
        return NULL;
    }
    fseek(lFile, 0, SEEK_END);
    lLength = ftell(lFile);
    lBuffer = (char*)malloc(lLength+1);
    fseek(lFile, 0, SEEK_SET);
    long lSize = fread(lBuffer, lLength, 1, lFile);
    fclose(lFile);

    lBuffer[lLength] = 0;

    //cout << lBuffer << endl;

    return lBuffer;
}

/***************************/
bool shaderomatic::verifyShader(GLuint pShader)
{
    GLint lIsCompiled;
    glGetShaderiv(pShader, GL_COMPILE_STATUS, &lIsCompiled);

    if(lIsCompiled == true)
    {
        cout << "Shader successfully compiled." << endl;
    }
    else
    {
        cout << "Failed to compile shader." << endl;
    }

    GLint lLength;
    char* lLogInfo;
    glGetShaderiv(pShader, GL_INFO_LOG_LENGTH, &lLength);
    lLogInfo = (char*)malloc(lLength);
    glGetShaderInfoLog(pShader, lLength, &lLength, lLogInfo);
    string lLogInfoStr = string(lLogInfo);
    free(lLogInfo);

    cout << lLogInfoStr << endl;
    cout << "-------" << endl;

    if(lIsCompiled == false)
    {
        return false;
    }
    return true;
}

/***************************/
bool shaderomatic::verifyProgram(GLuint pProgram)
{
    GLint lIsLinked;
    glGetProgramiv(pProgram, GL_LINK_STATUS, &lIsLinked);

    if(lIsLinked == true)
    {
        cout << "Program successfully linked." << endl;
    }
    else
    {
        cout << "Failed to link program." << endl;
    }

    GLint lLength;
    char* lLogInfo;
    glGetProgramiv(pProgram, GL_INFO_LOG_LENGTH, &lLength);
    lLogInfo = (char*)malloc(lLength);
    glGetProgramInfoLog(pProgram, lLength, &lLength, lLogInfo);
    string lLogInfoStr = string(lLogInfo);
    free(lLogInfo);

    cout << lLogInfoStr << endl;
    cout << "-------" << endl;

    if(lIsLinked == false)
    {
        glfwTerminate();
        return false;
    }
    return true;
}

/***************************/
bool shaderomatic::shaderChanged()
{
    bool lResult = false;
    std::time_t lTime;

    if(boost::filesystem3::exists(mVertexFile.c_str()))
    {
        lTime = boost::filesystem3::last_write_time(mVertexFile.c_str());
        if(lTime != mVertexChange)
        {
            mVertexChange = lTime;
            lResult |= true;
        }
    }
    else
    {
        mVertexChange = 0;
    }

     if(boost::filesystem3::exists(mGeometryFile.c_str()))
    {
        lTime = boost::filesystem3::last_write_time(mGeometryFile.c_str());
        if(lTime != mVertexChange)
        {
            mGeometryChange = lTime;
            lResult |= true;
        }
    }
    else
    {
        mGeometryChange = 0;
    }
    
    if(boost::filesystem3::exists(mFragmentFile.c_str()))
    {
        lTime = boost::filesystem3::last_write_time(mFragmentFile.c_str());
        if(lTime != mFragmentChange)
        {
            mFragmentChange = lTime;
            lResult |= true;
        }
    }
    else
    {
        mFragmentChange = 0;
    }

    return lResult;
}


/***************************/
bool shaderomatic::textureChanged()
{
    bool lResult = false;
    std::time_t lTime;

    if(boost::filesystem3::exists(mImageFile.c_str()))
    {
        lTime = boost::filesystem3::last_write_time(mImageFile.c_str());
        if(lTime != mImageChange)
        {
            sleep(1); // Pour être certain que le fichier est écrit en entier
            mImageChange = lTime;
            lResult = true;
        }
    }
    
    return lResult;
}
