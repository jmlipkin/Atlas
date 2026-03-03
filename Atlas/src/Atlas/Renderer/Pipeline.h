#pragma once

#include <glm/glm.hpp>

#include "Atlas/Renderer/Framebuffer.h"
#include "Buffer.h"
#include "Shader.h"

namespace Atlas {

struct PipelineSpecification {
	std::string name;
	std::shared_ptr<Shader> shader;
	BufferLayout layout;
	bool enableDepthWrite = true;
};

class Pipeline {
  public:
	virtual ~Pipeline() = default;

	virtual void attachFramebuffer(std::shared_ptr<Framebuffer> framebuffer) = 0;
	virtual void setLayout(const BufferLayout& layout) = 0;

	virtual const std::string& getName() const = 0;

	static std::shared_ptr<Pipeline> create(const PipelineSpecification& specs);
};

}  // namespace Atlas