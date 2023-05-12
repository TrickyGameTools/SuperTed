// Lic:
// SuperTed - Editor
// Textures
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

#include <SlyvQCol.hpp>
#include <SlyvString.hpp>
#include <SlyvDir.hpp>
//#include <SlyvMath.hpp>

#include "Globals.hpp"
#include "Algemeen.hpp"
#include "Textures.hpp"
#include <SlyvSTOI.hpp>


using namespace Slyvina;
using namespace June19;
using namespace Units;

namespace Slyvina {
	namespace SuperTed {
		namespace Editor {

			static UI TexStage{ nullptr };
			static j19gadget
				* TexGroup{ nullptr },
				* TexList{ nullptr },
				* TexOkay{ nullptr },
				* TexCancel{ nullptr },
				* TexIndex{ nullptr },
				* TexR{ nullptr },
				* TexG{ nullptr },
				* TexB{ nullptr },
				* TexAlpha{ nullptr },
				* TexAnimSpeed{ nullptr };

			static map<string, TeddyTexType> TexTypesReg{
				{"Stretch",TeddyTexType::Stretch},
				{"Bottom Center",TeddyTexType::BottomCenter},
				{"Auto Correct Type",TeddyTexType::ACT}
			};
			static map < string, j19gadget*> TexFieldReg;
			static map<string, vector<j19gadget*>> TexRadioReg;


			void ScanForTextures() {
				QCol->Doing("Scanning", TextureDir());
				if (cBool(ProjectConfig, "AUTHOR", "TEXTURES")) {
					auto alist{ *FileList(TextureDir(),DirWant::Directories) };
					for (size_t i = 0; i < alist.size(); ++i) {
						QCol->Doing("=> Author", alist[i]);
						auto f{ TextureDir() + "/" + alist[i] };
						if (i == 0)
							JTEX = JCR6::JCR6_Dir(f);
						else
							JTEX->Patch(f);
					}
				} else {
					JTEX = JCR6::JCR6_Dir(TextureDir());
				}
			}

			static void DrawOkayCancel(j19gadget*, j19action) {
				TexOkay->X(TexGroup->W() - TexOkay->W());
				TexOkay->Y(TexGroup->H() - TexOkay->H());
				TexCancel->X(TexOkay->X() - TexCancel->W());
				TexCancel->Y(TexOkay->Y());
				TexOkay->FR = max(0, TexOkay->FR - 1);
				TexOkay->FG = 255;
				TexOkay->FB = TexOkay->FR;
				TexOkay->BR = 0;
				TexOkay->BG = min(25, TexOkay->BG + 1);
				TexOkay->BB = 0;
				TexCancel->SetForeground(255, TexOkay->FR, TexOkay->FR, 255);
				TexCancel->SetBackground(TexOkay->BG, 0, 0, 255);
			}

			static int GetTexIndex() {
				if (!Prefixed(TexIndex->Text, "$")) TexIndex->Text = "$" + TexIndex->Text;
				return ToInt(TexIndex->Text);
			}

			static void DrawTexList(j19gadget* tl, j19action) {
				double ticks = ((double)SDL_GetTicks() / 6400);
				auto dsr = sin((double)ticks);
				auto dsg = cos((double)ticks);
				auto dsb = sin((double)ticks / (double)300);
				int r = floor(abs(dsr * 255));
				int g = floor(abs(dsg * 255));
				int b = floor(abs(dsb * 255));
				tl->SetForeground(abs(r), g, b, 255);
				tl->SetBackground(r / 10, g / 10, b / 10, 255);
				// j19gadget::StatusText(TrSPrintF("DEBUG! Ticks: %f   color:%02x%02x%02x (%3.1f)", ticks, r, g, b,dsr)); // Debug Only!
				tl->Enabled = GetTexIndex();
				bool reqallow = tl->Enabled && tl->ItemText().size();
				for (auto k : TexRadioReg) for (auto l : k.second) l->Enabled = reqallow;
				for (auto k : TexFieldReg) k.second->Enabled = reqallow;
				TexOkay->Enabled = reqallow;
			}

			static void SelectTexList(j19gadget* tl, j19action) {
				auto tex{ tl->ItemText() }; //cout << "Selected: " << tex << endl;
				if (!tex.size()) return;
				ProjectConfig->NewValue("Texture::" + tex, "AnimSpeed", "-1");
				for (auto k : TexRadioReg) for (auto l : k.second) l->checked = ProjectConfig->NewValue("Texture::" + tex, k.first, "Stretch") == l->Caption;
				for (auto k : TexFieldReg) k.second->Text = ProjectConfig->NewValue("Texture::" + tex, k.first, "255");
			}

			static void TexCancelAct(j19gadget*, j19action) { _UI::GoToStage("Map"); }

			static void TexOkayAct(j19gadget*, j19action) {
				auto idx{ GetTexIndex() };
				auto tex{ TheMap->Tex(idx) };
				auto tag{ string("Texture::") + TexList->ItemText() };
				tex->TexFile = TexList->ItemText();
				tex->r = ToInt(TexR->Text);
				tex->g = ToInt(TexG->Text);
				tex->g = ToInt(TexB->Text);
				tex->alpha = ToInt(TexB->Text);
				tex->AnimSpeed = ToInt(TexAnimSpeed->Text);
				for (auto k : TexRadioReg["Type"]) if (k->checked) {
					tex->Type = TexTypesReg[k->Caption];
					ProjectConfig->Value(tag, "Type", k->Caption);
				}
				for (auto k : TexFieldReg) {
					ProjectConfig->Value(tag, k.first, k.second->Text);
				}
				_UI::GoToStage("Map");
				RenewTextures();
			}

			static void InitTexSelectUI() {
				if (TexStage) return;
				_UI::AddStage("Textures");
				TexStage = _UI::GetStage("Textures");
				TexGroup = TexStage->MainGadget;

				TexOkay = CreateButton("Okay", 0, 0, TexGroup);
				TexOkay->CBAction = TexOkayAct;
				TexCancel = CreateButton("Cancel", 0, 0, TexGroup);
				TexCancel->CBDraw = DrawOkayCancel;
				TexCancel->CBAction = TexCancelAct;
				CreateLabel("Texture Index:", 2, 2, 200, 30, TexGroup)->SetForeground(255, 180, 0, 255);
				TexIndex = CreateTextfield(200, 2, 200, 30, TexGroup);
				TexIndex->SetForeground(0, 180, 255, 255);
				TexIndex->SetBackground(0, 18, 25, 255);

				auto ColY{ TexGroup->H() - 50 };
				CreateLabel("Color & Alpha:", 2, ColY, 200, 25, TexGroup)->SetForeground(255, 180, 0, 255);
				TexR = CreateTextfield(200, ColY, 200, 25, TexGroup);
				TexR->SetForeground(255, 0, 0, 255);
				TexR->SetBackground(25, 0, 0, 255);
				TexG = CreateTextfield(402, ColY, 200, 25, TexGroup);
				TexG->SetForeground(0, 255, 0, 255);
				TexG->SetBackground(0, 25, 0, 255);
				TexB = CreateTextfield(604, ColY, 200, 25, TexGroup);
				TexB->SetForeground(0, 0, 255, 255);
				TexB->SetBackground(0, 0, 25, 255);
				TexAlpha = CreateTextfield(806, ColY, 200, 25, TexGroup);
				TexR->Text = "255";
				TexG->Text = "255";
				TexB->Text = "255";
				TexAlpha->Text = "255";

				auto TexTypeY = ColY - 25;
				auto TTGroup = CreateGroup(0, TexTypeY, TexGroup->W(), 25, TexGroup);
				CreateLabel("Texturing Type:", 2, 0, 200, 25, TTGroup)->SetForeground(255, 180, 0, 255);
				{
					int x = 200;
					for (auto ttr : TexTypesReg) {
						auto r = CreateRadioButton(ttr.first, x, 0, 200, 25, TTGroup, ttr.first == "Stretch");
						TexRadioReg["Type"].push_back(r);
						r->SetForeground(180, 255, 0, 255);
						x += 202;
					}
				}
				auto AnimY = TexTypeY - 25;
				CreateLabel("Animation speed:", 2, AnimY, 200, 25, TexGroup)->SetForeground(255, 180, 0, 255);
				TexAnimSpeed = CreateTextfield(200, AnimY, 200, 24, TexGroup);
				TexAnimSpeed->SetForeground(180, 0, 255, 255);
				TexAnimSpeed->SetBackground(18, 0, 25, 255);
				TexAnimSpeed->Text = "-1";

				auto TLY = TexIndex->Y() + TexIndex->H();
				TexList = CreateListBox(150, TLY, TexGroup->W() - (300), TexTypeY - TLY - 30, TexGroup);
				TexList->CBDraw = DrawTexList;
				TexList->CBAction = SelectTexList;
				TexList->ClearItems();
				for (auto t : JTEX.Entries()) {
					TexList->AddItem(t.second.Entry());
				}


				TexFieldReg = {
					{"Red",TexR},
					{"Green",TexG},
					{"Blue",TexB},
					{"Alpha",TexAlpha},
					{"AnimSpeed",TexAnimSpeed}
				};

			}

			void GoTextures(uint32 idx) {
				InitTexSelectUI();
				if (!idx) {
					do {} while (TheMap->Textures.count(++idx) && TheMap->Textures[idx] && TheMap->Textures[idx]->TexFile != "");
					j19gadget::StatusText("New Texture: " + TrSPrintF("$%06x", idx));
				} else
					j19gadget::StatusText("Edit Texture: " + TrSPrintF("$%06x", idx));
				TexIndex->Text = TrSPrintF("$%06x", idx);
				_UI::GoToStage("Textures");
			}
		}
	}
}