#include "HelloTriangleApplication.h"

#include <vector>
#include <set>

#include <limits>
#include <algorithm>


static const std::vector<const char*> arr_validation_layers{
	"VK_LAYER_KHRONOS_validation"
};

static const std::vector<const char*> arr_device_extensions = { //硬件层面上需要支持的扩展

	//硬件是否支持swapchain？ （如果支持present_queue的话就一定支持swap chain的，但一个好习惯是仍然check一下
	VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT,
	const VkDebugUtilsMessengerCallbackDataEXT*, void*);

VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerEXT pDebugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func)
	{
		return func(instance, pDebugMessenger, pAllocator);
	}
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
	create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	create_info.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)debugCallback;
	create_info.pUserData = nullptr;
}

#ifndef NDEBUG
static const bool enable_validation = true;
#else
static const bool enable_validation = false;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
	VkDebugUtilsMessageTypeFlagsEXT msg_type,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

void HelloTriangleApplication::run()
{

	initWindow();
	initVulkan();


	mainLoop();
	cleanup();
	
}

void HelloTriangleApplication::initVulkan()
{
	
	createInstance();
	setupDebugMessenger();
	createSurface();

	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
}

void HelloTriangleApplication::mainLoop()
{
	while (!glfwWindowShouldClose(m_window.get()))
	{
		glfwPollEvents();
	}

}

void HelloTriangleApplication::cleanup()
{

	if (enable_validation)
	{
		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
	}
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr); //必须在logical device和instance之前delete
	
	
	
	vkDestroyDevice(m_device, nullptr);
	vkDestroyInstance(m_instance, nullptr);//保证最后一个删除

	glfwDestroyWindow(m_window.get());
	m_window.release();
	glfwTerminate();
}

void HelloTriangleApplication::initWindow()
{
	if (!glfwInit())
	{
		throw std::runtime_error("init glfw failed!");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window.reset(glfwCreateWindow(m_window_size.width, m_window_size.height, "Vulkan", nullptr, nullptr));
	
}

void HelloTriangleApplication::createInstance()
{


	if (enable_validation && not checkValidationLayerSupported())
	{
		throw std::runtime_error("validation layer requested but not available!");
	}

	VkApplicationInfo app_info{};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "First Triangle";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	//uint32_t glfw_required_extension_count{ 0 };
	//const char** glfw_required_extensions{ nullptr };
	//glfw_required_extensions = glfwGetRequiredInstanceExtensions(&glfw_required_extension_count);
	std::vector<const char*> extension = getRequiredExtensions();
	uint32_t not_supported_ext_count = checkInstanceExtensionSupported(extension.data(), extension.size(), nullptr);
	if (not_supported_ext_count)
	{
		std::vector<uint32_t> indices(not_supported_ext_count, 0);
		checkInstanceExtensionSupported(extension.data(), extension.size(), indices.data());
		for (uint32_t index : indices)
		{
			std::cout << "extension [" << extension[index] << "] is not supported.\n";
		}
		throw std::runtime_error("not all extensions are supported");
	}
	else {
		std::cout << extension.size() << " extensions are required and " << extension.size() << " are supported.\n";
	}

	//uint32_t vk_extension_count{ 0 };
	//vkEnumerateInstanceExtensionProperties(nullptr, &vk_extension_count, nullptr);
	//std::vector<VkExtensionProperties> vk_extensions(vk_extension_count);
	//vkEnumerateInstanceExtensionProperties(nullptr, &vk_extension_count, vk_extensions.data());
	
	//std::cout << "total " << vk_extension_count << " available vk extensions\n";
	//for (int i = 0; i < vk_extensions.size(); ++i)
	//{
	//	std::cout << "\t" << vk_extensions[i].extensionName << '\n';
	//}



	//https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkInstanceCreateInfo.html
	VkInstanceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	
	
	create_info.enabledExtensionCount = extension.size();
	create_info.ppEnabledExtensionNames = extension.data();  //glfw_required_extensions;
	
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	if (enable_validation)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(arr_validation_layers.size());
		create_info.ppEnabledLayerNames = arr_validation_layers.data();
		populateDebugMessengerCreateInfo(debugCreateInfo);


		//在这里设置，只会在vkCreateInstance和vkDestroyInstance的时候调用；使用CreateDebugUtilsMessengerEXT可以创建一个全局的debugMessenger
		create_info.pNext =  &debugCreateInfo; 
		
	}
	else {
		create_info.enabledLayerCount = 0;
		create_info.pNext = nullptr;
	}
	
	
	
	VkResult res = vkCreateInstance(&create_info, nullptr, &m_instance);
	if (res != VK_SUCCESS)
	{
		throw std::runtime_error("cannot create vulkan instance!");
	}
}

bool HelloTriangleApplication::checkValidationLayerSupported() const
{
	uint32_t layer_count{ 0 };
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);//获取SDK支持的layer
	std::vector<VkLayerProperties> layer_properties{ layer_count };
	vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data());
	for (const char* validation_layer : arr_validation_layers)
	{
		bool found = false;
		for (int i = 0; i < layer_count; ++i)
		{
			
			if (strcmp(validation_layer, layer_properties[i].layerName) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			return false;
		}
	}
	return true;
}

int HelloTriangleApplication::checkInstanceExtensionSupported(const char* const* exts_to_check, const uint32_t num, uint32_t* __OUT__ not_supported) const
{

	//获取SDK支持的扩展
	uint32_t vk_extension_count{ 0 };
	vkEnumerateInstanceExtensionProperties(nullptr, &vk_extension_count, nullptr);
	std::vector<VkExtensionProperties> extensions{ vk_extension_count };
	vkEnumerateInstanceExtensionProperties(nullptr, &vk_extension_count, extensions.data());
	uint32_t not_found_count{ 0 };

	uint32_t* write_to = not_supported;

	for (int i = 0; i < num; ++i)
	{
		bool found = false;
		for (int j = 0; j < vk_extension_count; ++j)
		{
			if (strcmp(exts_to_check[i], extensions[j].extensionName) == 0)
			{
				found = true;
				break;
			}
		}
		if (not found)
		{
			not_found_count++;
			if (not_supported)
			{
				*(not_supported++) = i;
			}
		}
	}
	return not_found_count;
}

std::vector<const char*> HelloTriangleApplication::getRequiredExtensions() const
{
	uint32_t glfw_extension_count{ 0 };
	const char** glfw_extensions;
	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
	if (enable_validation)
	{
		extensions.insert(extensions.begin(),VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		//extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

void HelloTriangleApplication::setupDebugMessenger()
{
	if (!enable_validation) return;
	VkDebugUtilsMessengerCreateInfoEXT create_info{};
	populateDebugMessengerCreateInfo(create_info);
	//create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	//create_info.messageSeverity =
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
	//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	//create_info.messageType =
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
	//	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	//create_info.pfnUserCallback = debugCallback;
	//create_info.pUserData = nullptr;
	if (CreateDebugUtilsMessengerEXT(m_instance, &create_info, nullptr, &m_debugMessenger) != VK_SUCCESS) {

		throw std::runtime_error("failed to setup debug messenger.");
	}
}

void HelloTriangleApplication::pickPhysicalDevice()
{
	uint32_t deviceCount{ 0 };
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			m_physicalDevice = device;
			break;
		}
	}
	if (this->m_physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t ui32_extension_count{ 0 };
	vkEnumerateDeviceExtensionProperties(device, nullptr, &ui32_extension_count, nullptr);
	std::vector<VkExtensionProperties> arr_available_extensions(ui32_extension_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &ui32_extension_count, arr_available_extensions.data());
	std::set<std::string> set_required_extensions(arr_device_extensions.begin(), arr_device_extensions.end());
	for (const auto& extension : arr_available_extensions)
	{
		set_required_extensions.erase(extension.extensionName);
	}
	return set_required_extensions.empty();
}

bool HelloTriangleApplication::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);
	bool are_extensions_supported = checkDeviceExtensionSupport(device);
	bool is_swap_chain_adequate = false;
	if (are_extensions_supported) //一定先确保swap chain extension被支持了再查询swap chain的属性是否足够
	{
		SwapChainSupportDetails details = querySwapChainSupport(device);

		//支持任何形式的surface format和present mode都行
		is_swap_chain_adequate = !details.arr_formats.empty() && !details.arr_present_modes.empty(); 
	}

	return indices.isComplete() && are_extensions_supported && is_swap_chain_adequate;

}

QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queue_family_count{ 0 };
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

	int family_index = 0;
	for (const auto& family : queue_families)
	{
		VkBool32 present_support{ false };
		vkGetPhysicalDeviceSurfaceSupportKHR(device, family_index, m_surface, &present_support); //是否支持输出画面
		if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) //支持进行绘制
		{
			indices.graphics_family = family_index;
		}
		if (present_support)//支持画面输出
		{
			indices.present_family = family_index;
		}
		if (indices.isComplete())
		{
			break;
		}
		family_index++;
	}


	return indices;
}

void HelloTriangleApplication::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

	//VkDeviceQueueCreateInfo queue_create_info{};
	//queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	//queue_create_info.queueFamilyIndex = indices.graphics_family.value();
	//queue_create_info.queueCount = 1;

	std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

	float queue_priority{ 1 };
	for (uint32_t queue_family_index : unique_queue_families)
	{
		VkDeviceQueueCreateInfo queue_create_info{};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = queue_family_index;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = &queue_priority;
		queue_create_infos.push_back(queue_create_info);
	}

	VkPhysicalDeviceFeatures device_features{};//先空白

	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pQueueCreateInfos = queue_create_infos.data();
	device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	device_create_info.pEnabledFeatures = &device_features;

	device_create_info.enabledExtensionCount = static_cast<uint32_t>(arr_device_extensions.size());
	device_create_info.ppEnabledExtensionNames = arr_device_extensions.data();


	if (enable_validation)
	{
		device_create_info.enabledLayerCount = static_cast<uint32_t>(arr_validation_layers.size());
		device_create_info.ppEnabledLayerNames = arr_validation_layers.data();
	}
	else {
		device_create_info.enabledLayerCount = 0;
	}
	if (vkCreateDevice(m_physicalDevice, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_device, indices.graphics_family.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.present_family.value(), 0, &m_presentQueue);
}

void HelloTriangleApplication::createSurface()
{
	if (glfwCreateWindowSurface(m_instance, m_window.get(), nullptr, &m_surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}

}

SwapChainSupportDetails HelloTriangleApplication::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	//swap chain支持的属性
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);
	//swap chain支持的format
	uint32_t ui32_format_count{ 0 };
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &ui32_format_count, nullptr);
	if (ui32_format_count)
	{
		details.arr_formats.resize(ui32_format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &ui32_format_count, details.arr_formats.data());
	}
	//swap chain支持的present模式
	uint32_t ui32_present_mode_count{ 0 };
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &ui32_present_mode_count, nullptr);
	if (ui32_present_mode_count)
	{
		details.arr_present_modes.resize(ui32_present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &ui32_present_mode_count, details.arr_present_modes.data());
	}


	return details;
}

void HelloTriangleApplication::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.arr_formats);
	VkPresentModeKHR prensentMode = chooseSwapPresentMode(swapChainSupport.arr_present_modes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t image_count = swapChainSupport.capabilities.minImageCount + 1; //我们让swap chain内的图像数量比支持的最少的图像数量+1
	//但是也别超过maxImageCount
	//如果maxImageCount的值是特殊值0，说明支持的图像数量没有上限
	if (swapChainSupport.capabilities.maxImageCount > 0 && image_count > swapChainSupport.capabilities.maxImageCount)
	{
		image_count = swapChainSupport.capabilities.maxImageCount;
	}

	//正式创建swapchain
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = image_count;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1; //图像内部分为多少layer
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


	/*
	接下来我们需要处理需要跨【执行队列】的swap chain内部的图像；当硬件的graphics queue family和presentation queue family
		不一样的时候就是典型的情况——我们在graphics queue family渲染图像然后提交给presentation queue family进行图像输出。
	我们有两种方案来处理这些图像——
	1.VK_SHARING_MODE_EXCLUSIVE: 一个图像的所有权一个时刻只有一个queue family占有；更换queue family的时候必须显式地进行所
		有权的转移。这种方法最高效
	2.VK_SHARING_MODE_CONCURRENT:一个图像可以在多个queue中使用而无需显式转移图像所有权，但是必须提前指定哪些family queue可
		以共享这个swap chain里面的图像
	*/
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value()};
	if (indices.graphics_family != indices.present_family) //两个不同的queue family
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;//指定graphics queue family和presentation queue family共享
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	/*
	我们可以给swap chain添加一个tranform，这个transform的作用时机是在呈现（presentation）到屏幕前，例如90°旋转或者水平翻转
	如果不想添加就直接使用默认的
	*/
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;


	/*
	compositeAlpha的含义是在窗口系统中融合（composite）两个surface时候处理其alpha通道的行为
	一共有三种模式：
	1.VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR:在composite过程中完全忽视image的alpha通道，永远当作1.0来处理
	2.VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR:在composite过程中考虑图像的alpha通道；并且默认非alpha通道的值已经在应用层
		乘以了其alpha值
	3.VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR:在composite过程中考虑图像的alpha通道；并且默认非alpha通道的值并没有在应用
		程序里面乘以了alpha值，composite过程会完成这一过程
	4.VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR:vulkan不会在composite过程中处理alpha通道，完全由窗口系统来处理；设置窗口系统处理alpha
		composition模式是用户的责任。

	这里的alpha composition好像是多个透明窗口或者透明窗口和桌面元素的混合，是窗口系统的职责，不是shader意义上的blending
	*/
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //我们总是希望我们的窗口是不透明的

	createInfo.presentMode = prensentMode;

	/*
	这个选项决定vulkan是否丢弃在那些被遮住的像素上的渲染行为，例如当另一个窗口盖住了vulkan渲染窗口的一部分时，被遮住的部分的像素是否一定
		执行渲染操作。
	如果这个选项设置为true，那么我们如果尝试将这些被遮住像素读出来，得到的值是未定义的。片元着色器是否在这些像素上执行了是不确定，有可能没
		有执行。不过将这个选项设为true并不一定保证某种clip会发生，只是允许更有效的present方法会被使用
	如果这个选项设置为true，那么这些被遮住的像素会像没被遮住的像素一样处理
	如果用户不需要将整幅图像读回，或者片元着色器不会因为丢弃这些被遮住像素而产生一些错误效果，那么用户应该将这个选项设置为true；显然enable
	clip对性能有好处
	*/
	createInfo.clipped = VK_TRUE;


	/*
	我们在一些情况下需要完全创建新的swap chain，例如当窗口被resize的时候。
	当在某个surface上创建swap chain的时候，这个surface可能已经有一个关联的swap chain了，这个时候就可以在这个选项给出
	给出oldSwapchain对于资源的重用是有好处的
	*/
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	//logical device
	if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swap_chain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	uint32_t image_count{ 0 };
	vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, nullptr);
	m_swapchain_images.resize(image_count);
	vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, m_swapchain_images.data());
	m_swapchain_image_extent = extent;
	m_swapchain_image_format = surfaceFormat.format;


}

VkSurfaceFormatKHR HelloTriangleApplication::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
		return availableFormats[0];
	}
}

VkPresentModeKHR HelloTriangleApplication::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	/*
	blanking：屏幕呈现画面的时候移动“像素指针”的时间间隔。分为两种：
		1：horizontal blanking：当扫描像素从一行的结尾移动到零一行开头的时间
		2：vertical blanking：当扫描像素从一帧的最后一行的结尾移动到下一帧第一行的开头的时间间隔
	一共4种模式：
	1.VK_PRESENT_MODE_IMMEDIATE_KHR:应用程序提交的图像立即被转移到屏幕上，会造成撕裂
	2.VK_PRESENT_MODE_FIFO_KHR:swap chain是一个queue；显示器从队列头部取出画面，应用程序往队列尾部插入画面。当队列满的时候应用程序必须等待
	3.VK_PRESENT_MODE_RELAXED_KHR:这是第二种模式的一个变体。区别在当queue为空而下一个vertical blank还没结束的期间到来了新的图像：模式2会等
		待下一个vertical blanking结束在新的一帧上渲染；而这个模式不会等待而是直接将画面在屏幕上进行刷新，这样会导致新的画面从屏幕中间某一个位置
		开始刷新和旧的画面产生了撕裂
	4.VK_PRESENT_MODE_MAILBOX_KHR：这种模式也是模式2的一种变体。区别在于如果队列满的时候不会让应用程序等待，而是直接用新的画面将队列中旧的画面
		替换掉。这种模式一般被叫做“triple buffering”
	*/


	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR; //这个extension必须支持的一种模式
}

VkExtent2D HelloTriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	/*
	swap extent 就是swap chain图像的分辨率。一般而言它总是和我们绘制的窗口的像素分辨率是相等的。Vulkan会在VkSurfaceCapabilitiesKHR的
	currentExtent成员里面给我们设定窗口的像素分辨率，我们直接用就行
	但是有的窗口管理器允许我们手动地设置最适合窗口的分辨率；这个时候currentExtent的大小是一个特殊的值：uint32_t的最大值。不过我们设定窗口
	分辨率必须在minImageExtent和maxImageExtent的范围内
	除此之外我们必须注意分辨率的单位。GLFW在描述分辨率的时候会用到两种单位：（1）像素（2）屏幕坐标。我们创建GLFWWindow的时候传入的分辨率是
	屏幕坐标。而Vulkan工作的时候只认屏幕像素这一个单位，因此我们必须要保证窗口的渲染大小的单位也是像素。
	在一些高DPI显示器上，屏幕坐标并不是和像素一一对应的。由于像素的密度较高，导致一个单位的屏幕坐标对应多个像素。这个时候我们不能单纯地提供窗
	口大小给Vulkan，而是获取framebuffer大小。framebuffer大小是基于像素的。
	*/

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) //Vulkan指出了合适的像素分辨率
	{
		return capabilities.currentExtent;
	}
	else {//我们自己得手动设置
		int width{ 0 }, height{ 0 };
		glfwGetFramebufferSize(m_window.get(), &width, &height);
		VkExtent2D actualExtent{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actualExtent;
	}
}


