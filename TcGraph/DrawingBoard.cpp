#include "DrawingBoard.h"

DrawingBoard::DrawingBoard(GLFWwindow* window)
{
	if (!window) throw 0;
	this->window = window;
}

DrawingBoard::~DrawingBoard()
{
	glfwDestroyWindow(window);
	window = nullptr;
}

void DrawingBoard::ConfirmDraw()
{
	glfwSwapBuffers(window);
}
