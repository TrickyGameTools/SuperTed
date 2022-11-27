// Lic:
// SuperTed - Editor
// Run
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

#include <TQSE.hpp>

#include "Globals.hpp"

#include "Algemeen.hpp"

using namespace TrickyUnits;

namespace SuperTed {
	namespace Editor {
		bool Run() {
			TQSE_Poll();
			if (TQSE_Quit()) {
				// TODO: Save on quit
				return false;
			}						
			return(!GetThrown()) && (_UI::Run());
		}
	}
}