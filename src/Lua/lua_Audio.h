// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_AUDIO_H_
#define LUA_AUDIO_H_

#include "Lua/LuaMacros.h"

struct lua_State;

NS_RAINBOW_LUA_MODULE_BEGIN(Audio)
{
	void init(lua_State *);
} NS_RAINBOW_LUA_MODULE_END(Audio)

#endif
