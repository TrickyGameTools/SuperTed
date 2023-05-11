// Lic:
// SuperTed - Launcher
// Run
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

#define ESCAPE_IS_QUIT



#include <TQSE.hpp>
#include <TQSG.hpp>

#include <june19.hpp>

#include <SlyvQCol.hpp>
#include <SlyvMath.hpp>
#include <SlyvString.hpp>
#include <SlyvStream.hpp>
#include <SlyvDir.hpp>

#include "Glob.hpp"
#include "Run.hpp"


namespace Slyvina {
	using namespace June19;
	using namespace Units;
	using namespace TQSG;
	using namespace TQSE;

	namespace SuperTed {
		namespace Launcher {
			static TImage Background{ nullptr };
			static TImage Bear{ nullptr };

			static bool ErrorThrown{ false };
			static void Err(std::string e) {
				ErrorThrown = true;
				QCol->Error(e);
			}

			static j19gadget* UI{ nullptr };
			static j19gadget* selPrj{ nullptr };
			static j19gadget* haveprj{ nullptr };
			static j19gadget* selMap{ nullptr };
			static j19gadget* newMap{ nullptr };
			static j19gadget* newMapAct{ nullptr };
			static j19gadget* edtMapButton{ nullptr };

			static int Exe(std::string cmd) {
				QCol->Doing("Executing", cmd);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
				cmd = ChReplace(cmd, '/', '\\'); // Guarantee cmd support in Windows!
#endif
				Cls();
				SetColor(255, 0, 0);
				j19gadget::GetDefaultFont()->Text("External call in session...", floor(ScreenWidth() / 2), floor(ScreenHeight() / 2), 2, 2);
				Flip();
				auto ret = system(cmd.c_str());
				return ret;
			}

			static std::string ChosenProject() {
				return selPrj->ItemText();
			}

			static void PCheck(j19gadget* j, j19action a);
			static void ActRenewProjectList(j19gadget* j, j19action a) {
				auto List = FileList(ProjectsDir(), Directories);
				selPrj->ClearItems();
				for (auto F : List) selPrj->AddItem(F);
				PCheck(j, a);
			}

			static void ActNewProject(j19gadget* j, j19action a) {
				std::string cmd{ MyDir + "/NewProjectWizard.exe" };
				if (!FileExists(cmd)) {
					Notify("The wizard (" + cmd + ") could not be found");
					return;
				}
				Exe(cmd);
				ActRenewProjectList(j, a);
			}

			static void PCheck(j19gadget* j, j19action a) {
				if (!haveprj) return;
				haveprj->Visible = selPrj->ItemText() != "";
			}


			static void StartEditor(std::string Project, std::string Map) {
				// This looks needlessly complicated, and frankly, it is....
				// However in Windows, execution commands are mostly taken over cmd 
				// which is quite primitive and will me quite a load of errors, so this way I hope to prevent that
				static std::string Editor = TrSPrintF("%s/SuperTed_Editor.exe", MyDir.c_str());
				if (!FileExists(Editor)) { Err("Editor (" + Editor + ") not found"); return; }
				std::string pP{ "" };
				std::string pM{ "" };
				for (size_t i = 0; i < Project.size(); i++) pP += TrSPrintF(":%d:", (int)Project[i]);
				for (size_t i = 0; i < Map.size(); i++) pM += TrSPrintF(":%d:", (int)Map[i]);
				Exe(Editor + " -dc " + pP + " " + pM);
			}

			static void ActNew(j19gadget*, j19action a) {
				//QCol->Error("Action for new map not yet implemented");
				if (newMap->Text.size()) StartEditor(ChosenProject(), newMap->Text);
			}

			static void TypeNew(j19gadget* j, j19action a) {
				switch (a) {
				case j19action::Activate:
					break;
				case j19action::Enter:
					ActNew(j, a);
					break;
				case j19action::Type:
				case j19action::BackSpace:
					newMapAct->Enabled = newMap->Text.size();
					break;
				default:
					QCol->Error("Unknown action on NewMapName Gadget (" + std::to_string((int)a) + ")");
					QCol->Pink("Please notify Jeroen P. Broks about this, as you likely encountered a bug!");
					break;
				}
			}

			void CreateUI() {
				QCol->Doing("Configuring", "Font");
				j19gadget::SetDefaultFont(JAS, "Fonts/DOS.jfbf");
				QCol->Doing("Creating", "User Interface");
				UI = WorkScreen();
				auto h = UI->H(), dh = (int)ceil(h * 0.25);
				auto w = UI->W(), dw = w - 250;

				//auto bear = CreatePicture(w - 260, 100, 260, 300, UI, Pic_FullStretch);
				//bear->Image(*JAS, "IMG/TeddyBear");

				auto labPrj{ CreateLabel("Projects",0,0,40,20,UI) };
				labPrj->FR = 255; labPrj->FG = 180; labPrj->FB = 0; labPrj->FA = 255;
				selPrj = CreateListBox(5, 20, dw - 10, dh - 20, UI);
				selPrj->FR = 255; selPrj->FG = 180; selPrj->FB = 0; selPrj->FA = 255;
				selPrj->BR = 25; selPrj->BG = 18; selPrj->BB = 0; selPrj->BA = 255;
				selPrj->CBAction = PCheck;
				auto newPrj = CreateButton("New Project", dw, 5, UI);
				newPrj->CBAction = ActNewProject;
				newPrj->FR = 255; newPrj->FG = 180; newPrj->FB = 0; newPrj->FA = 255;
				newPrj->BR = 25; newPrj->BG = 18; newPrj->BB = 0; newPrj->BA = 255;
				auto renPrj = CreateButton("Renew Project List", dw, 30, UI);
				renPrj->FR = 255; renPrj->FG = 180; renPrj->FB = 0; renPrj->FA = 255;
				renPrj->BR = 25; renPrj->BG = 18; renPrj->BB = 0; renPrj->BA = 255;

				renPrj->CBAction = ActRenewProjectList;
				ActRenewProjectList(nullptr, j19action::Unknown);

				haveprj = CreateGroup(0, dh, dw, UI->H() - dh, UI);
				haveprj->Visible = false;
				auto labMap{ CreateLabel("Maps",0,0,40,20,haveprj,0) };
				labMap->FR = 0; labMap->FG = 180; labMap->FB = 0; labMap->FA = 255;
				selMap = CreateListBox(5, 20, dw - 10, haveprj->H() - 60, haveprj);
				selMap->FR = 0; selMap->FG = 180; selMap->FB = 255; selMap->FA = 255;
				selMap->BR = 0; selMap->BG = 18; selMap->BB = 25; selMap->BA = 255;
				edtMapButton = CreateButton("Edit chosen map", dw, 0, haveprj);
				edtMapButton->FR = 0; edtMapButton->FG = 180; edtMapButton->FB = 255; edtMapButton->FA = 255;
				edtMapButton->BR = 0; edtMapButton->BG = 18; edtMapButton->BB = 25; edtMapButton->BA = 255;
				edtMapButton->Enabled = false;

				auto newy{ haveprj->H() - 30 };
				newMap = CreateTextfield(5, newy, dw - 10, haveprj);
				newMap->FB = 0; newMap->FR = 180; newMap->FG = 255; newMap->FA = 255;
				newMap->BB = 0; newMap->BR = 18; newMap->BG = 25; newMap->BA = 255;
				newMap->CBAction = TypeNew;
				newMapAct = CreateButton("Create new map", dw, newy, haveprj);
				newMapAct->FB = 0; newMapAct->FR = 180; newMapAct->FG = 255; newMapAct->FA = 255;
				newMapAct->BB = 0; newMapAct->BR = 18; newMapAct->BG = 25; newMapAct->BA = 255;
				newMapAct->Enabled = false;
				newMapAct->CBAction = ActNew;

			}

			void LoadBackGround() {
				QCol->Doing("Loading", "Background image");
				Background = LoadImage(JAS, "IMG/Background.png");
				auto a{ JCR6::Last()->ErrorMessage };
				std::string b{ SDL_GetError() };
				if (a != "" && Upper(Trim(a)) != "OK") Err("JCR6 ERROR>" + a);
				if (b != "") Err("SDL2 ERROR>" + b);
				if (!Background->Frames()) Err("For some reasons beyond me, the background didn't load");
				Bear = LoadImage(JAS, "IMG/Teddybear.png");
			}

			bool Run() {
				if (ErrorThrown) return false;
				bool KeepGoing{ true };
				static int r{ 0 };
				Poll();
				Cls();
				SetColor(255, 255, 255);
				Background->Tile(0, 0, ScreenWidth(), ScreenHeight(), floor((ScreenWidth() / 2) + (DegSin(r) * 250)), floor((ScreenHeight() / 2) + (DegCos(r) * 250)));
				r = (r + 1) % 360;
				Bear->Draw(ScreenWidth() - Bear->W(), ceil(ScreenHeight() / 2) - ceil(Bear->H() / 2));
				UI->Draw();
				KeepGoing = KeepGoing && !TQSE_Quit();
#ifdef ESCAPE_IS_QUIT
				KeepGoing = KeepGoing && !TQSE_KeyHit(SDLK_ESCAPE);
#endif
				Flip(30);
				if (!KeepGoing) FreeGadget(UI);
				return KeepGoing;
			}
		}
	}
}