#ifndef OPENGL_HPP_INCLUDED
#define OPENGL_HPP_INCLUDED

#include <glad/glad.h>

#define USE_DEBUG_CONTEXT 1
#define USE_CHECK_ERROR 1

#if !_DEBUG || !USE_CHECK_ERROR
	#define checkGlErrors()
#else
	void _internal_checkGlErrors(const char* file, int line); //defined in main.cpp
	#define checkGlErrors() _internal_checkGlErrors(__FILE__, __LINE__)
#endif

#endif//OPENGL_HPP_INCLUDED