// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_INPUT_H_
#define LUA_INPUT_H_

#include "Lua/LuaMacros.h"

class  Acceleration;
class  Key;
struct lua_State;
struct Touch;

NS_RAINBOW_LUA_MODULE_BEGIN(Input)
{
	void init(lua_State *L);

	void accelerated(lua_State *L, const Acceleration &acceleration);

	void clear(lua_State *L);

#ifdef RAINBOW_BUTTONS
	void key_down(lua_State *L, const Key &key);
	void key_up(lua_State *L, const Key &key);
#endif

	void touch_began(lua_State *L, const Touch *const touches, const size_t count);
	void touch_canceled(lua_State *L);
	void touch_ended(lua_State *L, const Touch *const touches, const size_t count);
	void touch_moved(lua_State *L, const Touch *const touches, const size_t count);
} NS_RAINBOW_LUA_MODULE_END(Input)

#endif
