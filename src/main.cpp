#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "../include/rve_engine.hpp"

int main() {
	rve::RveEngine engine{};

	try {
		engine.Run();
	} catch (const std::exception &exception) {
		std::cerr << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
