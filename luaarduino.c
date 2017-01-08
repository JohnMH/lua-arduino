/*
Copyright (C) 2016 John M. Harris, Jr. <johnmh@openblox.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
This file is the result of my extreme laziness. Be warned, it's not
clean, but it works.
*/

//Default is to build the entire interpreter, we don't want that.
#ifndef MAKE_LIB
#ifndef MAKE_LUAC
#ifndef MAKE_LUA
#define MAKE_LUA
#endif
#endif
#endif

//Platform-specific features we're opting out of.
#if 0
#define LUA_USE_LINUX
#define LUA_USE_MACOSX
#define LUA_USE_POSIX
#define LUA_ANSI
#endif

#define HUGE_VAL	(__builtin_huge_val())

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZ 256

size_t strxfrm(char* dest, const char* src, size_t n){
	/* This implementation does not know about any locale but "C"... */
	size_t n2 = strlen(src);
	if(n > n2){
		strcpy(dest,src);
	}
	return n2;
}

int strcoll(const char* s1, const char* s2){
	char t1[1 + strxfrm(0, s1, 0)];
	strxfrm(t1, s1, sizeof(t1));
	char t2[1 + strxfrm(0, s2, 0)];
	strxfrm(t2, s2, sizeof(t2));
	return strcmp(t1, t2);
}

FILE* fopen(const char* path, const char* mode){
	(void)path;
	(void)mode;

	return NULL;
}

FILE* freopen(const char* filename, const char* mode, FILE* stream){
	return NULL;
}

char* strerror(int errnum){
	return "Uknown error";
}

static int _lastSeedNumber = 38;//Magic number, how do you like me now?

//Modulus for prng
#define M 0x7FFFFFFF
 
unsigned long int prng(){//random number generator; call with 1 <= x <=M-1
	unsigned long int x = _lastSeedNumber++;

	if(x <= M-1){
		_lastSeedNumber = 26;//More magic numbers!
	}
	
	x = (x >> 16) + ((x << 15) & M)  - (x >> 21) - ((x << 10) & M);
	
	if(x < 0){
		x += M;
	}
	
	return x;
}

#define luai_makeseed prng

#define l_randomizePivot prng

#define LUA_C89_NUMBERS
#define LUA_USE_C89

#include "luaconf.h"

#undef LUA_API
#define LUA_API

#undef LUALIB_API
#define LUALIB_API

#undef LUAMOD_API
#define LUAMOD_API

#undef LUAI_FUNC
#define LUAI_FUNC

#undef LUAI_DDEC
#define LUAI_DDEC

#if defined(lua_getlocaledecpoint)
#undef lua_getlocaledecpoint
#define lua_getlocaledecpoint() '.'
#endif

#define l_signalT int

#include "lapi.c"
#include "lcode.c"
#include "lctype.c"
#include "ldebug.c"
#include "ldo.c"
#include "ldump.c"
#include "lfunc.c"
#include "lgc.c"
#include "llex.c"
#include "lmem.c"
#include "lobject.c"
#include "lopcodes.c"
#include "lparser.c"
#include "lstate.c"
#include "lstring.c"
#include "ltable.c"
#include "ltm.c"
#include "lundump.c"
#define pushclosure pushclosure2
#include "lvm.c"
#include "lzio.c"

//Auxilary lib
#include "lauxlib.c"

//Standard lib, not useeded for luac
#ifndef MAKE_LUAC
#include "lbaselib.c"
//#include "lbitlib.c"
//#include "lcorolib.c"
#include "ldblib.c"
//#include "liolib.c"
#include "lmathlib.c"
//#include "loadlib.c"
//#include "loslib.c"
#include "lstrlib.c"
#include "ltablib.c"
#include "lutf8lib.c"
//#include "linit.c"
#endif

//lua
#ifdef MAKE_LUA
//#include "lua.c"
#endif

//luac
#ifdef MAKE_LUAC
#include "luac.c"
#endif

#include "main.c"
