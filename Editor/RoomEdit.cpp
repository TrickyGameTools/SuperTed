// Lic:
// SuperTed - Editor
// Rooms
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

#include <SlyvAsk.hpp>
#include <SlyvQCol.hpp>
#include <SlyvString.hpp>

#include <june19.hpp>

#include <SuperTed_Core.hpp>

#include "Algemeen.hpp"
#include "Globals.hpp"

using namespace std;
using namespace Slyvina;
using namespace june19;
using namespace Units;

namespace Slyvina {
	namespace SuperTed {
		namespace Editor {

			enum class REStatus { NewRoom, RenameRoom };

			static UI REStage{ nullptr };
			inline j19gadget* REG() { return REStage->MainGadget; }
			static j19gadget* RERoomName{ nullptr };
			static j19gadget* REOK{ nullptr };
			static j19gadget* RECancel{ nullptr };
			static j19gadget* REAlreadyThere{ nullptr };


			static REStatus Status;
			inline string RoomName() { return Trim(Upper(RERoomName->Text)); }

			static void Cycle() {
				RECancel->X(REOK->X() + REOK->W());
			}

			static bool Allowed() {
				bool ret{ true };
				if (TheMap->Rooms.count(RoomName())) {
					ret = false;
					REAlreadyThere->Visible = true;
				} else {
					REAlreadyThere->Visible = false;
				}
				ret = ret && RoomName().size();
				REOK->Enabled = ret;
				return ret;
			}

			static void Ok_Action(j19gadget*, j19action) {
				if (!Allowed()) return; // extra safeguard
				switch (Status) {
				case REStatus::NewRoom:
					TheMap->CreateRoom(
						RoomName(),
						AskInt(&ProjectConfig, "Default Map Format", "Width", "Default width in tiles:", 64),
						AskInt(&ProjectConfig, "Default Map Format", "Height", "Default height in tiles:", 64),
						AskInt(&ProjectConfig, "Grid", "Width", "Default Tile Width:", 32),
						AskInt(&ProjectConfig, "Grid", "Height", "Default Tile Height:", 32)
					);
					break;
				case REStatus::RenameRoom:
					TheMap->RenameRoom(CurrentRoom(), RoomName());
					break;
				default:
					QCol->Error("Request kind for room editing which is unknown! (" + to_string((int)Status) + ")");
					break;
				}
				RenewRooms();
				_UI::GoToStage("Map");
			}

			static void Cancel_Action(j19gadget*, j19action) { _UI::GoToStage("Map"); }

			static void TypeRoomName(j19gadget* s, j19action a) {
				if (Allowed() && a == j19action::Enter) Ok_Action(s, a);
			}


			static void CreateRoomStage() {
				_UI::AddStage("RoomEdit");
				REStage = _UI::GetStage("RoomEdit");
				REG()->BR = 25;
				REG()->BG = 18;
				REG()->BB = 0;

				RERoomName = CreateTextfield(20, REG()->H() / 2, REG()->W() - 40, REG());
				RERoomName->BR = 12;
				RERoomName->BG = 9;
				RERoomName->BB = 0;
				RERoomName->FR = 255;
				RERoomName->FG = 180;
				RERoomName->FB = 0;

				REAlreadyThere = CreateLabel("A room with that label already exists", 20, RERoomName->Y() - 40, RERoomName->W(), 30, REG());
				REAlreadyThere->FR = 255;
				REAlreadyThere->FG = 0;
				REAlreadyThere->FB = 0;
				REAlreadyThere->BR = 25;
				REAlreadyThere->BG = 0;
				REAlreadyThere->BB = 0;


				REOK = CreateButton("Okay", 20, RERoomName->Y() + 80, REG());
				REOK->FR = 0;
				REOK->FG = 255;
				REOK->FB = 0;
				REOK->BR = 0;
				REOK->BG = 25;
				REOK->BB = 0;

				RECancel = CreateButton("Cancel", 0, REOK->Y(), REG());
				RECancel->FR = 255;
				RECancel->FG = 0;
				RECancel->FB = 0;
				RECancel->BR = 25;
				RECancel->BG = 0;
				RECancel->BB = 0;


				RERoomName->CBAction = TypeRoomName;
				REOK->CBAction = Ok_Action;
				RECancel->CBAction = Cancel_Action;
				REStage->PreJune = Cycle;

				Allowed();
			}

			void NewRoom(june19::j19gadget*, june19::j19action) {
				if (!REStage) CreateRoomStage();
				_UI::GoToStage("RoomEdit");
				j19gadget::StatusText("Create a new room");
				RERoomName->Text = "";
				Allowed();
				Status = REStatus::NewRoom;
			}

			void RenameRoom(june19::j19gadget*, june19::j19action) {
				if (!REStage) CreateRoomStage();
				_UI::GoToStage("RoomEdit");
				j19gadget::StatusText("Rename room: " + CurrentRoom());
				RERoomName->Text = CurrentRoom();
				Allowed();
				Status = REStatus::RenameRoom;
			}
		}
	}
}