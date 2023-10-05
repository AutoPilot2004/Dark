#pragma once
#include <string>

namespace Dark
{
	class Scene;

	class SceneSerializer
	{
	public:
		static void serialize(const Scene* scene, const std::string& filePath, const std::string& fileName);
		static void deserialize(Scene* scene, const std::string& filePath, const std::string& fileName);

		static void runtimeSerialize(const Scene* scene);
		static void runtimeDeserialize(Scene* scene);

		static void resetAndClearScene(Scene* scene);

	private:

	};
}