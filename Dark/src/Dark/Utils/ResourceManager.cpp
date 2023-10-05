#include "ResourceManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "ECheck/ECheck.h"
#include "Utils/Dark.h"

#include <GL/glew.h>
#include <iostream>

namespace Dark
{
	std::unordered_map<std::string, std::unique_ptr<Texture>> ResourceManager::m_textureMap;

	const Texture& ResourceManager::getWhiteTexture()
	{
		if (m_textureMap.find("whiteTexture") == m_textureMap.end()) {
			std::unique_ptr<Texture> texture = std::make_unique<Texture>();

			unsigned int whiteColor = 0xffffffff;
			texture->m_width = 1;
			texture->m_height = 1;

			GLEC(glGenTextures(1, &texture->m_id));
			GLEC(glBindTexture(GL_TEXTURE_2D, texture->m_id));
			GLEC(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->m_width, texture->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &whiteColor));

			GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
			GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
			GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
			GLEC(glGenerateMipmap(GL_TEXTURE_2D));

			m_textureMap["whiteTexture"] = std::move(texture);
		}

		return *m_textureMap.at("whiteTexture");
	}

	const Texture& ResourceManager::getTexture(const std::string& texturePath)
	{
		if (m_textureMap.find(texturePath) == m_textureMap.end()) {
			std::unique_ptr<Texture> texture = std::make_unique<Texture>();

			loadTexture(texturePath, std::move(texture));
		}

		return *m_textureMap.at(texturePath);
	}

	const TextureAtlas& ResourceManager::getTextureAtlas(const std::string& texturePath, int rows, int cols, float offset /*= 0.0f*/)
	{
		if (m_textureMap.find(texturePath) == m_textureMap.end()) {
			std::unique_ptr<Texture> texture = std::make_unique<TextureAtlas>();

			loadTexture(texturePath, std::move(texture));

			TextureAtlas& tmp = (TextureAtlas&)*m_textureMap.at(texturePath);

			tmp.m_rows = rows;
			tmp.m_cols = cols;

			tmp.m_maxIndex = (rows * cols) - 1;

			tmp.m_perRow = (float)tmp.m_height / (float)tmp.m_rows;
			tmp.m_perCol = (float)tmp.m_width / (float)tmp.m_cols;
			
			tmp.m_offset = offset;
		}

		return (TextureAtlas&)*m_textureMap.at(texturePath);
	}

	void ResourceManager::loadTexture(const std::string& texturePath, std::unique_ptr<Texture> texture)
	{
		stbi_set_flip_vertically_on_load(1);
		unsigned char* img = stbi_load((texture->m_filePath = texturePath).c_str(), &texture->m_width, &texture->m_height, &texture->m_channels, 4);

		if (!img) {
			error("Couldn't load \"" + texturePath + "\" texture!", DARK_FILE, DARK_LINE);
		}

		GLEC(glGenTextures(1, &texture->m_id));
		GLEC(glBindTexture(GL_TEXTURE_2D, texture->m_id));
		GLEC(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->m_width, texture->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img));

		GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLEC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLEC(glGenerateMipmap(GL_TEXTURE_2D));

		stbi_image_free(img);

		m_textureMap[texturePath] = std::move(texture);
	}
}