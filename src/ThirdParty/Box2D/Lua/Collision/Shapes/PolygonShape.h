// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_POLYGONSHAPE_H_
#define THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_POLYGONSHAPE_H_

#include "ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h"

class b2PolygonShape;

NS_B2_LUA_BEGIN
{
	class PolygonShape : public ShapeBase<PolygonShape>
	{
		friend Bind;

	public:
		explicit PolygonShape(lua_State *);
		~PolygonShape();

		b2PolygonShape* get() const { return polygon_; }

	private:
		static int GetType(lua_State *);
		static int GetChildCount(lua_State *);
		static int Set(lua_State *);
		static int SetAsBox(lua_State *);
		static int Validate(lua_State *);

		b2PolygonShape *polygon_;
		bool is_owner_;
	};
} NS_B2_LUA_END

#endif