#pragma once

#include "Atlas/Core/UUID.h"

namespace Atlas {

enum class AudioCommandType {
	PlayVoice,
	StopVoice,
	PauseVoice,
	SeekVoice,
	SetVoiceParam,
	SetBusParam,
	AddBus,
	RemoveBus,
	ConnectBus,
	InsertDSPNode,
	RemoveDSPNode
};

enum class AudioParamType { Volume,
							Pitch,
							Pan,
							SendLevel };

struct PlayVoiceCommand {
	UUID  voiceID;
	UUID  clipID;
	UUID  busID;
	float volume;
	float pitch;
	bool  loop;
};

struct StopVoiceCommand {
	UUID voiceID;
};

struct PauseVoiceCommand {
	UUID voiceID;
};

struct SeekVoiceCommand {
	UUID	 voiceID;
	uint64_t frameIndex;
};

struct SetVoiceParamCommand {
	UUID		   voiceID;
	AudioParamType param;
	float		   target;
	uint32_t	   rampSamples;
};

struct SetBusParamCommand {
	UUID		   busID;
	AudioParamType param;
	float		   target;
	uint32_t	   rampSamples;
};

struct AddBusCommand {
	UUID busID;
	char name[64];
};

struct RemoveBusCommand {
	UUID busID;
};

struct ConnectBusCommand {
	UUID srcBusID;
	UUID dstBusID;
};

struct InsertDSPNodeCommand {
	UUID	 busID;
	UUID	 nodeID;
	uint32_t slotIndex;
};

struct RemoveDSPNodeCommand {
	UUID busID;
	UUID nodeID;
};

using AudioCommand = std::variant<
	PlayVoiceCommand, StopVoiceCommand, PauseVoiceCommand, SeekVoiceCommand, SetVoiceParamCommand, SetBusParamCommand, AddBusCommand, RemoveBusCommand, ConnectBusCommand, InsertDSPNodeCommand, RemoveDSPNodeCommand>;

}  // namespace Atlas