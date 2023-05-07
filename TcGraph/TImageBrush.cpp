#include <GL/glew.h>
#include "TImageBrush.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


void TImageBrush::GenerateImageData(TrecPointer<TFileShell> file)
{
	data = stbi_load(
		file->GetPath().GetRegString().c_str(),
		&width,
		&height,
		&channelCount,
		0);
}

TImageBrush::~TImageBrush()
{
	if (data)
		stbi_image_free(data);
	data = nullptr;

	glDeleteTextures(1, &textureId);
}

brush_type TImageBrush::GetBrushType()
{
	return brush_type::brush_type_bitmap;
}

float* TImageBrush::GeneratePictureVertices(const RECT_F& picLocation, const RECT_F& clipLocation)
{
	float width = picLocation.right - picLocation.left;
	float height = picLocation.top - picLocation.bottom;

	float topDif = (picLocation.top > clipLocation.top) ? ((picLocation.top - clipLocation.top) / height) : 0.0f;
	float leftDif = (clipLocation.left > picLocation.left) ? ((clipLocation.left - picLocation.left) / width) : 0.0f;
	float bottomDif = (clipLocation.bottom > picLocation.bottom) ? ((clipLocation.bottom - picLocation.bottom) / height) : 0.0f;
	float rightDif = (picLocation.right > clipLocation.right) ? ((picLocation.right - clipLocation.right) / width) : 0.0f;

	if (width < 0 ||
		height < 0 /*||
		topDif < 0 ||
		leftDif < 0 ||
		bottomDif < 0 ||
		rightDif < 0*/)
		return nullptr;

	float* vertices = new float[24] {
		// positions                                    // texture coords
		

		 picLocation.left , picLocation.top   , 0.0f + leftDif , 0.0f + bottomDif,
		 picLocation.left , picLocation.bottom, 0.0f + leftDif , 1.0f - topDif,
		 picLocation.right, picLocation.bottom, 1.0f - rightDif, 1.0f - topDif,

		 picLocation.left , picLocation.top   , 0.0f + leftDif , 0.0f + bottomDif,
		 picLocation.right, picLocation.bottom, 1.0f - rightDif, 1.0f - topDif,
		 picLocation.right, picLocation.top   , 1.0f - rightDif, 0.0f + bottomDif};

	return vertices;
}

bool TImageBrush::NormalizeRect(RECT_F& output, const RECT_F& input, TrecPointer<DrawingBoard> board)
{
	if(!board.Get())
	return false;

	float height = board->GetArea().bottom;

	output = input;

	output.bottom = -output.bottom + height;
	output.top = -output.top + height;
	return true;
}


void TImageBrush::FillRectangle(const RECT_F& location)
{
	FillRectangle(location, location);
}

void TImageBrush::FillRectangle(const RECT_F& picLocation, const RECT_F& clipLocation)
{
	TrecPointer<DrawingBoard> board = TrecPointerKey::TrecFromSoft<>(window);
	if (!board.Get())
		return;
	float* verticies = GeneratePictureVertices(picLocation, clipLocation);
	if (!verticies)
		return;

	board->SetShader(TrecPointer<TShader>(), shader_type::shader_texture);

	

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glBindTexture(GL_TEXTURE_2D, this->textureId);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float[32]), verticies, GL_STATIC_DRAW);

	glBindVertexArray(VBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	delete[] verticies;
	verticies = nullptr;
}
