// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaHelper.h"

#include <cstring>
#include <memory>

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"
#include "Lua/LuaDebugging.h"
#include "Lua/LuaSyntax.h"

namespace
{
	const char kLuaErrorErrorHandling[] = "error handling";
	const char kLuaErrorGeneral[] = "general";
	const char kLuaErrorMemory[] = "memory allocation";
	const char kLuaErrorRuntime[] = "runtime";
	const char kLuaErrorSyntax[] = "syntax";
	const char kLuaErrorType[] = "Object is not of type '%s'";

	int load_module(lua_State* L,
	                char* path,
	                const char* module,
	                const char* suffix)
	{
		strcpy(path, module);
		strcat(path, suffix);
		const Path asset(path);
#ifndef RAINBOW_OS_ANDROID
		if (!asset.is_file())
			return 0;
#endif  // RAINBOW_OS_ANDROID
		const File& file = File::open(asset);
		if (!file)
			return 0;
		const int result = rainbow::lua::load(L, Data(file), module, false);
		if (result == 0)
			return luaL_error(L, "Failed to load '%s'", module);
		return result;
	}
}

NS_RAINBOW_LUA_BEGIN
{
	ScopedRef::ScopedRef(lua_State* L)
	    : state_(L), ref_(luaL_ref(L, LUA_REGISTRYINDEX)) {}

	ScopedRef::~ScopedRef()
	{
		if (ref_ == LUA_REFNIL ||
		    lua_rawlen(state_, LUA_REGISTRYINDEX) < static_cast<size_t>(ref_))
			return;

		luaL_unref(state_, LUA_REGISTRYINDEX, ref_);
	}

	void ScopedRef::reset(lua_State* L)
	{
		if (ref_ != LUA_REFNIL)
			luaL_unref(state_, LUA_REGISTRYINDEX, ref_);
		state_ = L;
		ref_ = (!L ? LUA_REFNIL : luaL_ref(L, LUA_REGISTRYINDEX));
	}

	void error(lua_State* L, int result)
	{
		R_ASSERT(result != LUA_OK, "No error to report");

		const char* desc = kLuaErrorGeneral;
		switch (result)
		{
			case LUA_ERRRUN:
				desc = kLuaErrorRuntime;
				break;
			case LUA_ERRSYNTAX:
				desc = kLuaErrorSyntax;
				break;
			case LUA_ERRMEM:
				desc = kLuaErrorMemory;
				break;
			case LUA_ERRERR:
				desc = kLuaErrorErrorHandling;
				break;
			default:
				break;
		}
		LOGE("Lua %s error: %s", desc, lua_tostring(L, -1));
		lua_pop(L, 1);
		dump_stack(L);
	}

	int load(lua_State* L)
	{
		const char* module = lua_tostring(L, -1);
		std::unique_ptr<char[]> path(new char[strlen(module) + 10]);
		const int result = load_module(L, path.get(), module, ".lua");
		return (!result ? load_module(L, path.get(), module, "/init.lua")
		                : result);
	}

	int load(lua_State* L, const Data& chunk, const char* name, bool exec)
	{
		int e = luaL_loadbuffer(L, chunk, chunk.size(), name);
		if (e == LUA_OK && exec)
			e = lua_pcall(L, 0, LUA_MULTRET, 0);
		if (e != LUA_OK)
		{
			error(L, e);
			return 0;
		}
		return 1;
	}

	template <>
	void push<bool>(lua_State* L, bool b)
	{
		lua_pushboolean(L, b);
	}

	template <>
	void push<const char*>(lua_State* L, const char* str)
	{
		lua_pushstring(L, str);
	}

	template <>
	void push<lua_CFunction>(lua_State* L, lua_CFunction c)
	{
		lua_pushcfunction(L, c);
	}

	template <>
	void push<lua_Integer>(lua_State* L, lua_Integer i)
	{
		lua_pushinteger(L, i);
	}

	template <>
	void push<lua_Number>(lua_State* L, lua_Number n)
	{
		lua_pushnumber(L, n);
	}

	void pushpointer(lua_State* L, void* ptr, const char* name)
	{
		lua_createtable(L, 1, 1);
		lua_pushlightuserdata(L, ptr);
		lua_rawseti(L, -2, 0);
		luaR_rawsetstring(L, "__type", name);
	}

	int reload(lua_State* L, const Data& chunk, const char* name)
	{
		lua_getglobal(L, "package");
		lua_pushliteral(L, "loaded");
		lua_rawget(L, -2);
		R_ASSERT(lua_istable(L, -1), "Missing control table 'package.loaded'");
		lua_pushstring(L, name);
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 2);
		return load(L, chunk, name, true);
	}

	void replacetable(lua_State* L, int n)
	{
		if (!lua_istable(L, n))
			return;

		lua_pushliteral(L, "__userdata");
		lua_rawget(L, n);
		if (!lua_isuserdata(L, -1))
		{
			lua_pop(L, 1);
			return;
		}
		lua_replace(L, n);
	}

	void sethook(lua_State* L, int mask)
	{
		if (g_level >= 0)
			return;

		int depth = -1;
		lua_Debug ar;
		while (lua_getstack(L, ++depth, &ar)) {}
		--depth;
		while (lua_getstack(L, ++g_level, &ar))
		{
			lua_getinfo(L, "Sl", &ar);
			g_callstack[depth - g_level].currentline = ar.currentline;
			g_callstack[depth - g_level].nparams = ar.nparams;
			g_callstack[depth - g_level].source = ar.source;
		}

		lua_sethook(L, lua_Hook, mask, 0);
	}

	void* topointer(lua_State* L, const char* name)
	{
		LUA_ASSERT(L, !lua_isnil(L, -1), "Unexpected nil value");
		LUA_ASSERT(L, lua_istable(L, -1), kLuaErrorType, name);
		lua_pushliteral(L, "__type");
		lua_rawget(L, -2);
		const char* type = lua_tostring(L, -1);
		if (!type)
		{
			LUA_ASSERT(L, type, kLuaErrorType, name);
			lua_pop(L, 1);
			return nullptr;
		}
		LUA_ASSERT(L, strcmp(type, name) == 0, kLuaErrorType, name);
		lua_rawgeti(L, -2, 0);
		void* ptr = lua_touserdata(L, -1);
		lua_pop(L, 2);
		return ptr;
		static_cast<void>(name);
		static_cast<void>(kLuaErrorType);
	}
} NS_RAINBOW_LUA_END
