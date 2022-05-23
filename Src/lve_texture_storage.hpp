#pragma once

#include "lve_device.hpp"

#include <string>
#include <unordered_map>

namespace lve {
	
	class LveTextureStorage {
	public:
		struct TextureData {
			VkImage image;
			VkImageView imageView;
			VkDeviceMemory imageMemory;
		};

		LveTextureStorage(LveDevice& device);
		~LveTextureStorage();

		LveTextureStorage(const LveTextureStorage&) = delete;
		LveTextureStorage& operator=(const LveTextureStorage&) = delete;

		void loadTexture(const std::string& texturePath);
		void unloadTexture(const std::string& textureName);

	private:
		void createTextureImage(LveTextureStorage::TextureData& imageData, const std::string& texturePath);
		void destroyAndFreeTextureData(const TextureData& data);

		std::unordered_map<std::string, TextureData> textureDatas;
		LveDevice& lveDevice;
	};
} // namespace lve