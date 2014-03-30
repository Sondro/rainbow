// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SPRITE_H_
#define LUA_SPRITE_H_

#include "Lua/LuaBind.h"

class Sprite;
class SpriteBatch;

NS_RAINBOW_LUA_BEGIN
{
	class Sprite : public Bind<Sprite>
	{
		friend Bind;

	public:
		Sprite(lua_State *);

		inline ::Sprite* get() const;

	private:
		static int get_angle(lua_State *);
		static int get_color(lua_State *);
		static int get_position(lua_State *);
		static int set_color(lua_State *);
		static int set_normal(lua_State *);
		static int set_pivot(lua_State *);
		static int set_position(lua_State *);
		static int set_rotation(lua_State *);
		static int set_scale(lua_State *);
		static int set_texture(lua_State *);

		static int mirror(lua_State *);
		static int move(lua_State *);
		static int rotate(lua_State *);

		const unsigned int id;
		const SpriteBatch *batch;

		::Sprite& sprite() const;
	};

	::Sprite* Sprite::get() const
	{
		return &this->sprite();
	}
} NS_RAINBOW_LUA_END

#endif
