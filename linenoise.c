/* vim:sts=4 sw=4 expandtab
 */

/*
* Copyright (c) 2011-2015 Rob Hoelz <rob@hoelz.ro>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is furnished
* to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>
#include "linenoise.h"

#define LN_COMPLETION_TYPE "linenoiseCompletions*"

#ifdef _WIN32
#define LN_EXPORT __declspec(dllexport)
#else
#define LN_EXPORT extern
#endif

static int completion_func_ref;
static int hints_func_ref;
static lua_State *completion_state;

static int handle_ln_error(lua_State *L)
{
    lua_pushnil(L);
    return 1;
}

static int handle_ln_ok(lua_State *L)
{
    lua_pushboolean(L, 1);
    return 1;
}

static void completion_callback_wrapper(const char *line, linenoiseCompletions *completions)
{
    lua_State *L = completion_state;

    lua_rawgeti(L, LUA_REGISTRYINDEX, completion_func_ref);
    *((linenoiseCompletions **) lua_newuserdata(L, sizeof(linenoiseCompletions *))) = completions;
    luaL_getmetatable(L, LN_COMPLETION_TYPE);
    lua_setmetatable(L, -2);

    lua_pushstring(L, line);

    // XXX handle error
    lua_pcall(L, 2, 0, 0);
}

// XXX document this
static char *
hints_callback_wrapper(const char *line, int *color, int *bold)
{
    lua_State *L = completion_state;
    char *result = NULL;

    lua_rawgeti(L, LUA_REGISTRYINDEX, hints_func_ref);

    lua_pushstring(L, line);

    // XXX handle error
    lua_pcall(L, 1, 1, 0);

    // XXX if it's not a table, or if the fields aren't of the correct types...
    if(lua_istable(L, -1)) {
        lua_getfield(L, -1, "hint"); // XXX or t[1]?
        if(lua_isstring(L, -1)) {
            const char *hint;
            lua_Alloc alloc_f;
            void *ud;

            hint = lua_tostring(L, -1);
            alloc_f = lua_getallocf(L, &ud);
            result = alloc_f(&ud, NULL, LUA_TSTRING, strlen(hint) + 1);
            if(result) {
                strcpy(result, hint);
            }
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, "color");
        if(lua_isinteger(L, -1)) {
            *color = lua_tointeger(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, "bold");
        *bold = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    lua_pop(L, 1);

    return result;
}

static void
free_hints_callback(void *p)
{
    lua_State *L = completion_state;
    lua_Alloc alloc_f;
    void *ud;

    alloc_f = lua_getallocf(L, &ud);

    alloc_f(ud, p, 0, 0);
}

static int l_linenoise(lua_State *L)
{
    const char *prompt = luaL_checkstring(L, 1);
    char *line;

    completion_state = L;
    line = linenoise(prompt);
    completion_state = NULL;

    if(! line) {
        return handle_ln_error(L);
    }
    lua_pushstring(L, line);
    free(line);
    return 1;
}

static int lines_next(lua_State *L)
{
    lua_pushcfunction(L, l_linenoise);
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_call(L, 1, 1);
    return 1;
}

static int l_lines(lua_State *L)
{
    luaL_checkstring(L, 1);
    lua_pushcclosure(L, lines_next, 1);
    return 1;
}

static int l_historyadd(lua_State *L)
{
    const char *line = luaL_checkstring(L, 1);

    if(! linenoiseHistoryAdd(line)) {
        return handle_ln_error(L);
    }

    return handle_ln_ok(L);
}

static int l_historysetmaxlen(lua_State *L)
{
    int len = luaL_checkinteger(L, 1);

    if(! linenoiseHistorySetMaxLen(len)) {
        return handle_ln_error(L);
    }

    return handle_ln_ok(L);
}

static int l_historysave(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);

    if(linenoiseHistorySave((char *) filename) < 0) {
        return handle_ln_error(L);
    }
    return handle_ln_ok(L);
}

static int l_historyload(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);

    if(linenoiseHistoryLoad((char *) filename) < 0) {
        return handle_ln_error(L);
    }
    return handle_ln_ok(L);
}

static int l_clearscreen(lua_State *L)
{
    linenoiseClearScreen();
    return handle_ln_ok(L);
}

static int l_setcompletion(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);

    lua_pushvalue(L, 1);
    completion_func_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    linenoiseSetCompletionCallback(completion_callback_wrapper);

    return handle_ln_ok(L);
}

static int l_addcompletion(lua_State *L)
{
    linenoiseCompletions *completions = *((linenoiseCompletions **) luaL_checkudata(L, 1, LN_COMPLETION_TYPE));
    const char *entry                 = luaL_checkstring(L, 2);

    linenoiseAddCompletion(completions, (char *) entry);

    return handle_ln_ok(L);
}

static int
l_setmultiline(lua_State *L)
{
    int is_multi_line = lua_toboolean(L, 1);

    linenoiseSetMultiLine(is_multi_line);

    return handle_ln_ok(L);
}

// XXX if you set to nil, remove the hints callback (do similar for completion callback)
static int
l_sethints(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);

    lua_pushvalue(L, 1);
    hints_func_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    linenoiseSetHintsCallback(hints_callback_wrapper);
    linenoiseSetFreeHintsCallback(free_hints_callback);
    return handle_ln_ok(L);
}

static int
l_printkeycodes(lua_State *L)
{
    linenoisePrintKeyCodes();
    return handle_ln_ok(L);
}

luaL_Reg linenoise_funcs[] = {
    { "linenoise", l_linenoise },
    { "historyadd", l_historyadd },
    { "historysetmaxlen", l_historysetmaxlen },
    { "historysave", l_historysave },
    { "historyload", l_historyload },
    { "clearscreen", l_clearscreen },
    { "setcompletion", l_setcompletion},
    { "addcompletion", l_addcompletion },
    { "setmultiline", l_setmultiline },
    { "sethints", l_sethints },
    { "printkeycodes", l_printkeycodes },

    /* Aliases for more consistent function names */
    { "addhistory", l_historyadd },
    { "sethistorymaxlen", l_historysetmaxlen },
    { "savehistory", l_historysave },
    { "loadhistory", l_historyload },

    { "line", l_linenoise },
    { "lines", l_lines },

    { NULL, NULL }
};

luaL_Reg linenoise_methods[] = {
    { "add", l_addcompletion },
    { NULL, NULL }
};

LN_EXPORT int luaopen_linenoise(lua_State *L)
{
    lua_newtable(L);

    luaL_newmetatable(L, LN_COMPLETION_TYPE);
    lua_pushboolean(L, 0);
    lua_setfield(L, -2, "__metatable");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

#if LUA_VERSION_NUM > 501
    luaL_setfuncs(L, linenoise_methods, 0);
    lua_pop(L, 1);
    luaL_setfuncs(L,linenoise_funcs,0);
#else
    luaL_register(L, NULL, linenoise_methods);
    lua_pop(L, 1);
    luaL_register(L, NULL, linenoise_funcs);
#endif
    return 1;
}
