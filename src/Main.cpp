/***************************************************************
 *							Main.cpp
 *
 * Program startup, initialization, application management, and
 * main loop
 **************************************************************/

#include <iostream>

#include "OpenGL.hpp"
#include <GLFW/glfw3.h>

using std::cout;
using std::cerr;
using std::endl;

//Context Creation Flags:
const int OPENGL_VERSION[2] = { 4, 3 }; // Major, Minor
const bool OPENGL_COMPATABILITY = false;
const bool DOUBLE_BUFFER = true;
const bool SRGB_COLOR_BUFFER = false;

//Default Window Settings:
const int DEF_RESOLUTION[2] = { 720, 480 };
const char* DEF_TITLE = "Graphics Prototyper";

static GLFWwindow* window = nullptr;

//Foward declarations for application code
bool initialize(int width, int height);
bool update(float deltaTime);
void render();
void onResize(int width, int height);

//==============================================================
//						GL Error Handling
//==============================================================

#if _DEBUG

void APIENTRY putDebugContextMessage(
	GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	const char* colorStr = "";
	const char* typeStr = "";
	const char* sourceStr = "";
	const char* severityStr = "";

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:				colorStr = "\033[91m", typeStr = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	colorStr = "", typeStr = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	colorStr = "", typeStr = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:			colorStr = "", typeStr = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:			colorStr = "", typeStr = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER:				colorStr = "", typeStr = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:			colorStr = "", typeStr = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:			colorStr = "", typeStr = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:				colorStr = "", typeStr = "Other"; break;
	}

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             sourceStr = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sourceStr = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     sourceStr = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     sourceStr = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           sourceStr = "Other"; break;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         severityStr = "high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       severityStr = "medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          severityStr = "low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "notification"; break;
	}

	cerr << "Debug Message(" << id << "): " << colorStr << message <<
		"\033[0m\n  Type:" << typeStr <<
		"\n  Source: " << sourceStr <<
		"\n  Severity: " << severityStr << endl;
}

void _internal_checkGlErrors(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		const char* error = "";
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}

		cerr << file << '(' << line << "): " << error << '\n' << endl;
	}
}

#endif//#if DEBUG

//==============================================================
//						GLFW Callbaacks
//==============================================================

void printGlfwError(int code, const char* string)
{
	cerr << "GLFW Error " << code << ": " << (const char*)(string ? string : "") << endl;
}

//==============================================================
//						Main Function/Loop
//==============================================================

int main(int argc, char** argv)
{
	glfwSetErrorCallback(printGlfwError);

	if (!glfwInit()) {
		cerr << "Could not initialize GLFW. Cannot continue." << endl;
		return -1;
	}

	//Set all the window hints for the OpenGL window we want
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION[0]);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION[1]);
	glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_COMPATABILITY ? GLFW_OPENGL_COMPAT_PROFILE : GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, DOUBLE_BUFFER);
	glfwWindowHint(GLFW_SRGB_CAPABLE, SRGB_COLOR_BUFFER);

#if _DEBUG && USE_DEBUG_CONTEXT
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif //#if DEBUG

	window = glfwCreateWindow(DEF_RESOLUTION[0], DEF_RESOLUTION[1], DEF_TITLE, nullptr, nullptr);
	if (window == nullptr) {
		cerr << "Could not create a window. Cannot continue." << endl;
		glfwTerminate();
		return -1;
	}

	//It is necessary to make the context current because a valid OpenGl context must be bound
	//before OpenGL functions can be loaded.
	glfwMakeContextCurrent(window);

	//Load OpenGL. OpenGL 4.3 is required for debug contexts without needing extensions. This shouldn't be an issue
	//as 4.3 is nearly 10 years old, but if it turns out to be a switch to GLEW would be preferential because it
	//would load the extensions into the normal name automatically. GLAD doesn't.
	if (!gladLoadGL()) {
		cerr << "Could not load OpenGL 4.3. Cannot continue." << endl;
		glfwTerminate();
		return -1;
	}

	//Initialization for debug contexts must be done after GLEW loads all of the
	//OpenGL functionality
#if _DEBUG && USE_DEBUG_CONTEXT
	int contextFlags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);

	//Use the debug context if it was properly created, otherwise use normal OpenGL error checking
	if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		cout << "Using debug context for error logging" << endl;
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(putDebugContextMessage, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	else {
		cout << "Debug Contexts Not Available" << endl;
	}

#elif _DEBUG
	cout << "Debug Contexts Disabled" << endl;
#endif //#if DEBUG

	if (!initialize(DEF_RESOLUTION[0], DEF_RESOLUTION[1])) {
		cerr << "Application failed to initialize. Cannot continue." << endl;
		return -1;
	}

	bool running = true;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (!update(0.0f))
			glfwSetWindowShouldClose(window, true);

		render();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}