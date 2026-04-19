#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/AudioHardwareBase.h>
#include "Atlas/Audio/AudioDevice.h"

namespace Atlas {

class CoreAudioDevice : public AudioDevice {
  public:
	CoreAudioDevice(const AudioDeviceSpecifications& specs) : m_specs(specs) {
		m_audioUnit = nullptr;
		m_isRunning = false;
	}

	virtual void open() override;
	virtual void close() override;

	virtual void start() override;
	virtual void stop() override;

	virtual AudioDeviceSpecifications& getConfig() override { return m_specs; }

  private:
	static OSStatus renderCallback(void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList* ioData);

    static OSStatus deviceChangeCallback(AudioObjectID inObjectID, UInt32 inNumberAddresses, const AudioObjectPropertyAddress* inAddresses, void* inClientData);

  private:
	AudioComponentInstance m_audioUnit;

	AudioDeviceSpecifications m_specs;
	bool					  m_isRunning;
};

}  // namespace Atlas