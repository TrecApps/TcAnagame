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

void TVulkanShader::Initialize(VkDevice device, const VulkanShaderParams& params)
{
	TrecPointer<TFileShell> rootDirectory = TFileShell::GetFileInfo(params.rootDirectory);

	assert(rootDirectory.Get());

	TrecPointer<TFileShell> shaderFile = TFileShell::GetFileInfo(rootDirectory->GetPath() + TC_FILE_SEP + params.vertexShader);
	TFile file(shaderFile, TFile::t_file_open_existing | TFile::t_file_read);
	assert(file.IsOpen());

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
		throw std::runtime_error("failed to create shader module!");
	}
	VkPipelineShaderStageCreateInfo pipelineInfo;
	pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	pipelineInfo.module = shaderModule;
	if (params.vertexFunction.GetSize())
		functionNames.push_back(params.vertexFunction.GetRegString());
	else functionNames.push_back(params.defaultFunction.GetRegString());
	pipelineInfo.pName =
		functionNames.at(functionNames.Size() -1).c_str();
	this->shaderStages.push_back(pipelineInfo);

	// Fragment Shader
	file.Close();

	code.RemoveAll();
	shaderFile = TFileShell::GetFileInfo(rootDirectory->GetPath() + TC_FILE_SEP + params.fragmentShader);
	file.Open(shaderFile, TFile::t_file_open_existing | TFile::t_file_read);
	assert(file.IsOpen());

	while (bytesRead = file.Read(buffer, 50)) {
		for (UINT Rust = 0; Rust < bytesRead; Rust++)
			code.push_back(buffer[Rust]);
	}

	info.codeSize = code.Size();
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(device, &info, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
	pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pipelineInfo.module = shaderModule;
	if (params.fragmentFunction.GetSize())
		functionNames.push_back(params.fragmentFunction.GetRegString());
	else functionNames.push_back(params.defaultFunction.GetRegString());
	pipelineInfo.pName =
		functionNames.at(functionNames.Size() - 1).c_str();
	this->shaderStages.push_back(pipelineInfo);

	//vkCreateGraphicsPipelines()
}
