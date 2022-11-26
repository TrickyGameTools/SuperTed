#pragma once


#pragma once
// C++
#include <map>
#include <string>

// June19
#include <june19.hpp>

// TQSG
#include <TQSG.hpp>

// Kthura Editor
//#include "../headers/Config.hpp"



namespace SuperTed {
	namespace Editor {

		typedef void(*UIV)();

		class UI;

		class UI {
		private:
			static bool _initialized;
			static TrickyUnits::TQSG_AutoImage Mouse;
			static std::map<std::string, UI> Stage;
			std::string _Name{};
			UI(std::string name);
			static UI* _Current;
		public:
			UI();

			static void Crash(std::string m);

			june19::j19gadget* MainGadget{ nullptr };
			UIV PreJune{ nullptr };
			UIV PostJune{ nullptr };

			static void AddStage(std::string st);
			static UI* GetStage(std::string st);
			static UI* CurrentStage();
			static void GoToStage(std::string st);

			// Notify
			static void Start();
			static bool Run();
			static void Done();
		};
	}
}