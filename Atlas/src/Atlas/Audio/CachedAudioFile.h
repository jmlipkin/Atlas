#pragma once

#include "Atlas/Audio/AudioFile.h"

namespace Atlas {

/**
 * @brief Audio asset stored in memory.
 *
 */
class CachedAudioFile : public AudioFile {
  public:
	/**
	 * @brief Construct a new CachedAudioFile from a file on disk.
	 *
	 * @param filepath - absolute path to audio file.
	 */
	CachedAudioFile(const std::string& filepath);

	/**
	 * @brief Construct a new CachedAudioFile from manual parameters.
	 *
	 * @param specs - metadata.
	 * @param data - audio data, as interleaved PCM float32 format.
	 */
	CachedAudioFile(const AudioFileSpecification& specs, float* data);

	/**
	 * @brief Reads audio data into destination buffers. User must ensure that buffers is of adequate size.
	 *
	 * @param buffers - read destination. Assumes 2D array of size [channelCount][frameCount]. (ChannelCount is stored internally)
	 * @param offset - frame offset from start of file.
	 * @param frameCount - number of frames to read.
	 * @return uint64_t - returns number of frames read successfully.
	 */
	virtual uint64_t readFrames(float** buffers, uint32_t offset, uint64_t frameCount) override;

	virtual std::string	  getFilepath() const override { return m_filepath; }
	virtual AudioFileType getType() const override { return m_specs.type; }

	virtual uint32_t getSampleRate() const override { return m_specs.sampleRate; }
	virtual uint32_t getChannelCount() const override { return m_specs.channelCount; }

	/**
	 * @brief Get the frame count of the AudioFile
	 *
	 * @return uint64_t - number of frames
	 */
	virtual uint64_t getFrameCount() const override { return m_specs.frameCount; }

	/**
	 * @brief Get the duration of the AudioFile in seconds
	 *
	 * @return float - length in seconds
	 */
	virtual float getDuration() const override { return m_specs.duration(); }

  private:
	bool loadFile(const std::string& filepath);

	void deinterleave(std::vector<float>& data);

  private:
	std::string			   m_filepath;
	AudioFileSpecification m_specs;

	std::vector<float> m_data;
};

}  // namespace Atlas