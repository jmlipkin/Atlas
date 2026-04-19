#include "atpch.h"
#include "CoreAudioDevice.h"

#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CFString.h>

#define MAX_CHANNEL_COUNT 16

#define AT_CHECK_OS(expr) checkOSStatus((expr), #expr)

namespace Atlas {

static bool checkOSStatus(OSStatus status, const char* label) {
	if (status == noErr) return true;

	char   code[5];
	UInt32 error = CFSwapInt32HostToBig(status);
	memcpy(code, &error, 4);
	code[4] = '\0';

	bool isPrintable = true;
	for (int i = 0; i < 4; i++) {
		if (code[i] < 32 || code[i] > 126) {
			isPrintable = false;
			break;
		}
	}

	if (isPrintable)
		AT_CORE_ERROR("CoreAudio error at {}: '{}' ({})", label, code, status);
	else
		AT_CORE_ERROR("CoreAudio error at {}: {}", label, status);

	return false;
}

OSStatus CoreAudioDevice::renderCallback(void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList* ioData) {
	CoreAudioDevice* device = (CoreAudioDevice*)inRefCon;
	float*			 buffers[MAX_CHANNEL_COUNT];

	for (int i = 0; i < ioData->mNumberBuffers; i++)
		buffers[i] = (float*)ioData->mBuffers[i].mData;

	device->m_renderCallback(buffers, ioData->mNumberBuffers, inNumberFrames);

	return noErr;
}

OSStatus CoreAudioDevice::deviceChangeCallback(AudioObjectID inObjectID, UInt32 inNumberAddresses, const AudioObjectPropertyAddress* inAddresses, void* inClientData) {
	CoreAudioDevice* device = (CoreAudioDevice*)inClientData;

	dispatch_async(dispatch_get_main_queue(), ^{
	  AT_CORE_TRACE("CoreAudio: output device changed, reconnecting...");
	  device->stop();
	  device->close();
	  device->open();
	  device->start();
	});

	return noErr;
}

void CoreAudioDevice::open() {
	AudioComponentDescription desc = {};
	desc.componentType			   = kAudioUnitType_Output;
	desc.componentSubType		   = kAudioUnitSubType_DefaultOutput;
	desc.componentManufacturer	   = kAudioUnitManufacturer_Apple;

	AudioComponent component = AudioComponentFindNext(NULL, &desc);
	if (!component) {
		AT_CORE_ERROR("CoreAudio: failed to find default output");
		return;
	}

	if (!AT_CHECK_OS(AudioComponentInstanceNew(component, &m_audioUnit))) return;

	AudioStreamBasicDescription streamDesc = {};
	streamDesc.mSampleRate				   = m_specs.sampleRate;
	streamDesc.mFormatID				   = kAudioFormatLinearPCM;
	streamDesc.mFormatFlags				   = kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved;
	streamDesc.mBytesPerPacket			   = sizeof(float);
	streamDesc.mBytesPerFrame			   = sizeof(float);
	streamDesc.mFramesPerPacket			   = 1;
	streamDesc.mChannelsPerFrame		   = m_specs.channelCount;
	streamDesc.mBitsPerChannel			   = 32;

	if (!AT_CHECK_OS(AudioUnitSetProperty(m_audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamDesc, sizeof(streamDesc)))) return;

	AURenderCallbackStruct renderCBStruct;
	renderCBStruct.inputProc	   = &renderCallback;
	renderCBStruct.inputProcRefCon = this;

	if (!AT_CHECK_OS(AudioUnitSetProperty(m_audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &renderCBStruct, sizeof(renderCBStruct)))) return;

	if (!AT_CHECK_OS(AudioUnitInitialize(m_audioUnit))) return;

	AudioDeviceID			   deviceID;
	UInt32					   deviceIDSize		 = sizeof(AudioDeviceID);
	AudioObjectPropertyAddress defaultDeviceAddr = {};
	defaultDeviceAddr.mSelector					 = kAudioHardwarePropertyDefaultOutputDevice;
	defaultDeviceAddr.mScope					 = kAudioObjectPropertyScopeGlobal;
	defaultDeviceAddr.mElement					 = kAudioObjectPropertyElementMain;
	AT_CHECK_OS(AudioObjectGetPropertyData(kAudioObjectSystemObject, &defaultDeviceAddr, 0, nullptr, &deviceIDSize, &deviceID));

	CFStringRef				   deviceNameRef;
	UInt32					   deviceNameSize = sizeof(CFStringRef);
	AudioObjectPropertyAddress deviceNameAddr = {};
	deviceNameAddr.mSelector				  = kAudioDevicePropertyDeviceNameCFString;
	deviceNameAddr.mScope					  = kAudioObjectPropertyScopeOutput;
	deviceNameAddr.mElement					  = kAudioObjectPropertyElementMain;
	AT_CHECK_OS(AudioObjectGetPropertyData(deviceID, &deviceNameAddr, 0, nullptr, &deviceNameSize, &deviceNameRef));

	char deviceName[256];
	CFStringGetCString(deviceNameRef, deviceName, sizeof(deviceName), kCFStringEncodingUTF8);
	CFRelease(deviceNameRef);

	AT_CORE_TRACE("CoreAudio: output device '{}' connected", deviceName);

	AudioObjectPropertyAddress deviceChangeAddr = {};
	deviceChangeAddr.mSelector					= kAudioHardwarePropertyDefaultOutputDevice;
	deviceChangeAddr.mScope						= kAudioObjectPropertyScopeGlobal;
	deviceChangeAddr.mElement					= kAudioObjectPropertyElementMain;

	AT_CHECK_OS(AudioObjectAddPropertyListener(kAudioObjectSystemObject, &deviceChangeAddr, &deviceChangeCallback, this));
}

void CoreAudioDevice::close() {
	stop();

	AudioObjectPropertyAddress deviceChangeAddr = {};
	deviceChangeAddr.mSelector					= kAudioHardwarePropertyDefaultOutputDevice;
	deviceChangeAddr.mScope						= kAudioObjectPropertyScopeGlobal;
	deviceChangeAddr.mElement					= kAudioObjectPropertyElementMain;

	AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &deviceChangeAddr, &deviceChangeCallback, this);

	if (!AT_CHECK_OS((AudioUnitUninitialize(m_audioUnit)))) return;
	if (!AT_CHECK_OS(AudioComponentInstanceDispose(m_audioUnit))) return;

	m_audioUnit = nullptr;
}

void CoreAudioDevice::start() {
	if (!m_audioUnit) {
		AT_CORE_ERROR("Unable to start audio: Audio unit doesn't exist");
		return;
	}

	if (!m_isRunning) {
		AT_CHECK_OS(AudioOutputUnitStart(m_audioUnit));
		m_isRunning = true;
	}
}

void CoreAudioDevice::stop() {
	if (!m_audioUnit) AT_CORE_ERROR("Unable to stop audio: Audio unit doesn't exist");

	if (m_isRunning) {
		AT_CHECK_OS(AudioOutputUnitStop(m_audioUnit));
		m_isRunning = false;
	}
}

}  // namespace Atlas