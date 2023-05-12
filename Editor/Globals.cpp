// Lic:
// SuperTed - Editor
// Globals
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
// Version: 23.05.12
// EndLic

#include <SDL.h>

#include "Globals.hpp"
#include <SlyvDirry.hpp>
#include <SlyvQCol.hpp>
#include <SlyvString.hpp>
#include <SlyvAsk.hpp>
#include <SlyvArgParse.hpp>

//using namespace TrickyUnits;
using namespace Slyvina;
using namespace Units;
using namespace JCR6;

namespace Slyvina {

	namespace SuperTed {
		namespace Editor {
			std::string MyDir{ "." };
			JT_Dir JAS;
			JT_Dir JTEX;
			GINIE GlobalConfig;
			GINIE ProjectConfig;
			ParsedArg CLIOptions;
			std::string EdtProject{ "" };
			std::string EdtMap{ "" };
			lua_State* Script{ nullptr };
			bool ErrorThrown;
			ThrowAct PanicAct{ ThrowAct::InstantEnd };
			SuperTed::Teddy TheMap{ nullptr };


			static void LoadGlobalConfig(bool force = false) {
				static bool first{ true };
				static auto ginie_file{ Dirry("$AppSupport$/SuperTed.ini") };
				if (first || force) {
					QCol->Doing("Reading", ginie_file);
					GlobalConfig = LoadGINIE(ginie_file);
					GlobalConfig->AutoSave = ginie_file;
					first = false;
				}
			}

			static string AltMount(string file) {
				auto spfile = Split(file, ':');
				if (spfile->size() <= 0) { QCol->Error("Invalid file (" + file + ")"); return "???"; }
				if (spfile->size() > 2) { QCol->Error("Illegal file name (" + file + "). I'll try to go on, but results may be funny"); }
				auto drv{ Upper((*spfile)[0]) };
				auto path{ (*spfile)[1] };
#ifdef SlyvWindows
				if (drv.size() == 1) return ChReplace(file,'\\','/'); // One letter will just mean a drive letter in Windows. On Unix based systems (such as BSD or Linux) this is not so automatic, so there this will be ignored.
#endif
				auto mdrv{ Ask(GlobalConfig,"Mount",drv,TrSPrintF("In file \"%s\" the unrecognized drive '%s' appears to be requested.\nI can mount a directory to that.\nPlease give me one: ",file.c_str(),drv.c_str())) };
				return ChReplace(mdrv + "/" + path, '\\', '/');

			}

			std::string ProjectsDir() {
				LoadGlobalConfig();
				auto ret = AltMount(GlobalConfig->Value("Directories", "Projects"));
				if (!ret.size()) { QCol->Error("No project directory set"); exit(1); }

				return ret;
			}

			std::string EdtProjectDir() { return ProjectsDir() + "/" + EdtProject; }
			std::string EdtProjectIni() { return EdtProjectDir() + "/" + EdtProject + ".ini"; }
			std::string EdtProjectNeil() { return EdtProjectDir() + "/" + EdtProject + ".neil"; }
			std::string EdtProjectMapFile() { LoadGlobalConfig(); return AltMount(ProjectConfig->Value("Directory", "Maps") + "/" + EdtMap); }
			std::string EdtProjectLua() { return EdtProjectDir() + "/" + EdtProject + ".lua"; }
			std::string TextureDir() { return ChReplace(AltMount(ProjectConfig->Value("Directory", "Textures")), '\\', '/'); }
			void Throw(std::string message, std::string TraceBack, ThrowAct A) {
				QCol->Error(message);
				if (TraceBack.size()) QCol->LCyan(TraceBack);
				QCol->Reset(); std::cout << std::endl;
				switch (A) {
				case ThrowAct::InstantEnd:
					if (Script != NULL) lua_close(Script);
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
			void NFE(std::string message, std::string head) {
				QCol->Error(message);
				SDL_ShowSimpleMessageBox(0, head.c_str(), message.c_str(), nullptr);
			}
			std::string EdtProjectScript() { return EdtProjectDir() + "/" + EdtProject + ".neil"; }



		}
	}
}