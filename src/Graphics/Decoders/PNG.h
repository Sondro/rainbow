#include <cstring>
#include <png.h>

#include "Common/Debug.h"
#include "Common/Functional.h"

#define USE_PNG

namespace PNG
{
	bool check(const DataMap &data) pure;
	Rainbow::Image decode(const DataMap &data) pure;

	bool check(const DataMap &data)
	{
		return png_sig_cmp(data, 0, 8) == 0;
	}

	Rainbow::Image decode(const DataMap &data)
	{
		Rainbow::Image image;
		image.format = Rainbow::Image::Format::PNG;

		png_image pi;
		memset(&pi, 0, sizeof(pi));
		pi.version = PNG_IMAGE_VERSION;

		if (!png_image_begin_read_from_memory(&pi, data, data.size()))
			return image;

		image.width = pi.width;
		image.height = pi.height;
		switch (PNG_IMAGE_PIXEL_CHANNELS(pi.format))
		{
			case 1:
				pi.format = PNG_FORMAT_RGB;
				break;
			case 2:
				pi.format = PNG_FORMAT_GA;
				break;
			case 3:
				pi.format = PNG_FORMAT_RGB;
				break;
			case 4:
				pi.format = PNG_FORMAT_RGBA;
				break;
			default:
				R_ASSERT(false, "Invalid PNG format");
				break;
		}
		image.depth = PNG_IMAGE_SAMPLE_SIZE(pi.format) * 8;
		image.channels = PNG_IMAGE_SAMPLE_CHANNELS(pi.format);
		image.data = new unsigned char[PNG_IMAGE_SIZE(pi)];
		png_image_finish_read(&pi, nullptr, image.data, PNG_IMAGE_ROW_STRIDE(pi), nullptr);
		return image;
	}
}
