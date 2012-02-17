--! Rainbow math helper.
--!
--! Because some of us don't have a PhD in math.
--!
--! Note that if you're going to use any of these in a loop, it is better to
--! inline them, e.g.:
--!
--! \code
--! -- This is BAD!
--! function bad_code()
--! 	local d = 1.0;
--! 	while true do
--! 		d = rainbow.math.deg2rad(d);
--! 	end
--! end
--!
--! -- This is GOOD!
--! function good_code()
--! 	local d = 1.0;
--! 	local pi_over_180 = math.pi / 180;
--! 	while true do
--! 		d = d * pi_over_180;
--! 	end
--! end
--! \endcode
--!
--! Do also note that the loops in these examples are utterly useless.
--!
--! Copyright 2012 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen
rainbow.math = rainbow.math or {};

--! Calculate the angle between two points with bearing north.
function rainbow.math.angle(a_x, a_y, b_x, b_y)
	return math.atan((b_y - a_y) / (b_x - a_x));
end

--! Convert radians to degrees.
function rainbow.math.degrees(radians)
	return radians * 180 / math.pi;
end

--! Calculate the distance between two points.
--! \param a_x,a_y  Starting point.
--! \param b_x,b_y  End point.
function rainbow.math.distance(a_x, a_y, b_x, b_y)
	local s1 = b_x - a_x;
	local s2 = b_y - a_y;
	return math.sqrt(s1 * s1, s2 * s2);
end

--! Calculate the hitbox.
--! \param x,y           Centre of the box.
--! \param width,height  Dimension of the box.
--! \param scale         Scaling factor for on-screen box size.
function rainbow.math.hitbox(x, y, width, height, scale)
	if not scale then
		scale = 1.0;
	end
	local half_w = width * scale * 0.5;
	local half_h = height * scale * 0.5;
	local hitbox = {};
	hitbox.x0 = x - half_w;
	hitbox.y0 = y - half_h;
	hitbox.x1 = x + half_w;
	hitbox.y1 = y + half_h;
	return hitbox;
end

--! Check whether a point is inside a box.
--! \param box    Table with the upper-left and lower-right points of the box.
--! \param point  The point to check.
function rainbow.math.is_inside(box, point)
	return point.x >= box.x0 && point.x <= box.x1 && point.y >= box.y0 && point.y <= box.y1;
end

--! Convert degrees to radians.
function rainbow.math.radians(degrees)
	return degrees * math.pi / 180;
end
