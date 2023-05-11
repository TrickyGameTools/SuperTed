// Lic:
// C++ Class/headers/SuperTed_Draw_TQSG.hpp
// SuperTed Drawing driver for the TQSG system (header)
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

#include "SuperTed_Core.hpp"
namespace SuperTed{

	// Will allow this driver to show zones. Should normally always be false. 
	// Only implemented to make map editors possible.
	extern bool ZoneShow;

	/// <summary>
	/// Sets the TQSG driver! Whatch out! If you have maps loaded, some data in them may not be properly disposed.
	/// </summary>
	void SuperTed_InitTQSG(jcr6::JT_Dir *Res);
}