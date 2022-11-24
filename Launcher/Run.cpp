// Lic:
// SuperTed - Launcher
// Run
// 
// 
// 
// (c) Jeroen P. Broks, 2022
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
// Version: 22.11.24
// EndLic

#define ESCAPE_IS_QUIT

#include <TQSE.hpp>
#include <TQSG.hpp>

#include <QCol.hpp>
#include <TrickyMath.hpp>
#include <QuickString.hpp>

#include "Glob.hpp"
#include "Run.hpp"

using namespace TrickyUnits;

namespace SuperTed {
	namespace Launcher {
		static TQSG_AutoImage Background{ nullptr };

		static bool ErrorThrown{ false };
		static void Err(std::string e) {
			ErrorThrown = true;
			QCol->Error(e);
		}

		void LoadBackGround() {
			QCol->Doing("Loading", "Background image");
			Background = TQSG_LoadAutoImage(JAS, "IMG/Background.png");
			auto a{ JCR_Error };
			std::string b{ SDL_GetError() };
			if (a != "" && Upper(Trim(a)) != "OK") Err("JCR6 ERROR>" + a);
			if (b != "") Err("SDL2 ERROR>" + b);
			if (!Background->Frames()) Err("For some reasons beyond me, the background didn't load");
		}

		bool Run() {
			if (ErrorThrown) return false;
			bool KeepGoing{ true };
			static int r{ 0 };
			TQSE_Poll();
			TQSG_Cls();
			Background->Tile(0, 0, TQSG_ScreenWidth(), TQSG_ScreenHeight(), floor((TQSG_ScreenWidth() / 2) + (DegSin(r) * 250)), floor((TQSG_ScreenHeight() / 2) + (DegCos(r) * 250)));
			r = (r + 1) % 360;

			KeepGoing = KeepGoing && !TQSE_Quit();
#ifdef ESCAPE_IS_QUIT
			KeepGoing = KeepGoing && !TQSE_KeyHit(SDLK_ESCAPE);
#endif
			TQSG_Flip(30);
			return KeepGoing;
		}
	}
}