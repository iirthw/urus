#include "application.h"

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "engine/base/opengl_support.h"
#include "engine/graphics/shader.h"
#include "engine/window/window.h"
#ifndef NDEBUG
#include "engine/window/console.h"
#endif
#include "stb_image/stb_image.h"

using namespace urus;

unsigned int Application::VAO[NB_OBJECTS];
unsigned int Application::VBO[NB_OBJECTS];
std::unique_ptr<ShaderProgram> Application::shaders[NB_OBJECTS];

Application::~Application()
{
	try 
	{
		shutdown();  
	}
	catch(const std::exception& ex) 
	{
		// swallow 
		std::cerr << "shutdown failed " << ex.what() << std::endl;
	}  
}

bool Application::setup(int argc, char* argv[])
{
	if (!mWindow)
	{
		std::cerr << "[error] No window has been set" << std::endl;
		assert(false);
	}
	glutInit(&argc, argv);
#ifdef __APPLE__
    // FIXME: GLUT was deprecated on mac os :(
#elif _WIN64
    glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	int contextFlags = GLUT_FORWARD_COMPATIBLE;
#ifdef NDEBUG
//	contextFlags |= GLUT_DEBUG;
#endif

	glutInitContextFlags(contextFlags);
    
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(mWindow->title().c_str());

	glutIdleFunc(idle);
	glutDisplayFunc(render);
	glutVisibilityFunc(visible);
	/*
	   glutReshapeFunc(reshape);
	   glutKeyboardFunc(keyboardCallback);
	   glutMouseFunc(mouseCallback);
	   glutMouseWheelFunc(mouseWheelCallback);	   
	   glutIdleFunc(animate);
	 */

#ifdef _WIN64
	glewExperimental = true;
	GLint GlewInitResult = glewInit();
	if (GlewInitResult != GLEW_OK) 
	{
		std::cerr << "ERROR: %s\n" << glewGetErrorString(GlewInitResult);
	}
#endif
    
	std::cout << "OpenGL initialized: OpenGL version: " << glGetString(GL_VERSION) 
		<< " GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	int maxNbAttribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxNbAttribs);
	std::cout << "Max number of attribs supported on the current hardware: " << maxNbAttribs << "\n";

	glEnable(GL_DEPTH_TEST); // what the heck?

	// Generate all VBOs and VAOs
	glGenBuffers(NB_OBJECTS, &VBO[0]);
	glGenVertexArrays(NB_OBJECTS, &VAO[0]);

	// FIXME: load shaders and buffer data for each object
	{
		shaders[0] = std::make_unique<ShaderProgram>("shader0.vert", "shader0.frag");
		shaders[0]->useProgram();

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};

		// 1. bind Vertex Array Object
		glBindVertexArray(VAO[0]);
		// 2. copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// 3. then set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);		

		glUseProgram(0);
	}

	{
		shaders[1] = std::make_unique<ShaderProgram>("shader1.vert", "shader1.frag");
		shaders[1]->useProgram();

		float vertices[] = {
			0.0f,  0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			1.0f,  0.5f, 0.0f
		};

		// 1. bind Vertex Array Object
		glBindVertexArray(VAO[1]);
		// 2. copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// 3. then set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glUseProgram(0);
	}

	// setup textures
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// configure texture wrapping/filtering options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// load and generate the texture
		int texWidth = 0, texHeight = 0, texNbChannels = 0;
		const auto filename = "assets/container.jpg";
		unsigned char* texData = stbi_load(filename, &texWidth,
										   &texHeight, &texNbChannels, 0);

		if (texData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight,
				0, GL_RGB, GL_UNSIGNED_BYTE, texData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			std::cout << "Texture loading from " << filename << " failed.\n";
	}

	return true;
}

void Application::render()
{
	// FIXME: remove render count (debug info)
	static unsigned long renderCount = 0;
	std::cout << "Render count: " << renderCount++ << "\n";
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaders[1]->useProgram();
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	shaders[0]->useProgram();
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Note, an implicit glFlush is done by glutSwapBuffers before it returns
	glutSwapBuffers();
}

void Application::shutdown()
{
	// TODO: release all resources
}


void Application::run()
{
	glutMainLoop();
}

void Application::updateWindowPositionAndBounds() const
{
	if (mWindow)
		mWindow->updateWindowPositionAndBounds();

#ifndef NDEBUG
	if (mConsole)
		mConsole->updateWindowPositionAndBounds();
#endif
}

void Application::idle()
{
	// Mark the window for redisplay the next iteration through glutMainLoop 
	glutPostRedisplay();
}

void Application::keyboardCallback(unsigned char key, int x, int y)
{
	// FIXME: implement
}

void Application::mouseCallback(int button, int state, int x, int y)
{
	// FIXME: implement
}

void Application::mouseWheelCallback(int wheel, int direction, int x, int y)
{
	// FIXME: implement
}

void Application::animate()
{
	// FIXME: implement
}

void Application::reshape(int width, int height)
{
	return;
	auto window = sharedInstance().window();
	if (window)
		window->setWindowBounds(width, height);

	// Update transformation from NDC (Normalised Device Coordinates)
	// to WC (Window Coordinates)
	glViewport(0, 0, width, height);
}

void Application::visible(int isVisible)
{
	// Disable the call to idle func, when the window is not visible
	if (isVisible == GLUT_VISIBLE)
		glutIdleFunc(idle);
	else
		glutIdleFunc(0);
}
