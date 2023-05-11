// Lic:
// C++ Class/source/SuperTed_Core.cpp
// SuperTed Core
// version: 23.05.11
// Copyright (C) 2022, 2023 Jeroen P. Broks
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// EndLic
#define LoadChat

#include "../headers/SuperTed_Core.hpp"
#include <SlyvString.hpp>

using namespace Slyvina::Units;


namespace Slyvina {
	namespace SuperTed {

		uint64 _Teddy::Teller{ 0 };

		class EmptyDrawDriver :public TeddyDraw {
		public:
			void DrawLayer(_Teddy* TedMap, std::string Room, std::string Lay, int scollx = 0, int scrolly = 0) {}
			void Dispose(_Teddy* T) {}
		};
		EmptyDrawDriver _DrawDriverEmpty{ EmptyDrawDriver() };
		TeddyDraw* _Teddy::DrawDriver{ &_DrawDriverEmpty };

		void _Teddy::SetDrawDriver(TeddyDraw* DD) {
			if (!DD) DrawDriver = &_DrawDriverEmpty; else DrawDriver = DD;
		}

		void _Teddy::DrawLayer(std::string R, std::string L, int scrollx, int scrolly) { DrawDriver->DrawLayer(this, R, L, scrollx, scrolly); }

		_Teddy::~_Teddy() {
			DrawDriver->Dispose(this);
		}

		static void LChat(std::string m) {
#ifdef LoadChat
			std::cout << "\x1b[35mLoad Chat:\x1b[0m " << m << std::endl;
#endif
		}


		static void _Panic(std::string err) {
			std::cout << "SuperTed Error: " << err << std::endl;
			exit(20);
		}

		TeddyPanic TeddyPanicFunction{ nullptr };
		void TedPanic(std::string err) {
			if (!TeddyPanicFunction) _Panic(err); else TeddyPanicFunction(err);
		}

		void TedAssert(bool condition, std::string err) { if (!condition) TedPanic(err); }

		Teddy CreateTeddy(int w, int h, int gw, int gh, std::string rooms, std::string layers) {
			using namespace Units;
			auto _rooms{ *Split(rooms, ';') };
			auto _layers{ *Split(layers,';') };
			return CreateTeddy(w, h, gw, gh, _rooms, _layers);
		}
		Teddy SuperTed::CreateTeddy(int w, int h, int gw, int gh, std::vector<std::string> rooms, std::vector<std::string> layers) {
			auto ret{ std::make_shared<_Teddy>() };
			ret->default_w = w;
			ret->default_h = h;
			ret->defaultgw = gw;
			ret->defaultgh = gh;
			ret->BaseLayers = layers;
			for (auto r : rooms) ret->CreateRoom(r, w, h, gw, gh);
			return ret;
		}

		Teddy CreateTeddy(int w, int h, int gw, int gh, std::vector<std::string> rooms, std::vector<std::string> layers, std::vector<std::string> zones) {
			auto ret{ std::make_shared<_Teddy>() };
			ret->default_w = w;
			ret->default_h = h;
			ret->defaultgw = gw;
			ret->defaultgh = gh;
			ret->BaseLayers = layers;
			ret->BaseZones = zones;
			for (auto r : rooms) ret->CreateRoom(r, w, h, gw, gh);
			return ret;
		}

		Teddy LoadTeddy(std::string File, std::string prefix) {
			auto J{ JCR6::Dir(File) };
			if (JCR_Error != "" && Upper(JCR_Error) != "OK") return nullptr;
			return _Teddy::Load(&J, prefix);
		}

		Teddy LoadTeddy(JCR6::JT_Dir JCRResource, std::string prefix) { return _Teddy::Load(JCRResource, prefix); }



		void _Teddy::LoadLayer(TeddyRoom R, std::string Layer, JCR6::JT_Dir J, std::string EntryName, TeddyRoomLayerType t) {
			TeddyRoomLayer L{ nullptr };
			auto BT{ J->B(EntryName) };
		switch (t) { case TeddyRoomLayerType::Layer: L = R->CreateLayer(Layer); break; case TeddyRoomLayerType::Zones: L = R->CreateZone(Layer); break; default: _Panic("Invalid Layer type for LoadLayer"); return; }
												   auto TAG{ BT->ReadByte() }; // why the far ident? I don't know either, ask the guys at MicroSoft. It was VS2022 doing this automatically.
												   while (TAG) {
													   switch (TAG) {
													   case 1: {
														   auto
															   k{ BT->ReadString() },
															   v{ BT->ReadString() };
														   L->Data[k] = v;
													   }
															 break;
													   case 2:
														   for (uint32 y = 0; y < L->H(); y++) for (uint32 x = 0; x < L->W(); x++) {
															   switch (R->GetParent()->_MaxTiles) {
															   case TeddyMaxTile::B8: L->Field->Value(x, y, BT->ReadByte()); break;
															   case TeddyMaxTile::B16: L->Field->Value(x, y, BT->ReadUInt16()); break;
															   case TeddyMaxTile::B32: L->Field->Value(x, y, BT->ReadUInt32()); break;
															   default:
																   _Panic("I cannot read layers with the set MaxTile setting. Perhaps you need a later version of SuperTed?");
																   return;
															   }
														   } break;
													   default:
														   _Panic("Unknown layer tag (" + std::to_string(TAG) + ")");
														   return;

													   }
													   TAG = BT->ReadByte();
												   }
		}


		void _Teddy::LoadObjects(TeddyRoom R, JCR6::JT_Dir J, std::string p, std::string r) {
			R->ObjectLayer(); // Make sure the layer exists
			auto BT{ J->B(p + "Rooms/" + r + "/Objects") };
			auto Tag{ BT->ReadByte() };
			TeddyObject o{ nullptr };
			while (Tag) {
				switch (Tag) {
				case 1: {
					if (BT->ReadByte() != 2) { _Panic("Object load error!"); return; }
					auto
						x{ BT->ReadUInt32() },
						y{ BT->ReadUInt32() };
					o = R->AddObject(x, y);
				}
					  break;
				case 2:
					_Panic("Loose object tag 2");
					return;
				case 3:
					o->kind = BT->ReadUInt32();
					break;
				case 4: {
					auto
						k{ BT->ReadString() },
						v{ BT->ReadString() };
					o->Data[k] = v;
				} break;
				default:
					_Panic("Unknown object tag (" + std::to_string(Tag) + ")");
					return;
				}
				Tag = BT->ReadByte();
			}
		}

		int _Teddy::ID() { return _ID; }

		void _Teddy::RenameRoom(std::string original, std::string target) {
			auto ErTag{ TrSPrintF("Teddy->RenameRoom(\"%s\",\"%s\"): ",original.c_str(),target.c_str()) };
			original = Upper(original);
			target = Upper(target);
			TedAssert(Rooms.count(original), ErTag + "Original room not found!");
			TedAssert(!Rooms.count(target), ErTag + "Target room already exists");
			Rooms[target] = Rooms[original];
			Rooms.erase(original);
		}

		TeddyTex _Teddy::Tex(TrickyUnits::uint32 idx) {
			if (!Textures.count(idx)) Textures[idx] = std::make_shared<_TeddyTex>();
			if (!Textures[idx]) Textures[idx] = std::make_shared<_TeddyTex>();
			return Textures[idx];
		}

		TeddyRoom _Teddy::CreateRoom(std::string n, int w, int h, int gw, int gh, bool layerless) {
			using namespace TrickyUnits;
			auto ret{ std::make_shared<_TeddyRoom>(this, w,  h,  gw, gh) };
			ret->Remap_Dom();
			ret->W(w);
			ret->H(h);
			ret->GW(gw);
			ret->GH(gh);
			n = Upper(n);
			if (n == "OBJECTS") { TeddyPanicFunction("Name Objects is reserved"); return nullptr; }
			Rooms[n] = ret;
			auto objlay{ std::make_shared<_TeddyRoomLayer>(ret.get()) };
			objlay->SetType(TeddyRoomLayerType::Objects);
			ret->Layers["OBJECTS"] = objlay;
			for (int x = 0; x < w; ++x) for (int y = 0; y < h; ++y) ret->MapObjects->Value(x, y, std::make_shared<std::vector<TeddyObject>>());
			if (!layerless) {
				for (auto l : BaseLayers) ret->CreateLayer(l);
				for (auto z : BaseZones) ret->CreateZone(z);
			}
			return ret;
		}

		static bool JEC() {
			if (JCR_Error != "" && Upper(JCR_Error) != "OK") { _Panic("JCR Error: " + JCR_Error); return false; }
			return true;
		}
#define JECO if (!JEC()) return nullptr
		Teddy _Teddy::Load(JCR6::JT_Dir J, std::string p) {
			using namespace std;
			if (p.size() && !suffixed(p, "/")) p += "/";
			LChat("Reading: " + p + "Data");
			auto ret{ make_shared<_Teddy>() };
			auto BT{ J->B(p + "Data") }; JECO; ret->Data.clear();
			auto Tag{ BT->ReadByte() };
			// Data
			while (Tag) {
				LChat("Teddy Main Data Tag: " + to_string(Tag));
				switch (Tag) {
				case 1: {
					auto Key{ BT->ReadString() };
					auto Value{ BT->ReadString() };
					ret->Data[Key] = Value;
				} break;
				default:
					_Panic("Unknown main data tag (" + to_string(Tag) + ")");
					return nullptr;
				}
				Tag = BT->ReadByte();
			}
			// Textures
			ret->Textures.clear();
			BT = J->B(p + "Textures");
			Tag = BT->ReadByte();
			uint32 tindex{ 0 };
			while (Tag) {
				LChat("Textures Tag: " + to_string(Tag));
				switch (Tag) {
				case 1: {
					uint32 m{ BT->ReadUInt32() };
					if (m <= (int)TeddyMaxTile::B8)
						ret->_MaxTiles = TeddyMaxTile::B8;
					else if (m <= (int)TeddyMaxTile::B16)
						ret->_MaxTiles = TeddyMaxTile::B16;
					else if (m <= (int)TeddyMaxTile::B32)
						ret->_MaxTiles = TeddyMaxTile::B32;
					else {
						_Panic("Maxtile higher than this version of SuperTed supports (" + to_string(m) + "; SuperTed can go no higher than (" + to_string((int)TeddyMaxTile::B32) + ")!");
						return nullptr;
					}
					LChat("Max Tiles Set to: " + to_string((int)ret->_MaxTiles));
				}
					  break;
				case 2:
					switch (ret->_MaxTiles) {
					case TeddyMaxTile::B8: tindex = BT->ReadByte(); break;
					case TeddyMaxTile::B16: tindex = BT->ReadUInt16(); break;
					case TeddyMaxTile::B32: tindex = BT->ReadUInt32(); break;
					default: _Panic("Uknown MaxTile setting!"); return nullptr;
					}
					if (ret->Textures.count(tindex))
						printf("WARNING! Duplicate Texture Index (%d). This will lead to overwriting data!\n", tindex);
					else
						ret->Textures[tindex] = make_shared<_TeddyTex>();
					if (!ret->Textures[tindex]) { _Panic("Null Texture"); return nullptr; } // Safeguard only, as this should NEVER be possible at all!
					ret->Textures[tindex]->TexFile = BT->ReadString();
					LChat("Texture " + to_string(tindex) + ": " + ret->Textures[tindex]->TexFile);
					break;
				case 3:
					if (!tindex) {
						_Panic("Color request without TexIndex");
						return nullptr;
					} else {
						ret->Textures[tindex]->r = BT->ReadByte();
						ret->Textures[tindex]->g = BT->ReadByte();
						ret->Textures[tindex]->b = BT->ReadByte();
					}
					break;
				case 4:
					if (!tindex) { _Panic("Alpha request without TexIndex\n"); return nullptr; } else ret->Textures[tindex]->alpha = BT->ReadByte();
					break;
				case 5:
					if (!tindex) { _Panic("Frame request without TexIndex\n"); return nullptr; } else ret->Textures[tindex]->Frame = BT->ReadUInt32();
					break;
				case 6:
					if (!tindex) { _Panic("Animation speed request without TexIndex\n"); return nullptr; } else ret->Textures[tindex]->Frame = BT->ReadUInt32();
					break;
				default:
					_Panic("Unknown texture tag: (" + to_string(Tag) + ")! File corruption, or using an outdated version of SuperTed?");
					return nullptr;
				}
				Tag = BT->ReadByte();
			}
			// Room
			ret->Rooms.clear();
			BT = J->B(p + "RoomTable");
			Tag = BT->ReadByte();
			std::shared_ptr<JCR6::JT_EntryReader> BR{ nullptr };
			std::string Rm{ "" };
			TeddyRoom WR{ nullptr };
			while (Tag) {
				LChat("Roomtable tag: " + to_string(Tag));
				switch (Tag) {
				case 1: {
					Rm = BT->ReadString();
					WR = ret->CreateRoom(Rm);
					BR = J->B(p + "Rooms/" + Rm + "/Room"); JECO;
					auto RTag{ BR->ReadByte() };
					while (RTag) {
						LChat("Room." + Rm + " Tag: " + to_string(RTag));
						switch (RTag) {
						case 1:
							WR->W(BR->ReadInt());
							WR->H(BR->ReadInt());
#ifdef LoadChat
							{ char dbg[500]; sprintf_s(dbg, "Room Format:  %dx%d", WR->W(), WR->H()); LChat(dbg); }
#endif
							break;
						case 2:
							WR->GW(BR->ReadInt());
							WR->GH(BR->ReadInt());
							break;
						case 3: {
							auto
								k{ BR->ReadString() },
								v{ BR->ReadString() };
							WR->Data[k] = v;
						}
							  break;
						case 4:
						{
							auto l{ BR->ReadString() };
							auto t{ (TeddyRoomLayerType)BR->ReadByte() };
							switch (t) {
							case TeddyRoomLayerType::Layer:
								ret->LoadLayer(WR, l, J, p + "Rooms/" + Rm + "/Layers/" + l);
								break;
							case TeddyRoomLayerType::Zones:
								ret->LoadLayer(WR, l, J, p + "Rooms/" + Rm + "/Zones/" + l, TeddyRoomLayerType::Zones);
								break;
							case TeddyRoomLayerType::Objects:
								ret->LoadObjects(WR, J, p, Rm);
								break;
							default:
								_Panic("Unknown room-layer tag");
								return nullptr;
							}


						} break;
						default:
							_Panic("Unknown roomdata tag");
							return nullptr;
						}
						RTag = BR->ReadByte();
					}
					break;
				}
				default:
					_Panic("Uknown roomtable tag");
				}
				Tag = BT->ReadByte();
			}
			return ret;
		}

		void _TeddyRoom::RedimLayers() {
			for (auto l : Layers) l.second->Field->ReDim(w, h);
		}

		_TeddyRoom::_TeddyRoom() { TedPanic("Teddy Room without data"); }

		_TeddyRoom::_TeddyRoom(_Teddy* ouwe, int _w, int _h, int _gw, int _gh) {
			w = _w;
			h = _h;
			gw = _gw;
			gh = _gh;
			parent = ouwe;
			MapObjects = TrickyUnits::Array2D<TeddyObjectList>::Dim(_w, _h);
		}

		TeddyRoomLayer _TeddyRoom::CreateLayer(std::string a, bool dontremap) {
			a = TrickyUnits::Upper(a); TedAssert(a != "OBJECTS", "Layer cannot be named OBJECTS!");
			auto ret{ std::make_shared<_TeddyRoomLayer>(this) };
			//std::cout << "Created layer: " << a << "\n"; // debug only
			Layers[a] = ret;
			ret->SetParent(this);
			ret->Field = TrickyUnits::Array2D<int>::Dim(w, h);
			ret->Field->SetAll(0);
			return ret;
		}

		TeddyRoomLayer _TeddyRoom::CreateZone(std::string a) {
			a = TrickyUnits::Upper(a); TedAssert(a != "OBJECTS", "Zone cannot be named OBJECTS!");
			auto ret{ std::make_shared<_TeddyRoomLayer>(this) };
			//std::cout << "Created zone: " << a << "\n"; // debug only
			Layers[a] = ret;
			ret->SetType(TeddyRoomLayerType::Zones);
			ret->SetParent(this);
			ret->Field = TrickyUnits::Array2D<int>::Dim(w, h);
			ret->Field->SetAll(0);
			return ret;
		}

		void _TeddyRoom::Remap_Dom() {
			using namespace std;
			map<string, string> __map;
			DominanceOrder.clear();
			for (auto ol : Layers) {
				char tag[255];
				sprintf_s(tag, "%08d_%s", ol.second->Dominance(), ol.first.c_str());
				__map[tag] = ol.first;
			}
			for (auto ol : __map) DominanceOrder.push_back(Layers[ol.second].get());
		}

		int _TeddyRoom::W() { return w; }

		void _TeddyRoom::W(int _w) {
			w = _w;
			if (!MapObjects)
				MapObjects = std::make_shared<Array2D<TeddyObjectList>>(w, h);
			else
				MapObjects->ReDim(w, h);
			RedimLayers();
		}

		int _TeddyRoom::H() { return h; }

		void _TeddyRoom::H(int _w) {
			h = _w;
			if (!MapObjects)
				MapObjects = std::make_shared<Array2D<TeddyObjectList>>(w, h);
			else
				MapObjects->ReDim(w, h);
			RedimLayers();
		}

		int _TeddyRoom::GW() { return gw; }
		void _TeddyRoom::GW(int _v) { gw = _v; }
		int _TeddyRoom::GH() { return gh; }
		void _TeddyRoom::GH(int _v) { gh = _v; }

		int _TeddyRoom::PixW() { return w * gw; }
		int _TeddyRoom::PixH() { return h * gh; }

		int _TeddyRoom::LayVal(std::string Layer, int x, int y) {
			using namespace std;
			Layer = Upper(Layer);
			if (!Layers.count(Layer)) _Panic("Layer " + Layer + " not found");
			return Layers[Layer]->Field->Value(x, y);
		}

		void _TeddyRoom::LayVal(std::string Layer, int x, int y, int newvalue) {
			using namespace std;
			Layer = Upper(Layer);
			if (!Layers.count(Layer)) _Panic("Layer " + Layer + " not found");
			Layers[Layer]->Field->Value(x, y, newvalue);
		}

		_TeddyRoomLayer* _TeddyRoom::ObjectLayer() {
			return _TeddyRoomLayer::ObjectLayer(this);
		}

		TeddyObject _TeddyRoom::AddObject(int x, int y, int kind) {
			auto ret{ std::make_shared<_TeddyObject>() };
			ret->kind = kind;
			ObjectLayer();
			if (!MapObjects) MapObjects = std::make_shared<Array2D<TeddyObjectList>>(w, h);
			if (!MapObjects->Value(x, y)) MapObjects->Value(x, y, std::make_shared<std::vector<TeddyObject>>());
			MapObjects->Value(x, y)->push_back(ret);
			return ret;
		}

		_Teddy* _TeddyRoom::GetParent() {
			return parent;
		}


		_TeddyRoomLayer::_TeddyRoomLayer(_TeddyRoom* ouwe, std::string cmd) {
			using namespace TrickyUnits;
			auto cmds{ Split(cmd,';') };
			for (auto c : cmds) {
				if (Upper(c) == "ZONE" || Upper(c) == "ZONE") Type = TeddyRoomLayerType::Zones;
			}
			parent = ouwe;
			if (ouwe) Field = Array2D<int>::Dim(W(), H());
		}

		int _TeddyRoomLayer::W() { return parent->W(); }
		int _TeddyRoomLayer::H() { /*std::cout << "Parent: " << (int)parent << std::endl;*/ return parent->H(); }
		int _TeddyRoomLayer::Dominance() { return _Dominance; }
		void _TeddyRoomLayer::Dominance(int value) {
			_Dominance = value;
			if (parent->AutoRemap) parent->Remap_Dom();
		}

		_TeddyRoomLayer* _TeddyRoomLayer::ObjectLayer(_TeddyRoom* ouwe) {
			if (!ouwe->Layers.count("OBJECTS")) {
				auto ret{ std::make_shared<_TeddyRoomLayer>(ouwe) };
				ret->parent = ouwe;
				ouwe->Layers["OBJECTS"] = ret;
				ret->Type = TeddyRoomLayerType::Objects;
				return ret.get();
			}
			return ouwe->Layers["OBJECTS"].get();
		}

		TeddyRoomLayerType _TeddyRoomLayer::GetType() { return Type; }

		void _TeddyRoomLayer::SetType(TeddyRoomLayerType t) { Type = t; }

		void _TeddyRoomLayer::SetParent(_TeddyRoom* ouwe) {
			if (!parent) {
				parent = ouwe;
				Field = Units::Array2D<int>::Dim(W(), H());
			}
		}


		void _TeddyTex::Col(byte _r, byte _g, byte _b) {
			r = _r;
			g = _g;
			b = _b;
		}


	}
}