#pragma once
#include <string>
#include <jcr6_core.hpp>
namespace SuperTed {
	namespace Launcher {
		extern std::string MyDir;
		extern jcr6::JT_Dir* JAS;
		std::string ProjectsDir();
	}
}