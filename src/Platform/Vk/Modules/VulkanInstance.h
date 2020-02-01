#pragma once
#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"

class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();

	vk::Instance createNewInstance();

	void addExtension(const char* extension);
	void addLayer(const char* layer);
	inline std::vector<const char*> getExtensions()
	{ return instanceExtensions; };
	inline std::vector<const char*> getLayers()
	{ return instanceLayers; };

	void setApplicationName(const char* pApplicationName_);
	void setApplicationVersion(uint32_t applicationVersion_);
	void setEngineName(const char* pEngineName_);
	void setEngineVersion(uint32_t engineVersion_);
	void setApiVersion(uint32_t apiVersion_);

private:
	void getDefaults();

	bool checkAvailableExtensions(const char* extension);
	bool checkAvailableLayers(const char* layer);

	vk::ApplicationInfo appInfo;
	std::vector<const char*> instanceExtensions;
	std::vector<const char*> instanceLayers;
};

class VulkanSurface 
{
public:
	VulkanSurface();
	~VulkanSurface();

	vk::SurfaceKHR createNewSurface(GLFWwindow* window, vk::Instance& instance);

private:
	GLFWwindow* window;
	vk::Instance* instance;
};