#ifndef SoundPad_H
#define SoundPad_H

#include "SDK.h"

class SoundPad
{
	std::string Sound; BYTE Key;

public:
	float EndTime, SoundDuration; bool KeyWasPressed;

	SoundPad() : SoundDuration(0.f), EndTime(0.f), Sound(""), Key(0), KeyWasPressed(0) {} ~SoundPad() {}

	inline void Push(const char* sound, BYTE key,float Duration)
	{
		Sound = sound; Key = key; SoundDuration = Duration;
	}

	inline const char* GetName()
	{
		return Sound.c_str();
	}

	inline BYTE& GetKey()
	{
		return Key;
	}

	inline void Reset()
	{
		EndTime = KeyWasPressed = 0;
	}
};

extern char DirectoryPath[MAX_PATH];

extern std::vector<SoundPad> SoundList;

inline void RunSoundpad()
{
	static auto OldTick = TickCount; 

	if (auto CurTick = TickCount; CurTick > OldTick + 200)
	{
		OldTick = CurTick;

		for (auto& SoundInfo : SoundList)
		{
			if (SoundInfo.EndTime != 0 && Globals->curtime > SoundInfo.EndTime)
			{
				Engine->ClientCmd_Unrestricted("voice_loopback 0;-voicerecord");

				SoundInfo.EndTime = 0;
			}

			if (SoundInfo.KeyWasPressed && !IsVirtualKeyPressed(SoundInfo.GetKey()))
			{
				SoundInfo.KeyWasPressed = !SoundInfo.KeyWasPressed;
			}
		}
	}
}

#endif