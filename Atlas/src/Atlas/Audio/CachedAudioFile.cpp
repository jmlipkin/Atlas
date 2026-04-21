#include "atpch.h"
#include "CachedAudioFile.h"

#include "Atlas/Audio/AudioFile.h"

#include "libsndfile/include/sndfile.h"

namespace Atlas {

CachedAudioFile::CachedAudioFile(const std::string& filepath) {
	bool error = loadFile(filepath);

	if (error) AT_CORE_WARN("Could not load file '{}'", filepath);
}

// Assumes input data is interleaved
CachedAudioFile::CachedAudioFile(const AudioFileSpecification& specs, float* data) {
	m_specs		 = specs;
	m_specs.type = AudioFileType::CACHED;

	m_data.assign(data, data + m_specs.channelCount * m_specs.frameCount);
	deinterleave(m_data);
}

uint64_t CachedAudioFile::readFrames(float** buffers, uint32_t offset, uint64_t frameCount) {
	uint64_t available = m_specs.frameCount - offset;
	uint64_t readCount = std::min(frameCount, available);

	for (size_t ch = 0; ch < m_specs.channelCount; ch++) {
		memcpy(buffers[ch], m_data.data() + ch * m_specs.frameCount + offset, readCount * sizeof(float));
	}

	return readCount;
}

bool CachedAudioFile::loadFile(const std::string& filepath) {
	SNDFILE* file;
	SF_INFO	 sfinfo;

	file = sf_open(filepath.c_str(), SFM_READ, &sfinfo);
	if (file == nullptr) {
		AT_CORE_ASSERT(false, "Unable to open file '{}' for reading", filepath);
		return true;
	}

	m_specs.sampleRate	 = sfinfo.samplerate;
	m_specs.channelCount = sfinfo.channels;
	m_specs.frameCount	 = sfinfo.frames;
	m_specs.type		 = AudioFileType::CACHED;

	m_data.resize(m_specs.frameCount * m_specs.channelCount);

	sf_readf_float(file, m_data.data(), m_specs.frameCount);

	sf_close(file);

	deinterleave(m_data);

	return false;
}

void CachedAudioFile::deinterleave(std::vector<float>& data) {
	std::vector<float> buffer(data.size());

	int chCount = m_specs.channelCount;
	int FC		= m_specs.frameCount;

	for (size_t i = 0; i < data.size(); i++) {
		int ch	  = i / FC;
		int frame = i % FC;
		buffer[i] = data[frame * chCount + ch];
	}

	data = buffer;
}

}  // namespace Atlas