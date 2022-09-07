#pragma once

// C++ libraries needed
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <iostream>


// Tricky's Units
#include <2DArray.hpp> // Please note there is no 2DArray.cpp, because this entire header is a template based class
#include <QuickString.hpp>
#include <QuickTypes.hpp> // No .cpp as these are just typedefs and nothing more

//using namespace TrickyUnits; // Quick Test!

namespace SuperTed {
	enum class TeddyRoomLayerType { Layer, Zones, Objects };
	enum class TeddyDrawTile { TrueSize, FitToGrid };
	enum class TeddyMaxTile { B8 = 256, B16 = 65536, B32 = 4294967296 };


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

	typedef std::vector<TeddyObject> TeddyObjectList;

	class _Teddy {
	public:
		TeddyMaxTile _MaxTiles{ TeddyMaxTile::B8 };
		int
			default_w{ 64 },
			default_h{ 64 },
			defaultgw{ 32 },
			defaultgh{ 32 };
		std::vector<std::string> BaseLayers;
		std::map<std::string, TeddyRoom> Rooms;
		std::map<TrickyUnits::uint32, std::string> Textures;
		std::map<std::string, std::string> Data;

		TeddyRoom CreateRoom(std::string n, int w = 64, int h = 64, int gw = 32, int gh = 32, bool layerless = false);
	};

	class _TeddyRoom {
	private:
		_Teddy* parent;
		std::vector<_TeddyRoomLayer*> DominanceOrder;
		int w, h, gw, gh;
		void RedimLayers();
	public:
		_TeddyRoom();
		_TeddyRoom(_Teddy* ouwe,int _w, int _h, int _gw, int gh);
		std::shared_ptr<TrickyUnits::Array2D<TeddyObjectList>> MapObjects{nullptr};
		bool AutoRemap{ true };
		std::map<std::string, TeddyRoomLayer> Layers;
		TeddyRoomLayer CreateLayer(std::string a, bool dontremap = false);
		void Remap_Dom();
		int W(); void W(int _w);
		int H(); void H(int _h);
		_TeddyRoomLayer* ObjectLayer();
		std::map<std::string, std::string> Data;
	};

	class _TeddyRoomLayer {
	private:		
		TeddyRoomLayerType Type{TeddyRoomLayerType::Layer};
		_TeddyRoom* parent;
		int _Dominance{ 20 };
	public:
		_TeddyRoomLayer(_TeddyRoom* ouwe);
		int W();
		int H();
		int Dominance();
		void Dominance(int value);
		std::shared_ptr < TrickyUnits::Array2D<int>> Field{ nullptr };
		static _TeddyRoomLayer* ObjectLayer(_TeddyRoom* ouwe);
		TeddyRoomLayerType GetType();
		std::map<std::string, std::string> Data;

	};
	

	class _TeddyObject {
	public:
		TrickyUnits::uint32 kind;
		std::map<std::string, std::string> Data;
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


	Teddy CreateTeddy(int w , int h , int gw, int gh, std::vector<std::string> rooms , std::vector<std::string> layers);


}
