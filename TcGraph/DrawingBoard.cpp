#include "DrawingBoard.h"

DrawingBoard::DrawingBoard(GLFWwindow* window)
{
	if (!window) throw 0;
	this->window = window;

	defaultClearColor.SetColor(L"white");
}

DrawingBoard::~DrawingBoard()
{
	glfwDestroyWindow(window);
	window = nullptr;
}

void DrawingBoard::BeginDraw() const
{
	glClearColor(
		defaultClearColor.GetRed(),
		defaultClearColor.GetGreen(), 
		defaultClearColor.GetRed(),
		defaultClearColor.GetOpacity());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

	

	glBegin(GL_LINES);
}

void DrawingBoard::ConfirmDraw()
{
	glEnd();
	glfwSwapBuffers(window);
}
