#ifndef _VOID_STROKE_SHADER_PROGRAM_H
#define _VOID_STROKE_SHADER_PROGRAM_H

/* GLEW */
#include <GL/glew.h>

/* Qt */
#include <QOpenGLShaderProgram>

/* Internal */
#include "Definition.h"
#include "ShaderProgram.h"

VOID_NAMESPACE_OPEN

class StrokeShaderProgram : public ShaderProgram
{
public:
    StrokeShaderProgram() = default;
    ~StrokeShaderProgram();

    /**
     * Initializes the shaders and the internals
     * This method gets called from the renderer after the GL context has been initialized
     */
    virtual void Initialize() override;

    /**
     * Returns the Shader Program's id
     */
    virtual inline unsigned int ProgramId() const override { return m_Program->programId(); }

    /**
     * Bind the Shader to be used glUseProgram(programId)
     */
    virtual inline bool Bind() override { return m_Program->bind(); }
    /**
     * Release the Shader glUseProgram(0)
     */
    virtual inline void Release() override { m_Program->release(); }

protected:
    /**
     * Setup Shaders
     * Compilation and Linking of shaders Happens here
     */
    virtual bool SetupShaders() override;

    // /**
    //  * Setup Array Buffers
    //  * Initialize the Array Buffers to be used in the program
    //  */
    // virtual void SetupBuffers() override;

private: /* Members */
    QOpenGLShaderProgram* m_Program;

    // /* Array Buffers */
    // unsigned int m_VAO;
    // unsigned int m_VBO;

    // /* Uniforms */
    // int m_UProjection;
    // int m_UColor;
    // int m_USize;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_STROKE_SHADER_PROGRAM_H
