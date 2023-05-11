// Lic:
// SuperTed - Editor
// Script APIs
// 
// 
// 
// (c) Jeroen P. Broks, 2022, 2023
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Please note that some references to data like pictures or audio, do not automatically
// fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 23.05.11
// EndLic
 
 
// A quick .hpp file that makes everything 'extern "C"' and includes all needed headers. Not part of Lua itself, I made it myself.
#include <lua.hpp>


#include <map>
#include <string>

#include <SlyvQCol.hpp>
#include <SlyvString.hpp>
#include <SlyvStream.hpp>

#include "Globals.hpp"

using namespace std;
using namespace Slyvina::Units;

namespace Slyvina {

	namespace SuperTed {
		namespace Editor {
			static map<string, lua_CFunction> Functions;

			static int Paniek(lua_State* L) {
				string TB{ "Data received:\n" };
				//string Trace{};
				TB += lua_gettop(L); TB += "\n";
				for (int i = 1; i <= lua_gettop(L); i++) {
					TB += TrSPrintF("Arg #%04d\t", i);
					switch (lua_type(L, i)) {
					case LUA_TSTRING:
						TB += "String \""; TB += luaL_checkstring(L, i);
						//Trace += luaL_checkstring(L, i); Trace += "\n";
						break;
					case LUA_TNUMBER:
						TB += TrSPrintF("Number %d", luaL_checknumber(L, i));
					case LUA_TFUNCTION:
						TB += "Function";
					default:
						TB += "Unknown: "; TB += to_string(lua_type(L, i));
						break;
					}
					TB += "\n";
				}
				Throw("Lua panicked!", TB, PanicAct);
				return 0;
			}

			// Merely an Easter Egg, whcih was also meant to test if the function linkups work!
			static int Slyvina(lua_State* L) {
				QCol->Yellow("You can trust on Slyvina to catch anybody idiotic enough to murder a Befindo!\n");
				return 0;
			}

			static int SLoadString(lua_State* L) {
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
				return 0;
			}

			static int NonFatalError(lua_State* L) {
				QCol->Error(luaL_optstring(L, 1, "An error occurred. No further info known"));
				return 0;
			}

			static int Doing(lua_State* L) {
				QCol->Doing(luaL_checkstring(L, 1), luaL_checkstring(L, 2), luaL_optstring(L, 3, "\n"));
				return 0;
			}

			static void Reg(string fname, lua_CFunction cfun) {
				Functions[fname] = cfun;
			}

			static void Reg(bool force = false) {
				static bool first{ true };
				if (first || force) {
					QCol->Doing("Registering", "Script APIs");
					Functions.clear();
					Reg("Slyvina", Slyvina);
					Reg("LoadString", SLoadString);
					Reg("InstantCrash", InstantCrash);
					Reg("NonFatalError", NonFatalError);
					Reg("Doing", Doing);
					first = false;
				}
			}

			void InitState(lua_State* L, std::string ScriptFile) {
				Reg();
				QCol->Doing("Linking", "Script APIs");
				luaL_openlibs(L);
				lua_atpanic(L, Paniek);
				string NeilGroup{ "group SuperTed\n" };
				auto LuaGroup{ "SuperTed = Neil.Globals.SuperTed" };
				string LuaDispose{ "-- Let's get red of everything!\n" };
				for (auto f : Functions) {
					NeilGroup += TrSPrintF("\tReadOnly Var %s = Lua.INITSUPERTED_%s\n", f.first.c_str(), f.first.c_str());
					LuaDispose += TrSPrintF("INITSUPERTED_%s = nil", f.first.c_str());
					lua_register(L, TrSPrintF("INITSUPERTED_%s", f.first.c_str()).c_str(), f.second); // Ugly, but works?
				}
				NeilGroup += "\nEnd\n\n";

				QCol->Doing("Compiling", "Neil");
				string Neil{ "-- Neil --\nlocal function loadneil()\n" + JAS->String("Neil/Neil.lua") + "\n\nend Neil=loadneil()" };
				luaL_loadstring(L, Neil.c_str()); lua_call(L, 0, 0);
				QCol->Doing("Compiling", "Neil Group");
				//cout << "<DEBUG>\n" << NeilGroup << "\n</DEBUG>\n\n"; // DEBUG ONLY!
				luaL_loadstring(L, TrSPrintF("Neil.Globals.NeilLoadString([[%s]],\"SuperTed Group\")()", NeilGroup.c_str()).c_str()); lua_call(L, 0, 0);
				QCol->Doing("Compiling", "LuaLink");
				luaL_loadstring(L, LuaGroup); lua_call(L, 0, 0);
				QCol->Doing("Compiling", "Disposals");
				luaL_loadstring(L, LuaDispose.c_str());
				QCol->Doing("Compiling", ScriptFile);
				if (ExtractExt(Lower(ScriptFile)) == "neil") {
					auto src = TrSPrintF("-- %s\n\nlocal u,e = Neil.Use(\"%s\")\n", ScriptFile.c_str(), StripExt(ScriptFile).c_str());
					src += "if not u then SuperTed.InstantCrash(e) end";
					luaL_loadstring(L, src.c_str()); lua_call(L, 0, 0);
				} else if (ExtractExt(Lower(ScriptFile)) == "lua") {
					if (!FileExists(ScriptFile)) { Throw("Script file not found", ScriptFile, ThrowAct::InstantEnd); return; }
					auto src = LoadString(ScriptFile);
					src = "-- " + ScriptFile + "\n\n" + src + "\n\nif not SuperTedCallBack  then SuperTed.Crash(\"No SuperTed group present\") end\n\n";
					luaL_loadstring(L, src.c_str());
					lua_call(L, 0, 0);
				} else if (ScriptFile == "*NOSCRIPT*") {
					string src{ "-- NoScript --\n\n" };
					src += JAS->String("Script/NoScript.lua");
					luaL_loadstring(L, src.c_str());
					lua_call(L, 0, 0);
				} else {
					Throw("I really do not know what kind of script you want me to load", ScriptFile, ThrowAct::InstantEnd);
				}
			}


			void LoadScript(std::string ScriptFile) {
				Script = luaL_newstate();
				InitState(Script, ScriptFile);
			}
			void LoadScript() {
				if (FileExists(EdtProjectNeil()))
					LoadScript(EdtProjectNeil());
				else if (FileExists(EdtProjectLua()))
					LoadScript(EdtProjectLua());
				else
					LoadScript("*NOSCRIPT*");
			}

		}
	}
}