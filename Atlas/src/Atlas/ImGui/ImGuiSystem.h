#pragma once

#include <imgui/imgui.h>
#include <memory>

namespace Atlas {

class ImGuiSystem {
  public:
	virtual ~ImGuiSystem() = default;

	virtual void initImGuiLayer()  = 0;
	virtual void cleanImGuiLayer() = 0;

	virtual void rescale(float newScale) = 0;

	static std::unique_ptr<ImGuiSystem> create();

  protected:
	void setupImGuiStyle(ImGuiStyle& style);
};
}  // namespace Atlas