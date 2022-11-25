
#include <TQSE.hpp>

#include "Globals.hpp"

using namespace TrickyUnits;

namespace SuperTed {
	namespace Editor {
		bool Run() {
			TQSE_Poll();
			if (TQSE_Quit()) {
				// TODO: Save on quit
				return false;
			}
			return !GetThrown();
		}
	}
}
