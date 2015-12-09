#include "library/sp.h"
#include "framework/sound_interface.h"
#include "framework/logger.h"
#include <SDL_audio.h>
#include <SDL.h>

#include <list>

namespace
{

SDL_AudioSpec outputFormat;

using namespace OpenApoc;

struct SampleData
{
	unsigned char *samplePos;
	int volume;
	SampleData(unsigned char *samplePos, int volume) : samplePos(samplePos), volume(volume) {};
};

class SDLSampleData : public BackendSampleData
{
  public:
	std::list<SampleData> playingInfo;
	unsigned char *sampleStart;
	int sampleLen;
	bool isFinished;

	SDL_AudioCVT cvt;

	SDLSampleData(sp<Sample> sample) : sampleStart(0), sampleLen(0), isFinished(false)
	{
		SDL_AudioFormat sdlFormat;
		int smplSize;
		switch (sample->format.format)
		{
			case AudioFormat::SampleFormat::PCM_SINT16:
				sdlFormat = AUDIO_S16LSB;
				smplSize = 2;
				break;
			case AudioFormat::SampleFormat::PCM_UINT8:
				sdlFormat = AUDIO_U8;
				smplSize = 1;
				break;
			default:
				LogWarning("Unknown sample format: %d", sample->format.format);
				return;
		}
		Uint8 srcChannels = sample->format.channels;
		SDL_BuildAudioCVT(&cvt, sdlFormat, srcChannels, sample->format.frequency,
		                  outputFormat.format, outputFormat.channels, outputFormat.freq);

		cvt.len = smplSize * srcChannels * sample->sampleCount;

		cvt.buf = (Uint8 *)SDL_malloc(cvt.len * cvt.len_mult);
		SDL_memcpy(cvt.buf, sample->data.get(), cvt.len);
		SDL_ConvertAudio(&cvt);
		sampleStart = cvt.buf;
		sampleLen = cvt.len_cvt;
	}

	virtual ~SDLSampleData() { SDL_free(cvt.buf); }
};

class SDLRawBackend : public SoundBackend
{
	AudioFormat preferredFormat;

	long int sampleId; // FIXME: Handle (or don't handle) overflows?

	struct
	{
		int overallVolume;
		int musicVolume;
		int soundVolume;
	} mixVolumes;

	sp<MusicTrack> track;
	sp<Sample> musicSample;

	std::list<sp<Sample>> sampleQueue;
	SDL_AudioDeviceID devID;

	std::function<void(void *)> musicFinishedCallback;
	void *musicCallbackData;

	bool musicPaused;

	static void mixingCallback(void *userdata, Uint8 *stream, int len)
	{
		// pass "this" pointer as a user data pointer - might be useful?
		SDLRawBackend *_this = reinterpret_cast<SDLRawBackend *>(userdata);
		// initialize stream buffer
		SDL_memset(stream, 0, len); // FIXME: Calculate silence value for current output format?
		MusicTrack::MusicCallbackReturn musReturn = MusicTrack::MusicCallbackReturn::Continue;

		if (!_this->musicPaused)
		{
			// mix music first
			if (!_this->musicSample.get())
			{
				int sampleSize;
				switch (_this->track->format.format)
				{
					case AudioFormat::SampleFormat::PCM_SINT16:
						sampleSize = 2;
						break;
					case AudioFormat::SampleFormat::PCM_UINT8:
						sampleSize = 1;
						break;
					default:
						LogWarning("Unknown sample format!");
						break;
				}
				sampleSize *= _this->track->format.channels;
				_this->musicSample.reset(new Sample());
				_this->musicSample->format = _this->track->format;
				_this->musicSample->sampleCount = _this->track->sampleCount;
				_this->musicSample->data.reset(
				    new unsigned char[_this->musicSample->sampleCount * sampleSize]);
				musReturn = _this->track->callback(_this->track, _this->musicSample->sampleCount,
				                                   _this->musicSample->data.get(),
				                                   &(_this->musicSample->sampleCount));
				_this->musicSample->backendData.reset(new SDLSampleData(_this->musicSample));
				SDLSampleData *musicData =
				    static_cast<SDLSampleData *>(_this->musicSample->backendData.get());
				musicData->playingInfo.push_back(SampleData{ musicData->sampleStart, 128 });
			}

			SDLSampleData *musicData =
			    static_cast<SDLSampleData *>(_this->musicSample->backendData.get());
			int smplLen = (musicData->sampleStart + musicData->sampleLen) -
			              musicData->playingInfo.front().samplePos;
			int musLen = std::min(len, smplLen);

			SDL_MixAudioFormat(stream, (Uint8 *)musicData->playingInfo.front().samplePos,
			                   musicData->cvt.dst_format, musLen, _this->mixVolumes.musicVolume);
			musicData->playingInfo.front().samplePos += musLen;

			if (musicData->playingInfo.front().samplePos ==
			    (musicData->sampleStart + musicData->sampleLen))
			{
				if (musReturn == MusicTrack::MusicCallbackReturn::End)
				{
					_this->musicFinishedCallback(_this->musicCallbackData);
				}
				_this->musicSample.reset();
			}
		}

		// mix pending samples
		auto sndIt = _this->sampleQueue.begin();
		while (sndIt != _this->sampleQueue.end())
		{
			SDLSampleData *smplData = static_cast<SDLSampleData *>((*sndIt)->backendData.get());
			auto smplIt = smplData->playingInfo.begin();
			while (smplIt != smplData->playingInfo.end())
			{
				int smplLen = (smplData->sampleStart + smplData->sampleLen) -
				              smplIt->samplePos; // Remaining length of sample
				int mixLen = std::min(len, smplLen);
				int volume = ((float)_this->mixVolumes.soundVolume / 128) * smplIt->volume;
				SDL_MixAudioFormat(stream, (Uint8 *)smplIt->samplePos,
				                   smplData->cvt.dst_format, mixLen, _this->mixVolumes.soundVolume);
				smplIt->samplePos += mixLen;
				if (smplIt->samplePos == (smplData->sampleStart + smplData->sampleLen))
				{
					smplIt = smplData->playingInfo.erase(smplIt);
				}
				else
				{
					++smplIt;
				}
			}

			if (smplData->playingInfo.size() == 0) // Remove played sound
			{
				sndIt = _this->sampleQueue.erase(sndIt);
			}
			else
			{
				++sndIt;
			}
		}
	}

  public:
	SDLRawBackend() : musicPaused(true), sampleId(2) // use sampleId of 1 for music?
	{
		SDL_Init(SDL_INIT_AUDIO);
		preferredFormat.channels = 2;
		preferredFormat.format = AudioFormat::SampleFormat::PCM_SINT16;
		preferredFormat.frequency = 22050;
		LogInfo("Current audio driver: %s", SDL_GetCurrentAudioDriver());
		LogWarning("Changing audio drivers is not currently implemented!");
		int numDevices = SDL_GetNumAudioDevices(0); // Request playback devices only
		LogInfo("Number of audio devices: %d", numDevices);
		for (int i = 0; i < numDevices; ++i)
		{
			LogInfo("Device %d: %s", i, SDL_GetAudioDeviceName(i, 0));
		}
		LogWarning(
		    "Selecting audio devices not currently implemented! Selecting first available device.");
		const char *deviceName = SDL_GetAudioDeviceName(0, 0);
		SDL_AudioSpec wantFormat;
		wantFormat.channels = 2;
		wantFormat.format = AUDIO_S16LSB;
		wantFormat.freq = 22050;
		wantFormat.samples = 512;
		wantFormat.callback = mixingCallback;
		wantFormat.userdata = this;
		devID = SDL_OpenAudioDevice(
		    deviceName,
		    0, // capturing is not supported
		    &wantFormat, &outputFormat,
		    SDL_AUDIO_ALLOW_ANY_CHANGE); // hopefully we'll get a sane output format
		mixVolumes.musicVolume = 64;
		mixVolumes.soundVolume = 32;
		mixVolumes.overallVolume = 128;
		SDL_PauseAudioDevice(devID, 0); // Run at once?
	}
	virtual void playSample(sp<Sample> sample, float gain) override
	{
		std::ignore = gain; // FIXME: Actually make use of this value
		SDL_LockAudioDevice(devID);
		if (!sample->backendData)
		{
			sample->backendData.reset(new SDLSampleData(sample));
		}
		SDLSampleData *data = static_cast<SDLSampleData *>(sample->backendData.get());
		data->playingInfo.push_back(SampleData{ data->sampleStart, static_cast<int>(gain * 128) });
		sampleId++;
		sampleQueue.push_back(sample);
		LogInfo("Placed sound %p on queue", sample.get());
		SDL_UnlockAudioDevice(devID);
	}

	virtual void playMusic(std::function<void(void *)> finishedCallback,
	                       void *callbackData) override
	{
		musicFinishedCallback = finishedCallback;
		musicCallbackData = callbackData;
		musicPaused = false;
		LogInfo("Playing music on SDL backend");
	}

	virtual void setTrack(sp<MusicTrack> track) override
	{
		SDL_LockAudioDevice(devID);
		LogInfo("Setting track to %p", track.get());
		this->track = track;
		SDL_UnlockAudioDevice(devID);
	}

	virtual void stopMusic() override { musicPaused = true; }

	virtual ~SDLRawBackend()
	{
		this->stopMusic();
		SDL_CloseAudioDevice(devID);
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}

	virtual const AudioFormat &getPreferredFormat() { return preferredFormat; }

	virtual float getGain(Gain g) override
	{
		int reqGain = 0;
		switch (g)
		{
			case Gain::Global:
				reqGain = mixVolumes.overallVolume;
				break;
			case Gain::Sample:
				reqGain = mixVolumes.soundVolume;
				break;
			case Gain::Music:
				reqGain = mixVolumes.musicVolume;
				break;
		}
		return (float)reqGain / (128);
	}
	virtual void setGain(Gain g, float f) override
	{
		int sdlVolume = f * 128;
		switch (g)
		{
			case Gain::Global:
				mixVolumes.overallVolume = sdlVolume;
				break;
			case Gain::Sample:
				mixVolumes.soundVolume = sdlVolume;
				break;
			case Gain::Music:
				mixVolumes.musicVolume = sdlVolume;
				break;
		}
		return;
	}
};

class SDLRawBackendFactory : public SoundBackendFactory
{
  public:
	virtual SoundBackend *create() override
	{
		LogWarning("Creating SDLRaw sound backend (Might have issues!)");
		return new SDLRawBackend();
	}

	virtual ~SDLRawBackendFactory() {}
};

SoundBackendRegister<SDLRawBackendFactory> load_at_init_SDLRaw_sound("SDLRaw");

}; // anonymous namespace
