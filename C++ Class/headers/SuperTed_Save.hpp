// Lic:
// C++ Class/headers/SuperTed_Save.hpp
// SuperTed Save (header)
// version: 23.05.11
// Copyright (C) 2022, 2023 Jeroen P. Broks
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// EndLic
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