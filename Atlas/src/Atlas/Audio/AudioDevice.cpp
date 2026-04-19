#include "atpch.h"
#include "AudioDevice.h"

#include "Atlas/Platform/CoreAudio/CoreAudioDevice.h"

namespace Atlas {

std::shared_ptr<AudioDevice> AudioDevice::create(const AudioDeviceSpecifications& specs) {
#ifdef AT_PLATFORM_MACOS
	return std::make_shared<CoreAudioDevice>(specs);
#endif

	return nullptr;
}

}  // namespace Atlas