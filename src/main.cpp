#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "../include/engine.hpp"

int main() {
	rve::Engine engine{};
	
	try {
		engine.Run();
	} catch (const std::exception &exception) {
		std::cerr << exception.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}