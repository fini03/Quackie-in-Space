#include "game/MyVulkanEngine.hpp"

using namespace ve;

int main(int argc, char *argv[]) {
	std::unordered_map<std::string, bool> cmd_flags = {
		{ "-d", false },
		{ "-s", false }
	};

	// "Parse" the arguments
	for (int i = 0; i < argc; i++) {
		cmd_flags[argv[i]] = true;
	}

    MyVulkanEngine mve(
		cmd_flags["-d"],
		cmd_flags["-s"]
	);

	mve.initEngine();
	mve.setup_game();
	mve.run();

	return 0;
}
