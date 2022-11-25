// Lic:
// SuperTed - Editor
// Textures
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
// Version: 22.11.25
// EndLic

#include <QCol.hpp>
#include <QuickString.hpp>
#include <FileList.hpp>

#include "Textures.hpp"

using namespace TrickyUnits;

namespace SuperTed {
	namespace Editor {
		void ScanForTextures() {
			QCol->Doing("Scanning", TextureDir());
			if (cBool(ProjectConfig, "AUTHOR", "TEXTURES")) {
				auto alist{ FileList(TextureDir(),Directories) };
				for (size_t i = 0; i < alist.size(); ++i) {
					QCol->Doing("=> Author", alist[i]);
					auto f{ TextureDir() + "/" + alist[i] };
					if (i == 0)
						JTEX = jcr6::Dir(f);
					else
						JTEX.PatchFile(f);	
				}
			} else {
				JTEX = jcr6::Dir(TextureDir());
			}
		}
	}
}