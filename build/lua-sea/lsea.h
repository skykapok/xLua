#ifndef __LUA_SEA_H_
#define __LUA_SEA_H_

#include "lauxlib.h"
#include <stdbool.h>
#include "matrix.h"

LUALIB_API int luaopen_sea_c(lua_State *L);

#define SEA_RECT 1
#define SEA_CIRCLE 2

struct shape {
	struct {
		unsigned int dirty : 1;
		unsigned int type : 7;
	};
	unsigned int bound2;
	union {
		struct {
			unsigned short width;
			unsigned short height;
		} rect;
		struct {
			unsigned int radius;
		} circle;
	};
	struct srt srt;
	struct matrix mat;
};


#endif
