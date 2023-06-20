#pragma once
#include <TDataArray.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <TFile.h>
class TVulkanShader
{
public:
	class VulkanShaderParams {
	public:
		TString rootDirectory;
		TString defaultFunction;

		TString vertexShader;
		TString vertexFunction;
		bool isVertexShaderCompiled;

		TString fragmentShader;
		TString fragmentFunction;
		bool isFragmentShaderCompiled;

		void Initialize(TrecPointer<TVariable> shaderVars);
	};
private:
	TDataArray< VkPipelineShaderStageCreateInfo > shaderStages;
	TDataArray<std::string> functionNames;
public:
	void Initialize(VkDevice device, const VulkanShaderParams& params);
};

