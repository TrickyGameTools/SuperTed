// Lic:
// SuperTed JCR6 Quick Handler
// Code
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
#include "SupJCR.hpp"
#include <jcr6_zlib.hpp>
#include <GINIE.hpp>
#include <QuickString.hpp>

using namespace jcr6;
using namespace TrickyUnits;

namespace SuperTed {
	namespace JCR6 {

		static bool Loaded{false};
		static JT_Dir STEDA;

		static GINIE STEDA_ID;

		jcr6::JT_Dir* STED_Assets(std::string d) {

			if (!Loaded) {
				QCol->Doing("Initializing", "JCR6"); init_JCR6();
				QCol->Doing("Initializing", "JCR6 zlib driver"); init_zlib();
				QCol->Doing("Analyzing", "SuperTed.JCR");
				STEDA = Dir(d + "/SuperTed.JCR");
				STEDA_ID.Parse(STEDA.String("ID/ID.inig"));
				QCol->Doing("JCR file build", STEDA_ID.Value("Build", "Date"));
				if (Lower(STEDA_ID.Value("ID", "Sig")) != "fbeb79a1") { QCol->Error("SuperTed.JCR signature incorrect!"); exit(255); }
			}
			return &STEDA;
		}
	}
}