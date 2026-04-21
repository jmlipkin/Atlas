#include "atpch.h"
#include "AudioFile.h"

#include "Atlas/Audio/CachedAudioFile.h"
#include "Atlas/Audio/StreamedAudioFile.h"

namespace Atlas {

std::shared_ptr<AudioFile> AudioFile::create(const std::string& filepath, AudioFileType type) {
	switch (type) {
		case AudioFileType::CACHED:
			return std::make_shared<CachedAudioFile>(filepath);
		case AudioFileType::STREAMED:
			return std::make_shared<StreamedAudioFile>(filepath);
	}
	AT_CORE_ASSERT(false, "Unable to create AudioFile of unknown type");
	return nullptr;
}

std::shared_ptr<AudioFile> AudioFile::create(const AudioFileSpecification& specs, float* data) {
	switch (specs.type) {
		case AudioFileType::CACHED:
			return std::make_shared<CachedAudioFile>(specs, data);
		case AudioFileType::STREAMED:
			return std::make_shared<StreamedAudioFile>(specs, data);
	}
	AT_CORE_ASSERT(false, "Unable to create AudioFile of unknown type");
	return nullptr;
}

}  // namespace Atlas