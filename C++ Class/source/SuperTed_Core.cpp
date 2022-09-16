
#include "../headers/SuperTed_Core.hpp"

//using namespace TrickyUnits;


namespace SuperTed {
	
	
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
		using namespace TrickyUnits;
		auto _rooms{ Split(rooms, ';') };
		auto _layers{ Split(layers,';') };
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

	TeddyTex _Teddy::Tex(TrickyUnits::uint32 idx) {
		if (!Textures.count(idx)) Textures[idx] = std::make_shared<_TeddyTex>();
		return Textures[idx];
	}

	TeddyRoom _Teddy::CreateRoom(std::string n, int w, int h, int gw, int gh, bool layerless) {
		using namespace TrickyUnits;
		auto ret{ std::make_shared<_TeddyRoom>(this, w,  h,  gw, gh) };
		ret->Remap_Dom();
		n = Upper(n);
		if (n == "OBJECTS") { TeddyPanicFunction("Name Objects is reserved"); return nullptr; }
		Rooms[n] = ret;
		auto objlay{ std::make_shared<_TeddyRoomLayer>(ret.get()) };
		objlay->SetType(TeddyRoomLayerType::Objects);
		ret->Layers["OBJECTS"] = objlay;
		for (int x = 0; x < w; ++x) for (int y = 0; y < h; ++y) ret->MapObjects->Value(x, y, std::make_shared<std::vector<TeddyObject>>());
		if (!layerless) for (auto l : BaseLayers) ret->CreateLayer(l);
		return TeddyRoom();
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
		a = TrickyUnits::Upper(a); TedAssert(a != "OBJECTS","Layer cannot be named OBJECTS!");
		auto ret{ std::make_shared<_TeddyRoomLayer>(this) };
		//std::cout << "Created layer: " << a << "\n"; // debug only
		Layers[a] = ret;
		ret->SetParent(this);
		ret->Field = TrickyUnits::Array2D<int>::Dim(w, h);
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
		MapObjects->ReDim(w, h);
		RedimLayers();
	}

	int _TeddyRoom::H() { return h; }

	void _TeddyRoom::H(int _w) {
		h = _w;
		MapObjects->ReDim(w, h);
		RedimLayers();
	}

	int _TeddyRoom::GW() { return gw; }
	void _TeddyRoom::GW(int _v) { gw = _v; }
	int _TeddyRoom::GH() { return gh; }
	void _TeddyRoom::GH(int _v) { gh = _v; }

	_TeddyRoomLayer* _TeddyRoom::ObjectLayer() {
		return _TeddyRoomLayer::ObjectLayer(this);
	}

	TeddyObject _TeddyRoom::AddObject(int x, int y, int kind) {
		auto ret{ std::make_shared<_TeddyObject>() };
		ret->kind = 0;
		MapObjects->Value(x, y)->push_back(ret);
		return ret;
	}


	_TeddyRoomLayer::_TeddyRoomLayer(_TeddyRoom* ouwe,std::string cmd) {
		using namespace TrickyUnits;
		auto cmds{ Split(cmd,';') };
		for (auto c : cmds) {
			if (Upper(c) == "ZONE" || Upper(c) == "ZONE") Type = TeddyRoomLayerType::Zones;
		}
		parent = ouwe;
		if(ouwe) Field = Array2D<int>::Dim(W(), H());
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
		}
		return nullptr;
	}

	TeddyRoomLayerType _TeddyRoomLayer::GetType() { return Type; }

	void _TeddyRoomLayer::SetType(TeddyRoomLayerType t) { Type = t; }

	void _TeddyRoomLayer::SetParent(_TeddyRoom* ouwe) {
		if (!parent) {
			parent = ouwe;
			Field = TrickyUnits::Array2D<int>::Dim(W(), H());
		}
	}


	void _TeddyTex::Col(TrickyUnits::byte _r, TrickyUnits::byte _g, TrickyUnits::byte _b) {
		r = _r;
		g = _g;
		b = _b;
	}


}