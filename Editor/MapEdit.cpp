// Lic:
// SuperTed - Editor
// Map Editor
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
#include <SlyvSTOI.hpp>

#include <june19.hpp>

#include <SuperTed_Core.hpp>

#include "Globals.hpp"
#include "Algemeen.hpp"
#include "Textures.hpp"


using namespace Slyvina;
using namespace June19;
using namespace Units;
using namespace TQSG;

namespace Slyvina {
	namespace SuperTed {
		namespace Editor {

#pragma region Variables
			static bool ShowGrid{ true };
			static int ScrollX{ 0 };
			static int ScrollY{ 0 };
			static map<string, j19gadget*> DataTabs{};
			static map<TeddyRoomLayerType, j19gadget*> LayerTypeTabs{};
			static map<string, map<string, bool>> ShowLayerReg{};
			static bool MarkArea{ false };
			static int AreaStartX{ 0 }, AreaStartY{ 0 };


			static j19gadget
				* MapGroup{ nullptr },
				* RoomPanel{ nullptr },
				* RoomList{ nullptr },

				* LayerList{ nullptr },
				* AddLayer{ nullptr },
				* RemLayer{ nullptr },
				* LayerString{ nullptr },
				* RadAddLayer{ nullptr },
				* RadAddZone{ nullptr },

				* TexList{ nullptr },
				* AddTex{ nullptr },
				* EdtTex{ nullptr },
				* RemTex{ nullptr },
				//* TexR{ nullptr },
				//* TexG{ nullptr },
				//* TexB{ nullptr },

				* REdit{ nullptr },
				* RScriptArea{ nullptr },
				* RScriptSpot{ nullptr },


				* ChkShowLayer{ nullptr },

				* DataPanel{ nullptr };
#pragma endregion

#pragma region General_Static_Headers
			static void GoDTap(string);
			static bool ShowLayer();
			static void ShowLayer(bool);
			static void LayTabShow();
#pragma endregion

#pragma region CallBackHeaders
			static void RoomSelected(j19gadget*, j19action);
			static void TabRadioAct(j19gadget*, j19action);
			static void LayerStringAction(j19gadget*, j19action);
			static void ButtonAddLayer(j19gadget*, j19action);
			static void ActRemoveLayer(j19gadget*, j19action);
			static void DwTextButton(j19gadget*, j19action);
			static void SelectLayer(j19gadget*, j19action);
			static void B_AddTexture(j19gadget*, j19action) { GoTextures(); }
			static void B_EditTexture(j19gadget*, j19action);
			static void ActShowLayer(j19gadget* g, j19action) { ShowLayer(g->checked); }
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
				TedPic->Image(JAS, "Img/TeddyBear.png");

				// Right
				DataPanel = CreatePanel(TQSG::ScreenWidth() - 400, 0, 400, MG->H(), MG);
				DataPanel->BR = 25; DataPanel->BG = 18; DataPanel->BB = 0;
				auto OptionGroup = CreateGroup(0, 0, DataPanel->W(), 50, DataPanel);
				REdit = CreateRadioButton("Edit Map", 5, 5, OptionGroup->W() - 10, 20, OptionGroup);
				REdit->FR = 255;
				REdit->FG = 180;
				REdit->FB = 0;
				REdit->checked = true;
				REdit->CBAction = TabRadioAct;
				RScriptSpot = CreateRadioButton("Script Spot", 5, 25, OptionGroup->W() - 10, 20, OptionGroup);
				RScriptSpot->FR = 255;
				RScriptSpot->FG = 180;
				RScriptSpot->FB = 0;
				RScriptSpot->checked = false;
				RScriptSpot->CBAction = TabRadioAct;
				RScriptArea = CreateRadioButton("Script Area", 5, 45, OptionGroup->W() - 10, 20, OptionGroup);
				RScriptArea->FR = 255;
				RScriptArea->FG = 180;
				RScriptArea->FB = 0;
				RScriptArea->checked = false;
				RScriptArea->CBAction = TabRadioAct;

				// Map Tab
				DataTabs["Edit Map"] = CreateGroup(5, 65, DataPanel->W(), DataPanel->H() - 65, DataPanel);

				auto
					EMT = DataTabs["Edit Map"];
				auto
					bly = (int)EMT->H() / 4;
				LayerList = CreateListBox(2, 0, EMT->W() - 6, bly, EMT);
				LayerList->FR = 0;
				LayerList->FG = 255;
				LayerList->FB = 0;
				LayerList->BR = 0;
				LayerList->BG = 25;
				LayerList->BB = 0;
				LayerList->CBAction = SelectLayer;

				AddLayer = CreateButton("+", 0, bly + 5, EMT);
				RemLayer = CreateButton("-", 25, bly + 5, EMT);
				LayerString = CreateTextfield(50, bly + 5, EMT->W() - 50, 20, EMT);
				AddLayer->FR = 0;
				AddLayer->FG = 255;
				AddLayer->FB = 0;
				AddLayer->BR = 0;
				AddLayer->BG = 25;
				AddLayer->BB = 0;
				AddLayer->Enabled = false;
				RemLayer->FG = 0;
				RemLayer->FR = 255;
				RemLayer->FB = 0;
				RemLayer->BG = 0;
				RemLayer->BR = 25;
				RemLayer->BB = 0;
				LayerString->FR = 255;
				LayerString->FG = 180;
				LayerString->FB = 0;
				LayerString->BR = 12;
				LayerString->BG = 9;
				LayerString->BB = 0;
				auto LayZon = CreateGroup(0, bly + 30, 10, 10, EMT);
				RadAddLayer = CreateRadioButton("Layer", 0, 0, EMT->W() / 2, 25, LayZon);
				RadAddZone = CreateRadioButton("Zone", RadAddLayer->W(), 0, EMT->W() / 2, 25, LayZon);
				RadAddLayer->SetForeground(255, 180, 0);
				RadAddZone->SetForeground(255, 180, 0);
				RadAddLayer->checked = true;
				RadAddZone->checked = false;
				LayerString->CBAction = LayerStringAction;
				AddLayer->CBAction = ButtonAddLayer;
				RemLayer->CBAction = ActRemoveLayer;
				LayerTypeTabs[TeddyRoomLayerType::Layer] = CreateGroup(0, bly + 50, DataPanel->W() - 4, EMT->H() - (bly + 50), EMT);
				auto LTLT = LayerTypeTabs[TeddyRoomLayerType::Layer];
				// Texture			
				TexList = CreateListBox(0, 0, LTLT->W(), LTLT->H() - 40, LTLT);
				TexList->SetForeground(255, 255, 0);
				TexList->SetBackground(255, 0, 0, 255);
				AddTex = CreateButton("New", 0, TexList->H(), LTLT); AddTex->SetForeground(0, 255, 0, 255); AddTex->SetBackground(0, 25, 0, 255);
				EdtTex = CreateButton("Edit", 0, TexList->H(), LTLT); EdtTex->SetForeground(255, 255, 0, 255); EdtTex->SetBackground(25, 25, 0, 255);
				RemTex = CreateButton("Remove", 0, TexList->H(), LTLT); RemTex->SetForeground(255, 0, 0, 255); RemTex->SetBackground(25, 0, 0, 255);
				RemTex->CBDraw = DwTextButton;
				AddTex->CBAction = B_AddTexture;
				EdtTex->CBAction = B_EditTexture;
				RenewTextures();

				ChkShowLayer = CreateCheckBox("Show layer", 0, TexList->H(), 50, 25, LTLT);
				ChkShowLayer->SetForeground(255, 180, 0, 255);
				ChkShowLayer->checked = true;
				ChkShowLayer->CBAction = ActShowLayer;





				// Color
				//int CH = TexList->H()+40;
				//int CW = (int)floor(LTLT->W() * .35);
				/*
				TexR = CreateTextfield(0, CH, CW, LTLT);
				TexR->SetBackground(100, 0, 0, 255);
				TexG = CreateTextfield(CW, CH, CW, LTLT);
				TexG->SetBackground(0, 100, 0, 255);
				TexB = CreateTextfield(CW*2, CH, CW, LTLT);
				TexB->SetBackground(0, 0, 100, 255);
				//*/

				// DrawType
				// Anim


				// Script Spot Tab
				if (ProjectConfig->ListCount("Script", "Spot")) {
					DataTabs["Script Spot"] = CreateGroup(5, 65, DataPanel->W(), DataPanel->H() - 65, DataPanel);
				} else RScriptSpot->Enabled = false;

				// Script Area Tab
				if (ProjectConfig->ListCount("Script", "Area")) {
					DataTabs["Script Area"] = CreateGroup(5, 65, DataPanel->W(), DataPanel->H() - 65, DataPanel);
				} else RScriptArea->Enabled = false;


				GoDTap("Edit Map");
				// Map Itself
				MapGroup = CreateGroup(RoomPanel->W(), RoomPanel->DrawY(), ScreenWidth() - (RoomPanel->W() + DataPanel->W()), RoomPanel->H(), MG);

				RenewRooms();
				RenewLayers();
				RenewTextures();
				LayTabShow();

			}
#pragma endregion


#pragma region CallBackFunctions
			static void RoomSelected(j19gadget*, j19action) {
				RenewLayers();
			}

			static void TabRadioAct(j19gadget* g, j19action) {
				if (g->checked) GoDTap(g->Caption);
			}

			static void ButtonAddLayer(j19gadget*, j19action) {
				if (RadAddLayer->checked)
					Room()->CreateLayer(Upper(LayerString->Text));
				else if (RadAddZone->checked)
					Room()->CreateZone(Upper(LayerString->Text));
				AddLayer->Enabled = false;
				LayerString->Text = "";
				RenewLayers();
			}

			static void ActRemoveLayer(j19gadget*, j19action) {
				if (LayerList->ItemText() == "OBJECTS")
					NFE("The layer OBJECTS may NOT be removed!");
				else if (LayerList->ItemText() == "")
					NFE("No layer chosen");
				else {
					Room()->Layers.erase(LayerList->ItemText());
					RenewLayers();
				}

			}

			static void LayerStringAction(j19gadget* g, j19action a) {
				string nLay{ Upper(g->Text) };
				bool allow{
					nLay.size() &&
					(!Room()->Layers.count(nLay)) &&
					DataTabs["Edit Map"]->Visible
				};
				AddLayer->Enabled = allow;
				if (allow && a == j19action::Enter) ButtonAddLayer(g, a);
			}

			static void DwTextButton(j19gadget*, j19action) {
				EdtTex->X(AddTex->W());
				RemTex->X(EdtTex->X() + EdtTex->W());
				ChkShowLayer->X(RemTex->X() + RemTex->W() + 4);
			}

			static void SelectLayer(j19gadget*, j19action) {
				LayTabShow();
			}

			static void B_EditTexture(j19gadget*, j19action) {
				if (!TexList->ItemText().size()) {
					QCol->Error("No texture chosen");
					return;
				}
				auto s = Split(TexList->ItemText(), ' ');
				auto i = ToInt((*s)[0]);
				if (i <= 0) {
					QCol->Error("$000000 reseved for 'nothing'.");
					return;
				}
				GoTextures(i);
			}

#pragma endregion

#pragma region CallBackFunctionPullDownMenus
			void ToggleShowGrid(June19::j19gadget*, June19::j19action) {
				QCol->Error("Grid toggling not yet implemented");
			}

			void ScrollDn(June19::j19gadget*, June19::j19action) {
				int SMax{ max(0,(Room()->GH() * Room()->H()) - MapGroup->H()) };
				ScrollY = min(SMax, ScrollY + 16);
			}
			void ScrollUp(June19::j19gadget*, June19::j19action) { ScrollY = max(0, ScrollY - 16); }
			void ScrollRi(June19::j19gadget*, June19::j19action) {
				int SMax{ max(0,(Room()->GW() * Room()->W()) - MapGroup->W()) };
				ScrollX = min(SMax, ScrollX + 16);
			}
			void ScrollLe(June19::j19gadget*, June19::j19action) { ScrollX = max(0, ScrollX - 16); }
			void OptimizeToOrigin(June19::j19gadget*, June19::j19action) { QCol->Error("No optimizing to origin yet"); }
#pragma endregion

#pragma region GeneralFunctions
			static void GoDTap(string k) {
				for (auto i : DataTabs) i.second->Visible = Upper(i.first) == Upper(k);
			}

			void AdeptStatus(std::string st) {
				//auto st{ "Actual stuff comes later!" };
				June19::j19gadget::StatusText(st);
			}

			void AdeptStatus() { AdeptStatus("Welcome to SuperTed"); }

			static bool EditingArea() {
				if (RScriptSpot->checked) return false;
				if (RScriptArea->checked) return true;
				return Layer()->GetType() != TeddyRoomLayerType::Objects;
			}
			inline bool EditingSpot() { return !EditingArea(); }

			static void MouseStatus() {
				auto
					ML{ TQSE::MouseHit(1) },
					MR{ TQSE::MouseHit(2) },
					MDL{ TQSE::MouseDown(1) },
					MDR{ TQSE::MouseDown(2) };
				auto
					MX{ TQSE::MouseX() },
					MY{ TQSE::MouseY() },
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

					SetColor(255, 255, 0);
					Rect(
						(MapX * Room()->GW()) + MapGroup->DrawX() - (ScrollX % Room()->GW()),
						(MapY * Room()->GH()) + MapGroup->DrawY() - (ScrollY % Room()->GH()),
						Room()->GW(),
						Room()->GH(),
						true
					);
					s += TrSPrintF("  Map(%03d,%03d) ", PlMX, PlMY);
					if (EditingArea()) {
						s += "Area";
						if (MarkArea) {
							SetColorHSV((SDL_GetTicks() / 75) % 360, 1, 1);
							SetAlpha(75);
							// TODO: Rect
							auto
								SX{ std::min(AreaStartX,PlMX) },
								SY{ std::min(AreaStartY,PlMY) },
								EX{ std::max(AreaStartX,PlMX) },
								EY{ std::max(AreaStartY,PlMY) };
							if (EX >= SX) EX++;
							if (EY >= SY) EY++;
							ExRect(
								((SX * Room()->GW()) - ScrollX)+MapGroup->DrawX(),
								((SY * Room()->GH()) - ScrollY)+MapGroup->DrawY(),
								((EX * Room()->GW()) - ScrollX) + MapGroup->DrawX(),
								((EY * Room()->GH()) - ScrollY) + MapGroup->DrawY()
							);
							s += TrSPrintF(" Area(%03d,%03d) %03dx%03d", AreaStartX, AreaStartY, abs(PlMX - AreaStartX), abs(PlMY - AreaStartY));
							SetAlpha(255);
							if (!MDL) {
								// TODO: Act when mousebutton is released!
								MarkArea = false;
							}
						} else if (MDL) {
							MarkArea = true;
							AreaStartX = PlMX;
							AreaStartY = PlMY;
						}
					} else {
						MarkArea = false;
						s += "Spot"; 
					}

				} else {
					MarkArea = false;
				}

				s += TrSPrintF("\t\tScroll (%04d,%04d)", ScrollX, ScrollY);
				AdeptStatus(s);
			}

			void DrawMap() {
				if (ShowGrid) {
					SetColor(100, 100, 100);
					for (int y = MapGroup->DrawY() - (ScrollY % TheMap->Rooms[CurrentRoom()]->GH()); y < MapGroup->DrawY() + MapGroup->H(); y += TheMap->Rooms[CurrentRoom()]->GH())
						Line(0, y, ScreenWidth(), y);

					for (int x = MapGroup->DrawX() - (ScrollX % TheMap->Rooms[CurrentRoom()]->GW()); x < MapGroup->DrawX() + MapGroup->W(); x += TheMap->Rooms[CurrentRoom()]->GW())
						Line(x, 0, x, ScreenHeight());
				}
				// TODO: Draw layers and stuff in order
				MouseStatus();
			}

			void RenewRooms() {
				RoomList->ClearItems();
				for (auto r : TheMap->Rooms) RoomList->AddItem(r.first);
				RoomList->SelectItem(0);
				RenewLayers();
			}

			void RenewLayers() {
				LayerList->ClearItems();
				for (auto k : Room()->Layers) LayerList->AddItem(k.first);
				LayerList->SelectItem(0);
				LayTabShow();
			}

			void RenewTextures() {
				TexList->ClearItems();
				TexList->AddItem("$000000 <Nothing>");
				for (auto tex : TheMap->Textures) {
					if (tex.first && tex.second && tex.second->TexFile.size())
						TexList->AddItem(TrSPrintF("$%06X %s", tex.first, tex.second->TexFile.c_str()));
				}
				TexList->SelectItem(0);
			}

			std::string CurrentRoom() { return RoomList->ItemText(); }
			std::string CurrentLayer() { return LayerList->ItemText(); }

			TeddyRoom Room(std::string groom) {
				if (!groom.size()) return Room(CurrentRoom());
				return TheMap->Rooms[groom];
			}

			TeddyRoomLayer Layer(std::string glayer) {
				if (!glayer.size()) return Layer(CurrentLayer());
				return Room()->Layers[glayer];
			}

			static bool ShowLayer() {
				if (!ShowLayerReg[CurrentRoom()].count(CurrentLayer())) return true;
				return ShowLayerReg[CurrentRoom()][CurrentLayer()];
			}

			static void ShowLayer(bool value) { ShowLayerReg[CurrentRoom()][CurrentLayer()] = value; }

			static void LayTabShow() {
				for (auto l : LayerTypeTabs) l.second->Visible = l.first == Layer()->GetType();
				if (Layer()->GetType() == TeddyRoomLayerType::Layer) {
					ChkShowLayer->checked = ShowLayer();
				}
			}

#pragma endregion
		}
	}
}