#ifndef _VOID_GL_H
#define _VOID_GL_H

/* glew */
#include <GL/glew.h>

/* Qt */
#include <QOpenGLShaderProgram>

/* Internal */
#include "Definition.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

class VoidShader
{
public:
    VoidShader();
    ~VoidShader();

    /**
     * Creates OpenGL Context
     * Initializes - Compiles and Links Shaders for use in rendering
     */
    void Initialize();

    /**
     * Binds the shader program to be used
     */
    inline void Bind() { m_Shader->bind(); }
    inline void Release() { m_Shader->release(); }

    /**
     * Sets Uniform value on the shader program
     */
    inline void SetUniform(const std::string& name, float value) { m_Shader->setUniformValue(name.c_str(), value); }
    inline void SetUniform(const std::string& name, const QMatrix4x4& value) { m_Shader->setUniformValue(name.c_str(), value); }

    /**
     * Loads the shaders into the program after compiling them
     * Returns whether the shaders were loaded correctly or not
     */
    bool LoadShaders();

    inline QOpenGLShaderProgram* GetShader() const { return m_Shader; }
    inline unsigned int ProgramId() const { return m_Shader->programId(); }

    /**
     * Sets Core Profile for OpenGL
     * This needs to be invoked before the context is initialized
     * That means it should get invoked before we create the instance of the VoidRenderer.
     */
    VOID_API static void SetProfile();

private:
    QOpenGLShaderProgram* m_Shader;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_GL_H
