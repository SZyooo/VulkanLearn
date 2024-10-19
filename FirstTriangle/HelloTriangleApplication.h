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

	//Ѱ��֧�ֻ��ƺͻ��������family queue���п�������family��ͬһ����
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	bool isComplete() {
		return graphics_family.has_value() && present_family.has_value();
	}
};

//����������Ҫcheckһ��Ӳ���Ƿ�֧��swap chain��Ȼ����Ҫ֪��Ӳ��֧�ֵ�swap chain��һЩ���ԡ���
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
	Vulkan��׼�У������豸Ϊ�û��ṩ��һϵ�еĹ������У���ͬ�Ĺ������а����书�ܷ��࣬�ֱ���ͬ���͵����һ����Ϊ��������������ͣ�
	1.VK_QUEUE_GRAPHICS_BIT:֧��ͼ�ι���
	2.VK_QUEUE_COMPUTE_BIT:֧�ּ��㹦�ܣ�������ɫ����
	3.VK_QUEUE_TRANSFER_BIT:֧��ת�ƹ��ܣ���Դ�Ĳ���ת���Լ�����е���Դת�ƣ�
	4.VK_QUEUE_SPARSE_BINDING_BIT:֧��ϡ��󶨹��ܣ�����ϡ���ڴ棩
	5.VK_QUEUE_PROTECTED_BIT:֧���ܱ������ܣ���Ҫ�����ܱ����ڴ�Ĺ���
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

	//����logical device�Զ�����������
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	VkSurfaceKHR m_surface;

	VkSwapchainKHR m_swap_chain;

	std::vector<VkImage> m_swapchain_images;
	VkFormat m_swapchain_image_format;
	VkExtent2D m_swapchain_image_extent;

private:
	/*������ȷ��swap chain����
	��Ҫ�����������棺
		1��Surface Format����ɫͨ�������ͨ����
		2��Presentation Mode
		3��Swap extent��������image��С��
	*/
	//�ϴ�git
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	

};

