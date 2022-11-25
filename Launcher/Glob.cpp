// Lic:
// SuperTed Launcher
// Globs
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
// Version: 22.11.24
// EndLic

#include <string>


#include <GINIE.hpp>
#include <Dirry.hpp>
#include <QCol.hpp>
#include <Ask.hpp>
#include <QuickString.hpp>
#include <Platform.hpp>

#include "Glob.hpp"

using namespace std;
using namespace TrickyUnits;

namespace SuperTed {
	namespace Launcher {
		std::string MyDir{ "." };
		jcr6::JT_Dir* JAS{ nullptr };

		static const char* gfile = "$AppSupport$/SuperTed.ini";
		static GINIE _globalconfig{};
		//static map<string, string> AltMountRegister;

		static void LoadGlobalConfig(bool force = false) {
			static bool Loaded{ false };
			if ((!Loaded) || force) {
				QCol->Doing("Reading", Dirry(gfile));
				_globalconfig.FromFile(Dirry(gfile));
				_globalconfig.AutoSave = Dirry(gfile);
				Loaded = true;
			}
		}

		static std::string AltMount(std::string file) {
			auto sfile = Split(file, ':');
			if (sfile.size() <= 1) return file;
			if (Platform() == "Windows" && sfile[0].size() == 1) return file;
			if (!sfile[0].size() || sfile.size()>2) { QCol->Error("AltMount: Invalid file " + file); return file; }
			auto drv{ Upper(sfile[0]) };
			auto path{ sfile[1] };
			auto mdrv{ Ask(&_globalconfig,"Mount",drv,TrSPrintF("In file \"%s\" the unrecognized drive '%s' appears to be requested.\nI can mount a directory to that.\nPlease give me one: ",file.c_str(),drv.c_str()))};
			return mdrv + "/" + path;
		}

		

		std::string ProjectsDir() {
			LoadGlobalConfig();
			auto ret = Ask(&_globalconfig,"Directories","Projects","I need to know in which directory you wish you store your projects",Dirry("$Home$/.SuperTed"));
			ret = AltMount(ret);
			return ret;
		}

		int WW() {
			return AskInt(&_globalconfig, "Launcher", "Width", "Preffered Window width for launcher:", 1200);
		}
		int WH() {
			return AskInt(&_globalconfig, "Launcher", "Height", "Preffered Window Height for launcher:", 800);
		}

	}
}