/*
 *  Copyright (C) 2017 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 *
 *  varargv.c
 *  lua-varargv
 *  Created by Masatoshi Teruya on 17/09/20.
 *
 */

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
// lua
#include <lua.h>
#include <lauxlib.h>
#include "lauxhlib.h"
#include "lauxhlib.h"


#define MODULE_MT   "argv"


typedef struct {
    int ref;
    int narg;
    lua_State *L;
} argv_t;


static int select_lua( lua_State *L )
{
    argv_t *argv = luaL_checkudata( L, 1, MODULE_MT );
    lua_Integer select = lauxh_optinteger( L, 2, argv->narg );

    lua_settop( L, 0 );
    if( argv->narg )
    {
        lua_xmove( argv->L, L, argv->narg );
        if( argv->narg > select ){
            argv->narg -= select;
            lua_xmove( L, argv->L, argv->narg );
            return select;
        }

        select = argv->narg;
        argv->narg = 0;

        return select;
    }

    return 0;
}


static int set_lua( lua_State *L )
{
    int argc = lua_gettop( L ) - 2;
    argv_t *argv = luaL_checkudata( L, 1, MODULE_MT );
    lua_Integer select = lauxh_checkinteger( L, 2 );

    // push new arguments
    lua_settop( argv->L, 0 );
    if( argc > select ){
        argv->narg = argc - select;
        lua_xmove( L, argv->L, argv->narg );
    }
    else {
        select = argc;
        argv->narg = 0;
    }

    return select;
}


static int len_lua( lua_State *L )
{
    argv_t *argv = luaL_checkudata( L, 1, MODULE_MT );

    lua_pushinteger( L, argv->narg );

    return 1;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, MODULE_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


static int gc_lua( lua_State *L )
{
    argv_t *argv = lua_touserdata( L, 1 );

    lua_settop( argv->L, 0 );
    lauxh_unref( L, argv->ref );

    return 0;
}


static int new_lua( lua_State *L )
{
    argv_t *argv = lua_newuserdata( L, sizeof( argv_t ) );

    // alloc thread
    argv->L = lua_newthread( L );
    argv->ref = lauxh_ref( L );
    argv->narg = 0;
    lauxh_setmetatable( L, MODULE_MT );

    return 1;
}


LUALIB_API int luaopen_argv( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { "__len", len_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "set", set_lua },
        { "select", select_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = mmethod;

    // create table __metatable
    luaL_newmetatable( L, MODULE_MT );
    // metamethods
    while( ptr->name ){
        lauxh_pushfn2tbl( L, ptr->name, ptr->func );
        ptr++;
    }
    // metamethods
    ptr = method;
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    while( ptr->name ){
        lauxh_pushfn2tbl( L, ptr->name, ptr->func );
        ptr++;
    }
    lua_rawset( L, -3 );
    lua_pop( L, 1 );

    // add new function
    lua_newtable( L );
    lauxh_pushfn2tbl( L, "new", new_lua );

    return 1;
}


