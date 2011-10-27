/*
* Copyright (c) 2011 Rob Hoelz <rob@hoelz.ro>
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
#include <linenoise.h>

static int completionFuncRef;
static lua_State *completionState;

static void completionCallbackWrapper(const char *line, linenoiseCompletions *completions)
{
    lua_State *L = completionState;
    int status;

    lua_rawgeti(L, LUA_REGISTRYINDEX, completionFuncRef);
    lua_pushlightuserdata(L, completions);
    lua_pushstring(L, line);

    /* XXX error handling */
    status = lua_pcall(L, 2, 0, 0);
}

static int l_linenoise(lua_State *L)
{
    const char *prompt = luaL_checkstring(L, 1);

    completionState = L;
    lua_pushstring(L, linenoise(prompt));
    completionState = NULL;

    return 1;
}

/* XXX error checking */
static int l_historyadd(lua_State *L)
{
    const char *line = luaL_checkstring(L, 1);

    linenoiseHistoryAdd(line);

    return 0;
}

/* XXX error checking */
static int l_historysetmaxlen(lua_State *L)
{
    int len = luaL_checkinteger(L, 1);

    linenoiseHistorySetMaxLen(len);

    return 0;
}

/* XXX error checking */
static int l_historysave(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);

    linenoiseHistorySave((char *) filename);
}

/* XXX error checking */
static int l_historyload(lua_State *L)
{
    const char *filename = luaL_checkstring(L, 1);

    linenoiseHistoryLoad((char *) filename);
}

static int l_clearscreen(lua_State *L)
{
    linenoiseClearScreen();
    return 0;
}

static int l_setcompletion(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TFUNCTION);

    lua_pushvalue(L, 1);
    completionFuncRef = luaL_ref(L, LUA_REGISTRYINDEX);
    linenoiseSetCompletionCallback(completionCallbackWrapper);

    return 0;
}

static int l_addcompletion(lua_State *L)
{
    /* XXX type check */
    linenoiseCompletions *completions = (linenoiseCompletions *) lua_touserdata(L, 1);
    const char *entry                 = luaL_checkstring(L, 2);

    linenoiseAddCompletion(completions, (char *) entry);

    return 0;
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
    { NULL, NULL }
};

int luaopen_linenoise(lua_State *L)
{
    lua_newtable(L);

    luaL_register(L, NULL, linenoise_funcs);
    return 1;
}
