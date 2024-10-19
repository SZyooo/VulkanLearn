#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include "winconfig.h"
#include <vector>
#include "defines.h"

#include<optional>





struct QueueFamilyIndices {

	//寻找支持绘制和画面输出的family queue。有可能两个family是同一个。
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	bool isComplete() {
		return graphics_family.has_value() && present_family.has_value();
	}
};

//我们首先需要check一下硬件是否支持swap chain，然后需要知道硬件支持的swap chain的一些属性——
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> arr_formats;
	std::vector<VkPresentModeKHR> arr_present_modes;
};



class HelloTriangleApplication
{
public:
	HelloTriangleApplication() 
		:m_window_size{ 600, 800 }, 
		m_window(nullptr, [](void*) {}), 
		m_instance{}, 
		m_debugMessenger(nullptr),
		m_physicalDevice(VK_NULL_HANDLE),
		m_device(VK_NULL_HANDLE),
		m_graphicsQueue(VK_NULL_HANDLE),
		m_presentQueue(VK_NULL_HANDLE),
		m_surface(VK_NULL_HANDLE),
		m_swap_chain{VK_NULL_HANDLE} {}
	~HelloTriangleApplication() = default;
	HelloTriangleApplication(const HelloTriangleApplication&) = delete;
	HelloTriangleApplication& operator=(const HelloTriangleApplication&) = delete;
	HelloTriangleApplication(HelloTriangleApplication&&) = default;
	HelloTriangleApplication& operator=(HelloTriangleApplication&&) = default;

public:
	void run();

private:
	void initVulkan();
	void mainLoop();
	void cleanup();
	void initWindow();
	void createInstance();
	bool checkValidationLayerSupported() const;
	int checkInstanceExtensionSupported(const char* const* exts_to_check, const uint32_t num, uint32_t* __OUT__ not_supported) const;
	std::vector<const char*> getRequiredExtensions() const;
	void setupDebugMessenger();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);


	/*
	Vulkan标准中，物理设备为用户提供了一系列的工作队列，不同的工作队列按照其功能分类，分别处理不同类型的命令。一共分为下面五个队列类型：
	1.VK_QUEUE_GRAPHICS_BIT:支持图形功能
	2.VK_QUEUE_COMPUTE_BIT:支持计算功能（计算着色器）
	3.VK_QUEUE_TRANSFER_BIT:支持转移功能（资源的布局转移以及跨队列的资源转移）
	4.VK_QUEUE_SPARSE_BINDING_BIT:支持稀疏绑定功能（管理稀疏内存）
	5.VK_QUEUE_PROTECTED_BIT:支持受保护功能（主要用于受保护内存的管理）
	*/
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	void createLogicalDevice();

	void createSurface();

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	void createSwapChain();

private:
	
	WindowSize m_window_size;
	
	std::unique_ptr<GLFWwindow, void(*)(void*)> m_window;

	VkDebugUtilsMessengerEXT m_debugMessenger;
private:
	VkInstance m_instance;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;

	//随着logical device自动创建和销毁
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	VkSurfaceKHR m_surface;

	VkSwapchainKHR m_swap_chain;

	std::vector<VkImage> m_swapchain_images;
	VkFormat m_swapchain_image_format;
	VkExtent2D m_swapchain_image_extent;

private:
	/*设置正确的swap chain设置
	主要包含三个方面：
		1：Surface Format（颜色通道、深度通道）
		2：Presentation Mode
		3：Swap extent（交换的image大小）
	*/
	//上传git
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	

};

