#include "TVulkanShader.h"
#include <TContainerVariable.h>
#include <cassert>
#include <stdexcept>

void TVulkanShader::VulkanShaderParams::Initialize(TrecPointer<TVariable> shaderVars)
{
	TrecPointer<TJsonVariable> jVars = TrecPointerKey::ConvertPointer<TVariable, TJsonVariable>(shaderVars);

	TrecPointer<TVariable> value;
	if (jVars->RetrieveField(L"RootDirectory", value)) {
		this->rootDirectory.Set(TStringVariable::Extract(value, L""));
	}

	value.Nullify();
	if (jVars->RetrieveField(L"DefaultFunction", value)) {
		this->defaultFunction.Set(TStringVariable::Extract(value, L""));
	}

	value.Nullify();
	if (jVars->RetrieveField(L"VertexShader", value)) {
		this->vertexShader.Set(TStringVariable::Extract(value, L""));
	}

	value.Nullify();
	if (jVars->RetrieveField(L"FragmentShader", value)) {
		this->fragmentShader.Set(TStringVariable::Extract(value, L""));
	}

	value.Nullify();
	if (jVars->RetrieveField(L"VertexFunction", value)) {
		this->vertexFunction.Set(TStringVariable::Extract(value, L""));
	}

	value.Nullify();
	if (jVars->RetrieveField(L"FragmentFunction", value)) {
		this->fragmentFunction.Set(TStringVariable::Extract(value, L""));
	}

	value.Nullify();
	this->isVertexShaderCompiled = true;
	if (jVars->RetrieveField(L"IsVertexCompiled", value) && value.Get()) {
		this->isVertexShaderCompiled = value->Get4Value();
	}

	value.Nullify();
	this->isFragmentShaderCompiled = true;
	if (jVars->RetrieveField(L"IsFragmentCompiled", value) && value.Get()) {
		this->isFragmentShaderCompiled = value->Get4Value();
	}

}



bool TVulkanShader::GetStage(
	vk_shader_stage stage,						// The shader stage we're looking at
	const TString& strStageFunction,			// The function name to look for
	const TString& defaultFunction,				// The default function name
	TrecPointer<TFileShell> fileToRead,			// The File holding the shader code
	VkPipelineShaderStageCreateInfo& stageInfo	// THe Pipeline Stage info structure to construct
)		// Returns: true if successful and "stageInfo" data is valid, false otherwise
{
	TFile file(fileToRead, TFile::t_file_open_existing | TFile::t_file_read);

	if (!file.IsOpen())
		return false;
	
	TDataArray<char> code;
	char buffer[50];
	UINT bytesRead = 0;
	while (bytesRead = file.Read(buffer, 50)) {
		for (UINT Rust = 0; Rust < bytesRead; Rust++)
			code.push_back(buffer[Rust]);
	}

	VkShaderModuleCreateInfo info;
	info.codeSize = code.Size();
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &info, nullptr, &shaderModule) != VK_SUCCESS) {
		return false;
	}
	switch (stage) {
	case vk_shader_stage::vk_vertex:
		stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case vk_shader_stage::vk_fragment:
		stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	default: // Unknown Value
		file.Close();
		return false;
	}
	

	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo.module = shaderModule;
	if (strStageFunction.GetSize())
		functionNames.push_back(strStageFunction.GetRegString());
	else functionNames.push_back(defaultFunction.GetRegString());
	stageInfo.pName =
		functionNames.at(functionNames.Size() - 1).c_str();

	return true;
}

void TVulkanShader::RemovePipeline()
{
	if (!device)
		return;
	if (graphicsPipeline)
		vkDestroyPipeline(device, graphicsPipeline, nullptr);
	if (pipelineLayout)
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

	graphicsPipeline = nullptr;
	pipelineLayout = nullptr;
}

void TVulkanShader::SetShader(VkPrimitiveTopology topology)
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = topology;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = shaderStages.Size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

TVulkanShader::TVulkanShader()
{
	graphicsPipeline = nullptr;
	pipelineLayout = nullptr;
	device = nullptr;
	renderPass = nullptr;
}

TVulkanShader::~TVulkanShader()
{
	for (UINT Rust = 0; Rust < shaderStages.Size(); Rust++) {
		auto& shaderModule = shaderStages[Rust].module;
		vkDestroyShaderModule(device, shaderModule, nullptr);
		shaderModule = nullptr;
	}
}



void TVulkanShader::Initialize(VkDevice device, VkRenderPass renderPass, const VulkanShaderParams& params)
{
	RemovePipeline();
	if (!device) return;
	this->device = device;
	this->renderPass = renderPass;

	TrecPointer<TFileShell> rootDirectory = TFileShell::GetFileInfo(params.rootDirectory);

	assert(rootDirectory.Get());

	// Vertex Shader
	TrecPointer<TFileShell> shaderFile = TFileShell::GetFileInfo(rootDirectory->GetPath() + TC_FILE_SEP + params.vertexShader);
	VkPipelineShaderStageCreateInfo stageInfo;
	if (!GetStage(vk_shader_stage::vk_vertex, params.vertexFunction, params.defaultFunction, shaderFile, stageInfo))
	{
		throw std::runtime_error("Failed to Create Vertex Shader");
	}
	shaderStages.push_back(stageInfo);

	// Fragment Shader
	shaderFile = TFileShell::GetFileInfo(rootDirectory->GetPath() + TC_FILE_SEP + params.fragmentShader);
	if (!GetStage(vk_shader_stage::vk_fragment, params.fragmentFunction, params.defaultFunction, shaderFile, stageInfo))
	{
		throw std::runtime_error("Failed to Create Fragment Shader");
	}
	shaderStages.push_back(stageInfo);

	// To-Do: Address other shaders as Anagame develops


	// End To-Do
	SetShader();
}
