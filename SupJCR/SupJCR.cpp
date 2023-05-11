// Lic:
// SuperTed JCR6 Quick Handler
// Code
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
// Version: 23.05.11
// EndLic
#include "SupJCR.hpp"
#include <jcr6_zlib.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvString.hpp>

#define sJCR6 Slyvina::JCR6

//using namespace jcr6;

namespace Slyvina {
	using namespace Units;

	namespace SuperTed {
		namespace JCR6 {

			static bool Loaded{ false };
			static Slyvina::JCR6::JT_Dir STEDA;

			static GINIE STEDA_ID;

			Slyvina::JCR6::JT_Dir STED_Assets(std::string d) {

				if (!Loaded) {
					QCol->Doing("Initializing", "JCR6"); // init_JCR6(); // Will be done automatically
					QCol->Doing("Initializing", "JCR6 zlib driver"); sJCR6::init_zlib();
					QCol->Doing("Analyzing", "SuperTed.JCR");
					STEDA = sJCR6::JCR6_Dir(d + "/SuperTed.JCR");
					STEDA_ID = ParseGINIE(STEDA->GetString("ID/ID.ini"));
					QCol->Doing("Checking", "SuperTed.JCR");
					if (Lower(STEDA_ID->Value("ID", "Sig")) != "fbeb79a1") { QCol->Error("SuperTed.JCR signature incorrect!"); exit(255); }
					QCol->Doing("JCR file build", STEDA_ID->Value("Build", "Date"));
				}
				return STEDA;
			}
		}
	}
}