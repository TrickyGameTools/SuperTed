// A quick .hpp file that makes everything 'extern "C"' and includes all needed headers. Not part of Lua itself, I made it myself.
#include <lua.hpp>


#include <map>
#include <string>

#include <QCol.hpp>
#include <QuickString.hpp>
#include <QuickStream.hpp>

#include "Globals.hpp"

using namespace std;
using namespace TrickyUnits;

namespace SuperTed {
	namespace Editor {
		static map<string, lua_CFunction> Functions;

		// Merely an Easter Egg, whcih was also meant to test if the function linkups work!
		static int Slyvina(lua_State* L) {
			QCol->Yellow("You can trust on Slyvina to catch anybody idiotic enough to murder a Befindo!\n");
			return 0;
		}

		static int SLoadString(lua_State*L) {
			auto file = string(luaL_checkstring(L, 1));
			auto what = luaL_optinteger(L, 2, 0);
			switch (what) {
			case 0:
				if (!FileExists(file)) { QCol->Error("Request to load " + file + " as a string failed. File not found!"); return 0; }
				lua_pushstring(L, LoadString(file).c_str());
				return 1;
			case 1:
				if (!JAS->EntryExists(file)) { QCol->Error("Request to load " + file + " (from SuperTed's resource) as a string failed. Entry not found!"); return 0; }
				lua_pushstring(L, JAS->String(file).c_str());
				return 1;
			case 2:
				if (!JTEX.EntryExists(file)) { QCol->Error("Request to load " + file + " (from Texture resource) as a string failed. Entry not found!"); return 0; }
				lua_pushstring(L, JTEX.String(file).c_str());
				return 1;
			default:
				QCol->Error("Request to load " + file + " as a string failed. The what-colde (" + to_string(what) + ") is unknown!");
				return 0;
			}
		}

		static int InstantCrash(lua_State* L) {
			Throw(string(luaL_optstring(L, 1, "Unkown error")), "", ThrowAct::InstantEnd);
			return 1;
		}

		static void Reg(string fname, lua_CFunction cfun) {
			Functions[fname] = cfun;
		}

		static void Reg(bool force=false) {
			static bool first{ true };
			if (first || force) {
				QCol->Doing("Registering", "Script APIs");
				Functions.clear();
				Reg("Slyvina", Slyvina);
				Reg("LoadString", SLoadString);
				Reg("InstantCrash", InstantCrash);
				first = false;
			}
		}

		void InitState(lua_State* L,std::string ScriptFile) {
			Reg();
			QCol->Doing("Linking", "Script APIs");
			string NeilGroup{ "group SuperTed\n" };
			auto LuaGroup{ "SuperTed = Neil.Globals.SuperTed" };
			string LuaDispose{ "-- Let's get red of everything!\n" };
			for (auto f : Functions) {
				NeilGroup += TrSPrintF("\tReadOnly Const %s = INITSUPERTED_%s\n",f.first.c_str(), f.first.c_str());
				LuaDispose += TrSPrintF("INITSUPERTED_%s = nil", f.first.c_str());
				lua_register(L, TrSPrintF("INITSUPERTED_%s",f.first.c_str()).c_str(), f.second); // Ugly, but works?
			}
			NeilGroup += "\n";

			QCol->Doing("Compiling", "Neil");
			string Neil{ JAS->String("Neil/Neil.lua") };
			luaL_loadstring(L, Neil.c_str()); lua_call(L, 0, 0);
			QCol->Doing("Compiling", "Neil Group");
			luaL_loadstring(L, TrSPrintF("Neil.Globals.LoadNeil([[%s]],\"SuperTed Group\")()", NeilGroup.c_str()).c_str()); lua_call(L, 0, 0);
			QCol->Doing("Compiling", "LuaLink");
			luaL_loadstring(L, LuaGroup); lua_call(L, 0, 0);
			QCol->Doing("Compiling", "Disposals");
			luaL_loadstring(L, LuaDispose.c_str());
			QCol->Doing("Compiling", ScriptFile);
			auto src = TrSPrintF("-- %s\n\nlocal u,e = Neil.Use(\"%s\")\n", ScriptFile.c_str(), ScriptFile.c_str());
			src += "if not u then SuperTed.InstantCrash(e) end";
			luaL_loadstring(L, src.c_str(); lua_call(L, 0, 0);
		}

	}
}