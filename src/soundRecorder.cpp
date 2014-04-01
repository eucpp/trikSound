#include "soundRecorder.h"

#include <QDebug>

#include "trikInputDeviceManager.h"

using namespace triksound;

SoundRecorder::SoundRecorder(const QAudioFormat& format, size_t frameLength):
	mBuffer(frameLength * 10),
	mFrameLength(frameLength)
{
	#ifdef TRIK
		mDevice.reset(new TrikInputDeviceManager());
	#else
		mDevice.reset(new DeviceManager());
	#endif
	mAudioInput.reset(new QAudioInput(mDevice->getDevice(), format));
	mBuffer.open(QIODevice::ReadWrite);
	connect(&mBuffer, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
}

SoundRecorder::SoundRecorder(const QAudioDeviceInfo& device, const QAudioFormat& format, size_t frameLength):
	mBuffer(frameLength * 10),
	mFrameLength(frameLength)
{
	#ifdef TRIK
		mDevice.reset(new TrikInputDeviceManager(device));
	#else
		mDevice.reset(new DeviceManager(device));
	#endif
	mAudioInput.reset(new QAudioInput(mDevice->getDevice(), format));
	mBuffer.open(QIODevice::ReadWrite);
	connect(&mBuffer, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
}

size_t SoundRecorder::getFrameLength() const
{
	return mFrameLength;
}

void SoundRecorder::setFrameLength(size_t length)
{
	if (mAudioInput->state() != QAudio::StoppedState) {
		return;
	}
	mFrameLength = length;
	mBuffer.setCapacity(length * 10);
}

QAudioFormat SoundRecorder::getFormat() const
{
	return mAudioInput->format();
}

QAudioDeviceInfo SoundRecorder::getDevice() const
{
	return mDevice->getDevice();
}

void SoundRecorder::setVolume(float vol)
{
	mDevice->setVolume(vol);
}

float SoundRecorder::volume() const
{
	return mDevice->volume();
}

void SoundRecorder::start()
{
	if (!mDevice->isReady()) {
		if (!mDevice->init()) {
			emit error(DEVICE_INIT_ERROR);
			return;
		}
	}
	mAudioInput->start(&mBuffer);
}

void SoundRecorder::stop()
{
	mAudioInput->stop();
	//disconnect(&mBuffer, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
}

void SoundRecorder::readyReadHandler()
{
	if (mBuffer.bytesAvailable() > mFrameLength) {
		emit captured(AudioBuffer(mBuffer.read(mFrameLength), mAudioInput->format()));
	}
}
