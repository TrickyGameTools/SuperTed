// Lic:
// SuperTed - Editor
// Globals (header)
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

#pragma once

// C++
#include <string>

// Lua
#include <lua.hpp>

// JCR
#include <jcr6_core.hpp>

// SuperTed
#include <SuperTed_Core.hpp>

// Slyvina Units
#include <SlyvGINIE.hpp>
#include <SlyvString.hpp>
#include <SlyvArgParse.hpp>

namespace Slyvina {
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
			extern lua_State* Script;
			extern SuperTed::Teddy TheMap;


			std::string ProjectsDir();
			std::string EdtProjectDir();
			std::string EdtProjectIni();
			std::string EdtProjectLua();
			std::string EdtProjectNeil();
			std::string EdtProjectMapFile();

			std::string TextureDir();

			inline bool cBool(TrickyUnits::GINIE& C, std::string k, std::string v) { return TrickyUnits::Upper(C.Value(k, v)) == "TRUE"; }

			enum class ThrowAct { None, InstantEnd, LogForRun };
			extern ThrowAct PanicAct;
			void Throw(std::string message, std::string TraceBack = "", ThrowAct A = ThrowAct::LogForRun);
			bool GetThrown();


			void NFE(std::string message, std::string e = "ERROR"); // Non-Fatal Error
		}
	}
}