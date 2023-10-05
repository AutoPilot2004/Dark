#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace Dark
{
	class Scene;

	class SceneManager
	{
	public:
		static void addScene(const std::string& sceneName, std::unique_ptr<Scene> scene);
		static void setScene(const std::string& sceneName);

		static void onInitScenes();
		static void onDestroyScenes();

		//Getters
		inline static Scene* getAndResetSceneInQueue() { Scene* scene = m_sceneInQueue; m_sceneInQueue = nullptr; return scene; }
		inline static bool sceneInQueue() { return m_sceneInQueue; }

	private:
		SceneManager() = delete;

	private:
		static std::unordered_map<std::string, std::unique_ptr<Scene>> m_sceneMap;

		static Scene* m_sceneInQueue;
	};
}