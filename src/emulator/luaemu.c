#include <lua.h>
#include <lauxlib.h>

#include "machine.h"

#if 0
static void print(lua_State *L, int idx) {
    idx = lua_absindex(L, idx);
    lua_getglobal(L, "tostring");
    lua_pushvalue(L, idx);
    lua_call(L, 1, 1);
    fprintf(stderr, "%02d: %s\n", idx, lua_tostring(L, -1));
    lua_pop(L, 1);
}
#endif

static int is_virt_machine(lua_State *L, int idx) {
    idx = lua_absindex(L, idx);
    if (!lua_getmetatable(L, idx)) lua_pushnil(L);
    int ret = lua_compare(L, -1, lua_upvalueindex(1), LUA_OPEQ);
    lua_pop(L, 1);
    return ret;
}

static VirtMachine *check_virt_machine(lua_State *L, int idx) {
    if (!is_virt_machine(L, idx)) {
        luaL_argerror(L, idx, "expected virtual machine");
    }
    VirtMachine **pv = (VirtMachine **) lua_touserdata(L, idx);
    return *pv;
}

static int virt_lua_interrupt_and_run(lua_State *L) {
    VirtMachine *v = check_virt_machine(L, 1);
    lua_Integer n = luaL_checkinteger(L, 2);
    int shuthost = virt_machine_interrupt_and_run(v, n);
    if (shuthost) {
        lua_pushnil(L);
        lua_pushinteger(L, virt_machine_get_cycle_counter(v));
        return 2;
    } else {
        lua_pushinteger(L, virt_machine_get_cycle_counter(v));
        return 1;
    }
}

static int virt_lua_create(lua_State *L) {
    VirtMachine *s;
    VirtMachineParams p_s, *p = &p_s;
    void **u;
    virt_lua_load_config(L, p, 1);
    s = virt_machine_init(p);
    virt_machine_free_config(p);
    if (!s) {
        luaL_error(L, "Failed to initialize machine.");
    }
    u = lua_newuserdata(L, sizeof(s));
    *u = s;
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);
    return 1;
}

static int virt_lua__tostring(lua_State *L) {
    lua_pushstring(L, "virtual machine");
    return 1;
}

static int virt_lua__gc(lua_State *L) {
    VirtMachine *v = check_virt_machine(L, 1);
    virt_machine_end(v);
    return 0;
}

static const luaL_Reg virt_lua__index[] = {
    {"interrupt_and_run", virt_lua_interrupt_and_run},
    { NULL, NULL }
};

static const luaL_Reg virt_lua_meta[] = {
    {"__gc", virt_lua__gc},
    {"__tostring", virt_lua__tostring},
    { NULL, NULL }
};

static const luaL_Reg emu_module[] = {
    {"create", virt_lua_create},
    { NULL, NULL }
};

__attribute__((visibility("default")))
int luaopen_emu(lua_State *L) {
    lua_newtable(L); /* mod */
    lua_newtable(L); /* mod emumeta */
    lua_newtable(L); /* mod emumeta emuidx */
    lua_pushvalue(L, -2); /* mod emumeta emuidx emumeta */
    luaL_setfuncs(L, virt_lua__index, 1); /* mod emumeta emuidx */
    lua_setfield(L, -2, "__index"); /* mod emumeta */
    lua_pushvalue(L, -1); /* mod emumeta emumeta */
    luaL_setfuncs(L, virt_lua_meta, 1); /* mod emumeta */
    luaL_setfuncs(L, emu_module, 1); /* mod */
    return 1;
}
