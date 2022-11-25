// Lic:
// SuperTed - Editor
// Globals
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

#include "Globals.hpp"
#include <Dirry.hpp>
#include <QCol.hpp>
using namespace TrickyUnits;

namespace SuperTed {
	namespace Editor {
		std::string MyDir{"."};
		jcr6::JT_Dir* JAS;
		GINIE GlobalConfig;
		TrickyUnits::ParsedArg CLIOptions;
		std::string EdtProject{ "" };
		std::string EdtMap{ "" };


		static void LoadGlobalConfig(bool force=false) {
			static bool first{ true };
			static auto ginie_file{ Dirry("$AppSupport$/SuperTed.ini") };
			if (first || force) {
				QCol->Doing("Reading", ginie_file);
				GlobalConfig.FromFile(ginie_file);
				GlobalConfig.AutoSave = ginie_file;
				first = false;
			}
		}

		std::string ProjectsDir() {
			LoadGlobalConfig();
			auto ret = GlobalConfig.Value("Directories", "Projects");
			if (!ret.size()) { QCol->Error("No project direcoty set"); exit(1); }
			return ret;
		}

		std::string EdtProjectDir() { return ProjectsDir() + "/" + EdtProject; }
		std::string EdtProjectIni() { return EdtProjectDir() + "/" + EdtProject + ".ini"; }
		std::string EdtProjectScript() { return EdtProjectDir() + "/" + EdtProject + ".neil"; }

	}
}