#include "HelloTriangleApplication.h"

#include <vector>
#include <set>

#include <limits>
#include <algorithm>


static const std::vector<const char*> arr_validation_layers{
	"VK_LAYER_KHRONOS_validation"
};

static const std::vector<const char*> arr_device_extensions = { //Ӳ����������Ҫ֧�ֵ���չ

	//Ӳ���Ƿ�֧��swapchain�� �����֧��present_queue�Ļ���һ��֧��swap chain�ģ���һ����ϰ������Ȼcheckһ��
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
	vkDestroySwapchainKHR(m_device, m_swap_chain, nullptr); //������logical device��instance֮ǰdelete
	
	
	
	vkDestroyDevice(m_device, nullptr);
	vkDestroyInstance(m_instance, nullptr);//��֤���һ��ɾ��

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


		//���������ã�ֻ����vkCreateInstance��vkDestroyInstance��ʱ����ã�ʹ��CreateDebugUtilsMessengerEXT���Դ���һ��ȫ�ֵ�debugMessenger
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
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);//��ȡSDK֧�ֵ�layer
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

	//��ȡSDK֧�ֵ���չ
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
	if (are_extensions_supported) //һ����ȷ��swap chain extension��֧�����ٲ�ѯswap chain�������Ƿ��㹻
	{
		SwapChainSupportDetails details = querySwapChainSupport(device);

		//֧���κ���ʽ��surface format��present mode����
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
		vkGetPhysicalDeviceSurfaceSupportKHR(device, family_index, m_surface, &present_support); //�Ƿ�֧���������
		if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) //֧�ֽ��л���
		{
			indices.graphics_family = family_index;
		}
		if (present_support)//֧�ֻ������
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

	VkPhysicalDeviceFeatures device_features{};//�ȿհ�

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
	//swap chain֧�ֵ�����
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);
	//swap chain֧�ֵ�format
	uint32_t ui32_format_count{ 0 };
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &ui32_format_count, nullptr);
	if (ui32_format_count)
	{
		details.arr_formats.resize(ui32_format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &ui32_format_count, details.arr_formats.data());
	}
	//swap chain֧�ֵ�presentģʽ
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

	uint32_t image_count = swapChainSupport.capabilities.minImageCount + 1; //������swap chain�ڵ�ͼ��������֧�ֵ����ٵ�ͼ������+1
	//����Ҳ�𳬹�maxImageCount
	//���maxImageCount��ֵ������ֵ0��˵��֧�ֵ�ͼ������û������
	if (swapChainSupport.capabilities.maxImageCount > 0 && image_count > swapChainSupport.capabilities.maxImageCount)
	{
		image_count = swapChainSupport.capabilities.maxImageCount;
	}

	//��ʽ����swapchain
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = image_count;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1; //ͼ���ڲ���Ϊ����layer
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


	/*
	������������Ҫ������Ҫ�硾ִ�ж��С���swap chain�ڲ���ͼ�񣻵�Ӳ����graphics queue family��presentation queue family
		��һ����ʱ����ǵ��͵��������������graphics queue family��Ⱦͼ��Ȼ���ύ��presentation queue family����ͼ�������
	���������ַ�����������Щͼ�񡪡�
	1.VK_SHARING_MODE_EXCLUSIVE: һ��ͼ�������Ȩһ��ʱ��ֻ��һ��queue familyռ�У�����queue family��ʱ�������ʽ�ؽ�����
		��Ȩ��ת�ơ����ַ������Ч
	2.VK_SHARING_MODE_CONCURRENT:һ��ͼ������ڶ��queue��ʹ�ö�������ʽת��ͼ������Ȩ�����Ǳ�����ǰָ����Щfamily queue��
		�Թ������swap chain�����ͼ��
	*/
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value()};
	if (indices.graphics_family != indices.present_family) //������ͬ��queue family
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;//ָ��graphics queue family��presentation queue family����
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	/*
	���ǿ��Ը�swap chain���һ��tranform�����transform������ʱ�����ڳ��֣�presentation������Ļǰ������90����ת����ˮƽ��ת
	���������Ӿ�ֱ��ʹ��Ĭ�ϵ�
	*/
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;


	/*
	compositeAlpha�ĺ������ڴ���ϵͳ���ںϣ�composite������surfaceʱ������alphaͨ������Ϊ
	һ��������ģʽ��
	1.VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR:��composite��������ȫ����image��alphaͨ������Զ����1.0������
	2.VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR:��composite�����п���ͼ���alphaͨ��������Ĭ�Ϸ�alphaͨ����ֵ�Ѿ���Ӧ�ò�
		��������alphaֵ
	3.VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR:��composite�����п���ͼ���alphaͨ��������Ĭ�Ϸ�alphaͨ����ֵ��û����Ӧ��
		�������������alphaֵ��composite���̻������һ����
	4.VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR:vulkan������composite�����д���alphaͨ������ȫ�ɴ���ϵͳ���������ô���ϵͳ����alpha
		compositionģʽ���û������Ρ�

	�����alpha composition�����Ƕ��͸�����ڻ���͸�����ں�����Ԫ�صĻ�ϣ��Ǵ���ϵͳ��ְ�𣬲���shader�����ϵ�blending
	*/
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //��������ϣ�����ǵĴ����ǲ�͸����

	createInfo.presentMode = prensentMode;

	/*
	���ѡ�����vulkan�Ƿ�������Щ����ס�������ϵ���Ⱦ��Ϊ�����統��һ�����ڸ�ס��vulkan��Ⱦ���ڵ�һ����ʱ������ס�Ĳ��ֵ������Ƿ�һ��
		ִ����Ⱦ������
	������ѡ������Ϊtrue����ô����������Խ���Щ����ס���ض��������õ���ֵ��δ����ġ�ƬԪ��ɫ���Ƿ�����Щ������ִ�����ǲ�ȷ�����п���û
		��ִ�С����������ѡ����Ϊtrue����һ����֤ĳ��clip�ᷢ����ֻ���������Ч��present�����ᱻʹ��
	������ѡ������Ϊtrue����ô��Щ����ס�����ػ���û����ס������һ������
	����û�����Ҫ������ͼ����أ�����ƬԪ��ɫ��������Ϊ������Щ����ס���ض�����һЩ����Ч������ô�û�Ӧ�ý����ѡ������Ϊtrue����Ȼenable
	clip�������кô�
	*/
	createInfo.clipped = VK_TRUE;


	/*
	������һЩ�������Ҫ��ȫ�����µ�swap chain�����統���ڱ�resize��ʱ��
	����ĳ��surface�ϴ���swap chain��ʱ�����surface�����Ѿ���һ��������swap chain�ˣ����ʱ��Ϳ��������ѡ�����
	����oldSwapchain������Դ���������кô���
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
	blanking����Ļ���ֻ����ʱ���ƶ�������ָ�롱��ʱ��������Ϊ���֣�
		1��horizontal blanking����ɨ�����ش�һ�еĽ�β�ƶ�����һ�п�ͷ��ʱ��
		2��vertical blanking����ɨ�����ش�һ֡�����һ�еĽ�β�ƶ�����һ֡��һ�еĿ�ͷ��ʱ����
	һ��4��ģʽ��
	1.VK_PRESENT_MODE_IMMEDIATE_KHR:Ӧ�ó����ύ��ͼ��������ת�Ƶ���Ļ�ϣ������˺��
	2.VK_PRESENT_MODE_FIFO_KHR:swap chain��һ��queue����ʾ���Ӷ���ͷ��ȡ�����棬Ӧ�ó���������β�����뻭�档����������ʱ��Ӧ�ó������ȴ�
	3.VK_PRESENT_MODE_RELAXED_KHR:���ǵڶ���ģʽ��һ�����塣�����ڵ�queueΪ�ն���һ��vertical blank��û�������ڼ䵽�����µ�ͼ��ģʽ2���
		����һ��vertical blanking�������µ�һ֡����Ⱦ�������ģʽ����ȴ�����ֱ�ӽ���������Ļ�Ͻ���ˢ�£������ᵼ���µĻ������Ļ�м�ĳһ��λ��
		��ʼˢ�º;ɵĻ��������˺��
	4.VK_PRESENT_MODE_MAILBOX_KHR������ģʽҲ��ģʽ2��һ�ֱ��塣�������������������ʱ�򲻻���Ӧ�ó���ȴ�������ֱ�����µĻ��潫�����оɵĻ���
		�滻��������ģʽһ�㱻������triple buffering��
	*/


	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR; //���extension����֧�ֵ�һ��ģʽ
}

VkExtent2D HelloTriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	/*
	swap extent ����swap chainͼ��ķֱ��ʡ�һ����������Ǻ����ǻ��ƵĴ��ڵ����طֱ�������ȵġ�Vulkan����VkSurfaceCapabilitiesKHR��
	currentExtent��Ա����������趨���ڵ����طֱ��ʣ�����ֱ���þ���
	�����еĴ��ڹ��������������ֶ����������ʺϴ��ڵķֱ��ʣ����ʱ��currentExtent�Ĵ�С��һ�������ֵ��uint32_t�����ֵ�����������趨����
	�ֱ��ʱ�����minImageExtent��maxImageExtent�ķ�Χ��
	����֮�����Ǳ���ע��ֱ��ʵĵ�λ��GLFW�������ֱ��ʵ�ʱ����õ����ֵ�λ����1�����أ�2����Ļ���ꡣ���Ǵ���GLFWWindow��ʱ����ķֱ�����
	��Ļ���ꡣ��Vulkan������ʱ��ֻ����Ļ������һ����λ��������Ǳ���Ҫ��֤���ڵ���Ⱦ��С�ĵ�λҲ�����ء�
	��һЩ��DPI��ʾ���ϣ���Ļ���겢���Ǻ�����һһ��Ӧ�ġ��������ص��ܶȽϸߣ�����һ����λ����Ļ�����Ӧ������ء����ʱ�����ǲ��ܵ������ṩ��
	�ڴ�С��Vulkan�����ǻ�ȡframebuffer��С��framebuffer��С�ǻ������صġ�
	*/

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) //Vulkanָ���˺��ʵ����طֱ���
	{
		return capabilities.currentExtent;
	}
	else {//�����Լ����ֶ�����
		int width{ 0 }, height{ 0 };
		glfwGetFramebufferSize(m_window.get(), &width, &height);
		VkExtent2D actualExtent{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actualExtent;
	}
}


