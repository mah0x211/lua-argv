/**
 *  Copyright (C) 2017 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  varargv.c
 *  lua-varargv
 *  Created by Masatoshi Teruya on 17/09/20.
 *
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// lua
#include "lauxhlib.h"
#include <lauxlib.h>
#include <lua.h>

#define MODULE_MT "argv"

typedef struct {
    int ref;
    int tmp_ref;
    lua_State *L;
    lua_State *tmp;
    uint16_t narg;
} argv_t;

static int select_lua(lua_State *L)
{
    int argc      = lua_gettop(L) - 2;
    argv_t *argv  = luaL_checkudata(L, 1, MODULE_MT);
    lua_Integer n = lauxh_optinteger(L, 2, argv->narg);

    // return the passed arguments
    if (argv->narg == 0) {
        if (argc > 0) {
            return argc;
        }
        return 0;
    }

    // return all stored arguments
    if (argc < 0) {
        if (n) {
            lua_xmove(argv->L, L, n);
            argv->narg = 0;
        }
        return n;
    }

    if (argc > 0) {
        lua_xmove(L, argv->tmp, argc);
    } else {
        argc = 0;
    }
    lua_settop(L, 0);

    // select specified number of values
    if (n > 0) {
        lua_xmove(argv->L, L, argv->narg);
        if (n >= argv->narg) {
            argv->narg = 0;
        } else {
            // select n arguments from the head
            argv->narg -= n;
            lua_xmove(L, argv->L, argv->narg);
        }
    } else if (-n >= argv->narg) {
        lua_xmove(argv->L, L, argv->narg);
        argv->narg = 0;
    } else {
        // select n arguments from the tail
        argv->narg += n;
        lua_xmove(argv->L, L, -n);
    }

    if (argc > 0) {
        lua_xmove(argv->tmp, L, argc);
    }

    return lua_gettop(L);
}

static int add_lua(lua_State *L)
{
    int argc        = lua_gettop(L) - 2;
    argv_t *argv    = luaL_checkudata(L, 1, MODULE_MT);
    lua_Integer idx = lauxh_checkinteger(L, 2);
    int narg        = 0;

    // do nothing without value arguments
    if (argc < 1) {
        lua_pushboolean(L, 1);
        return 1;
    } else if (!lua_checkstack(argv->L, argc)) {
        // cannot add extra slots to argv->L
        lua_pushboolean(L, 0);
        return 1;
    }

    if (idx == 0 || idx > argv->narg) {
        // insert values to tail
        argv->narg += argc;
        lua_xmove(L, argv->L, argc);
        lua_pushboolean(L, 1);
        return 1;
    } else if (idx == 1 || -idx >= argv->narg) {
        if (!lua_checkstack(L, argv->narg)) {
            // cannot add extra slots to L
            lua_pushboolean(L, 0);
            return 1;
        }
        // insert values to head
        lua_xmove(argv->L, L, argv->narg);
        argv->narg += argc;
        lua_xmove(L, argv->L, argv->narg);
        lua_pushboolean(L, 1);
        return 1;
    } else if (idx < 0) {
        narg = -idx;
    } else {
        narg = argv->narg - idx + 1;
    }

    if (!lua_checkstack(L, narg)) {
        // cannot add extra slots to L
        lua_pushboolean(L, 0);
        return 1;
    }
    // insert values to idx
    lua_xmove(argv->L, L, narg);
    argv->narg += argc;
    lua_xmove(L, argv->L, argc + narg);

    lua_pushboolean(L, 1);
    return 1;
}

static int set_lua(lua_State *L)
{
    int argc      = lua_gettop(L) - 2;
    argv_t *argv  = luaL_checkudata(L, 1, MODULE_MT);
    lua_Integer n = 0;

    // clear arguments
    lua_settop(argv->L, 0);
    argv->narg = 0;
    // do nothing without arguments
    if (argc < 0) {
        lua_pushboolean(L, 1);
        return 1;
    }

    n = lauxh_checkinteger(L, 2);
    if (argc < 1) {
        // do nothing without value arguments
        lua_pushboolean(L, 1);
        return 1;
    } else if (n >= argc || -n >= argc) {
        // return all arguments
        lua_pushboolean(L, 1);
        lua_replace(L, 2);
        return argc + 1;
    } else if (n == 0) {
        // store all arguments
        if (!lua_checkstack(argv->L, argc)) {
            // cannot add extra slots to argv->L
            lua_pushboolean(L, 0);
            return 1;
        }
        lua_xmove(L, argv->L, argc);
        argv->narg = argc;
        lua_pushboolean(L, 1);
        return 1;
    } else if (n > 0) {
        // store arguments except first n arguments
        int narg = argc - n;
        if (!lua_checkstack(argv->L, narg)) {
            // cannot add extra slots to argv->L
            lua_pushboolean(L, 0);
            return 1;
        }
        lua_xmove(L, argv->L, narg);
        argv->narg = narg;
        lua_pushboolean(L, 1);
        lua_replace(L, 2);
        return n + 1;
    }

    // store arguments except last n arguments
    if (!lua_checkstack(argv->L, argc)) {
        // cannot add extra slots to argv->L
        lua_pushboolean(L, 0);
        return 1;
    }
    lua_xmove(L, argv->L, argc);
    lua_pushboolean(L, 1);
    n = -n;
    lua_xmove(argv->L, L, n);
    argv->narg = argc - n;
    return n + 1;
}

static int len_lua(lua_State *L)
{
    argv_t *argv = luaL_checkudata(L, 1, MODULE_MT);

    lua_pushinteger(L, argv->narg);

    return 1;
}

static int tostring_lua(lua_State *L)
{
    lua_pushfstring(L, MODULE_MT ": %p", lua_touserdata(L, 1));
    return 1;
}

static int gc_lua(lua_State *L)
{
    argv_t *argv = lua_touserdata(L, 1);

    lua_settop(argv->L, 0);
    lua_settop(argv->tmp, 0);
    lauxh_unref(L, argv->ref);
    lauxh_unref(L, argv->tmp_ref);

    return 0;
}

static int new_lua(lua_State *L)
{
    argv_t *argv = lua_newuserdata(L, sizeof(argv_t));

    // alloc thread
    argv->L       = lua_newthread(L);
    argv->ref     = lauxh_ref(L);
    argv->tmp     = lua_newthread(L);
    argv->tmp_ref = lauxh_ref(L);
    argv->narg    = 0;
    lauxh_setmetatable(L, MODULE_MT);

    return 1;
}

LUALIB_API int luaopen_argv(lua_State *L)
{
    // create table __metatable
    if (luaL_newmetatable(L, MODULE_MT)) {
        struct luaL_Reg mmethod[] = {
            {"__gc",       gc_lua      },
            {"__tostring", tostring_lua},
            {"__len",      len_lua     },
            {NULL,         NULL        }
        };
        struct luaL_Reg method[] = {
            {"set",    set_lua   },
            {"add",    add_lua   },
            {"select", select_lua},
            {NULL,     NULL      }
        };
        struct luaL_Reg *ptr = mmethod;

        // metamethods
        while (ptr->name) {
            lauxh_pushfn2tbl(L, ptr->name, ptr->func);
            ptr++;
        }
        // metamethods
        ptr = method;
        lua_pushstring(L, "__index");
        lua_newtable(L);
        while (ptr->name) {
            lauxh_pushfn2tbl(L, ptr->name, ptr->func);
            ptr++;
        }
        lua_rawset(L, -3);
        lua_pop(L, 1);
    }

    // add new function
    lua_newtable(L);
    lauxh_pushfn2tbl(L, "new", new_lua);

    return 1;
}
