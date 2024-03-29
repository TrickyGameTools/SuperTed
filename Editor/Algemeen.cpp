// Lic:
// SuperTed - Editor
// User Interface - General
// 
// 
// 
// (c) Jeroen P. Broks, 2021, 2022, 2023
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
// Version: 23.08.03
// EndLic

#include <SlyvQCol.hpp>

#include <TQSG.hpp>

#include <june19.hpp>

#include "Algemeen.hpp"
#include "Opslaan.hpp"
#include "Globals.hpp"

using namespace Slyvina;
using namespace June19;
using namespace Units;
using namespace TQSG;
using namespace TQSE;

namespace Slyvina {
	namespace SuperTed {
		namespace Editor {
			bool _UI::_initialized{ false };
			TImage _UI::Mouse{ nullptr };
			UI _UI::_Current{ nullptr };
			std::map<std::string, std::shared_ptr<_UI>> _UI::Stage{};




			_UI::_UI(std::string name) {
				_Name = name;
			}
			_UI::_UI() {}
			void _UI::Crash(std::string m) {
				Throw(m);
			}

			void _UI::AddStage(std::string st) {
				st = Upper(st);
				if (Stage.count(st)) Crash("Dupe stage: " + st);
				Stage[st] = std::make_shared<_UI>();
				Stage[st]->_Name = st;
				Stage[st]->MainGadget = CreateGroup(0, 0, ScreenWidth(), ScreenHeight() - 36, WorkScreen());
			}

			UI _UI::GetStage(std::string st) {
				st = Upper(st);
				if (!Stage.count(st)) Crash("Non-existent stage: " + st);
				return Stage[st];
			}

			UI _UI::CurrentStage() {
				return _Current;
			}

			void _UI::GoToStage(std::string st) {
				st = Upper(st);
				if (!Stage.count(st)) Crash("Non-existent stage: " + st);
				_Current = Stage[st];
				for (auto si : Stage) {
					si.second->MainGadget->Visible = si.first == st;
				}
			}

			static void JPanic(std::string msg) { Throw(msg); }

			static bool go_on{ true };
			static void dquit(j19gadget* g, j19action a) { go_on = false; }

			void _UI::Start() {


				// JCR6
				//auto J{ JAS };

				JCR6::JCR6PANIC = JPanic;


				// Interface load
				_initialized = true;
				QCol->Doing("Staring", "User Interface");

				// Mouse
				//TQSE_Init();
				HideMouse();
				JCR6::JT_Dir JD{ JAS };
				Mouse = LoadImage(JD, std::string("Img/Mouse.png"));

				// SuperTed
				// TODO: Init SuperTed and the TQSG driver


				// Main screen config
				AdeptStatus();
				j19gadget::SetDefaultFont(JD, "Fonts/DOS.jfbf");				
				auto Scr = WorkScreen();
				Scr->BR = 25;
				Scr->BG = 18;
				Scr->BB = 0;
				Scr->BA = 255;
				Scr->FR = 255;
				Scr->FG = 180;
				Scr->FB = 0;
				Scr->FA = 255;
				auto
					MenuFile = Scr->AddMenu("File"),
					GridMenu = Scr->AddMenu("Grid"),
					RoomMenu = Scr->AddMenu("Rooms"),
					ScrollMenu = Scr->AddMenu("Scroll"),
					OptionsMenu = Scr->AddMenu("Options"),
					DebugMenu = Scr->AddMenu("Debug");
				MenuFile->AddItem("Save", MenuSave, SDLK_s);
				MenuFile->AddStrike();
				MenuFile->AddItem("Edit Meta Data", GoMeta, SDLK_m);
				MenuFile->AddStrike();
				MenuFile->AddItem("Quit", dquit, SDLK_q);
				//std::cout << "GridMenu=" << (unsigned long long)GridMenu << "\t"<<GridMenu->Caption<<std::endl;
				//GridMenu->AddItem("Toggle Grid", ToggleUseGrid, SDLK_g);
				GridMenu->AddItem("Toggle GridShow", ToggleShowGrid, SDLK_d);
				RoomMenu->AddItem("New Room", NewRoom, SDLK_n);
				RoomMenu->AddItem("Rename Room", RenameRoom, SDLK_KP_5);
				ScrollMenu->AddItem("Down", ScrollDn, SDLK_DOWN);
				ScrollMenu->AddItem("Left", ScrollLe, SDLK_LEFT);
				ScrollMenu->AddItem("Up", ScrollUp, SDLK_UP);
				ScrollMenu->AddItem("Right", ScrollRi, SDLK_RIGHT);
				//DebugMenu->AddItem("View BlockMap", ShowBlockMap, SDLK_b);
				//DebugMenu->AddItem("Tag Overview", ShowTags, SDLK_z);
				//DebugMenu->AddItem("PNG to JPBF", PNG2JPBF, SDLK_F12);
				//DebugMenu->AddItem("Remove \"Rotten\" Objects", RemoveRottenObjects, SDLK_INSERT);
				//DebugMenu->AddItem("Pic Remove", RemovePicObjects, SDLK_F9);
				DebugMenu->AddItem("Optimize To Origin", OptimizeToOrigin, SDLK_F2);
				//DebugMenu->AddItem("Next Object", NextObject, SDLK_TAB);
				// Stages


				UI_MapStart(); // Must be last

			}

			bool _UI::Run() {
				auto CS{ CurrentStage() };
				//auto go_on{ true };
				go_on = true;
				Cls();
				if (!CS) { Throw("No stage"); return false; }
				if (CS->PreJune) CS->PreJune();
				//TQSE_Poll();
				Screen()->Draw();
				if (CS->PostJune) CS->PostJune();
				if (TQSE::AppTerminate()) go_on = false;
#ifdef QUICK_QUIT
				if (TQSE_KeyHit(SDLK_ESCAPE)) go_on = false;
#endif
				SetColor(255, 255, 255, 255);
				Mouse->Draw(MouseX(), MouseY());
				Flip(20);
				return go_on;
			}

			void _UI::Done() {
				if (!_initialized) return;
				_initialized = false;
				QCol->Doing("Closing", "User Interface");
				MenuSave(nullptr, j19action::Unknown);
				QCol->Doing("Closing", "June19");
				FreeJune19();
			}



		}
	}
}