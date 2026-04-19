#pragma once

#include "Atlas/Core/UUID.h"
#include "Atlas/Events/Event.h"

namespace Atlas {

class AudioEvent : public Event {
  public:
	EVENT_CLASS_CATEGORY(EVENT_CATEGORY_AUDIO)
};

class AudioVoiceFinishedEvent : public AudioEvent {
  public:
	AudioVoiceFinishedEvent(UUID voiceID) : m_voiceID(voiceID) {}

	UUID getVoiceID() const { return m_voiceID; }

	EVENT_CLASS_TYPE(AUDIO_VOICE_FINISHED)
  private:
	UUID m_voiceID;
};

class AudioVoiceLoopedEvent : public AudioEvent {
  public:
	AudioVoiceLoopedEvent(UUID voiceID) : m_voiceID(voiceID) {}

	UUID getVoiceID() const { return m_voiceID; }

	EVENT_CLASS_TYPE(AUDIO_VOICE_LOOPED)

  private:
	UUID m_voiceID;
};

class AudioLoadErrorEvent : public AudioEvent {
  public:
	AudioLoadErrorEvent(const std::string& path, const std::string& message) : m_path(path), m_message(message) {}

	const std::string& getPath() const { return m_path; }
	const std::string& getMessage() const { return m_message; }

	EVENT_CLASS_TYPE(AUDIO_LOAD_ERROR)
  private:
	std::string m_path;
	std::string m_message;
};

}  // namespace Atlas