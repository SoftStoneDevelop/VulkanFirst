#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vulkan/vulkan.h>
#include "lve_texture_storage.hpp"
#include <stdexcept>
#include "lve_buffer.hpp"

namespace lve {

    LveTextureStorage::LveTextureStorage(
        LveDevice& device)
        : lveDevice{ device } 
    {
    }

    LveTextureStorage::~LveTextureStorage()
    {
        for (auto& kv : textureDatas)
        {
            auto& textureData = kv.second;
            destroyAndFreeTextureData(textureData);
        }
    }

    void LveTextureStorage::createTextureImage(LveTextureStorage::TextureData& imageData, const std::string& texturePath) {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            throw std::runtime_error("failed to load image!");
        }

        LveBuffer stagingBuffer{
            lveDevice,
            imageSize,
            1,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)pixels);
        stbi_image_free(pixels);

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = texWidth;
        imageInfo.extent.height = texHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        lveDevice.createImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            imageData.image,
            imageData.imageMemory
        );
        lveDevice.transitionImageLayout(
            imageData.image,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        );

        lveDevice.copyBufferToImage(
            stagingBuffer.getBuffer(),
            imageData.image,
            static_cast<uint32_t>(texWidth),
            static_cast<uint32_t>(texHeight),
            1
        );

        lveDevice.transitionImageLayout(
            imageData.image,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
    }

    void LveTextureStorage::loadTexture(const std::string& textureName) {
        LveTextureStorage::TextureData imageData{};
        createTextureImage(imageData, textureName);
        //TODO create imageView
    }

    void LveTextureStorage::unloadTexture(const std::string& textureName) {
        auto& obj = textureDatas.at(textureName);
        destroyAndFreeTextureData(obj);
        textureDatas.erase(textureName);
    }

    void LveTextureStorage::destroyAndFreeTextureData(const TextureData& data) {
        vkDestroyImageView(lveDevice.device(), data.imageView, nullptr);
        vkDestroyImage(lveDevice.device(), data.image, nullptr);
        vkFreeMemory(lveDevice.device(), data.imageMemory, nullptr);
    }

}//namespace lve