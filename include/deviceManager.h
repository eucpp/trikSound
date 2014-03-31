#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QAudioDeviceInfo>

class DeviceManager
{
public:
	DeviceManager(const QAudioDeviceInfo& device = QAudioDeviceInfo::defaultInputDevice()):
		mDevice(device)
	{}
	virtual ~DeviceManager() {}

	virtual bool init();
	virtual bool isReady() const;

	virtual void setVolume(float vol) {}
	virtual float volume() const {}

	QAudioDeviceInfo getDevice() const;
private:
	QAudioDeviceInfo mDevice;
};

inline bool DeviceManager::init()
{
	return true;
}

inline bool DeviceManager::isReady() const
{
	return true;
}

inline QAudioDeviceInfo DeviceManager::getDevice() const
{
	return mDevice;
}

#endif // DEVICEMANAGER_H
