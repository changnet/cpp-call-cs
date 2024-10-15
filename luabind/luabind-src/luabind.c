#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <assert.h>

#include "skynet_malloc.h"
#include "malloc_hook.h"

int main(int argc, char *argv[])
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
	
	void *p = skynet_malloc(2048);
	assert(p);
	skynet_free(p);
	
	int e = luaL_dofile(L, "test.lua");
	if (LUA_OK != e)
    {
        const char *err_msg = lua_tostring(L, -1);
        printf("error:%s\n", err_msg);

        return 1;
    }
    lua_close(L);

	return 0;
}
