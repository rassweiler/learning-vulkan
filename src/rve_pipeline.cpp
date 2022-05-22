#include "../include/rve_pipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace rve {
	RvePipeline::RvePipeline(const std::string& vertFilePath, const std::string& fragFilePath) {
		CreateGraphicsPipeline(vertFilePath, fragFilePath);
	}
	
	RvePipeline::~RvePipeline() {
		
	}
	
	std::vector<char> RvePipeline::ReadFile(const std::string& filePath) {
		std::ifstream fileStream{filePath, std::ios::ate | std::ios::binary};
		
		if(!fileStream.is_open()){
			throw std::runtime_error("(rve_pipeline.cpp) Failed to open file: " + filePath);
		}
		
		size_t fileSize = static_cast<size_t>(fileStream.tellg());
		std::vector<char> characterBuffer(fileSize);
		fileStream.seekg(0);
		fileStream.read(characterBuffer.data(), fileSize);
		fileStream.close();
		
		return characterBuffer;
	}
	
	void RvePipeline::CreateGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath) {
		auto vertCode = ReadFile(vertFilePath);
		auto fragCode = ReadFile(fragFilePath);
		
		std::cout << "vertex shader code size: " << vertCode.size() << std::endl;
		std::cout << "fragment shader code size: " << fragCode.size() << std::endl;
	}
}