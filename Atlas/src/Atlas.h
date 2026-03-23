#pragma once

#include "Atlas/Core/Application.h"
#include "Atlas/Core/Layer.h"
#include "Atlas/Core/Log.h"
#include "Atlas/Core/AssetManager.h"
#include "Atlas/Core/UUID.h"

#include "Atlas/Core/Input.h"
#include "Atlas/Core/KeyCodes.h"
#include "Atlas/Core/MouseButtonCodes.h"
#include "Atlas/Core/Font.h"

#include "Atlas/Events/Event.h"
#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/Events/KeyEvent.h"
#include "Atlas/Events/MouseEvent.h"
#include "Atlas/Events/UserEvent.h"

#include "Atlas/Renderer/Renderer.h"
#include "Atlas/Renderer/OrthographicCameraController.h"
#include "Atlas/Renderer/Texture.h"
#include "Atlas/Renderer/SubTexture.h"

#include "Atlas/ImGui/ImGuiLayer.h"

#include "Atlas/Scene/Scene.h"
#include "Atlas/ECS/Registry.h"
#include "Atlas/ECS/Entities/Entity.h"
#include "Atlas/ECS/Components/Components.h"
#include "Atlas/ECS/Components/Animation.h"
#include "Atlas/ECS/Components/Behavior.h"
#include "Atlas/ECS/Systems/Systems.h"

// ---- Entry Point ----------------------
#include "Atlas/Core/EntryPoint.h"
// ---------------------------------------