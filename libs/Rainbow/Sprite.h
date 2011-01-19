/// Custom sprite object, created by TextureAtlas.

/// Implemented using interleaved vertex data buffer objects.
///
/// \see http://developer.apple.com/library/ios/#documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/TechniquesforWorkingwithVertexData/TechniquesforWorkingwithVertexData.html
/// \see http://iphonedevelopment.blogspot.com/2009/06/opengl-es-from-ground-up-part-8.html
/// \see http://mathforum.org/mathimages/index.php/Transformation_Matrix
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef SPRITE_H_
#define SPRITE_H_

#include <cassert>

#include <Rainbow/Algorithm.h>
#include <Rainbow/OpenGL.h>
#include <Rainbow/TextureAtlas.h>

class Sprite
{
public:
	static const unsigned int vertex_array_sz = 4 * sizeof(SpriteVertex);

	const unsigned int width;   ///< Width of sprite (not scaled)
	const unsigned int height;  ///< Height of sprite (not scaled)

	Sprite(const unsigned int width, const unsigned int height, TextureAtlas *parent = 0);
	~Sprite();

	/// Draw the sprite.
	void draw();

	/// Rotate the sprite by an angle (in radian).
	void rotate(const float r);

	/// Uniform scaling of sprite (does not affect width and height properties).
	void scale(const float f);

	/// Non-uniform scaling of sprite (does not affect width and height properties).
	void scale(const Vec2f &f);

	/// Sets the pivot point for rotation and translation.
	/// \param x  Normalised x-component of pivot point
	/// \param y  Normalised y-component of pivot point
	void set_pivot(const float x, const float y);

	/// Sets sprite position (absolute).
	void set_position(const float x, const float y);

	/// Sets sprite position (absolute).
	void set_position(const Vec2f &position);

	/// Sets the texture.
	/// \param id  Id of texture to use
	void set_texture(const unsigned int id)
	{
		this->parent->get_texture(id, this->vertex_array);
	}

	/// Updates the vertices of this sprite.
	void update();

protected:
	static const unsigned char stale_pivot    = 0x01;
	static const unsigned char stale_position = 0x02;
	static const unsigned char stale_scale    = 0x04;
	static const unsigned char stale_angle    = 0x08;

private:
	bool buffered;               ///< Whether or not this sprite is buffered
	unsigned char stale;         ///< Sprite is stale if its properties has changed

	float angle;                 ///< Sprite rotation angle
	float cos_r;                 ///< Cosine of angle
	float sin_r;                 ///< Sine of angle

	GLuint texture;              ///< Texture buffer name
	SpriteVertex *vertex_array;  ///< Vertex array or, if buffered, the sprite batch's buffer
	TextureAtlas *parent;        ///< Texture atlas pointer

	Vec2f pivot;                 ///< Pivot point (normalised)
	Vec2f position;              ///< Current position
	Vec2f position_d;            ///< Difference between current and next position
	Vec2f scale_f;               ///< Scaling factor
	Vec2f origin[4];             ///< Original rendering at origo

	template<int N> friend class SpriteBatch;
	friend class TextureAtlas;
};

#endif