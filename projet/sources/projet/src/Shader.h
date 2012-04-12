
#ifndef _Shader_h_
#define _Shader_h_

#include "OpenGL.h"
#include <iostream>



#include <QString>

/** Permet de manipuler des shaders en GLSL (OpenGL2.0)
    Exemple d'utilisation:
    \code
    // shader creation:
    Shader* myShader = new Shader();
    // loading from files (compilation + linking):
    myShader->loadFromFiles("myShaderFile.vtx", "myShaderFile.frg");

    // ...

    // at rending time:
    myShader->activate();
    // draw objects
    \endcode
*/

class Shader
{
public:
    Shader()
      : mIsValid(false)
    {}

    /** Compiles and links the shader from 2 source files
        \param fileV vertex shader ("" if no vertex shader)
        \param fileF fragment shader ("" if no fragment shader)
        \return true if no error occurs
    */
    bool loadFromFiles(const std::string& fileV, const std::string& fileF);

    bool loadSources(const std::string& vsrc, const std::string& fsrc);

    /** Enable the shader
    */
    void activate() const;

    /** \return the index of the uniform variable \a name
    */
    int getUniformLocation(const char* name) const;

    /** Forces a sampler to a given unit
        Example:
        \code
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE2D, myTextureID);
    myShader->setSamplerUnit("mySampler", 2);
        \endcode
    */
    void setSamplerUnit(const char* samplerName, int textureUnit) const;

    /** \returns the index of the generic attribute \a name
        To be used with glVertexAttribPointer(...)
        Example:
        \code
    int tangentAttribID = myShader->getAttribLocation("tangent");
    Vector3f* tangents = new Vector3f[...];
    glVertexAttribPointer(tangentAttribID, 3, GL_FLOAT, GL_FALSE, 0, tangents);
    glEnableVertexAttribArray(tangentAttribID);
        \endcode
    */
    int getAttribLocation(const char* name) const;

    /** \returns the OpenGL object id of the GLSL program */
    int id() const { return mProgramID; }
    
    bool valid() const { return mIsValid; }

protected:

    bool mIsValid;
    static void printProgramInfoLog(GLuint objectID);
    static void printShaderInfoLog(GLuint objectID);
    GLuint mProgramID;
};

#endif
