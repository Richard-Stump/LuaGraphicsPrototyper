/***************************************************************
 *						Application.cpp
 * 
 * Main Application Logic
 **************************************************************/

#include <iostream>

#include "OpenGL.hpp"
 
//Foward declarations for application code
bool initialize(int width, int height);
bool update(float deltaTime);
void render();
void onResize(int width, int height);

bool initialize(int width, int height)
{
	onResize(width, height);

	return true;
}

bool update(float deltaTime)
{
	return true;
}

void render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void onResize(int width, int height)
{
	glViewport(0, 0, width, height);
}