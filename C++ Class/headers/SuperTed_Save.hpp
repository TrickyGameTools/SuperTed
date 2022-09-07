#pragma once

// C++
#include <string>

// JCR6
#include <jcr6_core.hpp>

// SuperTed
#include "SuperTed_Core.hpp"

namespace SuperTed {


	/// <summary>
	/// Saves a SuperTed files in the default format (based on JCR6)
	/// </summary>
	/// <param name="filename">File name</param>
	/// <param name="dir">Directory inside the JCR6 file created</param>
	/// <param name="Storage">Storage method (please note if the drivers are available or not</param>
	void TeddySave(Teddy TeddyMap, std::string filename, std::string dir = "", std::string Storage = "Store");


	void TeddySave(Teddy TeddyMap, jcr6::JT_Create* resource, std::string dir = "", std::string Storage = "Store");

}
