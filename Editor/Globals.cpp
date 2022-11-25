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
#include <QuickString.hpp>
#include <Ask.hpp>
using namespace TrickyUnits;

namespace SuperTed {
	namespace Editor {
		std::string MyDir{"."};
		jcr6::JT_Dir* JAS;
		jcr6::JT_Dir JTEX;
		GINIE GlobalConfig;
		GINIE ProjectConfig;
		ParsedArg CLIOptions;
		std::string EdtProject{ "" };
		std::string EdtMap{ "" };
		lua_State* Script;
		bool ErrorThrown;


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

		static string AltMount(string file) {
			auto spfile = Split(file, ':');
			if (spfile.size() <= 0) { QCol->Error("Invalid file (" + file + ")"); return "???"; }
			if (spfile.size() > 2) { QCol->Error("Illegal file name (" + file + "). I'll try to go on, but results may be funny"); }
			auto drv{ Upper(spfile[0]) };
			auto path{ spfile[1] };
			auto mdrv{ Ask(&GlobalConfig,"Mount",drv,TrSPrintF("In file \"%s\" the unrecognized drive '%s' appears to be requested.\nI can mount a directory to that.\nPlease give me one: ",file.c_str(),drv.c_str())) };
			return mdrv + "/" + path;

		}

		std::string ProjectsDir() {
			LoadGlobalConfig();
			auto ret = AltMount(GlobalConfig.Value("Directories", "Projects"));
			if (!ret.size()) { QCol->Error("No project directory set"); exit(1); }
			
			return ret;
		}

		std::string EdtProjectDir() { return ProjectsDir() + "/" + EdtProject; }
		std::string EdtProjectIni() { return EdtProjectDir() + "/" + EdtProject + ".ini"; }
		std::string TextureDir() { return TReplace(AltMount(ProjectConfig.Value("Directory", "Textures")),'\\','/'); }
		void Throw(std::string message, std::string TraceBack, ThrowAct A) {
			QCol->Error(message);
			if (TraceBack.size()) QCol->LCyan(TraceBack); 
			QCol->Reset(); std::cout << std::endl;
			switch (A) {
			case ThrowAct::InstantEnd:
				exit(255);
				break;
			case ThrowAct::LogForRun:
				ErrorThrown = true;
				break;
			default:
				break;
			}
		}
		bool GetThrown() { return ErrorThrown; }
		std::string EdtProjectScript() { return EdtProjectDir() + "/" + EdtProject + ".neil"; }

	}
}