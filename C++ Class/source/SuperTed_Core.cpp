//#include <QuickString.hpp>
#include "../headers/SuperTed_Core.hpp"

//using namespace TrickyUnits;


namespace SuperTed {
	
	
	static void _Panic(std::string err) {
		std::cout << "SuperTed Error: " << err << std::endl;
		exit(20);
	}

	TeddyPanic TeddyPanicFunction{ nullptr };
	static void Panic(std::string err) {
		if (!TeddyPanicFunction) _Panic(err); else TeddyPanicFunction(err);
		

	}

	Teddy CreateTeddy(int w, int h, int gw, int gh, std::string rooms, std::string layers) {
		using namespace TrickyUnits;
		auto _rooms{ Split(rooms, ':') };
		auto _layers{ Split(layers,':') };
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

	TeddyRoom _Teddy::CreateRoom(std::string n, int w, int h, int gw, int gh, bool layerless) {
		using namespace TrickyUnits;
		auto ret{ std::make_shared<_TeddyRoom>(this, w,  h,  gw, gh) };
		ret->Remap_Dom();
		n = Upper(n);
		if (n == "OBJECTS") { TeddyPanicFunction("Name Objects is reserved"); return nullptr; }
		if (!layerless) for (auto l : BaseLayers) ret->CreateLayer(l);
		return TeddyRoom();
	}

	void _TeddyRoom::RedimLayers() {
		for (auto l : Layers) l.second->Field->ReDim(w, h);
	}

	_TeddyRoom::_TeddyRoom() { Panic("Teddy Room without data"); }

	_TeddyRoom::_TeddyRoom(_Teddy* ouwe, int _w, int _h, int _gw, int _gh) {
		w = _w;
		h = _h;
		gw = _gw;
		gh = _gh;
		parent = ouwe;
		MapObjects = TrickyUnits::Array2D<TeddyObjectList>::Dim(_w, _h);
	}

	TeddyRoomLayer _TeddyRoom::CreateLayer(std::string a, bool dontremap) {
		auto ret{ std::make_shared<_TeddyRoomLayer>(this) };
		ret->Field = TrickyUnits::Array2D<int>::Dim(w, h);
		return ret;
	}

	void _TeddyRoom::Remap_Dom() {
		using namespace std;
		map<string, string> __map;
		DominanceOrder.clear();
		for (auto ol : Layers) {
			char tag[255];
			sprintf_s(tag, "%08d_%s", ol.second->Dominance(), ol.first);
			__map[tag] = ol.first;
		}
		for (auto ol : __map) DominanceOrder.push_back(Layers[ol.second].get());
	}

	int _TeddyRoom::W() { return w; }

	void _TeddyRoom::W(int _w) {
		w = _w;
		MapObjects->ReDim(w, h);
		RedimLayers();
	}

	int _TeddyRoom::H() { return h; }

	void _TeddyRoom::H(int _w) {
		h = _w;
		MapObjects->ReDim(w, h);
		RedimLayers();
	}

	_TeddyRoomLayer* _TeddyRoom::ObjectLayer() {
		return _TeddyRoomLayer::ObjectLayer(this);
	}


	_TeddyRoomLayer::_TeddyRoomLayer(_TeddyRoom* ouwe) {
		parent = ouwe;
		Field->ReDim(W(), H());
	}

	int _TeddyRoomLayer::W() { return parent->W(); }
	int _TeddyRoomLayer::H() { return parent->H(); }
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
		}
		return nullptr;
	}

	TeddyRoomLayerType _TeddyRoomLayer::GetType() { return Type; }


}