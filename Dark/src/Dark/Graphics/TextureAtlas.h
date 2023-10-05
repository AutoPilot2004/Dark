#pragma once
#include "ECheck/ECheck.h"
#include "Utils/Dark.h"

#include "Texture.h"

#include <unordered_map>

namespace Dark
{
	struct TextureAtlas : public Texture
	{
		TextureAtlas() = default;
		~TextureAtlas() override = default;

		inline const glm::vec4& getUV(int index)
		{
			if (m_uvMap.find(index) == m_uvMap.end()) {
				if (index > m_maxIndex)
					error("UV out of range!", DARK_FILE, DARK_LINE);

				m_uvMap[index] = glm::vec4(
					((float)((index % m_cols) * m_perCol) / (float)m_width) + m_offset,
					((float)((m_height - ((index / m_cols) * m_perRow)) - m_perRow) / (float)m_height) + m_offset,
					((float)m_perCol / (float)m_width) - (m_offset * 2),
					((float)m_perRow / (float)m_height) - (m_offset * 2)
				);
			}

			return m_uvMap.at(index);
		}

	private:
		int m_maxIndex;

		int m_rows, m_cols;
		float m_perRow, m_perCol;

		float m_offset;

		std::unordered_map<int, glm::vec4> m_uvMap;

		friend class ResourceManager;
	};
}