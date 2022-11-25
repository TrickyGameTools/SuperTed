// Lic:
// SuperTed - Editor
// Globals (header)
// 
// 
// 
// (c) Jeroen P. Broks, 2022
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
// Version: 22.11.25
// EndLic

#pragma once

#include <string>
#include <lua.hpp>
#include <jcr6_core.hpp>
#include <GINIE.hpp>
#include <QuickString.hpp>
#include <ArgParse.h>

namespace SuperTed {
	namespace Editor {
		extern std::string MyDir;
		extern jcr6::JT_Dir* JAS;
		extern jcr6::JT_Dir JTEX;
		extern TrickyUnits::GINIE GlobalConfig;
		extern TrickyUnits::GINIE ProjectConfig;
		extern TrickyUnits::ParsedArg CLIOptions;
		extern std::string EdtProject;
		extern std::string EdtMap;
		extern lua_State* Script{;
		

		std::string ProjectsDir();
		std::string EdtProjectDir();
		std::string EdtProjectIni();
		
		std::string TextureDir();

		inline bool cBool(TrickyUnits::GINIE& C, std::string k, std::string v) { return TrickyUnits::Upper(C.Value(k, v)) == "TRUE"; }

		enum class ThrowAct{None,InstantEnd,LogForRun};
		extern ThrowAct PanicAct;
		void Throw(std::string message, std::string TraceBack = "", ThrowAct A = ThrowAct::LogForRun);
		bool GetThrown();

	}
}