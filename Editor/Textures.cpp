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
// Version: 22.11.29
// EndLic

#include <QCol.hpp>
#include <QuickString.hpp>
#include <FileList.hpp>

#include "Algemeen.hpp"
#include "Textures.hpp"

using namespace june19;
using namespace TrickyUnits;

namespace SuperTed {
	namespace Editor {

		static UI TexStage{ nullptr };
		static j19gadget
			* TexGroup{ nullptr },
			* TexList{ nullptr },
			* TexOkay{ nullptr },
			* TexCancel{ nullptr },
			* TexIndex{ nullptr };


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

		static void DrawOkayCancel(j19gadget*, j19action) {
			TexOkay->X(TexGroup->W() - TexOkay->W());
			TexOkay->Y(TexGroup->H() - TexOkay->H());
			TexCancel->X(TexOkay->X() - TexCancel->W());
			TexCancel->Y(TexOkay->Y());
			TexOkay->FR = max(0,TexOkay->FR - 1);
			TexOkay->FG = 255;
			TexOkay->FB = TexOkay->FR;
			TexOkay->BR = 0;
			TexOkay->BG = min(25, TexOkay->BG + 1);
			TexOkay->BB = 0;
			TexCancel->SetForeground(TexOkay->FG, 0, 0, 255);
			TexCancel->SetBackground(TexOkay->BG, 0, 0, 255);			
		}

		static void InitTexSelectUI() {
			if (TexStage) return;
			_UI::AddStage("Textures");
			TexStage = _UI::GetStage("Textures");
			TexGroup = TexStage->MainGadget;
			
			TexOkay = CreateButton("Okay", 0, 0, TexGroup);
			TexCancel = CreateButton("Cancel", 0, 0, TexGroup);
			TexCancel->CBDraw = DrawOkayCancel;
			CreateLabel("Texture Index:", 2, 2, 200, 30, TexGroup)->SetForeground(255, 180, 0, 255);
			TexIndex = CreateTextfield(200, 2, 200, 30, TexGroup);
			TexIndex->SetForeground(0, 180, 255, 255);
			TexIndex->SetBackground(0, 18, 25, 255);



			_UI::GoToStage("Textures");
		}

		void GoTextures(TrickyUnits::uint32 idx) {
			InitTexSelectUI();
			if (!idx) {
				do {} while (TheMap->Textures.count(++idx) && TheMap->Textures[idx] && TheMap->Textures[idx]->TexFile != "");
				j19gadget::StatusText("New Texture: " + TrSPrintF("$%06x", idx));
			} else
				j19gadget::StatusText("Edit Texture: " + TrSPrintF("$%06x", idx));
			TexIndex->Text = TrSPrintF("$%06x", idx);
		}
	}
}