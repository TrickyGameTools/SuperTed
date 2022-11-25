// Lic:
// SuperTed - Editor
// Main
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


#include <Platform.hpp>
#include <QCol.hpp>
#include <QuickString.hpp>
#include <QuickStream.hpp>

#include "../SupJCR/SupJCR.hpp"
#include "builddate.hpp"
#include "Globals.hpp"

using namespace TrickyUnits;
using namespace SuperTed;
using namespace SuperTed::Editor;

void CLIParse(int argcount, char** args) {
	QCol->Doing("Parsing", "CLI arguments");
	FlagConfig C;
	AddFlag(C, "dc", false);
	CLIOptions = ParseArg(argcount, args, C);
	if (CLIOptions.arguments.size() < 3) { QCol->Error("I need a project and a map!"); exit(1); }
	EdtProject = CLIOptions.arguments[1];
	EdtMap = CLIOptions.arguments[2];
	if (CLIOptions.bool_flags["dc"]) {
		for (char ch = -125; ch < 127; ch++) {
			string rc = ""; rc += ch;
			EdtProject = TReplace(EdtProject, TrSPrintF(":%d:", (int)ch), rc);
			EdtMap = TReplace(EdtMap, TrSPrintF(":%d:", (int)ch), rc);
		}
	}
	QCol->Doing("Project", EdtProject);
	QCol->Doing("Map", EdtMap);
}

int main(int argcount, char** args) {
	MyDir = TReplace(ExtractDir(args[0]), '\\', '/');
	// Start
	QCol->LGreen("SuperTed - Editor\n");
	QCol->Magenta("(c) 2022 Jeroen P. Broks - Released under the terms of the GPL3\n\n");
	QCol->Doing("Coded by", "Jeroen P. Broks");
	QCol->Doing("Build", SuperTed_BuildDate);
	QCol->Doing("Platform", Platform());
	QCol->Doing("PlatformX", Platform(false));
	QCol->Doing("SuperTed Dir", MyDir);
	QCol->Doing("Called from", TReplace(CurrentDir(), '\\', '/'));
	QCol->Doing("Project Dir", ProjectsDir());
	JAS = SuperTed::JCR6::STED_Assets(MyDir);
	CLIParse(argcount, args);
	QCol->Doing("Initizing", "SDL2 and TQSG");
	return 0;
}