// Lic:
// SuperTed - Editor
// Save (header)
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

#include <june19.hpp>



namespace Slyvina {
	namespace SuperTed {
		namespace Editor {
			void MenuSave(June19::j19gadget*, June19::j19action);

			void CreateMap(std::string f);
			void CreateMap();
			void LoadMap(std::string f, bool CreateIfNonExistent = true);
			void LoadMap();
			void SaveMap();
			void SaveMap(std::string f);
		}
	}
}