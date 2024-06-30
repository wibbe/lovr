
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// For lua5.2 support, instead we could replace all the luaL_register's with whatever
// lua5.2's equivalent function is, but this is easier so whatever.
#define LUA_COMPAT_MODULE
#include "lua.h"
#include "lauxlib.h"

#define OUTER_RADIUS 1.1547f
#define INNER_RADIUS 1.0f
#define HEIGHT		 0.5f


static uint32_t make_coord(int32_t x, int32_t z) {
	return ((x & 0xFFFF) << 16) | (z & 0xFFFF);
}

static int coord_new(lua_State* L) {
	uint32_t x = luaL_checkinteger(L, 1);
	uint32_t z = luaL_checkinteger(L, 2);

	lua_pushinteger(L, make_coord(x, z));
	return 1;
}

static int coord_x(lua_State* L) {
	uint32_t coord = luaL_checkinteger(L, 1);
	int16_t x = (int16_t)(coord >> 16);
	lua_pushinteger(L, x);
	return 1;
}

static int coord_y(lua_State* L) {
	uint32_t coord = luaL_checkinteger(L, 1);
	int16_t x = (int16_t)(coord >> 16);
	int16_t z = (int16_t)(coord & 0xFFFF);
	lua_pushinteger(L, -x - z);
	return 1;
}

static int coord_z(lua_State* L) {
	uint32_t coord = luaL_checkinteger(L, 1);
	int16_t z = (int16_t)(coord & 0xFFFF);
	lua_pushinteger(L, z);
	return 1;
}

static int coord_from_position(lua_State* L) {
	float pos_x = luaL_checknumber(L, 1);
	float pos_y = luaL_checknumber(L, 2);
	float pos_z = luaL_checknumber(L, 3);

    float x = pos_x / (INNER_RADIUS * 2.0f);
	float y = -x;
    float offset = pos_z / (OUTER_RADIUS * 3.0f);
    x -= offset;
    y -= offset;

    int i_x = lroundf(x);
    int i_y = lroundf(y);
    int i_z = lroundf(-x - y);

    if (i_x + i_y + i_z != 0)
    {
        float dX = abs(x - i_x);
        float dY = abs(y - i_y);
        float dZ = abs(-x -y - i_z);

        if (dX > dY && dX > dZ) {
            i_x = -i_y - i_z;
        } else if (dZ > dY) {
            i_z = -i_x - i_y;
        }
    }

	lua_pushinteger(L, make_coord(i_x, i_z));
	return 1;
}

static int coord_to_position(lua_State* L) {
	uint32_t coord = luaL_checkinteger(L, 1);
	int height = luaL_optinteger(L, 2, 0);

	int16_t ix = (int16_t)(coord >> 16);
	int16_t iz = (int16_t)(coord & 0xFFFF);

	float x = (ix + iz * 0.5f) * INNER_RADIUS * 2.0f;
	float y = height * HEIGHT;
	float z = iz * OUTER_RADIUS * 1.5f;

	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);
	return 3;
}

static const struct luaL_Reg coord_funcs [] = {
	{"new", coord_new},
	{"fromPosition", coord_from_position},
	{"toPosition", coord_to_position},
	{"x", coord_x},
	{"y", coord_y},
	{"z", coord_z},
	{NULL, NULL}
};

int luaopen_coord(lua_State *l) {
	luaL_register(l, "coord", coord_funcs);
	return 1;
}
