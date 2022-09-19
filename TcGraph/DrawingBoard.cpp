#include <GL/glew.h>
#include "TColorBrush.h"
#include "TImageBrush.h"
#include <cassert>


DrawingBoard::DrawingBoard(GLFWwindow* window)
{
	if (!window) throw 0;
	this->window = window;

	defaultClearColor.SetColor(L"white");
}

GLFWwindow* DrawingBoard::GetGlfwWindow()
{
	return window;
}

TrecPointer<TBrush> DrawingBoard::GetHighlightBrush()
{
	if (!textHighlight.Get())
	{
		TColor color(L"lightyellow");
		// To-Do: get actual color from property

		// End To-Do

		textHighlight = GetSolidColorBrush(color);
	}
	return textHighlight;
}

bool DrawingBoard::IsContained(const TPoint& point, const RECT_F& loc)
{

	return (loc.bottom >= loc.top) &&
		(loc.right >= loc.left) &&

		(point.x >= loc.left) &&
		(loc.right >= point.x) &&
		(point.y >= loc.top) &&
		(loc.bottom >= point.y);
}

bool DrawingBoard::GetDisplayResolution(int& width, int& height)
{
	GLFWmonitor* mon = glfwGetWindowMonitor(window);
	if (!mon)
		mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(mon);

	if (!mode)
		return false;
	
	width = mode->width;
	height = mode->height;
	return true;
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

void DrawingBoard::SetShader(TrecPointer<TShader> shader, shader_type shaderType)
{
	if (shader.Get() && currentShader.Get() == shader.Get())
		return;
	switch (shaderType)
	{
	case shader_type::shader_3d:
		assert(shader.Get());
		currentShader = shader;
		break;
	case shader_type::shader_2d:
		//if(!shader2D.Get())
		//
		break;
	case shader_type::shader_texture:
		if (!shaderTex2D.Get())
			shaderTex2D = TrecPointerKey::GetNewTrecPointerAlt<TShader, Texture2DShader>();
		currentShader = shaderTex2D;
	case shader_type::shader_write:
		if (!shaderWrite.Get())
			shaderWrite = TrecPointerKey::GetNewTrecPointerAlt<TShader, TFreeTypeShader>();
		currentShader = shaderWrite;
	}
	this->shaderType = shaderType;
	currentShader->Use();
}

TrecPointer<TShader> DrawingBoard::GenerateShader(TrecPointer<TFileShell> shaderFile)
{
	return TShader::GenerateShader(shaderFile);
}


TrecPointer<TBrush> DrawingBoard::GetSolidColorBrush(const TColor& color) const
{
	TrecPointer<TColorBrush> ret = TrecPointerKey::GetNewTrecPointer<TColorBrush>();

	ret->colors.push_back(color);
	ret->window = self;
	ret->brushType = brush_type::brush_type_solid;
	return TrecPointerKey::ConvertPointer<TColorBrush, TBrush>(ret);
}

TrecPointer<TBrush> DrawingBoard::GetImageBrush(TrecPointer<TFileShell> file) const
{
	if (!file.Get())
		return TrecPointer<TBrush>();
	TrecPointer<TImageBrush> ret = TrecPointerKey::GetNewTrecPointer<TImageBrush>();

	ret->GenerateImageData(file);

	if (!ret->data)
		return TrecPointer<TBrush>();

	GLenum sourceChannel;

	switch (ret->channelCount)
	{
	case 3:
		sourceChannel = GL_RGB;
		break;
	case 4:
		sourceChannel = GL_RGBA;
		break;
	default:
		return TrecPointer<TBrush>();
	}

	glGenTextures(1, &(ret->textureId));
	glBindTexture(GL_TEXTURE_2D, ret->textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ret->width, ret->height, 0, sourceChannel, GL_UNSIGNED_BYTE, ret->data);
//	glGenerateMipmap(GL_TEXTURE_2D);
	return TrecPointerKey::ConvertPointer<TImageBrush, TBrush>(ret);
}
