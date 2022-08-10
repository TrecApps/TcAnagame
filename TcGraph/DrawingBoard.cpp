#include "TColorBrush.h"

DrawingBoard::DrawingBoard(GLFWwindow* window)
{
	if (!window) throw 0;
	this->window = window;

	mode2d = false;
	set2D();

	defaultClearColor.SetColor(L"white");
}

DrawingBoard::~DrawingBoard()
{
	glfwDestroyWindow(window);
	window = nullptr;
}

void DrawingBoard::BeginDraw() const
{
	glfwMakeContextCurrent(window);
	glClearColor(
		defaultClearColor.GetRed(),
		defaultClearColor.GetGreen(), 
		defaultClearColor.GetRed(),
		defaultClearColor.GetOpacity());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective

}

void DrawingBoard::ConfirmDraw()
{
	glfwSwapBuffers(window);
}

void DrawingBoard::set2D()
{
	if (mode2d)
		return;

	glMatrixMode(GL_PROJECTION); // Tell opengl that we are doing project matrix work
	glLoadIdentity(); // Clear the matrix
	glOrtho(-9.0, 9.0, -9.0, 9.0, 0.0, 30.0); // Setup an Ortho view
	glMatrixMode(GL_MODELVIEW); // Tell opengl that we are doing model matrix work. (drawing)
	glLoadIdentity(); // Clear the model matrix

	mode2d = true;
}

TrecPointer<TBrush> DrawingBoard::GetSolidColorBrush(const TColor& color)
{
	TrecPointer<TColorBrush> ret = TrecPointerKey::GetNewTrecPointer<TColorBrush>();

	ret->colors.push_back(color);
	ret->window = self;
	ret->brushType = brush_type::brush_type_solid;
	return TrecPointerKey::ConvertPointer<TColorBrush, TBrush>(ret);
}
