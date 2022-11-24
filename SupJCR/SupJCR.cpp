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