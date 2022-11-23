// Lic:
// C++ Class/source/SuperTed_Save.cpp
// SuperTed Save
// version: 22.11.23
// Copyright (C) 2022 Jeroen P. Broks
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
#include "../headers/SuperTed_Save.hpp"
#include <QuickTypes.hpp>

#define jerchk() SuperTed::TedAssert(jcr6::Get_JCR_Error_Message()=="" || jcr6::Get_JCR_Error_Message()=="Ok",jcr6::Get_JCR_Error_Message())


using namespace TrickyUnits;
using namespace jcr6;

namespace SuperTed {

	static void WriteTexCode(Teddy TeddyMap, JT_CreateBuf* Buf,uint32 t) {
		TedAssert(t < (int)TeddyMap->_MaxTiles, "Trying to save tex reference " + std::to_string(t) + " which is higher than the max tiles (" + std::to_string((uint32)TeddyMap->_MaxTiles) + ")");
		switch (TeddyMap->_MaxTiles) {
		case TeddyMaxTile::B8:
			Buf->Write((byte)t);
			break;
		case TeddyMaxTile::B16:
			Buf->Write((uint16)t);
			break;
		case TeddyMaxTile::B32:
			Buf->Write(t);
			break;
		default:
			TedPanic("Unknown max tile size");
			break;
		}
	}

	static void SaveLayer(JT_Create* res, std::string entry, TeddyRoomLayer lay, Teddy tmap, std::string Storage) {
		auto blay{ res->StartEntry(entry,Storage) };
		for (auto d : lay->Data) {
			blay->Write((byte)1); blay->Write(d.first); blay->Write(d.second);
		}
		blay->Write((byte)2);
		for (uint32 y = 0; y < lay->H(); y++)for (uint32 x = 0; x < lay->W(); x++) WriteTexCode(tmap, blay, lay->Field->Value(x, y));
		blay->Write((byte)0);
		blay->Close();
	}

	void TeddySave(Teddy TeddyMap, std::string filename, std::string dir, std::string Storage) {
		auto saveted{ jcr6::JT_Create(filename,Storage) };
		jerchk();
		TeddySave(TeddyMap, &saveted, dir, Storage);
		saveted.Close();
	}

	void TeddySave(Teddy TeddyMap, jcr6::JT_Create* resource, std::string dir, std::string Storage) {
		if (dir.size() && !suffixed(dir,"/")) dir+="/";

		// Data
		{
			auto bdata = resource->StartEntry(dir + "Data", Storage);
			for (auto i : TeddyMap->Data) {
				bdata->Write((byte)1);
				bdata->Write(i.first);
				bdata->Write(i.second);
			}
			bdata->Write((byte)0);
			bdata->Close();

		}

		// Textures
		{
			auto bttex = resource->StartEntry(dir + "Textures",Storage);
			jerchk();
			bttex->Write((byte)1); bttex->Write((uint32)TeddyMap->_MaxTiles);
			for (auto i : TeddyMap->Textures) {
				bttex->Write((byte)2);
				WriteTexCode(TeddyMap, bttex, i.first);
				bttex->Write(i.second->TexFile);
				bttex->Write((byte)3);
				bttex->Write(i.second->r);
				bttex->Write(i.second->g);
				bttex->Write(i.second->b);
				bttex->Write((byte)4);
				bttex->Write(i.second->alpha);
				bttex->Write((byte)5);
				bttex->Write(i.second->Frame);
				bttex->Write((byte)6);
				bttex->Write(i.second->AnimSpeed);
			}
			bttex->Write((byte)0);
			bttex->Close();
		}

		// Rooms
		{
			auto brt{ resource->StartEntry(dir + "RoomTable",Storage )};
			for(auto room:TeddyMap->Rooms){
				brt->Write((byte)1);
				brt->Write(room.first);
				for (auto layer : room.second->Layers) {				
					switch (layer.second->GetType()) {
					case TeddyRoomLayerType::Layer:
						SaveLayer(resource,dir + "Rooms/" + room.first + "/Layers/" + layer.first, layer.second, TeddyMap,Storage);
						break;
					case TeddyRoomLayerType::Zones:
						SaveLayer(resource, dir + "Rooms/" + room.first + "/Zones/" + layer.first, layer.second, TeddyMap, Storage);
						break;
					case TeddyRoomLayerType::Objects:
					{
						auto bob{ resource->StartEntry(dir + "Rooms/" + room.first + "/Objects") };
						for (uint32 y = 0; y < room.second->H(); y++)for (uint32 x = 0; x < room.second->W(); x++) {
							auto l{ *(room.second->MapObjects->Value(x, y).get()) };
							for (auto obj :l ){
								bob->Write((byte)1);
								bob->Write((byte)2);
								bob->Write(x);
								bob->Write(y);
								bob->Write((byte)3);
								bob->Write(obj->kind);
								for (auto d : obj->Data){
									bob->Write((byte)4);
									bob->Write(d.first);
									bob->Write(d.second);
								}
							}
						}
						bob->Write((byte)0);
						bob->Close();
					}
					break;
					default:
						TedPanic("Unknown layer type");
					}

				}
				auto brd{ resource->StartEntry(dir + "Rooms/" + room.first + "/Room",Storage) };
				brd->Write((byte)1);
				brd->Write(room.second->W());
				brd->Write(room.second->H());
				brd->Write((byte)2);
				brd->Write(room.second->GH());
				brd->Write(room.second->GW());
				for (auto d : room.second->Data) {
					brd->Write((byte)3);
					brd->Write(d.first);
					brd->Write(d.second);
				}
				for (auto layer : room.second->Layers) {
					brd->Write((byte)4);
					brd->Write(layer.first);
					brd->Write((byte)layer.second->GetType());
				}
				brd->Write((byte)0);
				brd->Close();
			}
			brt->Write((byte)0);
			brt->Close();

		}

		

	}
}