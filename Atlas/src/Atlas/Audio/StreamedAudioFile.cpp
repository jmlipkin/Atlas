#include "atpch.h"
#include "StreamedAudioFile.h"

#include "Atlas/Audio/AudioFile.h"

#include "libsndfile/include/sndfile.h"

namespace Atlas {

StreamedAudioFile::StreamedAudioFile(const std::string& filepath) {
	bool error = loadFile(filepath);

	if (error) AT_CORE_WARN("Could not load file '{}'", filepath);
}

// Assumes input data is interleaved
StreamedAudioFile::StreamedAudioFile(const AudioFileSpecification& specs, float* data) {
	m_specs		 = specs;
	m_specs.type = AudioFileType::STREAMED;
}

StreamedAudioFile::~StreamedAudioFile() {
	if (m_fp) {
		sf_close(m_fp);
		m_fp = nullptr;
	}
}

uint64_t StreamedAudioFile::readFrames(float** buffers, uint32_t offset, uint64_t frameCount) {
	uint64_t available = m_specs.frameCount - offset;
	uint64_t readCount = std::min(frameCount, available);

	std::vector<float> buffer(readCount * m_specs.channelCount);

	AT_CORE_ASSERT(m_fp != nullptr);

	sf_readf_float(m_fp, buffer.data(), readCount);

	deinterleave(buffer, m_specs.channelCount, readCount);

	for (size_t ch = 0; ch < m_specs.channelCount; ch++) {
		memcpy(buffers[ch], buffer.data() + ch * m_specs.frameCount + offset, readCount * sizeof(float));
	}

	return readCount;
}

bool StreamedAudioFile::loadFile(const std::string& filepath) {
	SF_INFO sfinfo;

	m_fp = sf_open(filepath.c_str(), SFM_READ, &sfinfo);
	if (m_fp == nullptr) {
		AT_CORE_ASSERT(false, "Unable to open file '{}' for reading", filepath);
		return true;
	}

	m_specs.sampleRate	 = sfinfo.samplerate;
	m_specs.channelCount = sfinfo.channels;
	m_specs.frameCount	 = sfinfo.frames;
	m_specs.type		 = AudioFileType::STREAMED;

	return false;
}

void StreamedAudioFile::deinterleave(std::vector<float>& data, int channelCount, int frameCount) {
	std::vector<float> buffer(data.size());

	for (size_t i = 0; i < data.size(); i++) {
		int ch	  = i / frameCount;
		int frame = i % frameCount;
		buffer[i] = data[frame * channelCount + ch];
	}

	data = buffer;
}

}  // namespace Atlas