#pragma once

#include "Atlas/Audio/AudioDevice.h"
#include "Atlas/Audio/AudioQueue.h"
#include "Atlas/Events/AudioEvent.h"

#include "Atlas/Core/Application.h"

namespace Atlas {

class AudioEngine {
  public:
	static void init() {
		AudioDeviceSpecifications specs = {};
		specs.sampleRate				= 48000;
		specs.bufferSize				= 512;
		specs.channelCount				= 2;

		get().m_device = AudioDevice::create(specs);
		get().m_device->open();

		get().m_device->setRenderCallback([specs](float** buffers, uint32_t numChannels, uint32_t numFrames) {
			for (uint32_t ch = 0; ch < numChannels; ch++) {
				for (size_t i = 0; i < numFrames; i++) {
					buffers[ch][i] = 0;
				}
			}
		});

		get().m_device->start();
	}

	static void shutdown() {
		get().m_device->stop();
		get().m_device->close();
	}

	static void update() {
		AudioEventPayload payload;
		while (get().m_eventQueue.pop(payload)) {
			std::visit([](auto&& e) {
				using T = std::decay_t<decltype(e)>;
				if constexpr (std::is_same_v<T, AudioVoiceFinishedPayload>) {
					AudioVoiceFinishedEvent event(e.voiceID);
					Application::get().onEvent(event);
				} else if constexpr (std::is_same_v<T, AudioVoiceLoopedPayload>) {
					AudioVoiceLoopedEvent event(e.voiceID);
					Application::get().onEvent(event);
				} else if constexpr (std::is_same_v<T, AudioLoadErrorPayload>) {
					AudioLoadErrorEvent event(e.path, e.message);
					Application::get().onEvent(event);
				}
			}, payload);
		}
	}

	static AudioEngine&
	get() {
		static AudioEngine instance;
		return instance;
	}

  private:
	std::shared_ptr<AudioDevice> m_device;

	AudioCommandQueue m_commandQueue;
	AudioEventQueue	  m_eventQueue;
};

}  // namespace Atlas