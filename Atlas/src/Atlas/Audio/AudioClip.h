#pragma once

#include "Atlas/Audio/AudioFile.h"

namespace Atlas {

struct AudioClipSpecification {
	uint64_t startFrame;
	uint64_t endFrame;
};

/**
 * @brief Object that stores metadata referencing an AudioFile asset.
 *
 */
class AudioClip {
  public:
	/**
	 * @brief Construct a new Audio Clip object (untrimmed)
	 *
	 * @param filepath - Absolute path to audio file
	 * @param type - Loads asset into either memory (CACHED) or loads file metadata to be streamed (STREAMED). Type processing is handled internally.
	 * @note If multiple AudioClips refer to the same AudioFile, the type (CACHED or STREAMED) is set according to the first request.
	 */
	AudioClip(const std::string& filepath, AudioFileType type = AudioFileType::CACHED);

	/**
	 * @brief Construct a new Audio Clip object with range (frames)
	 *
	 * @param filepath - Absolute path to audio file
	 * @param type - Loads asset into either memory (CACHED) or loads file metadata to be streamed (STREAMED). Type processing is handled internally.
	 * @param startFrame - frame offset from start of file
	 * @param endFrame - endFrame == 0 sets end to EOF.
	 * @note If multiple AudioClips refer to the same AudioFile, the type (CACHED or STREAMED) is set according to the first request.
	 */
	AudioClip(const std::string& filepath, uint64_t startFrame = 0, uint64_t endFrame = 0, AudioFileType type = AudioFileType::CACHED);

	/**
	 * @brief Construct a new Audio Clip object with time-based range (seconds)
	 *
	 * @param filepath - Absolute path to audio file
	 * @param type - Loads asset into either memory (CACHED) or loads file metadata to be streamed (STREAMED). Type processing is handled internally.
	 * @param offset - time offset from start of file.
	 * @param duration - duration == 0 sets end to EOF.
	 */
	AudioClip(const std::string& filepath, float offset = 0.0f, float duration = 0.0f, AudioFileType type = AudioFileType::CACHED);

	/**
	 * @brief Get the frame count of the AudioClip
	 *
	 * @return uint32_t - number of frames
	 */
	uint32_t getFrameCount() const { return m_specs.endFrame - m_specs.startFrame; }

	/**
	 * @brief Get the duration of the AudioClip in seconds
	 *
	 * @return float - length in seconds
	 */
	float getDuration() const { return float(getFrameCount()) / float(m_sampleRate); }

  private:
	/**
	 * @brief Helper function - loads metadata by fetching the stored AudioFile from AssetManager.
	 *
	 * @param dstSampleRate - address of uint32_t object to store sample rate
	 * @param dstFrameCount - address of uint32_t object to store frame count
	 */
	void setFileInfo(uint32_t* dstSampleRate, uint64_t* dstFrameCount);

  private:
	std::string			   m_path;
	AudioFileType		   m_type;
	AudioClipSpecification m_specs;
	uint32_t			   m_sampleRate;
};

}  // namespace Atlas