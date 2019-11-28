#include <math.h>
#include <stdbool.h>
#include <lua.h>
#include "lsea.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

static void
w2l(struct matrix* mat, double x, double y, double *lx, double *ly) {
	struct matrix inv;
	matrix_inverse(mat, &inv);
	matrix_mul_point(&inv, &x, &y);
	*lx = x;
	*ly = y;
}

static void
l2w(struct matrix* mat, double x, double y, double *lx, double *ly) {
	matrix_mul_point(mat, &x, &y);
	*lx = x;
	*ly = y;
}

static bool
check_circle(struct shape* p, float x, float y) {
	double lx, ly;
	w2l(&p->mat, x, y, &lx, &ly);
	return -lx <= p->circle.radius && lx <= p->circle.radius
		&& -ly <= p->circle.radius && ly <= p->circle.radius;
}

static bool
check_rect(struct shape* p, float x, float y) {
	double lx, ly;
	w2l(&p->mat, x, y, &lx, &ly);
	return lx >= -p->rect.width / 2 && lx <= p->rect.width / 2
		&& ly >= -p->rect.height / 2 && ly <= p->rect.height / 2;
}

static bool
check_shape(struct shape* p, float x, float y) {
	float dx = p->srt.offx - x;
	float dy = p->srt.offy - y;
	if (dx * dx + dy * dy > p->bound2) return false;

	if (p->dirty) {
		p->dirty = 0;

		matrix_identity(&p->mat);
		matrix_srt(&p->mat, &p->srt);
	}

	switch (p->type)
	{
	case SEA_CIRCLE:
		if (check_circle(p, x, y)) {
			return true;
		}
		break;
	case SEA_RECT:
		if (check_rect(p, x, y)) {
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}


/*************************************
lua interfaces
**************************************/
static int
lnew_shape(lua_State *L) {
	struct shape * p = (struct shape*)lua_newuserdata(L, sizeof(struct shape));
	matrix_identity(&p->mat);

	p->type = (unsigned int)luaL_checkinteger(L, 1);
	p->id = (unsigned int)luaL_checkinteger(L, 2);
	p->dirty = 0;

	switch (p->type)
	{
	case SEA_RECT:
		p->rect.width = (unsigned short)luaL_checkinteger(L, 3);
		p->rect.height = (unsigned short)luaL_checkinteger(L, 4);
		unsigned int bound = p->rect.width;
		if (bound < p->rect.height) bound = p->rect.height;
		p->bound2 = bound * bound;
		break;
	case SEA_CIRCLE:
		p->circle.radius = (unsigned int)luaL_checkinteger(L, 3);
		p->bound2 = p->circle.radius * p->circle.radius;
		break;
	default:
		luaL_error(L, "undefined shape type:%d", p->type);
		break;
	}

	return 1;
}

static int
lupdate_shape(lua_State *L) {
	struct shape* p = (struct shape*)lua_touserdata(L, 1);

	struct srt srt;
	p->srt.offx = luaL_checknumber(L, 2);
	p->srt.offy = luaL_checknumber(L, 3);
	p->srt.rot = luaL_checknumber(L, 4) * 2048 / M_PI;
	p->srt.scale = luaL_checknumber(L, 5) * 1024;
	p->dirty = 1;

	return 0;
}

static int
lcollide(lua_State *L) {
	struct shape* p = (struct shape*)lua_touserdata(L, 1);
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);

	lua_pushboolean(L, check_shape(p, x, y));
	return 1;
}

static int
lbd(lua_State *L) {
	struct shape* p = (struct shape*)lua_touserdata(L, 1);

	if (p->type != SEA_RECT)
		return 0;

	struct srt srt;
	srt.offx = luaL_checknumber(L, 2);
	srt.offy = luaL_checknumber(L, 3);
	srt.rot = luaL_checknumber(L, 4) * 2048 / M_PI;
	srt.scale = luaL_checknumber(L, 5) * 1024;

	struct matrix mat;
	matrix_identity(&mat);
	matrix_srt(&mat, &srt);

	double x, y;
	l2w(&mat, -p->rect.width / 2, -p->rect.height / 2, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);

	l2w(&mat, p->rect.width / 2, -p->rect.height / 2, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);

	l2w(&mat, p->rect.width / 2, p->rect.height / 2, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);

	l2w(&mat, -p->rect.width / 2, p->rect.height / 2, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 8;
}

LUAMOD_API int
luaopen_sea_c(lua_State *L) {
	//luaL_checkversion(L);
	luaL_Reg l[] = {
	//	{ "normalise_line", _normalise_line },
		{ "add_shape", lnew_shape },
		{ "update_shape", lupdate_shape },
		{ "collide", lcollide },
		{ "bd", lbd },
		{ NULL, NULL },
	};

#if LUA_VERSION_NUM < 502
	luaL_register(L, "sea_c", l);
#else
	luaL_newlib(L, l);
#endif
	return 1;
}
