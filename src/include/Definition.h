#ifndef _VOID_BASIC_DEFINITIONS_H
#define _VOID_BASIC_DEFINITIONS_H

/* Namespaces */
#define VOID_NAMESPACE voidplayer

#define VOID_NAMESPACE_OPEN namespace VOID_NAMESPACE { /* VOID namespace */
#define VOID_NAMESPACE_CLOSE } // namespace VOID

/* Symbols Visibility */
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(VOID_LIB)
#define VOID_API __declspec(dllexport)
#else
#define VOID_API __declspec(dllimport)
#endif // defined(VOID_LIB)
#else
#define VOID_API __attribute__((visibility("default")))
#endif // defined(_WIN32) || defined(__CYGWIN__)

#endif // _VOID_BASIC_DEFINITIONS_H
