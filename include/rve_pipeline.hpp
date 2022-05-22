#pragma once

#include <string>
#include <vector>

namespace rve {
	class RvePipeline {
	private:
		static std::vector<char> ReadFile(const std::string& filePath);
		void CreateGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath);
		
	public:
		RvePipeline(const std::string& vertFilePath, const std::string& fragFilePath);
		~RvePipeline();
		RvePipeline(const RvePipeline &) = delete;
		RvePipeline &operator=(const RvePipeline &) = delete;
	};
}