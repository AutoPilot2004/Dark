#pragma once
#include <Dark/Scene/Scene.h>
#include <Dark/Scene/Entity.h>
#include <Dark/Scene/World.h>

class EditorScene : public Dark::Scene
{
public:
	EditorScene() = default;
	~EditorScene() override = default;

	void onInit() override;
	void onDestroy() override;
	void onEntry() override;
	void onExit() override;
	void onUpdate(float dT) override;
	void onRender() override;

private:
	enum class SceneState
	{
		EDITOR,
		RUNNING
	};

private:
	void onUpdateEnvironment();
	void onUpdateInspector();
	void onUpdateProperties();
	void onUpdateGameViewport(float dT);
	void onUpdateEntityMM();

	void onProcessInput(float dT);

	void onRuntimeStart();
	void onRuntimeStop();
	void onRuntime(float dT);

	void onDraw();

private:
	Dark::World* m_world = nullptr;

	Dark::Entity m_selectedEntity;

	bool m_hoveringViewport = false;

	SceneState m_currentSceneState = SceneState::EDITOR;
};