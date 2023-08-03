
#include <june19.hpp>

#include <SlyvQCol.hpp>

#include "Algemeen.hpp"
#include "Globals.hpp"
#include "ObjectEdit.hpp"


using namespace std;
using namespace Slyvina::Units;
using namespace Slyvina::June19;

namespace Slyvina {
	namespace SuperTed {
		namespace Editor {

			enum class OES {
				None = 0,
				Place = 1, 
				Modify = 2, Edit = 2 
			};

			static UI EditObjScreen{nullptr};
			static map<string, j19gadget*> ObjPanels{};
			static map<string, map<string, j19gadget*>> ObjFields{};
			static j19gadget
				* EObjHead{nullptr},
				* Okay{ nullptr },
				* Cancel{ nullptr };
			static OES EditObjectStage{OES::None};
			static int cx{ 0 }, cy{ 0 },cikind{ 0 };
			static string cskind{""};

			static void ButtonView(j19gadget*, j19action) {
				Okay->Y(EditObjScreen->MainGadget->H() - Okay->H() - 10);
				Cancel->Y(EditObjScreen->MainGadget->H() - Cancel->H() - 10);
				Cancel->X(Okay->X() + Okay->W()+3);
			}

			static void ActCancel(j19gadget*,j19action){ _UI::GoToStage("Map"); }

			static void ActOkay(j19gadget*, j19action) {
				auto o{ Room()->AddObject(cx, cy, cikind) };
				o->Data["__kind"] = cskind;
				auto Fields{ *ProjectConfig->List("OBJECTFIELDS",cskind) };
				for (auto Field : Fields) {
					if (ProjectConfig->BoolValue("OBJECTFIELDS", "CapsLock"))
						o->Data[Upper(Field)] = ObjFields[Upper(cskind)][Upper(Field)]->Text;
					else
						o->Data[Field] = ObjFields[Upper(cskind)][Upper(Field)]->Text;
				}
				QCol->Doing("Object Created", TrSPrintF("%s (%d,%d)", cskind.c_str(), cx, cy));
				_UI::GoToStage("Map");
			}
			


			static void InitObjScreen(string skind) {
				_UI::AddStage("EditObjects");
				_UI::GoToStage("EditObjects");
				Trans2Upper(skind);
				if (!EditObjScreen) {
					QCol->Doing("Creating", "Edit Objects Screen");
					EditObjScreen = _UI::GetStage("EditObjects");
					EObjHead = CreateLabel("", 5, 5, 250, 100, EditObjScreen->MainGadget);
					EObjHead->SetForeground(255, 180, 0);
				}
				if (!ObjPanels.count(skind)) {
					QCol->Doing("Creating", "Edit panel for object " + skind);
					ObjPanels[skind] = CreateGroup(0, 50, EditObjScreen->MainGadget->W(), EditObjScreen->MainGadget->H() - 50, EditObjScreen->MainGadget);
					auto Fields{ *ProjectConfig->List("OBJECTFIELDS",skind) };
					int Y{ 0 };
					for (auto Field : Fields) {
						auto UField{ Upper(Field) };
						CreateLabel(Field, 5, Y, 200, 30, ObjPanels[skind])->SetForeground(255,180,0);
						ObjFields[skind][UField] = CreateTextfield(200, Y, ObjPanels[skind]->W() - 220, ObjPanels[skind]);
						ObjFields[skind][UField]->SetForeground(0, 255, 0);
						ObjFields[skind][UField]->SetBackground(0, 25, 0, 255);
						Y += 35;
					}
				}
				 Okay = CreateButton("Okay", 5, 0, EditObjScreen->MainGadget);
				 Okay->SetBackground(0, 25, 0);
				 Okay->SetForeground(0, 255, 0);
				 Okay->CBAction = ActOkay;
				 Cancel = CreateButton("Cancel", 5, 0, EditObjScreen->MainGadget);
				 Cancel->SetBackground(25, 0, 0);
				 Cancel->SetForeground(255, 0, 0);
				 Cancel->CBDraw = ButtonView;
				 Cancel->CBAction = ActCancel;
			}

			void PlaceObject(int x, int y, int ikind, string skind) {
				InitObjScreen(skind);
				EditObjectStage = OES::Place;
				EObjHead->Text = TrSPrintF("Place object %s on spot(%d,%d)", skind.c_str(), x, y);
				auto Fields{ *ProjectConfig->List("OBJECTFIELDS",skind) };
				for (auto Field : Fields) {
					QCol->Doing("Default", skind, "\t"); QCol->Green(Field + " "); QCol->Magenta(ProjectConfig->Value("ObjectFieldsDefault", ikind + "::" + Field)+"\n");
					ObjFields[Upper(skind)][Upper(Field)]->Text = ProjectConfig->Value("ObjectFieldsDefault", skind + "::" + Field);
				}
				cikind = ikind;
				cx = x;
				cy = y;
				cskind = skind;
			}
		}
	}
}