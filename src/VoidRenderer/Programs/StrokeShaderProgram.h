#ifndef _VOID_STROKE_SHADER_PROGRAM_H
#define _VOID_STROKE_SHADER_PROGRAM_H

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
     * Reinitialize the shaders and the internals
     */
    virtual void Reinitialize() override;

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

private: /* Members */
    QOpenGLShaderProgram* m_Program;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_STROKE_SHADER_PROGRAM_H
