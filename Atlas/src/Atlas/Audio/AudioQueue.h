#pragma once

#include "Atlas/Core/UUID.h"
#include "Atlas/Core/Thread/SPSCQueue.h"
#include "Atlas/Audio/AudioEngineCommand.h"

namespace Atlas {

struct AudioVoiceFinishedPayload {
	UUID voiceID;
};
struct AudioVoiceLoopedPayload {
	UUID voiceID;
};
struct AudioLoadErrorPayload {
	char path[256];
	char message[128];
};

using AudioEventPayload = std::variant<
	AudioVoiceFinishedPayload,
	AudioVoiceLoopedPayload,
	AudioLoadErrorPayload>;

constexpr size_t kAudioCommandCapacity = 256;
constexpr size_t kAudioEventCapacity   = 256;

using AudioCommandQueue = SPSCQueue<AudioCommand, kAudioCommandCapacity>;
using AudioEventQueue	= SPSCQueue<AudioEventPayload, kAudioEventCapacity>;

}  // namespace Atlas