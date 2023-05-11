// Lic:
// SuperTed - Editor
// Save
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

#include <SuperTed_Save.hpp>

#include <QCol.hpp>
#include <QuickStream.hpp>
#include <Ask.hpp>


#include "Globals.hpp"
#include "Opslaan.hpp"

#define __DPanic TeddyPanicFunction = TCrash
#define __ZLIB_Broken

using namespace june19;
using namespace TrickyUnits;

namespace SuperTed {
	namespace Editor {

		void TCrash(std::string e) { Throw(e); }
		
		void MenuSave(j19gadget*, j19action) {
			__DPanic;
			SaveMap();
		}

		void CreateMap(std::string f) { 
			__DPanic;
			QCol->Doing("Creating", f);
			static vector<string> BaseRoom{ "__BASE" };
			if (!ProjectConfig.ListCount("LAYERS", "LAYERS")) {
				ProjectConfig.Add("LAYERS", "LAYERS", "FLOORS");
				ProjectConfig.Add("LAYERS", "LAYERS", "WALLS");
			}
			TheMap = CreateTeddy(
				AskInt(&ProjectConfig,"Default Map Format","Width","Default width in tiles:",64),
				AskInt(&ProjectConfig, "Default Map Format", "Height", "Default height in tiles:", 64),
				AskInt(&ProjectConfig, "Grid", "Width", "Default Tile Width:", 32),
				AskInt(&ProjectConfig, "Grid", "Height", "Default Tile Height:", 32),
				BaseRoom,
				*ProjectConfig.List("LAYERS","LAYERS"),
				*ProjectConfig.List("LAYERS", "ZONES")
			);			
			SaveMap(f);
		}

		void CreateMap() { CreateMap(EdtProjectMapFile()); }

		void LoadMap(std::string f, bool CreateIfNonExistent) {
			__DPanic;
			if (!FileExists(f)) {
				if (CreateIfNonExistent) {
					CreateMap(f);
					return;
				} else {
					Throw("Map '" + f + "', could not be loaded. File not found");
					return;
				}
			}
			QCol->Doing("Loading", f);
			TheMap = LoadTeddy(f, ProjectConfig.Value("Save", "Prefix"));
			if (!TheMap) Throw("Map not properly loaded");
			TheMap->BaseZones = *ProjectConfig.List("LAYERS", "ZONES");
			TheMap->BaseLayers = *ProjectConfig.List("LAYERS", "LAYERS");
		}


		void LoadMap() { LoadMap(EdtProjectMapFile()); }
		void SaveMap() { SaveMap(EdtProjectMapFile()); }
		void SaveMap(std::string f) {
			if (!TheMap) {
				QCol->Warn("Map was never properly loaded, so it cannot be saved either!");
				return;
			}
			QCol->Doing("Saving", f);
			#ifdef __ZLIB_Broken
			if (ProjectConfig.Value("Save", "Storage") != "Store") {
				if (ProjectConfig.Value("Save", "Storage") != "")
					QCol->Warn("Compression storage method is set to '" + ProjectConfig.Value("Save", "Storage") + "'. Due to trouble with the compression this has been set to 'Store'");
				ProjectConfig.Value("Save", "Storage", "Store");
			}
			#endif
			TeddySave(
				TheMap,
				f,
				ProjectConfig.Value("Save", "Prefix"),
				Ask(&ProjectConfig, "Save", "Storage", "Please name the storage method: ", "zlib")
			);
		}
	}
}