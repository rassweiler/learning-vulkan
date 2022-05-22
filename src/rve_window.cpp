#include "../include/rve_window.hpp"

namespace rve {
	RveWindow::RveWindow(int width, int height, std::string name) : windowWidth{width}, windowHeight{height}, windowName{name} {
		Init();
	}
	RveWindow::~RveWindow(){
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	void RveWindow::Init() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
		
		window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);
	}
	
	bool RveWindow::ShouldClose() {
		return glfwWindowShouldClose(window);
	}
}