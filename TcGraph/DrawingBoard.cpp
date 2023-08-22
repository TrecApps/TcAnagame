#include <GL/glew.h>
#include "TColorBrush.h"
#include "TImageBrush.h"
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <set>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <TFormatReader.h>


// Vulkan Resources

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}


CharWithSize::CharWithSize(WCHAR ch, int weight)
{
	this->ch = ch;
	this->weight = weight;
}

int CharWithSize::compare(const CharWithSize& other) const
{
	int ret = ch - other.ch;
	if (!ret)
		ret = weight - other.weight;
	return ret;
}

bool CharWithSize::operator<(const CharWithSize& other) const {
	return compare(other) < 0;
}

UINT Animation::GetMillisecondRefresh()
{
	return millisecondRefresh;
}
UINT Animation::GetMillisecondLength()
{
	return millisecondLength;
}
void Animation::SetMillisecondRefresh(UINT mr)
{
	millisecondRefresh = mr;
}
void Animation::SetMillisecondLength(UINT ml)
{
	millisecondLength = ml;
}
bool Animation::DoReverse()
{
	return reverse;
}
void Animation::DoReverse(bool reverse)
{
	reverse = true;
}


class CaretRunner : public TcAsyncRunner
{
	friend class DrawingBoard;
	TrecPointerSoft<DrawingBoard> self;
protected:
	// Should Return true once it is "done" or no more rounds are needed
	virtual bool RunRound() override
	{
		TrecPointer<DrawingBoard> board = TrecPointerKey::TrecFromSoft<>(self);
		if (!board.Get())
			return true;
		if (board->caret.intercepter.Get())
		{
			board->LockDrawing();
			board->caret.OnDraw = !board->caret.OnDraw;
			board->needsRefresh = true;
			board->UnlockDrawing();
		}
#ifdef _WINDOWS
		Sleep(1000);
#else
		sleep(1000);
#endif
		return false;
	}
public:
	TrecPointer<TObject::TVariable> TObject::TVariable::Clone(void)
	{
		TrecPointer<TVariable> ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, CaretRunner>();
		dynamic_cast<CaretRunner*>(ret.Get())->self = this->self;
		return ret;
	}
};

DrawingBoard::DrawingBoard(GLFWwindow* window, VkPhysicalDevice anagameVulkanDevice, VkInstance instance)
{
	imageIndex = 0;
	if (!window) throw 0;
	this->window = window;
	this->instance = instance;
	logicalDevice = 0;
	renderPass = 0;

	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}

	tabSpace = 4;

	cursors.push_back(glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
	cursors.push_back(glfwCreateStandardCursor(GLFW_IBEAM_CURSOR));
	cursors.push_back(glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
	cursors.push_back(glfwCreateStandardCursor(GLFW_HAND_CURSOR));
	cursors.push_back(glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
	cursors.push_back(glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));


	defaultClearColor.SetColor(L"white");

	int w = 0;
	int h = 0;
	area.top = h;
	area.left = w;
	glfwGetWindowSize(window, &w, &h);

	area.bottom = h;
	area.right = w;
	origArea = area;
	origArea = area;
	ResetProjection();

	shaderType = shader_type::shader_2d;
	glfwMakeContextCurrent(window);

	caret.thickness = 2.0f;
	caret.OnDraw = false;

	needsRefresh = true;
	needsConstantRefresh = false;

	TcInitLock(&drawingThread);
	QueueFamilyIndices indices = createLogicalDevice(anagameVulkanDevice);
	createSwapChain(anagameVulkanDevice);
	createImageViews();
	createFramebuffers();


	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

	if (vkCreateCommandPool(this->logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	createSyncObjects();

}

DrawingBoard::QueueFamilyIndices DrawingBoard::createLogicalDevice(VkPhysicalDevice anagameVulkanDevice)
{
	QueueFamilyIndices indices = findQueueFamilies(anagameVulkanDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

	//if (enableValidationLayers) {
	//	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	//	createInfo.ppEnabledLayerNames = validationLayers.data();
	//}
	//else {
	//	createInfo.enabledLayerCount = 0;
	//}

	if (vkCreateDevice(anagameVulkanDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);

	return indices;
}

DrawingBoard::QueueFamilyIndices DrawingBoard::findQueueFamilies(VkPhysicalDevice anagameVulkanDevice)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(anagameVulkanDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(anagameVulkanDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(anagameVulkanDevice, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
	
}

void DrawingBoard::createSwapChain(VkPhysicalDevice physicalDevice)
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(this->logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
	createRenderPass();
}

void DrawingBoard::createRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(this->logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void DrawingBoard::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void DrawingBoard::createFramebuffers() {
	swapChainBuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &swapChainBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void DrawingBoard::createSyncObjects()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(this->logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(this->logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(this->logicalDevice, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to create synchronization objects for a frame!");
	}

}

void DrawingBoard::ResetProjection()
{
	orthoProjection = glm::ortho(0.0f, static_cast<float>(area.right), 0.0f, static_cast<float>(area.bottom));
}

bool DrawingBoard::PrepVulkanShader(TrecPointer<TFileShell>& file)
{
	TVulkanShader shade;
	if (vulkanShaders.retrieveEntry(file->GetPath(), shade))
		return true;
	
	TrecPointer<TFormatReader> reader = TFormatReader::GetReader(file);
	if(!reader.Get())
		return false;

	if (reader->Read().GetSize())
		return false;
	TVulkanShader::VulkanShaderParams params;
	params.Initialize(reader->GetData(), TrecActivePointer(file));

	shade.Initialize(this->logicalDevice, this->renderPass, params);
	vulkanShaders.addEntry(file->GetPath(), shade);
}

bool DrawingBoard::UseVulkanShader(const TString& shader, VkPrimitiveTopology topology)
{
	TVulkanShader shade;
	if(!vulkanShaders.retrieveEntry(shader, shade))
		return false;

	VkPipeline pipeline = shade.GetPipeline(topology);

	shade.RetrieveShaderDescriptors(bindingDesc, attributeDesc);

	vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);




	//shade.
	return true;
}

void DrawingBoard::RetrieveShaderDescriptors(TrecPointer<TDataArray<VkVertexInputBindingDescription>>& b, TrecPointer<TDataArray<VkVertexInputAttributeDescription>>& a)
{
	b = this->bindingDesc;
	a = this->attributeDesc;
}

void DrawingBoard::AddOperation(TrecPointer<GraphicsOp> ops)
{
	TObjectLocker lock(&thread);
	if (ops.Get())
	{
		this->graphicsOperations.push_back(ops);
	}
}

void DrawingBoard::PerformGraphicsOps()
{
	TObjectLocker lock(&thread);
	glfwMakeContextCurrent(window);
	auto drawBoard = TrecPointerKey::TrecFromSoft<>(self);
	for (UINT Rust = 0; Rust < graphicsOperations.Size(); Rust++) {
		graphicsOperations[Rust]->Perform(drawBoard);
	}
	graphicsOperations.RemoveAll();
}

void DrawingBoard::SetFocusObject(TrecPointer<TObject> focusObject)
{
}

UINT DrawingBoard::GetTabSpace()
{
	return tabSpace;
}


bool DrawingBoard::RetrieveFontEntry(const TString& name, std::map<CharWithSize, GLuint>& map)
{
	return charMap.retrieveEntry(name, map);
}

void DrawingBoard::SetFontEntry(const TString& name, const std::map<CharWithSize, GLuint>& map)
{
	charMap.setEntry(name, map);
}

void DrawingBoard::PrepRefresh()
{
	TObjectLocker lock(&thread);
	this->needsRefresh = true;
}

UINT DrawingBoard::PrepConstantRefresh()
{
	TObjectLocker lock(&thread);
	needsConstantRefresh = true;
	for (UINT Rust = 0; Rust < constantRefreshReqs.Size(); Rust++) {
		if (!constantRefreshReqs[Rust]) {
			constantRefreshReqs[Rust] = true;
			return Rust;
		}
	}
	return constantRefreshReqs.push_back(true);
}

void DrawingBoard::ReleaseConstantRefresh(UINT u)
{
	TObjectLocker lock(&thread);
	if (u >= constantRefreshReqs.Size()) return;
		constantRefreshReqs[u] = false;
	for (UINT Rust = 0; Rust < constantRefreshReqs.Size(); Rust++) {
		if (constantRefreshReqs[Rust])
			return;
	}
	needsConstantRefresh = false;
}

void DrawingBoard::SetCursor(ag_mouse_pointer mPointer)
{
	glfwSetCursor(window, cursors[static_cast<UINT>(mPointer)]);
}

void DrawingBoard::NormalizePoint(const TPoint& input, TPoint& output)
{
	auto r = this->area;
	r.bottom /= 2.0f;
	r.right /= 2.0f;

	output.x = (input.x / r.right) - 1.0f;
	output.y = -((input.y / r.bottom) - 1.0f);
}

void DrawingBoard::LockDrawing()
{
	TcLockObject(&drawingThread);
}
void DrawingBoard::UnlockDrawing()
{
	TcUnlockObject(&drawingThread);
}

void DrawingBoard::InitializeCaretRunner()
{
	if (this->caretRunner.Get())
		return;
	TrecPointer<CaretRunner> cRunner = TrecPointerKey::ConvertPointer<TVariable, CaretRunner>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, CaretRunner>());
	cRunner->self = this->self;

	caretRunner = TrecPointerKey::ConvertPointer<CaretRunner, TcAsyncRunner>(cRunner);
	ReturnObject obj;

	caretRunner->Run(obj);
	caretRunner->Pause();

}

void DrawingBoard::DrawCaret()
{
	if (caret.OnDraw && dynamic_cast<TColorBrush*>(caret.brush.Get()))
	{
		//ATLTRACE(L"");
		this->SetShader(TrecPointer<TShader>(), shader_type::shader_2d);

		dynamic_cast<TColorBrush*>(caret.brush.Get())->DrawLine(caret.bottom, caret.top, caret.thickness);
	}
}

void DrawingBoard::SetCaret(TrecPointer<TTextIntercepter> texter, const TPoint& top, const TPoint& bottom, float thickness)
{
	LockDrawing();
	InitializeCaretRunner();
	PauseCaret();

	NormalizePoint(top, caret.top);
	NormalizePoint(bottom, caret.bottom);
	if (!caret.brush.Get())
		caret.brush = GetSolidColorBrush(TColor());

	if (!caret.intercepter.Get())
		caret.intercepter = texter;
	else if (!texter.Get())
		caret.intercepter.Nullify();
	else if (caret.intercepter->GetTarget() != texter->GetTarget());

	if(thickness)
		caret.thickness = thickness;
	UnlockDrawing();
}
void DrawingBoard::SetCaret(TrecPointer<TTextIntercepter> texter, const TPoint& top, const TPoint& bottom, const TColor& color, float thickness)
{
	LockDrawing();
	InitializeCaretRunner();
	PauseCaret();

	NormalizePoint(top, caret.top);
	NormalizePoint(bottom, caret.bottom);
	caret.brush = GetSolidColorBrush(color);

	if (!caret.intercepter.Get())
		caret.intercepter = texter;
	else if (!texter.Get())
		caret.intercepter.Nullify();
	else if (caret.intercepter->GetTarget() != texter->GetTarget());

	if (thickness)
		caret.thickness = thickness;
	UnlockDrawing();
}
void DrawingBoard::ShowCaret()
{
	if (caretRunner.Get())
		caretRunner->Resume();
}
void DrawingBoard::PauseCaret()
{
	if (caretRunner.Get())
		caretRunner->Pause();
}


void DrawingBoard::AddLayer(const RECT_F& ref)
{

	glfwMakeContextCurrent(window);
	if (!layers.Size())
	{
		glEnable(GL_STENCIL_TEST);
	}
	layers.push_back(ref);


	glColorMask(false, false, false, false);
	glDepthMask(false);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClear(GL_STENCIL_BUFFER_BIT);

	
	RECT_F actRect;
	TColorBrush::NormalizeRect(actRect, ref, TrecPointerKey::TrecFromSoft<>(self));

	glBegin(GL_QUADS);
	glColor4f(0, 0, 0, 1.0f);
	glVertex2d(actRect.left, actRect.top);
	glVertex2d(actRect.right, actRect.top);
	glVertex2d(actRect.right, actRect.bottom);
	glVertex2d(actRect.left, actRect.bottom);
	glEnd();

	glColorMask(true, true, true, true);
	glDepthMask(true);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void DrawingBoard::PopLayer()
{
	if (!layers.Size())
		return;

	glColorMask(false, false, false, false);
	glDepthMask(false);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_DECR, GL_DECR, GL_DECR);

	RECT_F actRect;
	TColorBrush::NormalizeRect(actRect, layers.RemoveAt(layers.Size()-1), TrecPointerKey::TrecFromSoft<>(self));
	glBegin(GL_QUADS);
	glColor4f(0, 0, 0, 1.0f);
	glVertex2d(actRect.left, actRect.top);
	glVertex2d(actRect.right, actRect.top);
	glVertex2d(actRect.right, actRect.bottom);
	glVertex2d(actRect.left, actRect.bottom);
	glEnd();

	glColorMask(true, true, true, true);
	glDepthMask(true);

	if (!layers.Size())
		glDisable(GL_STENCIL_TEST);
}

GLuint DrawingBoard::GetTextureShaderId()
{
	if (!shaderTex2D.Get())
		shaderTex2D = TrecPointerKey::GetNewTrecPointerAlt<TShader, Texture2DShader>();
	return shaderTex2D->GetShaderId();
}

void DrawingBoard::SetSelf(TrecPointer<DrawingBoard> s)
{
	if (this != s.Get())
		throw 1;
	self = TrecPointerKey::SoftFromTrec<>(s);
}

RECT_F DrawingBoard::GetArea()
{
	return area;
}

RECT_F DrawingBoard::GetOrigArea()
{
	return origArea;
}

GLFWwindow* DrawingBoard::GetGlfwWindow()
{
	return window;
}

TrecPointer<TBrush> DrawingBoard::GetHighlightBrush()
{
	if (!textHighlight.Get())
	{
		TColor color(L"lightyellow");
		// To-Do: get actual color from property

		// End To-Do

		textHighlight = GetSolidColorBrush(color);
	}
	return textHighlight;
}

bool DrawingBoard::IsContained(const TPoint& point, const RECT_F& loc)
{

	return (loc.bottom >= loc.top) &&
		(loc.right >= loc.left) &&

		(point.x >= loc.left) &&
		(loc.right >= point.x) &&
		(point.y >= loc.top) &&
		(loc.bottom >= point.y);
}

bool DrawingBoard::IsContained(const TPoint& cp, const ELLIPSE_F& el)
{
	return (pow((cp.x - el.point.x), 2) / pow(el.x, 2)
		+ pow((cp.y - el.point.y), 2) / pow(el.y, 2)) <= 1;
}

bool DrawingBoard::GetDisplayResolution(int& width, int& height)
{
	GLFWmonitor* mon = glfwGetWindowMonitor(window);
	if (!mon)
		mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(mon);

	if (!mode)
		return false;
	
	width = mode->width;
	height = mode->height;
	return true;
}

DrawingBoard::~DrawingBoard()
{
	if (commandPool) {
		vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
		commandPool = nullptr;
	}

	for (auto framebuffer : swapChainBuffers) {
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
	}

	if (renderPass)
		vkDestroyRenderPass(this->logicalDevice, renderPass, nullptr);
	renderPass = nullptr;
	vkDestroySurfaceKHR(instance, surface, nullptr);
	glfwDestroyWindow(window);
	window = nullptr;
	TcRemoveLock(&drawingThread);
}

TrecPointer<TColorBrush> gBrush;

void DrawingBoard::BeginDraw() const
{
	glfwMakeContextCurrent(window);

	vkWaitForFences(logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(logicalDevice, 1, &inFlightFence);

	
	vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	vkResetCommandBuffer(commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = this->swapChainBuffers[0]; // To-Do: 
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;

	VkClearValue clearColor = { {{
			defaultClearColor.GetRed(), 
			defaultClearColor.GetGreen(), 
			defaultClearColor.GetRed(), 
			defaultClearColor.GetOpacity()
		}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


	// Former OpenGL Era code

	//glClearColor(
	//	defaultClearColor.GetRed(),
	//	defaultClearColor.GetGreen(), 
	//	defaultClearColor.GetRed(),
	//	defaultClearColor.GetOpacity());

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glLoadIdentity(); //Reset the drawing perspective
	//if(!gBrush.Get())
	//	gBrush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>( this->GetSolidColorBrush(defaultClearColor));
	//gBrush->FillRectangle(this->area);
}

void DrawingBoard::ConfirmDraw()
{
	DrawCaret();
	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(presentQueue, &presentInfo);
	glfwMakeContextCurrent(nullptr);
}

int DrawingBoard::SetShader(TrecPointer<TShader> shader, shader_type shaderType)
{
	if (shader.Get() && currentShader.Get() == shader.Get())
		return currentShader->GetShaderId();
	GLint i = 0;
	switch (shaderType)
	{
	case shader_type::shader_3d:
		assert(shader.Get());
		currentShader = shader;
		break;
	case shader_type::shader_2d:
		if (currentShader.Get())
		{
			glUseProgram(0);
			currentShader.Nullify();
		}
		this->shaderType = shaderType;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



		return -1;
	case shader_type::shader_texture:
		if (!shaderTex2D.Get())
			shaderTex2D = TrecPointerKey::GetNewTrecPointerAlt<TShader, Texture2DShader>();
		currentShader = shaderTex2D;
		break;
	case shader_type::shader_write:
		if (!shaderWrite.Get())
			shaderWrite = TrecPointerKey::GetNewTrecPointerAlt<TShader, TFreeTypeShader>();
		currentShader = shaderWrite;
		currentShader->Use();
		this->shaderType = shaderType;
		i = glGetUniformLocation(shaderWrite->GetShaderId(), "projection");
		glUniformMatrix4fv(i, 1, GL_FALSE,
			glm::value_ptr(orthoProjection));
		return currentShader->GetShaderId();
	}
	this->shaderType = shaderType;
	int ret = -1;
	if (currentShader.Get()) {
		currentShader->Use();
		ret = currentShader->GetShaderId();
	}
	return ret;
}

TrecPointer<TShader> DrawingBoard::GenerateShader(TrecPointer<TFileShell> shaderFile)
{
	return TShader::GenerateShader(shaderFile);
}


TrecPointer<TBrush> DrawingBoard::GetSolidColorBrush(const TColor& color) const
{
	TrecPointer<TColorBrush> ret = TrecPointerKey::GetNewTrecPointer<TColorBrush>();

	ret->colors.push_back(color);
	ret->window = self;
	ret->brushType = brush_type::brush_type_solid;
	return TrecPointerKey::ConvertPointer<TColorBrush, TBrush>(ret);
}

TrecPointer<TBrush> DrawingBoard::GetImageBrush(TrecPointer<TFileShell> file) const
{
	if (!file.Get())
		return TrecPointer<TBrush>();
	TrecPointer<TImageBrush> ret = TrecPointerKey::GetNewTrecPointer<TImageBrush>();

	ret->GenerateImageData(file);
	ret->window = self;
	if (!ret->data)
		return TrecPointer<TBrush>();

	GLenum sourceChannel;

	switch (ret->channelCount)
	{
	case 3:
		sourceChannel = GL_RGB;
		break;
	case 4:
		sourceChannel = GL_RGBA;
		break;
	default:
		return TrecPointer<TBrush>();
	}

	glGenTextures(1, &(ret->textureId));
	glBindTexture(GL_TEXTURE_2D, ret->textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ret->width, ret->height, 0, sourceChannel, GL_UNSIGNED_BYTE, ret->data);
//	glGenerateMipmap(GL_TEXTURE_2D);
	return TrecPointerKey::ConvertPointer<TImageBrush, TBrush>(ret);
}


TrecPointer<TBrush> DrawingBoard::GetImageBrush(UCHAR* data, UINT width, UINT height, GLenum unum) const
{
	if (!data || !width || !height)
		return TrecPointer<TBrush>();
	TrecPointer<TImageBrush> ret = TrecPointerKey::GetNewTrecPointer<TImageBrush>();
	ret->width = width;
	ret->height = height;
	ret->window = self;
	GLuint tex[] = { 0 };
	glGenTextures(1, tex);
	ret->textureId = tex[0];
	glBindTexture(GL_TEXTURE_2D, ret->textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ret->width, ret->height, 0, unum, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return TrecPointerKey::ConvertPointer<TImageBrush, TBrush>(ret);
}