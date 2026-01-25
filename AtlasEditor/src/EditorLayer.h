#pragma once

#include <Atlas.h>
#include "LoggingPanel.h"

namespace Atlas {
class EditorLayer : public Layer {
  public:
	EditorLayer() : Layer("Editor") {}

	virtual void onAttach() override {}
	virtual void onDetach() override {}
	virtual void onUpdate(DeltaTime dt) override {}
	virtual void onEvent(Event& event) override {}

	virtual void onImGuiRender() override;

  private:
	LoggingPanel logger;
};

}  // namespace Atlas