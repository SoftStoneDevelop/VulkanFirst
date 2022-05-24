#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vulkan/vulkan.h>
#include "lve_texture_storage.hpp"
#include <stdexcept>
#include "lve_buffer.hpp"
#include "Definitions/DefaultSamplersNames.hpp"

namespace lve {

    LveTextureStorage::LveTextureStorage(
        LveDevice& device)
        : lveDevice{ device } 
    {
    }

    LveTextureStorage::~LveTextureStorage()
    {
        for (auto& kv : textureSamplers)
        {
            auto& textureSampler = kv.second;
            vkDestroySampler(lveDevice.device(), textureSampler, nullptr);
        }

        for (auto& kv : textureDatas)
        {
            auto& textureData = kv.second;
            destroyAndFreeTextureData(textureData);
        }
    }

    VkSampler LveTextureStorage::getSampler(const std::string& samplerName)
    {
        if (textureSamplers.count(samplerName) == 0)
        {
            if (samplerName == defaultSamplerName)
            {
                VkSamplerCreateInfo defaultSampler{};
                defaultSampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                defaultSampler.magFilter = VK_FILTER_LINEAR;
                defaultSampler.minFilter = VK_FILTER_LINEAR;
                defaultSampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                defaultSampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                defaultSampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                defaultSampler.anisotropyEnable = VK_TRUE;
                defaultSampler.maxAnisotropy = lveDevice.properties.limits.maxSamplerAnisotropy;
                defaultSampler.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
                defaultSampler.unnormalizedCoordinates = VK_FALSE;
                defaultSampler.compareEnable = VK_FALSE;
                defaultSampler.compareOp = VK_COMPARE_OP_ALWAYS;
                defaultSampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

                VkSampler textureSampler{};
                createTextureSampler(defaultSampler, textureSampler, samplerName);

                return textureSampler;
            }
        }
        else
        {
            return textureSamplers[samplerName];
        }

        throw std::runtime_error("Not find sampler with name:" + samplerName);
    }

    void LveTextureStorage::createTextureSampler(
        VkSamplerCreateInfo& samplerInfo,
        VkSampler textureSampler,
        const std::string& samplerName
    )
    {
        if (vkCreateSampler(lveDevice.device(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }

        assert(textureSamplers.count(samplerName) == 0 && "Sampler already in use");
        textureSamplers[samplerName] = textureSampler;
    }

    void LveTextureStorage::destroySampler(const std::string& samplerName)
    {
        if (textureSamplers.count(samplerName) == 0)
            return;

        auto& textureSampler = textureSamplers.at(samplerName);
        vkDestroySampler(lveDevice.device(), textureSampler, nullptr);
        textureDatas.erase(samplerName);
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
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
    }

    void LveTextureStorage::loadTexture(const std::string& texturePath, const std::string& textureName) {
        LveTextureStorage::TextureData imageData{};
        createTextureImage(imageData, texturePath);
        lveDevice.createImageView(imageData.imageView, imageData.image, VK_FORMAT_R8G8B8A8_SRGB);

        assert(textureDatas.count(textureName) == 0 && "Texture already in use");
        textureDatas[textureName] = std::move(imageData);
    }

    void LveTextureStorage::unloadTexture(const std::string& textureName) {

        if (textureDatas.count(textureName) == 0)
            return;

        auto& textureData = textureDatas.at(textureName);
        destroyAndFreeTextureData(textureData);
        textureDatas.erase(textureName);
    }

    void LveTextureStorage::destroyAndFreeTextureData(const TextureData& data) {
        vkDestroyImageView(lveDevice.device(), data.imageView, nullptr);
        vkDestroyImage(lveDevice.device(), data.image, nullptr);
        vkFreeMemory(lveDevice.device(), data.imageMemory, nullptr);
    }

}//namespace lve