// Lic:
// C++ Class/headers/SuperTed_Core.hpp
// SuperTed Core (header)
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
#pragma once

// C++ libraries needed
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <iostream>


// Tricky's Units
#include <Slyv2DArray.hpp> 
#include <SlyvString.hpp>


#include <JCR6_Core.hpp>

//using namespace TrickyUnits; // Quick Test!

namespace Slyvina {
	namespace SuperTed {
		enum class TeddyRoomLayerType { Layer, Zones, Objects };
		enum class TeddyDrawTile { TrueSize, FitToGrid };
		enum class TeddyMaxTile { B8 = 256, B16 = 65536, B32 = 4294967295 }; // Note that B32 must be one value lower or bad stuff happens


		/// <summary>
		/// Type used by SuperTed to have a function named TeddyPanicFunction, which you can use to have your own error handlers.
		/// </summary>
		typedef void (*TeddyPanic)(std::string errmsg);


		void TedPanic(std::string err);
		void TedAssert(bool condition, std::string err);

		/// <summary>
		/// Replace this function with your own if you need your own error support. Only parameter taken in a C++ string with the error message.
		/// </summary>
		extern TeddyPanic TeddyPanicFunction;

		class _Teddy; typedef std::shared_ptr<_Teddy> Teddy;
		class _TeddyRoom; typedef std::shared_ptr<_TeddyRoom> TeddyRoom;
		class _TeddyRoomLayer; typedef std::shared_ptr<_TeddyRoomLayer> TeddyRoomLayer;
		class _TeddyObject; typedef std::shared_ptr<_TeddyObject> TeddyObject;
		class _TeddyTex; typedef std::shared_ptr<_TeddyTex> TeddyTex;


		class TeddyDraw {
		public:
			/// <summary>
			/// Will need to be replaced with a function which allows drawing a layer
			/// </summary>
			/// <param name=""></param>
			/// <param name=""></param>
			/// <param name="scollx"></param>
			/// <param name="scrolly"></param>
			virtual void DrawLayer(_Teddy* TedMap, std::string Room, std::string Lay, int scollx = 0, int scrolly = 0) = 0;

			/// <summary>
			/// Will be called automatically by the Teddy destructor
			/// </summary>
			/// <param name=""></param>
			virtual void Dispose(_Teddy* TedMap) = 0;
		};

		typedef std::shared_ptr<std::vector<TeddyObject>> TeddyObjectList;

		class _Teddy {
		private:
			static TeddyDraw* DrawDriver;
			static TrickyUnits::uint64 Teller;
			TrickyUnits::uint64 _ID{ Teller++ };
			void LoadLayer(TeddyRoom R, std::string Layer, jcr6::JT_Dir* J, std::string EntryName, TeddyRoomLayerType t = TeddyRoomLayerType::Layer);
			void LoadObjects(TeddyRoom R, jcr6::JT_Dir* J, std::string p, std::string r);
		public:
			TeddyMaxTile _MaxTiles{ TeddyMaxTile::B8 };
			int
				default_w{ 64 },
				default_h{ 64 },
				defaultgw{ 32 },
				defaultgh{ 32 };
			int ID();
			std::vector<std::string> BaseLayers;
			std::vector<std::string> BaseZones;
			std::map<std::string, TeddyRoom> Rooms;
			std::map<TrickyUnits::uint32, TeddyTex> Textures;
			std::map<std::string, std::string> Data;


			void RenameRoom(std::string original, std::string target);
			TeddyTex Tex(TrickyUnits::uint32 idx);
			TeddyRoom CreateRoom(std::string n, int w = 64, int h = 64, int gw = 32, int gh = 32, bool layerless = false);
			void DrawLayer(std::string R, std::string L, int scrollx = 0, int scrolly = 0);

			static Teddy Load(jcr6::JT_Dir* J, std::string p); // Don't call this directly. Use LoadTeddy() in stead

			static void SetDrawDriver(TeddyDraw* DD = nullptr); // To be used by draw drivers
			~_Teddy();
		};

		class _TeddyRoom {
		private:
			_Teddy* parent;
			std::vector<_TeddyRoomLayer*> DominanceOrder;
			int w, h, gw, gh;
			void RedimLayers();
		public:
			_TeddyRoom();
			_TeddyRoom(_Teddy* ouwe, int _w, int _h, int _gw, int gh);
			std::shared_ptr<TrickyUnits::Array2D<TeddyObjectList>> MapObjects{ nullptr };
			bool AutoRemap{ true };
			std::map<std::string, TeddyRoomLayer> Layers;
			TeddyRoomLayer CreateLayer(std::string a, bool dontremap = false);
			TeddyRoomLayer CreateZone(std::string a);
			void Remap_Dom();
			int W(); void W(int _w);
			int H(); void H(int _h);
			int GW(); void GW(int _gw);
			int GH(); void GH(int _gh);
			int PixW();
			int PixH();
			_TeddyRoomLayer* ObjectLayer();
			std::map<std::string, std::string> Data;
			TeddyObject AddObject(int x, int y, int kind = 0);
			_Teddy* GetParent();
			int LayVal(std::string Layer, int x, int y);
			void LayVal(std::string Layer, int x, int y, int newvalue);

		};

		class _TeddyRoomLayer {
		private:
			TeddyRoomLayerType Type{ TeddyRoomLayerType::Layer };
			_TeddyRoom* parent{ nullptr };
			int _Dominance{ 20 };
		public:
			_TeddyRoomLayer(_TeddyRoom* ouwe, std::string cmd = "");
			int W();
			int H();
			int Dominance();
			void Dominance(int value);
			std::shared_ptr < TrickyUnits::Array2D<int>> Field{ nullptr };
			static _TeddyRoomLayer* ObjectLayer(_TeddyRoom* ouwe);
			TeddyRoomLayerType GetType();
			void SetType(TeddyRoomLayerType t); // NEVER use this unless you know what you are doing!
			std::map<std::string, std::string> Data;
			void SetParent(_TeddyRoom* ouwe);
		};


		class _TeddyObject {
		public:
			TrickyUnits::uint32 kind;
			std::map<std::string, std::string> Data;
		};

		enum class TeddyTexType { Unknown, NoEffect, Stretch, BottomCenter, ACT }; // (ACT=AutoCorrectTile. Only use when you know what you're doing).

		class _TeddyTex {
		public:
			std::string TexFile{ "" };
			TrickyUnits::byte
				r{ 255 },
				g{ 255 },
				b{ 255 },
				alpha{ 255 };
			TeddyTexType Type{ TeddyTexType::Stretch };
			int AnimSpeed{ -1 };
			TrickyUnits::uint32 Frame{ 0 };
			void Col(TrickyUnits::byte _r, TrickyUnits::byte _g, TrickyUnits::byte _b);

		};


		/// <summary>
		/// Creates a basic SuperTed map
		/// </summary>
		/// <param name="w">Map Width (in tiles)</param>
		/// <param name="h">Map Height (in tiles)</param>
		/// <param name="gw">Grid Width (width of tiles in pixels)</param>
		/// <param name="gh">Grid Height (height of tiles in pixels)</param>
		/// <param name="rooms">Basic set of rooms (if you want multiple rooms, put them all in this string separated by semi-colons)</param>
		/// <param name="layers">Basic set of layers used for each room (if you want multiple rooms, put them all in this string separated by semi-colons)</param>
		/// <returns>A shared pointed with the map</returns>
		Teddy CreateTeddy(int w = 64, int h = 64, int gw = 32, int gh = 32, std::string rooms = "__BASE", std::string layers = "__BASE");


		Teddy CreateTeddy(int w, int h, int gw, int gh, std::vector<std::string> rooms, std::vector<std::string> layers);
		Teddy CreateTeddy(int w, int h, int gw, int gh, std::vector<std::string> rooms, std::vector<std::string> layers, std::vector<std::string> Zones);

		Teddy LoadTeddy(std::string File, std::string prefix = "");
		Teddy LoadTeddy(jcr6::JT_Dir* JCRResource, std::string prefix = "");
	}
}