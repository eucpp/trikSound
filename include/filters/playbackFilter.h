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

public slots:
	void input(AudioBuffer buf);
	void start();
private:
	CircularBuffer mBuffer;
	QAudioOutput mOut;
};

inline PlaybackFilter::PlaybackFilter(const QAudioFormat& format,
							   const QAudioDeviceInfo& device,
							   size_t bufSize,
							   QObject* parent):
	AudioFilter(parent),
	mBuffer(bufSize),
	mOut(device, format)
{
	mBuffer.open(QIODevice::ReadWrite);
}

inline void PlaybackFilter::input(AudioBuffer buf)
{
	if (buf.getFormat() == mOut.format()) {
		mBuffer.write(buf.bytes());
		if (mOut.state() != QAudio::ActiveState) {
//			mOut.start(&mBuffer);
		}
	}
}

inline void PlaybackFilter::start()
{
	mOut.start(&mBuffer);
}

}

#endif // PLAYBACKFILTER_H
