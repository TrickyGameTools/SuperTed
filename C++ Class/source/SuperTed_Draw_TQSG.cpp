#define SPT_Debug
#include <iostream>
#include <memory>
#include <TQSG.hpp>
#include "../headers/SuperTed_Core.hpp"
#include "../headers/SuperTed_Draw_TQSG.hpp"


#ifdef SPT_Debug
#define Chat(abc) std::cout << "\x1b[36mSuperTed Draw Driver Debug>\x1b[0m " << abc << std::endl
#else
#define Chat(abc)
#endif


using namespace std;
using namespace TrickyUnits;
using namespace jcr6;
namespace SuperTed {

	bool ZoneShow{ false };
	static JT_Dir* TEXJCR{ nullptr };

	class _TTex; typedef shared_ptr<_TTex> TTex;
	class _MTex; typedef shared_ptr<_MTex> MTex;

	static void _Panic(std::string k) {
		if (TeddyPanicFunction)
			TeddyPanicFunction(k);
		else {
			cout << "ERROR!\n" << k << endl;
			exit(40);
		}
	}

	class _TTex {
	public:
		byte
			r{ 0 },
			g{ 0 },
			b{ 0 },
			Alpha{ 0 };
		int
			Speed{ -1 },
			Tick{ 0 },
			Frame{ 0 };
		TeddyTexType Type{TeddyTexType::Stretch};
		TQSG_AutoImage Img{nullptr};
		_TTex(std::string f, byte _r = 255, byte _g = 255, byte _b = 255, byte _alpha = 255, int _Speed = -1, int _Frame = 0,TeddyTexType _Type=TeddyTexType::Stretch) {
			r = _r;
			g = _g;
			b = _b;
			Alpha = _alpha;
			Speed = _Speed;
			Frame = _Frame;
			Type = _Type;
			if (TEXJCR) {
				Chat("Loading texture: " << f);
				if ((ExtractExt(Upper(f)) == "JPBF" && TEXJCR->DirectoryExists(f)) || (TEXJCR->EntryExists(f))) {
					Img = TQSG_LoadAutoImage(TEXJCR, f);
				} else {
					_Panic("JCR Resource does not contain a texture named: " + f);
				}
			} else {
				_Panic("Cannot load texture " + f + "! JCR Resource still contains a null pointer!");
			}
		}

		void Draw( int x, int y, int gw, int gh, int tx, int ty) {
			TQSG_Color(r, g, b);
			switch (Type) {
			case TeddyTexType::Unknown:
				_Panic("Unknown drawing type");
				return;
			case TeddyTexType::NoEffect:
				Img->Draw(x, y, Frame);
				break;
			case TeddyTexType::Stretch:
				Img->Stretch(x, y, gw, gh);
				break;
			case TeddyTexType::BottomCenter:
				Img->Draw((x + (gw / 2)) - (Img->W() / 2), (y + gh) - Img->H());
				break;
			case TeddyTexType::ACT:
				Img->Tile(x, y, gw, gh, tx % gw, ty % gh);
				break;
			default:
				// This scenario should never be possible, but just in case
				_Panic("Unhandeled drawing type.\nThis is an internal error in the SuperTed TQSG driver.\nPlease report this on the SuperTed issue tracker");
				return;
			}
		}
	};

	class _MTex {
	public:
		map<int, TTex> Tex;
		uint64 VID;
	};
	map<uint64, MTex> MapTex;


	class SuperTed_TQSG :public TeddyDraw {
	public:
		void Dispose(_Teddy* M) {
			if (MapTex.count(M->ID())) { MapTex.erase(M->ID()); Chat("Disposed tex register for map ID #" << M->ID()); }
		}
		void DrawLayer(_Teddy* M, std::string Room, std::string Lay, int scrollx = 0, int scrolly = 0) {
			if (!MapTex.count(M->ID())) {
				MapTex[M->ID()] = make_shared<_MTex>();
				Chat("Tex register made for Map ID #" << M->ID());
			}
			auto URoom = Upper(Room); if (!M->Rooms.count(URoom)) { _Panic("There is no room named " + Room); return; }
			auto TRoom = M->Rooms[URoom];
			auto ULay = Upper(Lay); if (!TRoom->Layers.count(ULay)) { _Panic("Room '" + Room+"' does not have a layer named "+Lay); return; }
			auto TLay = TRoom->Layers[ULay];
			switch (TLay->GetType()) {
			case TeddyRoomLayerType::Objects:
				return;
			case TeddyRoomLayerType::Zones:
				if (!ZoneShow) return;
				_Panic("Zone drawing not (yet) supported");
				break;
			case TeddyRoomLayerType::Layer:
				break;
			default:
				_Panic("Unknown layer type for drawing!");
			}
			auto MT{ MapTex[M->ID()] };
			for (auto IT : MT->Tex) {
				if (IT.second->Speed >= 0 && IT.second->Img) {
					IT.second->Tick = (IT.second->Tick + 1) % IT.second->Speed;
					if (!IT.second->Tick) IT.second->Frame = (IT.second->Frame + 1) % IT.second->Img->Frames();
				}
			}
			for (auto iy = 0; iy < TRoom->H(); iy++) {
				for (auto ix = 0; ix < TRoom->W(); ix++) {
					auto F{ TLay->Field->Value(ix,iy) };
					auto
						tx{ ix * TRoom->GW() },
						ty{ iy * TRoom->GH() },
						x{ tx - scrollx },
						y{ ty - scrolly };
					//Chat("True (" << tx << "," << ty << ")\t Scroll(" << scrollx << "," << scrolly << ")\t => (" << x << "," << y << ")"); // Hardcore debug!
					if (F && M->Textures[F] && M->Textures[F]->TexFile.size()) {
						if (!MT->Tex.count(F)) {
							auto TT{ M->Textures[F]};
							MT->Tex[F] = make_shared<_TTex>(TT->TexFile, TT->r, TT->g, TT->b, TT->alpha, TT->AnimSpeed, TT->Frame, TT->Type);
						}
						
						if (MT->Tex[F])	MT->Tex[F]->Draw(x, y, TRoom->GW(), TRoom->GH(), tx, ty);
					}
				}
			}
		}

	};
	SuperTed_TQSG TheDriver{};



	void SuperTed_InitTQSG(JT_Dir* J) {
		_Teddy::SetDrawDriver(&TheDriver);
		TEXJCR = J;
		Chat("SuperTed TQSG driver initiated");
	}


}