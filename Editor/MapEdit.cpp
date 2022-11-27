// Lic:
// SuperTed - Editor
// Map Editor
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
#include <QCol.hpp>

#include <june19.hpp>

#include "Globals.hpp"
#include "Algemeen.hpp"


using namespace june19;
using namespace TrickyUnits;

namespace SuperTed {
	namespace Editor {

#pragma region Variables
		bool ShowGrid{ true };

		static j19gadget* MapGroup;
		static j19gadget* RoomPanel{ nullptr };
		static j19gadget* RoomList{ nullptr };

		static j19gadget* DataPanel{ nullptr };
#pragma endregion

#pragma region CallBackHeaders
		static void RoomSelected(j19gadget*, j19action);
#pragma endregion


#pragma region UI
		static UI* UI_MapEdit{ nullptr };
		void UI_MapStart() {
			// Start
			UI::AddStage("Map");
			UI::GoToStage("Map");
			UI_MapEdit = UI::GetStage("Map");
			UI_MapEdit->PreJune = DrawMap;
			auto MG{ UI_MapEdit->MainGadget };
			RoomPanel = CreatePanel(0, 0, 125, MG->H(), MG);
			RoomPanel->BR = 25; RoomPanel->BB = 0; RoomPanel->BG = 18;
			RoomList = CreateListBox(1, 0, RoomPanel->W() - 2, RoomPanel->H() - 120, RoomPanel);
			RoomList->BR = 0;
			RoomList->BG = 25;
			RoomList->BB = 25;
			RoomList->FR = 0;
			RoomList->FG = 255;
			RoomList->FB = 255;
			RoomList->CBAction = RoomSelected;

			DataPanel = CreatePanel(TQSG_ScreenWidth() - 400, 0, 400, MG->H(), MG);
			DataPanel->BR = 25; DataPanel->BG = 18; DataPanel->BB = 0;

			auto TedPic = CreatePicture(0, RoomPanel->H() - 120, RoomPanel->W(), 120, RoomPanel, Pic_FullStretch);
			TedPic->Image(*JAS, "Img/TeddyBear.png");


			MapGroup = CreateGroup(RoomPanel->W(), RoomPanel->DrawY(), TQSG_ScreenWidth() - (RoomPanel->W() + DataPanel->W()), RoomPanel->H(), MG);

			RenewRooms();

		}
#pragma endregion


#pragma region CallBackFunctions
		static void RoomSelected(j19gadget*, j19action) {
			QCol->Error("Layer selection NOT yet present");
		}
#pragma endregion

#pragma region CallBackFunctionPullDownMenus
		void ToggleShowGrid(june19::j19gadget*, june19::j19action) {
			QCol->Error("Grid toggling not yet implemented");
		}

		void ScrollDn(june19::j19gadget*, june19::j19action) { QCol->Error("Scrolling not yet implemented"); }
		void ScrollUp(june19::j19gadget*, june19::j19action) { QCol->Error("Scrolling not yet implemented"); }
		void ScrollRi(june19::j19gadget*, june19::j19action) { QCol->Error("Scrolling not yet implemented"); }
		void ScrollLe(june19::j19gadget*, june19::j19action) { QCol->Error("Scrolling not yet implemented"); }
		void OptimizeToOrigin(june19::j19gadget*, june19::j19action) { QCol->Error("No optimizing to origin yet"); }
#pragma endregion

#pragma region GeneralFunctions
		void AdeptStatus() {
			auto st{ "Actual stuff comes later!" };
			june19::j19gadget::StatusText(st);
		}

		void DrawMap() {

		}

		void RenewRooms() {
			RoomList->ClearItems();
			for (auto r : TheMap->Rooms) RoomList->AddItem(r.first);
			RoomList->SelectItem(0);
		}

#pragma endregion
	}
}