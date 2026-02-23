// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_BASIC_DEFINITIONS_H
#define _VOID_BASIC_DEFINITIONS_H

/* Namespaces */
#define VOID_NAMESPACE voidplayer

#define VOID_NAMESPACE_OPEN namespace VOID_NAMESPACE { /* voidplayer namespace */
#define VOID_NAMESPACE_CLOSE } // namespace voidplayer

/* Symbols Visibility */
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(VOID_LIB)
#define VOID_API __declspec(dllexport)
#else
#define VOID_API __declspec(dllimport)
#endif // defined(VOID_LIB)

/**
 * Logging interface is defined in Code and all other libraries
 * just need to import it, hance a separate MACRO for controlling
 * the visibility of the Logger::logger_ symbol 
 */
#if defined(LOGGER_LIB)
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif // defined(LOGGER_LIB)
#else
#define VOID_API __attribute__((visibility("default")))
#define LOGGER_API __attribute__((visibility("default")))
#endif // defined(_WIN32) || defined(__CYGWIN__)

/* type definitions */
typedef signed long v_frame_t; // Represents a frame number or something to hold a frame duration

/* Internal */
#include "Version.h"

#endif // _VOID_BASIC_DEFINITIONS_H
