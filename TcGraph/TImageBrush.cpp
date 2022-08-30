#include "TImageBrush.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TImageBrush::~TImageBrush()
{
	if (data)
		stbi_image_free(data);
	data = nullptr;
}
