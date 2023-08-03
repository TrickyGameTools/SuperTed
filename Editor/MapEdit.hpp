// Lic:
// SuperTed - Editor
// Map Editor (header)
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

#pragma once
#include <june19.hpp>
#include <SuperTed_Core.hpp>

namespace Slyvina {
	namespace SuperTed {
		namespace Editor {
			// General
			void AdeptStatus();
			void AdeptStatus(std::string st);
			void DrawMap();
			void RenewRooms();
			void RenewLayers();
			void RenewTextures();
			void RenewObjects();
			std::string CurrentRoom();
			std::string CurrentLayer();
			TeddyRoom Room(std::string groom = "");
			TeddyRoomLayer Layer(std::string glayer = "");

			// Pulldown menu callbacks
			void ToggleShowGrid(June19::j19gadget*, June19::j19action);
			void ScrollDn(June19::j19gadget*, June19::j19action);
			void ScrollUp(June19::j19gadget*, June19::j19action);
			void ScrollRi(June19::j19gadget*, June19::j19action);
			void ScrollLe(June19::j19gadget*, June19::j19action);
			void OptimizeToOrigin(June19::j19gadget*, June19::j19action);

			// UI Start
			void UI_MapStart();

		}
	}
}