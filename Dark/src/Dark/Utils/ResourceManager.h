#pragma once
#include "Graphics/Texture.h"
#include "Graphics/TextureAtlas.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace Dark
{
	class ResourceManager
	{
	public:
		static const Texture& getTexture(const std::string& texturePath);
		static const Texture& getWhiteTexture();
		static const TextureAtlas& getTextureAtlas(const std::string& texturePath, int rows, int cols, float offset = 0.0f);

	private:
		ResourceManager() = delete;

		static void loadTexture(const std::string& texturePath, std::unique_ptr<Texture> texture);

	private:
		static std::unordered_map<std::string, std::unique_ptr<Texture>> m_textureMap;

		friend class Renderer2D;
	};
}