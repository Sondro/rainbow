// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SHADERS_H_
#define LUA_SHADERS_H_

#include "Graphics/Shaders/Diffuse.h"
#include "Lua/LuaMacros.h"

struct lua_State;

NS_RAINBOW_LUA_BEGIN
{
	class Shader
	{
	public:
		virtual int id() const = 0;

	protected:
		~Shader() { }
	};
} NS_RAINBOW_LUA_END

NS_RAINBOW_LUA_MODULE_BEGIN(Shaders)
{
	void init(lua_State *);

	class Diffuse : public Shader
	{
	public:
		static const char class_name[];
		static const Rainbow::Lua::Method<Diffuse> methods[];

		Diffuse(lua_State *);

		const Rainbow::Shaders::Diffuse* get() const;

		virtual int id() const override;

	private:
		Rainbow::Shaders::Diffuse lighting;

		int set_cutoff(lua_State *);
		int set_radius(lua_State *);
		int set_position(lua_State *);
	};
} NS_RAINBOW_LUA_MODULE_END(Shaders)

#endif
