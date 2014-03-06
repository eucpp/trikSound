#include "include/audioBuffer.h"

#include <QDebug>

#include <cstring>

using namespace triksound;

AudioBuffer::AudioBuffer(const QAudioFormat& format) :
	mBytes(),
	mFormat(format)
{
}

AudioBuffer::AudioBuffer(size_t n, const QAudioFormat& format):
	mBytes(n * format.sampleSize() / 8, 0),
	mFormat(format)
{

}

AudioBuffer::AudioBuffer(const QByteArray& bytes, const QAudioFormat& format):
	mBytes(bytes),
	mFormat(format)
{
	// num of bytes must be multiples to sample size
	if (mBytes.size() % (mFormat.sampleSize() / 8) != 0) {
		// we can reach this only if sample size is 16 bit and byteArray size is odd number
		// so we discard last byte
		mBytes.resize(mBytes.size() - 1);
		qDebug() << "QByteArray passed to AudioBuffer construct may be incorrect (size isn't multiples to sample size)";
	}
}

void* AudioBuffer::data()
{
	return mBytes.data();
}

const void* AudioBuffer::data() const
{
	return mBytes.constData();
}

const void* AudioBuffer::constData() const
{
	return mBytes.constData();
}

QByteArray AudioBuffer::bytes() const
{
	return mBytes;
}

size_t AudioBuffer::size() const
{
	return mBytes.size();
}

size_t AudioBuffer::samplesNum() const
{
	return mBytes.size() / (mFormat.sampleSize() / 8);
}

QAudioFormat AudioBuffer::getFormat() const
{
	return mFormat;
}

void AudioBuffer::append(const AudioBuffer& buffer)
{
	if (mFormat == buffer.mFormat) {
		mBytes.append(buffer.mBytes);
	}
	else {
		qDebug() << "AudioBuffer::append error, buffers have different formats.";
	}
}

int AudioBuffer::sampleRate() const
{
	return mFormat.sampleRate();
}

int AudioBuffer::sampleSize() const
{
	return mFormat.sampleSize() / 8;
}

QAudioFormat::SampleType AudioBuffer::sampleType() const
{
	return mFormat.sampleType();
}

AudioBuffer AudioBuffer::subBuffer(int pos, int length) const
{
	int ss = sampleSize();
	if (length != -1) {
		length *= ss;
	}
	return AudioBuffer(mBytes.mid(pos * ss, length), mFormat);
}

bool AudioBuffer::operator==(const AudioBuffer& other) const
{
	if (size() != other.size()) {
		return false;
	}
	int cmpRes = memcmp(constData(), other.constData(), size());
	return ((cmpRes == 0) && (mFormat == other.mFormat));
}

QAudioFormat AudioBuffer::getDefaultFormat()
{
	static QAudioFormat defaultFormat = QAudioFormat();
	if (!defaultFormat.isValid()) {
		defaultFormat.setCodec("audio/pcm");
		defaultFormat.setChannelCount(1);
		defaultFormat.setSampleRate(16000);
		defaultFormat.setSampleSize(16);
		defaultFormat.setSampleType(QAudioFormat::SignedInt);
		if (QSysInfo::ByteOrder == QSysInfo::LittleEndian) {
			defaultFormat.setByteOrder(QAudioFormat::LittleEndian);
		}
		else {
			defaultFormat.setByteOrder(QAudioFormat::BigEndian);
		}
	}
	return defaultFormat;
}
