#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Atlas {

/**
 * @brief Used to determined storage method of AudioFile.
 *
 */
enum AudioFileType { CACHED,
					 STREAMED };

/**
 * @brief Core properties of an audio file.
 *
 */
struct AudioFileSpecification {
	uint32_t	  sampleRate;
	uint32_t	  channelCount;
	uint64_t	  frameCount;
	AudioFileType type;

	/**
	 * @brief Calculates duration (seconds) using frameCount and sampleRate
	 *
	 * @return float - duration in seconds
	 */
	float duration() const { return float(frameCount) / sampleRate; }
};

/**
 * @brief An abstract representation of an audio asset. Uses AudioFileType enum to create either a CachedAudioFile or a StreamedAudioFile.
 *
 */
class AudioFile {
  public:
	virtual ~AudioFile() = default;

	/**
	 * @brief Reads audio data into destination buffers. User must ensure that buffers is of adequate size.
	 *
	 * @param buffers - read destination. Assumes 2D array of size [channelCount][frameCount]. (ChannelCount is stored internally)
	 * @param offset - frame offset from start of file.
	 * @param frameCount - number of frames to read.
	 * @return uint64_t - returns number of frames read successfully.
	 */
	virtual uint64_t readFrames(float** buffers, uint32_t offset, uint64_t frameCount) = 0;

	virtual std::string	  getFilepath() const = 0;
	virtual AudioFileType getType() const	  = 0;

	virtual uint32_t getSampleRate() const	 = 0;
	virtual uint32_t getChannelCount() const = 0;

	/**
	 * @brief Get the frame count of the AudioFile
	 *
	 * @return uint64_t - number of frames
	 */
	virtual uint64_t getFrameCount() const = 0;

	/**
	 * @brief Get the duration of the AudioFile in seconds
	 *
	 * @return float - length in seconds
	 */
	virtual float getDuration() const = 0;

	/**
	 * @brief Create a new AudioFile from a file on disk.
	 *
	 * @param filepath - absolute path to audio file.
	 * @param type - select whether audio file is cached or streamed.
	 * @return std::shared_ptr<AudioFile> - generated AudioFile object.
	 */
	static std::shared_ptr<AudioFile> create(const std::string& filepath, AudioFileType type);

	/**
	 * @brief Create a new AudioFile from manual parameters.
	 *
	 * @param specs - metadata.
	 * @param data - audio data, as interleaved PCM float32 format.
	 * @return std::shared_ptr<AudioFile> - generated AudioFile object.
	 */
	static std::shared_ptr<AudioFile> create(const AudioFileSpecification& specs, float* data);
};

}  // namespace Atlas