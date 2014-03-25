#ifndef TRIKINPUTDEVICEMANAGER_H
#define TRIKINPUTDEVICEMANAGER_H

#include <QProcess>
#include <QDebug>

#include "deviceManager.h"

class TrikInputDeviceManager : public DeviceManager
{
public:
	TrikInputDeviceManager(const QAudioDeviceInfo& device = QAudioDeviceInfo::defaultInputDevice());

	void init();
	bool isReady() const;

	void setVolume(float vol);
	float volume() const;

private:
	float mVolume;
	bool mIsReady;

	static const int mMaxVol;
};

bool TrikInputDeviceManager::isReady() const
{
	return mIsReady;
}

inline float TrikInputDeviceManager::volume() const
{
	return mVolume;
}

#endif // TRIKINPUTDEVICEMANAGER_H
