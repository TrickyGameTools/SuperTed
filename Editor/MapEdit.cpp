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
// Version: 23.08.03
// EndLic

#include <SlyvQCol.hpp>
#include <SlyvSTOI.hpp>

#include <june19.hpp>

#include <SuperTed_Core.hpp>

#include "Globals.hpp"
#include "Algemeen.hpp"
#include "Textures.hpp"
#include "ObjectEdit.hpp"
#include <SlyvMath.hpp>


using namespace Slyvina;
using namespace June19;
using namespace Units;
using namespace TQSG;
using namespace TQSE;

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
			static int SpotObjX{ 0 }, SpotObjY{ 0 };
			static bool SpotObj{ false };


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
				* SpotObjGroup{ nullptr },

				* PlaceObjList{ nullptr },
				* SpotObjList{ nullptr },
				* RemoveObjButton{ nullptr },


				* ChkShowLayer{ nullptr },

				* DataPanel{ nullptr };
#pragma endregion

#pragma region General_Static_Headers
			static void GoDTap(string);
			static bool ShowLayer();
			static void ShowLayer(bool);
			static void LayTabShow();			
			static void GetChosenObject(int32& idx, std::string& tname);
			inline int32 GetChosenObjectIdx() { int32 i; std::string n; GetChosenObject(i, n); return i; }
			inline std::string GetChosenObject() { int32 i; std::string n; GetChosenObject(i, n); return n; }
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
			static void DrawSpotObjButton(j19gadget* g, j19action);
			static void ActRemoveObject(j19gadget*, j19action);
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


				LayerTypeTabs[TeddyRoomLayerType::Objects] = CreateGroup(0, bly + 50, DataPanel->W() - 4, EMT->H() - (bly + 50), EMT);
				auto LTOBJ = LayerTypeTabs[TeddyRoomLayerType::Objects];
				auto LTOBJMidY = LTOBJ->H() / 2;
				PlaceObjList = CreateListBox(0, 0, LTOBJ->W(), LTOBJMidY, LTOBJ);
				PlaceObjList->BR = 18;
				PlaceObjList->BG = 0;
				PlaceObjList->BB = 25;
				PlaceObjList->FR = 180;
				PlaceObjList->FG = 0;
				PlaceObjList->FB = 255;
				RenewObjects();

				SpotObjGroup = CreateGroup(0, LTOBJMidY, LTOBJ->W(), LTOBJMidY, LTOBJ);
				SpotObjList = CreateListBox(0, 0, LTOBJ->W(), LTOBJMidY, SpotObjGroup);
				SpotObjList->SetForeground(0, 180, 255);
				SpotObjList->SetBackground(0, 18, 25, 255);
				auto EditO{ CreateButton("Modify",0,0,SpotObjGroup) };
				EditO->SetForeground(255, 180, 0);
				EditO->SetBackground(25, 180, 0);
				EditO->CBDraw = DrawSpotObjButton;
				auto RemoveO{ CreateButton("Remove",0,0,SpotObjGroup) }; RemoveObjButton = RemoveO;
				RemoveO->SetForeground(255, 0, 0);
				RemoveO->SetBackground(25, 0, 0);
				RemoveO->CBAction = ActRemoveObject;
				






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

			void DrawSpotObjButton(j19gadget* g, j19action) {
				g->Y(g->GetParent()->H() - (g->H() + 5));
				RemoveObjButton->Y(g->Y());
				RemoveObjButton->X(g->X() + g->W() + 3);
				SpotObjList->H(g->Y() - 3);
				SpotObjGroup->Enabled = SpotObj;
				auto SI{ SpotObjList->SelectedItem() >= 0 };
				g->Enabled = SpotObj && SI;
				RemoveObjButton->Enabled = SpotObj && SI;
			}

			void ActRemoveObject(j19gadget*, j19action) {				
				auto OL{ Room()->MapObjects->Value(SpotObjX,SpotObjY)}; if (!OL) return;
				auto Idx{ SpotObjList->SelectedItem() }; if (Idx < 0) return;
				if (Yes(TrSPrintF("Do you really want to remove Object #%d from (%d,%d)?", Idx + 1, SpotObjX, SpotObjY))) {
					OL->erase(OL->begin() + Idx);
					SpotObj = false;
					SpotObjList->ClearItems();
				}
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

			static void LayerAreaChange(int x1, int y1, int x2, int y2,int value) {
				for (int y = std::min(y1, y2); y <= std::max(y1, y2); ++y) {
					for (int x = std::min(x1, x2); x <= std::max(x1, x2); ++x) {
						Layer()->Field->Value(x, y, value);
					}
				}
			}

			static void MouseStatus() {
				auto
					ML{ TQSE::MouseHit(1) },
					MR{ TQSE::MouseHit(SDL_BUTTON_RIGHT) },
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
								((SX * Room()->GW()) - ScrollX) + MapGroup->DrawX(),
								((SY * Room()->GH()) - ScrollY) + MapGroup->DrawY(),
								((EX * Room()->GW()) - ScrollX) + MapGroup->DrawX(),
								((EY * Room()->GH()) - ScrollY) + MapGroup->DrawY()
							);
							s += TrSPrintF(" Area(%03d,%03d) %03dx%03d", AreaStartX, AreaStartY, abs(PlMX - AreaStartX), abs(PlMY - AreaStartY));
							SetAlpha(255);
							if (!MDL) {
								// TODO: Act when mousebutton is released!
								if (RScriptArea->checked) {
									BThrow("Scripts for Area not yet supported","MouseStatus() function in MapEdit.cpp");
								} else if (RScriptSpot->checked) {
									BThrow("INTERNAL ERROR!\nPlease report\n\nArea checkup during spot scripting should NEVER be possible", "MouseStatus() function in MapEdit.cpp",ThrowAct::InstantEnd);
								} else if (REdit->checked) {
									switch (Layer()->GetType()) {
									case TeddyRoomLayerType::Objects:
										BThrow("INTERNAL ERROR!\nPlease report\n\nArea checkup during object placement should NEVER be possible", "MouseStatus() function in MapEdit.cpp", ThrowAct::InstantEnd);
										return; // Safety precaution, but the editor should actually end instantly.
									case TeddyRoomLayerType::Zones:
										BThrow("Zone placement not yet coded!", "", ThrowAct::None);
										break;
									case TeddyRoomLayerType::Layer: {
										auto Tex{ 0 };
										auto TexItem{ TexList->ItemText() };
										if (TexItem.size()) {
											auto TIS{ Split(TexItem,' ') };
											Tex = ToInt((*TIS)[0]);		
											LayerAreaChange(PlMX, PlMY, AreaStartX, AreaStartY,Tex);
										}

										//BThrow("Layer placement not yet coded!", "", ThrowAct::None);
									}break;
									default:
										BThrow(TrSPrintF("INTERNAL ERROR!\nPlease report\n\nUnknown layer type give (%d)",(int)Layer()->GetType()), "MouseStatus() function in MapEdit.cpp", ThrowAct::InstantEnd);
										return;
									}
								}
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
						if (RScriptSpot->checked && ML) {
							TQSE::Notify("Sorry. Scripting not yet supported! Please come back later! Okay?");
						} else if (Layer()->GetType() == TeddyRoomLayerType::Objects) {
							//cout << "Left:" << ML << ";\tRight:" << ML << endl; // debug only
							if (ML) {
								if (GetChosenObjectIdx())
									PlaceObject(PlMX, PlMY, GetChosenObjectIdx(), GetChosenObject());
							}
							if (MR) {
								auto OL{ Room()->MapObjects->Value(PlMX,PlMY) };
								SpotObj = OL && OL->size() > 0;
								SpotObjX = PlMX;
								SpotObjY = PlMY;
								SpotObjList->ClearItems();
								//cout << SpotObj << " -> (" << SpotObjX << "," << SpotObjY << ") S:" << OL->size() << "\n"; // debug only
								for (auto O : *OL) SpotObjList->AddItem(O->Data["__kind"]);
							}
						}
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
				// Layers first
				for (auto& l : Room()->Layers) {
					switch (l.second->GetType()) {
					case TeddyRoomLayerType::Layer:
						if (ShowLayerReg[CurrentRoom()][l.first])
							TheMap->DrawLayer(CurrentRoom(), l.first, ScrollX - MapGroup->DrawX(), ScrollY - MapGroup->DrawY());
					}
				}
				// Zones now!
				if (Layer()->GetType() == TeddyRoomLayerType::Zones) {
					for (int zy = 0; zy < Layer()->H(); ++zy) {
						for (int zx = 0; zx < Layer()->W(); ++zx) {
							auto v{ Layer()->Field->Value(zx,zy) };
							if (v) {
								switch (v % 10) {
								case 1: SetColor(0, 180, 255, 75); break;
								case 2: SetColor(255, 0, 0, 75); break;
								case 3: SetColor(0, 255, 0, 75); break;
								case 4: SetColor(255, 180, 180, 75); break;
								case 5: SetColor(255, 180, 0, 75); break;
								case 6: SetColor(0, 0, 255, 75); break;
								case 7: SetColor(125, 100, 0, 75); break;
								case 8: SetColor(180, 255, 0, 75); break;
								case 9: SetColor(0, 255, 255, 75); break;
								case 0: SetColor(180, 0, 255, 0); break;
								}
								auto dx{ ((zx * Room()->GW()) + MapGroup->DrawX()) - ScrollX };
								auto dy{ ((zy * Room()->GH()) + MapGroup->DrawY()) - ScrollY };
								Rect(dx, dy, Room()->GW(), Room()->GH());
								SetAlpha(255);
								j19gadget::GetDefaultFont()->Text(TrSPrintF("%x", v), zx, zy);
							}
						}
					}
				}
				// Objects
				for (int zy = 0; zy < Layer()->H(); ++zy) {
					for (int zx = 0; zx < Layer()->W(); ++zx) {
						auto L{ Room()->MapObjects->Value(zx,zy) };
						if (L && L->size()) {
							double c{ abs(DegSin((double)(SDL_GetTicks() / (double)20)) * (double)255) };
							SetColor((byte)c, (byte)c, (byte)c, (byte)c);
							auto dx{ ((zx * Room()->GW()) + MapGroup->DrawX()) - ScrollX };
							auto dy{ ((zy * Room()->GH()) + MapGroup->DrawY()) - ScrollY };
							//cout << "Obj (" << zx << "," << zy << ") => " << L->size() << " Draw@("<<dx<<","<<dy<<") ["<<c<<"]\n"; // debug only!
							Rect(dx, dy, Room()->GW(), Room()->GH(), true);
							SetColor(255, 255, 255, 255);
							auto zo{ TrSPrintF("O:%d", L->size()) };
							//cout << zo << "  ("<<zx<<","<<zy<<")"<< endl; // debug only
							j19gadget::GetDefaultFont()->Text(zo, dx, dy);
						}
					}
				}
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

			void RenewObjects() {
				PlaceObjList->ClearItems();
				auto lobj{ ProjectConfig->List("Layers","Objects") };
				PlaceObjList->AddItem(TrSPrintF("$%08x <Nothing>", 0));
				for (uint32 i = 0; i < lobj->size(); i++) PlaceObjList->AddItem(TrSPrintF("$%08x %s", i + 1, (*lobj)[i].c_str()));
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

			void GetChosenObject(int32& idx, std::string& tname) {
				auto IText = PlaceObjList->ItemText();
				if (!IText.size()) {
					idx = 0;
					tname = "<nothing>";
				} else {
					auto p = FindFirst(IText, ' ');
					if (p <= -1) {
						idx = ToInt(IText);
						tname = TrSPrintF("Object $%08x", idx);
					} else {
						idx = ToInt(Trim(IText.substr(0, p)));
						tname = Trim(IText.substr(p + 1));
					}
				}
			}

#pragma endregion
		}
	}
}