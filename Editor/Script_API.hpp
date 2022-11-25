#pragma once

#include <string>

#include <lua.hpp>


namespace SuperTed {
	namespace Editor {
		void InitState(lua_State* L, std::string ScriptFile);
	}
}