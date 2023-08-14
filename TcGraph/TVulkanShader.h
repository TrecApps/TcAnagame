#pragma once
#include <TDataArray.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <TFile.h>
#include <map>

using vk_shader_stage = enum class vk_shader_stage {
	vk_vertex,
	vk_fragment
};

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

		void Initialize(TrecPointer<TVariable> shaderVars, TrecActivePointer<TFileShell> sourceFile);
	};
private:
	TDataArray< VkPipelineShaderStageCreateInfo > shaderStages;
	TDataArray<std::string> functionNames;

	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;
	VkDevice device;
	VkRenderPass renderPass;
	std::map<VkPrimitiveTopology, VkPipeline> graphicsPipelines;
protected:
	bool GetStage(vk_shader_stage stage, const TString& strStage, const TString& defaultFunction, TrecPointer<TFileShell> fileToRead, VkPipelineShaderStageCreateInfo& info);
	void RemovePipeline();

	void SetShader(VkPrimitiveTopology topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
public:
	TVulkanShader();
	TVulkanShader(const TVulkanShader& copy);
	~TVulkanShader();
	void Initialize(VkDevice device, VkRenderPass renderPass, const VulkanShaderParams& params);
	VkPipeline GetPipeline(VkPrimitiveTopology topology);

	void SetUpShader(VkPrimitiveTopology topology);
};

