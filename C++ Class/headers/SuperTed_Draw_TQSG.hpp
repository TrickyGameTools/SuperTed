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
