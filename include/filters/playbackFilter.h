#ifndef PLAYBACKFILTER_H
#define PLAYBACKFILTER_H

#include <QAudioDeviceInfo>
#include <QAudioOutput>

#include <QDebug>

#include "audioFilter.h"
#include "../circularBuffer.h"

namespace triksound {

class TRIKSOUNDSHARED_EXPORT PlaybackFilter : public AudioFilter
{
	Q_OBJECT
public:
	explicit PlaybackFilter(const QAudioFormat& format = AudioBuffer::getDefaultFormat(),
				   const QAudioDeviceInfo& device = QAudioDeviceInfo::defaultOutputDevice(),
				   // 2 sec buffering (if sample rate == 16000)
				   size_t bufSize = 16000 * 2,
				   QObject* parent = 0);

	explicit PlaybackFilter(const QSharedPointer<QAudioOutput>& out,
							// 2 sec buffering (if sample rate == 16000)
							size_t bufSize = 16000 * 2,
							QObject* parent = 0);


	QSharedPointer<QAudioOutput> getAudioOutput() const;
	void setAudioOutput(const QSharedPointer<QAudioOutput>& out);
public slots:
	void input(AudioBuffer buf);
	void start();
	void stop();
private:
	CircularBuffer mBuffer;
	QSharedPointer<QAudioOutput> mOut;
};

inline PlaybackFilter::PlaybackFilter(const QAudioFormat& format,
							   const QAudioDeviceInfo& device,
							   size_t bufSize,
							   QObject* parent):
	AudioFilter(parent),
	mBuffer(bufSize),
	mOut(new QAudioOutput(device, format))
{
	mBuffer.open(QIODevice::ReadWrite);
}

PlaybackFilter::PlaybackFilter(const QSharedPointer<QAudioOutput>& out, size_t bufSize, QObject* parent):
	AudioFilter(parent),
	mBuffer(bufSize),
	mOut(out)
{
	mBuffer.open(QIODevice::ReadWrite);
}

inline QSharedPointer<QAudioOutput> PlaybackFilter::getAudioOutput() const
{
	return mOut;
}

inline void PlaybackFilter::setAudioOutput(const QSharedPointer<QAudioOutput>& out)
{
	mOut->stop();
	mOut = out;
}

inline void PlaybackFilter::input(AudioBuffer buf)
{
	if (buf.getFormat() == mOut->format()) {
		mBuffer.write(buf.bytes());
	}
}

inline void PlaybackFilter::start()
{
	mOut->start(&mBuffer);
}

void PlaybackFilter::stop()
{
	mOut->stop();
}

}

#endif // PLAYBACKFILTER_H
