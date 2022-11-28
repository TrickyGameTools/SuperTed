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
// Version: 22.11.28
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
		static bool ShowGrid{ true };
		static int ScrollX{ 0 };
		static int ScrollY{ 0 };
		static map<string, j19gadget*> DataTabs;

		static j19gadget* MapGroup;
		static j19gadget* RoomPanel{ nullptr };
		static j19gadget* RoomList{ nullptr };

		static j19gadget* DataPanel{ nullptr };
#pragma endregion

#pragma region CallBackHeaders
		static void RoomSelected(j19gadget*, j19action);
#pragma endregion


#pragma region UI
		static UI UI_MapEdit{ nullptr };
		void UI_MapStart() {
			// Start
			_UI::AddStage("Map");
			_UI::GoToStage("Map");
			UI_MapEdit = _UI::GetStage("Map");
			UI_MapEdit->PreJune = DrawMap;
			auto MG{ UI_MapEdit->MainGadget };
			// Left
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

			auto TedPic = CreatePicture(0, RoomPanel->H() - 120, RoomPanel->W(), 120, RoomPanel, Pic_FullStretch);
			TedPic->Image(*JAS, "Img/TeddyBear.png");

			// Right
			DataPanel = CreatePanel(TQSG_ScreenWidth() - 400, 0, 400, MG->H(), MG);
			DataPanel->BR = 25; DataPanel->BG = 18; DataPanel->BB = 0;
			auto OptionGroup = CreateGroup(0, 0, DataPanel->W(), 50, DataPanel);
			auto REdit = CreateRadioButton("Edit Map", 5, 5, OptionGroup->W() - 10, 20,OptionGroup);
			REdit->FR = 255;
			REdit->FG = 180;
			REdit->FB = 0;
			REdit->checked = true;
			auto RScriptSpot = CreateRadioButton("Script Spot", 5, 25, OptionGroup->W() - 10, 20, OptionGroup);
			RScriptSpot->FR = 255;
			RScriptSpot->FG = 180;
			RScriptSpot->FB = 0;
			RScriptSpot->checked = false;
			auto RScriptArea = CreateRadioButton("Script Area", 5, 45, OptionGroup->W() - 10, 20, OptionGroup);
			RScriptArea->FR = 255;
			RScriptArea->FG = 180;
			RScriptArea->FB = 0;
			RScriptArea->checked = false;

			// Map
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

		void ScrollDn(june19::j19gadget*, june19::j19action) {
			int SMax{ max(0,(Room()->GH()*Room()->H())-MapGroup->H()) };
			ScrollY = min(SMax, ScrollY + 16);
		}
		void ScrollUp(june19::j19gadget*, june19::j19action) { ScrollY = max(0, ScrollY - 16); }
		void ScrollRi(june19::j19gadget*, june19::j19action) {
			int SMax{ max(0,(Room()->GW() * Room()->W()) - MapGroup->W()) };
			ScrollX = min(SMax, ScrollX + 16);
		}
		void ScrollLe(june19::j19gadget*, june19::j19action) { ScrollX = max(0, ScrollX - 16); }
		void OptimizeToOrigin(june19::j19gadget*, june19::j19action) { QCol->Error("No optimizing to origin yet"); }
#pragma endregion

#pragma region GeneralFunctions
		void AdeptStatus(std::string st) {
			//auto st{ "Actual stuff comes later!" };
			june19::j19gadget::StatusText(st);
		}

		void AdeptStatus() { AdeptStatus("Welcome to SuperTed"); }

		static void MouseStatus() {
			auto
				MX{ TQSE_MouseX() },
				MY{ TQSE_MouseY() },
				GBX{ MapGroup->DrawX() },
				GEX{ MapGroup->DrawX() + MapGroup->W() },
				GBY{ MapGroup->DrawY() },
				GEY{ MapGroup->DrawY() + MapGroup->H() };
				auto s{ TrSPrintF("%s:\tMouse(%04d,%04d)",CurrentRoom().c_str(),MX,MY) };
				if (MX > GBX && MX<GEX && MY>GBY && MY < GEY) {
					int
						MapX{ (int)floor((MX - GBX) / Room()->GW()) },
						MapY{ (int)floor((MY - GBY) / Room()->GH()) },
						PlMX{ (int)floor(((MapX * Room()->GW()) + ScrollX) / Room()->GW()) },
						PlMY{ (int)floor(((MapY * Room()->GH()) + ScrollY) / Room()->GH()) };

					TQSG_Color(255, 255, 0);
					TQSG_Rect(
						(MapX * Room()->GW()) + MapGroup->DrawX() - (ScrollX%Room()->GW()),
						(MapY * Room()->GH()) + MapGroup->DrawY() - (ScrollY%Room()->GH()),
						Room()->GW(),
						Room()->GH(),
						true
					);
					s += TrSPrintF("  Map(%03d,%03d)", PlMX, PlMY);

			}
			
			s += TrSPrintF("\tScroll (%04d,%04d)", ScrollX, ScrollY);
			AdeptStatus(s);
		}

		void DrawMap() {
			if (ShowGrid) {
				TQSG_Color(100, 100, 100);
				for (int y = MapGroup->DrawY() - (ScrollY % TheMap->Rooms[CurrentRoom()]->GH()); y < MapGroup->DrawY() + MapGroup->H(); y += TheMap->Rooms[CurrentRoom()]->GH())
					TQSG_Line(0, y, TQSG_ScreenWidth(), y);

				for (int x = MapGroup->DrawX() - (ScrollX % TheMap->Rooms[CurrentRoom()]->GW()); x < MapGroup->DrawX() + MapGroup->W(); x += TheMap->Rooms[CurrentRoom()]->GW())
					TQSG_Line(x, 0, x, TQSG_ScreenHeight());
			}
			MouseStatus();
		}

		void RenewRooms() {
			RoomList->ClearItems();
			for (auto r : TheMap->Rooms) RoomList->AddItem(r.first);
			RoomList->SelectItem(0);
		}

		std::string CurrentRoom() { return RoomList->ItemText(); }

		TeddyRoom Room(std::string groom) {
			if (!groom.size()) return Room(CurrentRoom());
			return TheMap->Rooms[groom];
		}

#pragma endregion
	}
}