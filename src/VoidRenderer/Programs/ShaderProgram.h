// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_BASIC_SHADER_PROGRAM_H
#define _VOID_BASIC_SHADER_PROGRAM_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class ShaderProgram
{
public:
    /**
     * Initializes the shaders and the internals
     * This method gets called from the renderer after the GL context has been initialized
     */
    virtual void Initialize() = 0;

    /**
     * Reinitialize the shaders and the internals
     */
    virtual void Reinitialize() = 0;

    /**
     * Returns the Shader Program's id
     */
    virtual unsigned int ProgramId() const = 0;

    /**
     * Bind the Shader to be used glUseProgram(programId)
     */
    virtual bool Bind() = 0;
    /**
     * Release the Shader glUseProgram(0)
     */
    virtual void Release() = 0;

protected:
    /**
     * Setup Shaders
     * Compilation and Linking of shaders Happens here
     */
    virtual bool SetupShaders() = 0;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_BASIC_SHADER_PROGRAM_H
