// Lic:
// SuperTed - Launcher
// Main
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

#include <SlyvQCol.hpp>
//#include <Platform.hpp>
#include <SlyvString.hpp>
#include <SlyvStream.hpp>
#include <TQSG.hpp>
#include <TQSE.hpp>


#include "builddate.hpp"
#include "Glob.hpp"
#include "Run.hpp"
#include "../SupJCR/SupJCR.hpp" 

using namespace Slyvina;
using namespace Units;
using namespace TQSG;
using namespace Slyvina::SuperTed::Launcher;
using namespace Slyvina::SuperTed::SupJCR6;

int main(int ac, char** arg) {
	MyDir = ChReplace(ExtractDir(arg[0]), '\\', '/');
	// Start
	QCol->LGreen("SuperTed - Launcher\n");
	QCol->Magenta("(c) 2022 Jeroen P. Broks - Released under the terms of the GPL3\n\n");
	QCol->Doing("Coded by", "Jeroen P. Broks");
	QCol->Doing("Build", SuperTed_BuildDate);
	QCol->Doing("Platform", Platform());
	QCol->Doing("PlatformX", Platform(false));
	QCol->Doing("SuperTed Dir", MyDir);
	QCol->Doing("Called from", ChReplace(CurrentDir(), '\\', '/'));
	QCol->Doing("Project Dir", ProjectsDir());
	JAS = STED_Assets(MyDir);
	QCol->Doing("Initizing", "SDL2 and TQSG"); 
	Graphics(
		WW(), WH(),
		"SuperTed - Launcher - (c) Jeroen P. Broks"
	); 
	Cls(); Flip();
	//QCol->Doing("Initizing", "TQSE"); TQSE_Init();
	LoadBackGround();
	CreateUI();
	QCol->Yellow("Let's get ready to rumble\n\n");
	// Run
	do {} while (Run());

	// End
	QCol->Doing("Closing", "SDL2 and TQSG");
	//TQSG_Close(); // No longer needed. Has been automated.
	QCol->Reset();
}