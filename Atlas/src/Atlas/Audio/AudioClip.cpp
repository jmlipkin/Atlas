#include "atpch.h"
#include "AudioClip.h"

#include "Atlas/Core/AssetManager.h"

namespace Atlas {

AudioClip::AudioClip(const std::string& filepath, AudioFileType type) : m_path(filepath), m_type(type) {
	uint64_t frameCount;
	setFileInfo(&m_sampleRate, &frameCount);

	m_specs.startFrame = 0;
	m_specs.endFrame   = frameCount;
}

AudioClip::AudioClip(const std::string& filepath, uint64_t startFrame, uint64_t endFrame, AudioFileType type) : m_path(filepath), m_type(type) {
	uint64_t frameCount;
	setFileInfo(&m_sampleRate, &frameCount);

	m_specs.startFrame = startFrame;
	m_specs.endFrame   = (endFrame) ? endFrame : frameCount;
}

AudioClip::AudioClip(const std::string& filepath, float offset, float duration, AudioFileType type) : m_path(filepath), m_type(type) {
	uint64_t frameCount;
	setFileInfo(&m_sampleRate, &frameCount);

	m_specs.startFrame = offset * m_sampleRate;
	m_specs.endFrame   = (duration) ? duration * m_sampleRate : frameCount;
}

void AudioClip::setFileInfo(uint32_t* dstSampleRate, uint64_t* dstFrameCount) {
	std::shared_ptr<AudioFile> file = AssetManager::loadAudio(m_path, m_type);
}

}  // namespace Atlas