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
// Version: 22.11.27
// EndLic


#include <Ask.hpp>
#include <Platform.hpp>
#include <QCol.hpp>
#include <QuickString.hpp>
#include <QuickStream.hpp>

#include <TQSG.hpp>
#include <TQSE.hpp>

#include <jcr6_realdir.hpp>

#include <SuperTed_Core.hpp>
#include <SuperTed_Draw_TQSG.hpp>

#include "../SupJCR/SupJCR.hpp"
#include "builddate.hpp"
#include "Globals.hpp"
#include "Textures.hpp"
#include "Script_API.hpp"
#include "Run.hpp"

#include "Algemeen.hpp"

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

	QCol->Doing("Reading", EdtProjectIni());
	if (!FileExists(EdtProjectIni())) { QCol->Error("File not found"); exit(2); }
	ProjectConfig.FromFile(EdtProjectIni());
	ProjectConfig.AutoSave = EdtProjectIni();
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
	jcr6::InitRealDir(); ScanForTextures();
	LoadScript();
	// Start TQSG (always after all non-SDL2 things are done).
	QCol->Doing("Initizing", "SDL2 and TQSG");
	TQSG_Init(
		TrSPrintF("SuperTed - %s - %s", EdtProject.c_str(), EdtMap.c_str()),
		AskInt(&ProjectConfig, "Editor", "Width", "Preferred window width for editor: ", 1600),
		AskInt(&ProjectConfig, "Editor", "Height", "Preferred window height for editor: ", 900),
		false);
	TQSG_Cls();
	TQSG_Flip();
	QCol->Doing("Initizing", "TQSE");	
	TQSE_Init();
	QCol->Doing("Initizing", "SuperTed TQSG Driver");
	SuperTed_InitTQSG(&JTEX);


	// Run
	UI::Start();
	do {} while (Run());
	UI::Done();

	// Close
	if (Script != NULL) { QCol->Doing("Disposing", "Script");  lua_close(Script); }
	return 0;
}