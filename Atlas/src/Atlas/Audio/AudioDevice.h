#pragma once

namespace Atlas {

struct AudioDeviceSpecifications {
	int			sampleRate;
	int			channelCount;
	size_t		bufferSize;
	std::string deviceName;
};

class AudioDevice {
  public:
	using AudioRenderCallback = std::function<void(float** buffers, uint32_t numChannels, uint32_t numFrames)>;

	virtual void open()	 = 0;
	virtual void close() = 0;

	virtual void start() = 0;
	virtual void stop()	 = 0;

	void setRenderCallback(AudioRenderCallback callback) { m_renderCallback = callback; }

	virtual AudioDeviceSpecifications&	getConfig() = 0;
	static std::shared_ptr<AudioDevice> create(const AudioDeviceSpecifications& specs);

  protected:
	AudioRenderCallback m_renderCallback;
};

}  // namespace Atlas