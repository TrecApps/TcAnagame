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
}

brush_type TImageBrush::GetBrushType()
{
	return brush_type::brush_type_bitmap;
}

float* TImageBrush::GeneratePictureVertices(const RECT_F& picLocation, const RECT_F& clipLocation)
{
	float width = picLocation.right - picLocation.left;
	float height = picLocation.top - picLocation.bottom;

	float topDif = (picLocation.top - clipLocation.top) / height;
	float leftDif = (clipLocation.left - picLocation.left) / width;
	float bottomDif = (clipLocation.bottom - picLocation.bottom) / height;
	float rightDif = (picLocation.right - clipLocation.right) / width;

	if (width < 0 ||
		height < 0 ||
		topDif < 0 ||
		leftDif < 0 ||
		bottomDif < 0 ||
		rightDif < 0)
		return nullptr;

	float* vertices = new float[32] {
		// positions                                     // colors           // texture coords
		clipLocation.right, clipLocation.top   , 0.0f,   1.0f, 0.0f, 0.0f,   1.0f - rightDif, 1.0f - topDif	,  // top right
		clipLocation.right, clipLocation.bottom, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f - rightDif, bottomDif		,  // bottom right
		clipLocation.left,  clipLocation.bottom, 0.0f,   0.0f, 0.0f, 1.0f,   leftDif		, bottomDif		,  // bottom left
		clipLocation.left,  clipLocation.top   , 0.0f,   1.0f, 1.0f, 0.0f,   leftDif		, 1.0f - topDif	   // top left 
	};

	return vertices;
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
