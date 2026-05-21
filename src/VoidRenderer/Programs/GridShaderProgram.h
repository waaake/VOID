// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _GRID_SHADER_PROGRAM_H
#define _GRID_SHADER_PROGRAM_H

/* Qt */
#include <QOpenGLShaderProgram>

/* Internal */
#include "Definition.h"
#include "ShaderProgram.h"

VOID_NAMESPACE_OPEN

class GridShaderProgram : public ShaderProgram
{
public:
    GridShaderProgram() = default;
    ~GridShaderProgram();

    void Initialize() override;
    void Reinitialize() override;

    unsigned int ProgramId() const override { return m_Program->programId(); }

    bool Bind() override { return m_Program->bind(); }
    void Release() override { m_Program->release(); }

protected:
    bool SetupShaders() override;

private:
    QOpenGLShaderProgram* m_Program;
};

VOID_NAMESPACE_CLOSE

#endif // _GRID_SHADER_PROGRAM_H
