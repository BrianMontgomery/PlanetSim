#include "PSIMPCH.h"
#include "Core.h"

#include "Layer.h"

#include "Logging/Log.h"

ValidationLayer::ValidationLayer()
{
}


ValidationLayer::~ValidationLayer()
{
}

bool ValidationLayer::checkValidationLayerSupport() {
	//check what layers can be used for the program
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}