#include "EditorScene.h"

#include <Dark/Graphics/Window.h>
#include <Dark/GUI/GUI.h>
#include <Dark/Events/InputManager.h>
#include <Dark/Scene/SceneSerializer.h>
#include <Dark/Scene/Components.h>
#include <Dark/Renderer/Renderer2D.h>
#include <Dark/Graphics/TextureAtlas.h>
#include <Dark/Utils/ResourceManager.h>

#include <glm/gtc/type_ptr.hpp>

static ImGuiWindowFlags s_winFlags = 0;

static constexpr int32 VEL_ITERATIONS = 8;
static constexpr int32 POS_ITERATIONS = 3;

void EditorScene::onInit()
{
	printf("Editor Scene Initialized!\n");

	m_fbo = std::make_unique<Dark::FrameBuffer>();
	m_fbo->gen();

	m_activeCamera = &m_editorCamera;
}

void EditorScene::onDestroy()
{
	printf("Editor Scene Destroyed!\n");

	m_fbo->del();
}

void EditorScene::onEntry()
{
	printf("Entered Editor Scene!\n");
}

void EditorScene::onExit()
{
	printf("Exited Editor Scene!\n");
}

void EditorScene::onUpdate(float dT)
{
	ImGui::onUpdate();
	if (ImGui::dockSpace(this, s_winFlags))
		m_selectedEntity = Dark::Entity();

	onUpdateEnvironment();
	onUpdateInspector();
	onUpdateProperties();
	onUpdateGameViewport(dT);

	if (m_currentSceneState == SceneState::RUNNING)
		onRuntime(dT);

	onUpdateEntityMM();
}

void EditorScene::onUpdateEnvironment()
{
	ImGui::Begin("Environment", 0, s_winFlags);

	if (ImGui::ColorEdit3("Bacgkround Color", &Dark::Environment::backgroundColor[0])) {
		Dark::Window::setBackgroundColor(Dark::Environment::backgroundColor);
	}
	ImGui::Separator();

	ImGui::DragFloat2("Gravity", &Dark::Environment::gravity.x);
	ImGui::Separator();

	ImGui::End();
}

void EditorScene::onUpdateInspector()
{
	ImGui::Begin("Inspector", 0, s_winFlags);

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && m_selectedEntity && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
		m_selectedEntity = Dark::Entity();
	}
	else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
		if (!ImGui::IsAnyItemHovered())
			ImGui::OpenPopup("InspectorMenu");
		else
			ImGui::OpenPopup("EntityMenu");
	}

	if (ImGui::BeginPopup("InspectorMenu", ImGuiWindowFlags_NoMove)) {
		ImGui::Text("Create:");
		ImGui::Separator();
		if (ImGui::Selectable("Camera", false, 0, { 150.0f, 0.0f })) {
			m_selectedEntity = createCamera();
		}
		if (ImGui::Selectable("Entity", false, 0, { 150.0f, 0.0f })) {
			m_selectedEntity = createEntity();
		}
		ImGui::EndPopup();
	}
	else if (ImGui::BeginPopup("EntityMenu", ImGuiWindowFlags_NoMove)) {
		ImGui::Text("Delete:");
		ImGui::Separator();
		if (ImGui::Selectable("Entity", false, 0, { 150.0f, 0.0f })) {
			deleteEntity(m_selectedEntity);
			m_selectedEntity = Dark::Entity();
			m_registry.sort<Dark::IdentifierComponent>([](auto id1, auto id2) { return id1.id > id2.id; });
		}
		ImGui::EndPopup();
	}

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	for (const auto& [entity, id, tag, renderer] : m_registry.view<Dark::IdentifierComponent, Dark::TagComponent, Dark::RendererComponent>().each()) {
		if ((ImGui::Selectable((tag.tag + "##" + std::to_string(id.id)).c_str(), (m_selectedEntity.getID() == id.id), ImGuiSelectableFlags_AllowItemOverlap)) ||
			(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
			m_selectedEntity = getEntityWithID((std::uint32_t)entity);

		ImGui::SameLine(ImGui::GetWindowWidth() - 20.0f);
		static GLuint whiteTex = Dark::ResourceManager::getWhiteTexture().getID();
		if (ImGui::ImageButton(("##" + std::to_string(id.id)).c_str(), (ImTextureID)whiteTex,
			ImVec2(8.0f, 8.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
			(renderer.isVisible ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 0.5f)))) {
			renderer.isVisible = !renderer.isVisible;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Visibility");
			ImGui::EndTooltip();
		}
	}
	ImGui::PopStyleColor(3);

	ImGui::End();
}

void EditorScene::onUpdateProperties()
{
	ImGui::Begin("Properties", 0, s_winFlags);

	if (m_selectedEntity) {
		int id = m_selectedEntity.getID();

		//Tag Component
		ImGui::InputText("Name", &m_selectedEntity.getComponent<Dark::TagComponent>().tag);
		ImGui::Separator();
		
		//For all other entities other than cameras
		if (!m_selectedEntity.hasComponent<Dark::CameraComponent>()) {
			//Renderer Component
			if (ImGui::TreeNode(("Sprite##" + std::to_string(id)).c_str())) {
				auto& renderer = m_selectedEntity.getComponent<Dark::RendererComponent>();

				if (renderer.texture) {
					float widthOfPreview = renderer.texture->getWidth() / 32.0f;
					float heightOfPreview = renderer.texture->getHeight() / 32.0f;
					ImGui::Image((ImTextureID)renderer.texture->getID(), { widthOfPreview, heightOfPreview }, ImVec2(0, 1), ImVec2(1, 0));
					if (ImGui::BeginItemTooltip()) {
						ImGui::Text("Texture Width:  %dpx", renderer.texture->getWidth());
						ImGui::Text("Texture Height: %dpx", renderer.texture->getHeight());
						static float zoom = 4.0f;
						ImGui::Image((ImTextureID)renderer.texture->getID(), { widthOfPreview * zoom, heightOfPreview * zoom }, ImVec2(0, 1), ImVec2(1, 0));
						ImGui::EndTooltip();
					}
					ImGui::SameLine();
					if (ImGui::Button("Change texture..."))
						ImGuiFileDialog::Instance()->OpenDialog("TextureSelection", "Select a texture", ".png,.jpg", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
					ImGui::SameLine();
					if (ImGui::Button("Remove Texture")) {
						delete renderer.texture;
						renderer.texture = nullptr;
					}

					if (renderer.texture) {
						ImGui::Checkbox("Texture Atlas", &renderer.isTextureAtlas);

						if (renderer.isTextureAtlas) {
							if (ImGui::DragInt("Index", &renderer.index, 1.0f, 0, (renderer.rows * renderer.cols) - 1)) {
								renderer.texture->isDirty();
							}
							ImGui::Separator();

							if (ImGui::DragInt("Rows", &renderer.rows, 1.0f, 1, INT_MAX)) {
								renderer.texture->isDirty();
							}
							if (ImGui::DragInt("Columns", &renderer.cols, 1.0f, 1, INT_MAX)) {
								renderer.texture->isDirty();
							}
							if (ImGui::DragFloat("OffsetX", &renderer.offsetX, 0.001f)) {
								renderer.texture->isDirty();
							}
							if (ImGui::DragFloat("OffsetY", &renderer.offsetY, 0.001f)) {
								renderer.texture->isDirty();
							}
						}
					}
				}
				else {
					if (ImGui::Button("Choose texture..."))
						ImGuiFileDialog::Instance()->OpenDialog("TextureSelection", "Select a texture", ".png,.jpg", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
				}
				ImGui::Separator();

				if (ImGuiFileDialog::Instance()->Display("TextureSelection", 32, { ImGui::DIALOG_MIN_WINDOW_SIZE_X, ImGui::DIALOG_MIN_WINDOW_SIZE_Y })) {
					if (ImGuiFileDialog::Instance()->IsOk()) {
						renderer.texture = new Dark::Texture(Dark::ResourceManager::getTexture(ImGuiFileDialog::Instance()->GetFilePathName()));
					}

					ImGuiFileDialog::Instance()->Close();
				}

				ImGui::ColorEdit4("Color", &renderer.color[0]);

				ImGui::TreePop();
			}
			ImGui::Separator();

			//Transform Component
			if (ImGui::TreeNode(("Transform##" + std::to_string(id)).c_str())) {
				auto& trans = m_selectedEntity.getComponent<Dark::TransformComponent>();

				if (ImGui::DragFloat3("Translate", glm::value_ptr(trans.translation))) {
					trans.updateTranslationMat();

					if (m_selectedEntity.hasComponent<Dark::Rigidbody2DComponent>()) {
						m_selectedEntity.getComponent<Dark::Rigidbody2DComponent>().bodyDef.position.Set(trans.translation.x, trans.translation.y);
					}
				}
				if (ImGui::DragFloat("Rotate", &trans.rotation)) {
					trans.updateRotationMat();

					if (m_selectedEntity.hasComponent<Dark::Rigidbody2DComponent>()) {
						m_selectedEntity.getComponent<Dark::Rigidbody2DComponent>().bodyDef.angle = glm::radians(trans.rotation);
					}
				}
				if (ImGui::DragFloat2("Scale", glm::value_ptr(trans.scale))) {
					trans.updateScaleMat();

					if (m_selectedEntity.hasComponent<Dark::BoxCollider2DComponent>()) {
						m_selectedEntity.getComponent<Dark::BoxCollider2DComponent>().polygonShape.SetAsBox(trans.scale.x / 2.0f, trans.scale.y / 2.0f);
					}
				}

				ImGui::TreePop();
			}
			ImGui::Separator();

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
				if (!ImGui::IsAnyItemHovered())
					ImGui::OpenPopup("AddComponent");
				else
					ImGui::OpenPopup("RemoveComponent");
			}

			if (ImGui::BeginPopup("AddComponent", ImGuiWindowFlags_NoMove)) {
				ImGui::Text("Add:");
				ImGui::Separator();

				//Rigidbody Component
				bool hasComponent = m_selectedEntity.hasComponent<Dark::Rigidbody2DComponent>();
				if (ImGui::Selectable("Rigidbody Component", false, hasComponent ? ImGuiSelectableFlags_Disabled : 0, { 200.0f, 0.0f })) {
					auto& rb = m_selectedEntity.addComponent<Dark::Rigidbody2DComponent>();
					const auto& trans = m_selectedEntity.getComponent<Dark::TransformComponent>();

					rb.bodyDef.type = b2BodyType::b2_staticBody;
					rb.bodyDef.position.Set(trans.translation.x, trans.translation.y);
					rb.bodyDef.angle = glm::radians(trans.rotation);
				}

				//Box Collider 2D Component
				hasComponent = m_selectedEntity.hasComponent<Dark::BoxCollider2DComponent>();
				if (ImGui::Selectable("BoxCollider2D Component", false, hasComponent ? ImGuiSelectableFlags_Disabled : 0, { 200.0f, 0.0f })) {
					auto& bc = m_selectedEntity.addComponent<Dark::BoxCollider2DComponent>();
					const auto& trans = m_selectedEntity.getComponent<Dark::TransformComponent>();

					bc.polygonShape.SetAsBox(trans.scale.x / 2.0f, trans.scale.y / 2.0f);

					bc.fixtureDef.shape = &bc.polygonShape;
					bc.fixtureDef.density = 1.0f;
					bc.fixtureDef.friction = 0.3f;
					bc.fixtureDef.restitution = 0.0f;
					bc.fixtureDef.restitutionThreshold = 0.5f;
				}

				ImGui::EndPopup();
			}
			else if (ImGui::BeginPopup("RemoveComponent", ImGuiWindowFlags_NoMove)) {
				ImGui::Text("Remove:");
				ImGui::Separator();

				//Rigidbody Component
				bool hasComponent = m_selectedEntity.hasComponent<Dark::Rigidbody2DComponent>();
				if (ImGui::Selectable("Rigidbody Component", false, hasComponent ? 0 : ImGuiSelectableFlags_Disabled, { 200.0f, 0.0f })) {
					m_selectedEntity.removeComponent<Dark::Rigidbody2DComponent>();
				}

				//Box Collider 2D Component
				hasComponent = m_selectedEntity.hasComponent<Dark::BoxCollider2DComponent>();
				if (ImGui::Selectable("BoxCollider2D Component", false, hasComponent ? 0 : ImGuiSelectableFlags_Disabled, { 200.0f, 0.0f })) {
					m_selectedEntity.removeComponent<Dark::BoxCollider2DComponent>();
				}

				ImGui::EndPopup();
			}

			//Rigidbody Component
			if (m_selectedEntity.hasComponent<Dark::Rigidbody2DComponent>()) {
				auto& rb = m_selectedEntity.getComponent<Dark::Rigidbody2DComponent>();

				static const char* const items[] = { "STATIC", "DYNAMIC", "KINEMATIC" };
				if (ImGui::TreeNode(("Rigidbody##" + std::to_string(id)).c_str())) {
					ImGui::Text("Body Type:");
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.18f, 0.18f, 0.18f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, s_winFlags != 0);
					if (ImGui::ListBox("##", &rb.bodyType, items, 3)) {
						switch (rb.bodyType) {
						case Dark::BodyType::STATIC:
							rb.bodyDef.type = b2BodyType::b2_staticBody;
							break;
						case Dark::BodyType::DYNAMIC:
							rb.bodyDef.type = b2BodyType::b2_dynamicBody;
							break;
						case Dark::BodyType::KINEMATIC:
							rb.bodyDef.type = b2BodyType::b2_kinematicBody;
							break;
						}
					}
					ImGui::PopStyleColor(2);
					ImGui::PopItemFlag();

					ImGui::Checkbox("Fixed Rotation", &rb.bodyDef.fixedRotation);

					ImGui::TreePop();
				}
				ImGui::Separator();
			}

			//Box Collider 2D Component
			if (m_selectedEntity.hasComponent<Dark::BoxCollider2DComponent>()) {
				auto& bc = m_selectedEntity.getComponent<Dark::BoxCollider2DComponent>();

				if (ImGui::TreeNode(("BoxCollider2D##" + std::to_string(id)).c_str())) {
					ImGui::DragFloat("Density", &bc.fixtureDef.density, 0.1f, 0.0f, FLT_MAX);
					ImGui::DragFloat("Friction", &bc.fixtureDef.friction, 0.1f, 0.0f, 1.0f);
					ImGui::DragFloat("Restitution", &bc.fixtureDef.restitution, 0.1f, 0.0f, 1.0f);
					ImGui::DragFloat("Rest. Threshold", &bc.fixtureDef.restitutionThreshold, 0.1f, 0.0f, FLT_MAX);

					ImGui::TreePop();
				}
				ImGui::Separator();
			}
		}
		//For Cameras
		else {
			ImGui::RadioButton("Active", &Dark::CameraComponent::active, m_selectedEntity.getID());
			ImGui::Separator();

			//Transform Component
			if (ImGui::TreeNode(("Transform##" + std::to_string(id)).c_str())) {
				auto& trans = m_selectedEntity.getComponent<Dark::TransformComponent>();
				auto& camera = m_selectedEntity.getComponent<Dark::CameraComponent>();

				if (ImGui::DragFloat3("Translate", glm::value_ptr(trans.translation))) {
					trans.updateTranslationMat();

					camera.camera.setPosition(trans.translation);
				}
				if (ImGui::DragFloat("Rotate", &trans.rotation)) {
					trans.updateRotationMat();

					camera.camera.setRotation(-trans.rotation);
				}
				if (ImGui::DragFloat("Scale", &trans.scale.x)) {
					trans.scale.y = trans.scale.x;

					trans.updateScaleMat();

					camera.camera.setScale(trans.scale.x);
				}

				ImGui::TreePop();
			}
			ImGui::Separator();
		}
	}

	ImGui::End();
}

void EditorScene::onUpdateGameViewport(float dT)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	m_hoveringViewport = (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive());

	static const Dark::Texture& playButtonTex = Dark::ResourceManager::getTexture("resources/textures/play-button.png");
	static const Dark::Texture& stopButtonTex = Dark::ResourceManager::getTexture("resources/textures/stop-button.png");

	constexpr float BUTTON_SIZE = 15.0f;
	constexpr float BUTTON_OFFSET = 4.0f;

	if (ImGui::BeginMenuBar()) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
		ImGui::SetCursorPosX(((ImGui::GetWindowWidth() / 2.0f) - (BUTTON_SIZE / 2.0f)) - BUTTON_OFFSET);
		if (ImGui::ImageButton(m_currentSceneState == SceneState::EDITOR ? (ImTextureID)playButtonTex.getID() : (ImTextureID)stopButtonTex.getID(), { BUTTON_SIZE, BUTTON_SIZE })) {
			if (m_currentSceneState == SceneState::EDITOR)
				onRuntimeStart();
			else
				onRuntimeStop();
		}
		ImGui::PopStyleColor(3);
		ImGui::EndMenuBar();
	}

	ImVec2 windowSize = ImGui::getLargestSizeForViewport();
	ImVec2 windowPos = ImGui::getCenteredPosForViewport(windowSize);

	ImGui::SetCursorPos(windowPos);

	ImGui::Image((ImTextureID)m_fbo->getTex(),
		windowSize,
		ImVec2(0, 1),
		ImVec2(1, 0));

	onProcessInput(dT);

	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorScene::onUpdateEntityMM()
{
	for (const auto& [entity, trans] : m_registry.view<Dark::TransformComponent>().each()) {
		trans.updateModelMatrix();
	}
}

void EditorScene::onProcessInput(float dT)
{
	if (m_hoveringViewport && m_currentSceneState == SceneState::EDITOR) {
		if (Dark::InputManager::isDown(DARK_KEY_A))
			m_editorCamera.offsetPosition({ -6.5f * dT, 0.0f });
		if (Dark::InputManager::isDown(DARK_KEY_D))
			m_editorCamera.offsetPosition({ 6.5f * dT, 0.0f });
		if (Dark::InputManager::isDown(DARK_KEY_W))
			m_editorCamera.offsetPosition({ -0.0f, 6.5f * dT });
		if (Dark::InputManager::isDown(DARK_KEY_S))
			m_editorCamera.offsetPosition({ 0.0f, -6.5f * dT });
		if (Dark::InputManager::isDown(DARK_KEY_E))
			m_editorCamera.offsetScale(5.0f * dT);
		if (Dark::InputManager::isDown(DARK_KEY_Q))
			m_editorCamera.offsetScale(-5.0f * dT);
	}
}

void EditorScene::onRuntimeStart()
{
	m_currentSceneState = SceneState::RUNNING;
	Dark::SceneSerializer::runtimeSerialize(this);
	s_winFlags = ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	m_world = new Dark::World();

	m_world->get().SetGravity(Dark::Environment::gravity);

	for (const auto& [entity, rb] : m_registry.view<Dark::Rigidbody2DComponent>().each()) {
		rb.body = m_world->get().CreateBody(&rb.bodyDef);
	}

	for (const auto& [entity, rb, bc] : m_registry.view<Dark::Rigidbody2DComponent, Dark::BoxCollider2DComponent>().each()) {
		rb.body->CreateFixture(&bc.fixtureDef);
	}
}

void EditorScene::onRuntimeStop()
{
	m_currentSceneState = SceneState::EDITOR;
	Dark::SceneSerializer::runtimeDeserialize(this);
	m_selectedEntity = Dark::Entity();
	s_winFlags = 0;

	m_activeCamera = &m_editorCamera;

	delete m_world;
	m_world = nullptr;

	for (const auto& [entity, rb] : m_registry.view<Dark::Rigidbody2DComponent>().each()) {
		rb.body = nullptr;
	}

	/*for (const auto& [entity, bc] : m_registry.view<Dark::BoxCollider2DComponent>().each()) {
		bc.fixture = nullptr;
	}*/
}

void EditorScene::onRuntime(float dT)
{
	for (const auto& [entity, id, camera] : m_registry.view<Dark::IdentifierComponent, Dark::CameraComponent>().each()) {
		if (camera.active == id.id) {
			m_activeCamera = &camera.camera;
			break;
		}
	}

	m_world->get().Step(dT, VEL_ITERATIONS, POS_ITERATIONS);

	for (const auto& [entity, transform, rb] : m_registry.view<Dark::TransformComponent, Dark::Rigidbody2DComponent>().each()) {
		if ((rb.body->GetType() != b2BodyType::b2_staticBody)) {
			if (rb.body->GetLinearVelocity() != b2Vec2{ 0.0f, 0.0f }) {
				transform.setTranslation2f({ rb.body->GetPosition().x, rb.body->GetPosition().y });
			}

			if (rb.body->GetAngularVelocity() != 0.0f) {
				transform.setRotation(glm::degrees(rb.body->GetAngle()));
			}
		}
	}
}

void EditorScene::onDraw()
{
	const auto& group = m_registry.group<Dark::TransformComponent, Dark::RendererComponent>();

	for (const auto& [entity, transform, renderer] : group.each()) {
		if (renderer.isVisible) {
			if (renderer.texture) {
				if (renderer.isTextureAtlas) {
					Dark::Renderer2D::drawQuad(*renderer.texture, renderer.color, transform.getModelMatrix(), const_cast<Dark::Texture*>(renderer.texture)->getUV(renderer.index, renderer.rows, renderer.cols, renderer.offsetX, renderer.offsetY));
				}
				else {
					Dark::Renderer2D::drawQuad(*renderer.texture, renderer.color, transform.getModelMatrix());
				}
			}
			else {
				Dark::Renderer2D::drawQuad(renderer.color, transform.getModelMatrix());
			}
		}
	}
}

void EditorScene::onRender()
{
	Dark::Renderer2D::begin();
	onDraw();
	Dark::Renderer2D::end();
	Dark::Renderer2D::flush();

	m_fbo->unbind();

	ImGui::ShowDebugLogWindow();

	ImGui::onRender();
}