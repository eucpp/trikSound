#include "include/soundRecorder.h"

using namespace triksound;

SoundRecorder::SoundRecorder(const QAudioFormat& format, size_t frameLength):
	mDevice(QAudioDeviceInfo::defaultInputDevice()),
	mAudioInput(mDevice, format),
	mBuffer(frameLength * 10),
	mFrameLength(frameLength)
{
	mBuffer.open(QIODevice::ReadWrite);
	connect(&mBuffer, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
}

SoundRecorder::SoundRecorder(const QAudioDeviceInfo& device, const QAudioFormat& format, size_t frameLength):
	mDevice(device),
	mAudioInput(mDevice, format),
	mBuffer(frameLength * 10),
	mFrameLength(frameLength)
{
	mBuffer.open(QIODevice::ReadWrite);
	connect(&mBuffer, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
}

size_t SoundRecorder::getFrameLength() const
{
	return mFrameLength;
}

void SoundRecorder::setFrameLength(size_t length)
{
	mFrameLength = length;
	mBuffer.setCapacity(length * 10);
}

QAudioFormat SoundRecorder::getFormat() const
{
	return mAudioInput.format();
}

QAudioDeviceInfo SoundRecorder::getDevice() const
{
	return mDevice;
}

void SoundRecorder::start()
{
	mAudioInput.start(&mBuffer);
}

void SoundRecorder::stop()
{
	mAudioInput.stop();
	//disconnect(&mBuffer, SIGNAL(readyRead()), this, SLOT(readyReadHandler()));
}

void SoundRecorder::readyReadHandler()
{
	if (mBuffer.bytesAvailable() > mFrameLength) {
		emit captured(AudioBuffer(mBuffer.read(mFrameLength), mAudioInput.format()));
	}
}
