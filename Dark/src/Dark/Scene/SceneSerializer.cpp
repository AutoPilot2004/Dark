#include "SceneSerializer.h"
#include "Scene.h"
#include "Entity.h"
#include "Graphics/Window.h"
#include "Utils/ResourceManager.h"

#include "yaml-cpp/yaml.h"

#include <fstream>

#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace
{
	const std::string S_RUNTIME_FILENAME = "runtime.dark";
	const std::string S_RUNTIME_FILE_PATH = "./" + S_RUNTIME_FILENAME;
}

namespace YAML
{
	Emitter& operator<<(Emitter& emitter, const Dark::Color& color)
	{
		emitter << Flow;
		emitter << BeginSeq << color.r << color.g << color.b << color.a << EndSeq;
		return emitter;
	}

	Emitter& operator<<(Emitter& emitter, const b2Vec2& b2vec2)
	{
		emitter << Flow;
		emitter << BeginSeq << b2vec2.x << b2vec2.y << EndSeq;
		return emitter;
	}

	Emitter& operator<<(Emitter& emitter, const glm::vec2& glmvec2)
	{
		emitter << Flow;
		emitter << BeginSeq << glmvec2.x << glmvec2.y << EndSeq;
		return emitter;
	}

	Emitter& operator<<(Emitter& emitter, const glm::vec3& glmvec3)
	{
		emitter << Flow;
		emitter << BeginSeq << glmvec3.x << glmvec3.y << glmvec3.z << EndSeq;
		return emitter;
	}

	//Deserialization
	template<>
	struct convert<Dark::Color>
	{
		static bool decode(const Node& node, Dark::Color& color)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			for (int i = 0; i < 4; i++) {
				color[i] = node[i].as<float>();
			}

			return true;
		}
	};

	template<>
	struct convert<b2Vec2>
	{
		static bool decode(const Node& node, b2Vec2& b2vec2)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			b2vec2.x = node[0].as<float>();
			b2vec2.y = node[1].as<float>();

			return true;
		}
	};

	template<>
	struct convert<glm::vec2>
	{
		static bool decode(const Node& node, glm::vec2& glmvec2)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			for (int i = 0; i < 2; i++) {
				glmvec2[i] = node[i].as<float>();
			}

			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static bool decode(const Node& node, glm::vec3& glmvec3)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			for (int i = 0; i < 3; i++) {
				glmvec3[i] = node[i].as<float>();
			}

			return true;
		}
	};
}

namespace Dark
{
	void SceneSerializer::serialize(const Scene* scene, const std::string& filePath, const std::string& fileName)
	{
		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << fileName;
		out << YAML::Value << YAML::BeginMap;

		//Environment
			out << YAML::Key << "ENVIRONMENT";
			out << YAML::Value << YAML::BeginMap;
				out << YAML::Key << "Background Color" << YAML::Value << Environment::backgroundColor;
				out << YAML::Key << "Gravity" << YAML::Value << Environment::gravity;
			out << YAML::EndMap;

		//Config
		out << YAML::Key << "CONFIG";
		out << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "ID Counter" << YAML::Value << IdentifierComponent::idCounter;
		out << YAML::Key << "Active Camera ID" << YAML::Value << CameraComponent::active;

		out << YAML::Key << "EC Translation" << YAML::Value << scene->m_editorCamera.getPosition();
		out << YAML::Key << "EC Rotation" << YAML::Value << scene->m_editorCamera.getRotation();
		out << YAML::Key << "EC Scale" << YAML::Value << scene->m_editorCamera.getScale();
		out << YAML::EndMap;

		//Entities
		out << YAML::Key;
		out << "ENTITIES";
		out << YAML::Value;
		out << YAML::BeginSeq;

		scene->m_registry.each([&, &scene = std::as_const(scene)](auto entity) {
			Entity ent = const_cast<Scene*>(scene)->getEntityWithID((uint32_t)entity);

			if (!ent.hasComponent<Environment>()) {
				const auto& id = ent.getComponent<IdentifierComponent>();
				const auto& tag = ent.getComponent<TagComponent>();
				const auto& renderer = ent.getComponent<RendererComponent>();
				const auto& trans = ent.getComponent<TransformComponent>();

				out << YAML::BeginMap;

				//Identifier Component
				out << YAML::Key << "ID" << YAML::Value << id.id;

				const bool isCamera = ent.hasComponent<CameraComponent>();
				out << YAML::Key << "isCamera" << YAML::Value << isCamera;

				//Tag Component
				out << YAML::Key << "TagComponent";
				out << YAML::Value << YAML::BeginMap;
				out << YAML::Key << "Tag" << YAML::Value << tag.tag;
				out << YAML::EndMap;

				//Renderer Component
				out << YAML::Key << "RendererComponent";
				out << YAML::Value << YAML::BeginMap;
				out << YAML::Key << "Visibility" << YAML::Value << renderer.isVisible;
				out << YAML::Key << "Color" << YAML::Value << renderer.color;
				if (renderer.texture)
					out << YAML::Key << "Texture" << YAML::Value << renderer.texture->getFilePath();
				out << YAML::Key << "TextureAtlas" << YAML::Value << renderer.isTextureAtlas;
				out << YAML::Key << "Index" << YAML::Value << renderer.index;
				out << YAML::Key << "Rows" << YAML::Value << renderer.rows;
				out << YAML::Key << "Columns" << YAML::Value << renderer.cols;
				out << YAML::Key << "OffsetX" << YAML::Value << renderer.offsetX;
				out << YAML::Key << "OffsetY" << YAML::Value << renderer.offsetY;
				out << YAML::EndMap;

				//Transform Component
				out << YAML::Key << "TransformComponent";
				out << YAML::Value << YAML::BeginMap;
				out << YAML::Key << "Translation" << YAML::Value << trans.translation;
				out << YAML::Key << "Rotation" << YAML::Value << trans.rotation;
				out << YAML::Key << "Scale" << YAML::Value << trans.scale;
				out << YAML::EndMap;

				if (!isCamera) {
					auto type = [](BodyType type) {
						switch (type) {
						case BodyType::STATIC:
							return "STATIC";
						case BodyType::DYNAMIC:
							return "DYNAMIC";
						case BodyType::KINEMATIC:
							return "KINEMATIC";
						}
					};

					//Rigidbody 2D Component
					if (ent.hasComponent<Rigidbody2DComponent>()) {
						const auto& rb = ent.getComponent<Rigidbody2DComponent>();

						out << YAML::Key << "Rigidbody2DComponent";
						out << YAML::Value << YAML::BeginMap;
						out << YAML::Key << "Type" << YAML::Value << type((BodyType)rb.bodyType);
						out << YAML::Key << "FixedRotation" << YAML::Value << rb.bodyDef.fixedRotation;
						out << YAML::EndMap;
					}

					//Box Collider 2D Component
					if (ent.hasComponent<BoxCollider2DComponent>()) {
						const auto& bc = ent.getComponent<BoxCollider2DComponent>();

						out << YAML::Key << "BoxCollider2DComponent";
						out << YAML::Value << YAML::BeginMap;
						out << YAML::Key << "Density" << YAML::Value << bc.fixtureDef.density;
						out << YAML::Key << "Friction" << YAML::Value << bc.fixtureDef.friction;
						out << YAML::Key << "Restitution" << YAML::Value << bc.fixtureDef.restitution;
						out << YAML::Key << "Restitution Threshold" << YAML::Value << bc.fixtureDef.restitutionThreshold;
						out << YAML::EndMap;
					}
				}

				out << YAML::EndMap;
			}
		});

		out << YAML::EndSeq;

		out << YAML::EndMap;

		std::ofstream outFile(filePath);
		outFile << out.c_str();
	}

	void SceneSerializer::deserialize(Scene* scene, const std::string& filePath, const std::string& fileName)
	{
		scene->m_registry.clear();

		YAML::Node root = YAML::LoadFile(filePath)[fileName];

		//Environment
		{
			YAML::Node env = root["ENVIRONMENT"];

			//Background Color
			Environment::backgroundColor = env["Background Color"].as<Color>();
			Window::setBackgroundColor(Environment::backgroundColor);

			//Gravity
			Environment::gravity = env["Gravity"].as<b2Vec2>();
		}

		//Config
		{
			YAML::Node config = root["CONFIG"];

			//ID Counter
			IdentifierComponent::idCounter = config["ID Counter"].as<int>();

			//Active Camera ID
			CameraComponent::active = config["Active Camera ID"].as<int>();

			//Editor Camera Transformation
			scene->m_editorCamera.setPosition(config["EC Translation"].as<glm::vec2>());
			scene->m_editorCamera.setRotation(config["EC Rotation"].as<float>());
			scene->m_editorCamera.setScale(config["EC Scale"].as<float>());
		}

		//Entities
		{
			YAML::Node entities = root["ENTITIES"];

			for (auto ite : entities) {
				Entity entity;

				const bool isCamera = ite["isCamera"].as<bool>();

				if (isCamera)
					entity = scene->createCameraWithID(ite["ID"].as<int>());
				else
					entity = scene->createEntityWithID(ite["ID"].as<int>());

				//Tag Component
				entity.getComponent<TagComponent>().tag = ite["TagComponent"].begin()->second.as<std::string>();

				//Renderer Component
				auto& renderer = entity.getComponent<RendererComponent>();
				for (auto it = ite["RendererComponent"].begin(); it != ite["RendererComponent"].end(); it++) {
					std::string key = it->first.as<std::string>();
					YAML::Node value = it->second;

					if (key == "Visibility") {
						renderer.isVisible = value.as<bool>();
					}
					else if (key == "Texture") {
						renderer.texture = new Texture(ResourceManager::getTexture(value.as<std::string>()));
					}
					else if (key == "Color") {
						renderer.color = value.as<Color>();
					}
					else if (key == "TextureAtlas") {
						renderer.isTextureAtlas = value.as<bool>();
					}
					else if (key == "Index") {
						renderer.index = value.as<int>();
					}
					else if (key == "Rows") {
						renderer.rows = value.as<int>();
					}
					else if (key == "Columns") {
						renderer.cols = value.as<int>();
					}
					else if (key == "OffsetX") {
						renderer.offsetX = value.as<float>();
					}
					else if (key == "OffsetY") {
						renderer.offsetY = value.as<float>();
					}
				}

				//Transform Component
				auto& trans = entity.getComponent<TransformComponent>();
				for (auto it = ite["TransformComponent"].begin(); it != ite["TransformComponent"].end(); it++) {
					std::string key = it->first.as<std::string>();
					YAML::Node value = it->second;

					if (key == "Translation") {
						trans.translation = value.as<glm::vec3>();
					}
					else if (key == "Rotation") {
						trans.rotation = value.as<float>();
					}
					else if (key == "Scale") {
						trans.scale = value.as<glm::vec2>();
					}
				}

				trans.updateTranslationMat();
				trans.updateRotationMat();
				trans.updateScaleMat();

				trans.updateModelMatrix();

				if (isCamera) {
					auto& camera = entity.getComponent<CameraComponent>();
					camera.camera.setPosition(trans.translation);
					camera.camera.setRotation(trans.rotation);
					camera.camera.setScale(trans.scale.x);
				}

				//Rigidbody 2D Component
				if (ite["Rigidbody2DComponent"]) {
					auto& rb = entity.addComponent<Rigidbody2DComponent>();

					rb.bodyDef.position.Set(trans.translation.x, trans.translation.y);
					rb.bodyDef.angle = glm::radians(trans.rotation);

					for (auto it = ite["Rigidbody2DComponent"].begin(); it != ite["Rigidbody2DComponent"].end(); it++) {
						std::string key = it->first.as<std::string>();
						YAML::Node value = it->second;

						if (key == "Type") {
							if (value.as<std::string>() == "STATIC") {
								rb.bodyType = BodyType::STATIC;
								rb.bodyDef.type = b2BodyType::b2_staticBody;
							}
							else if (value.as<std::string>() == "DYNAMIC") {
								rb.bodyType = BodyType::DYNAMIC;
								rb.bodyDef.type = b2BodyType::b2_dynamicBody;
							}
							else if (value.as<std::string>() == "KINEMATIC") {
								rb.bodyType = BodyType::KINEMATIC;
								rb.bodyDef.type = b2BodyType::b2_kinematicBody;
							}
						}
						else if (key == "FixedRotation") {
							rb.bodyDef.fixedRotation = value.as<bool>();
						}
					}
				}

				//Box Collider 2D Component
				if (ite["BoxCollider2DComponent"]) {
					auto& bc = entity.addComponent<BoxCollider2DComponent>();

					bc.polygonShape.SetAsBox(trans.scale.x / 2.0f, trans.scale.y / 2.0f);
					bc.fixtureDef.shape = &bc.polygonShape;

					for (auto it = ite["BoxCollider2DComponent"].begin(); it != ite["BoxCollider2DComponent"].end(); it++) {
						std::string key = it->first.as<std::string>();
						YAML::Node value = it->second;

						if (key == "Density") {
							bc.fixtureDef.density = value.as<float>();
						}
						else if (key == "Friction") {
							bc.fixtureDef.friction = value.as<float>();
						}
						else if (key == "Restitution") {
							bc.fixtureDef.restitution = value.as<float>();
						}
						else if (key == "Restitution Threshold") {
							bc.fixtureDef.restitutionThreshold = value.as<float>();
						}
					}
				}
			}

			scene->m_registry.sort<IdentifierComponent>([](auto id1, auto id2) { return id1.id > id2.id; });
		}
	}

	void SceneSerializer::runtimeSerialize(const Scene* scene)
	{
		serialize(scene, S_RUNTIME_FILE_PATH, S_RUNTIME_FILENAME);
	}

	void SceneSerializer::runtimeDeserialize(Scene* scene)
	{
		deserialize(scene, S_RUNTIME_FILE_PATH, S_RUNTIME_FILENAME);
	}

	void SceneSerializer::resetAndClearScene(Scene* scene)
	{
		scene->m_registry.clear();

		Components::resetStaticVars();

		Window::setBackgroundColor(Environment::backgroundColor);
	}
}