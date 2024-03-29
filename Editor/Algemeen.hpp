// Lic:
// SuperTed Editor
// User Interface - General (header)
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
// Version: 23.05.12
// EndLic
#pragma once


#pragma once
// C++
#include <map>
#include <string>

// June19
#include <june19.hpp>

// TQSG
#include <TQSG.hpp>


// All UI Based crap
#include "MapEdit.hpp"
#include "MetaEdit.hpp"
#include "RoomEdit.hpp"


namespace Slyvina {
	namespace SuperTed {
		namespace Editor {

			typedef void(*UIV)();

			class _UI; typedef	std::shared_ptr<_UI> UI;

			class _UI {
			private:
				static bool _initialized;
				static Slyvina::TQSG::TImage Mouse;
				static std::map<std::string, UI> Stage;
				std::string _Name{ "" };
				static UI _Current;
				_UI(std::string name);
			public:
				_UI();

				static void Crash(std::string m);

				June19::j19gadget* MainGadget{ nullptr };
				UIV PreJune{ nullptr };
				UIV PostJune{ nullptr };

				static void AddStage(std::string st);
				static UI GetStage(std::string st);
				static UI CurrentStage();
				static void GoToStage(std::string st);

				// Notify
				static void Start();
				static bool Run();
				static void Done();
			};
		}
	}
}