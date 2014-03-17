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
	/**
	 * @brief The PlayMode enum describes the rules of playback control.
	 *		  ON_INPUT - playback will be activated each time when input() slot called.
	 *		  MANUAL - playback managed only by start() and stop() slots.
	 */
	enum PlayMode {
		ON_INPUT,
		MANUAL
	};

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

	PlayMode getPlayMode() const;
	void setPlayMode(PlayMode mode);
public slots:
	void input(AudioBuffer buf);
	void start();
	void stop();
private:
	CircularBuffer mBuffer;
	QSharedPointer<QAudioOutput> mOut;
	PlayMode mMode;
};

inline PlaybackFilter::PlaybackFilter(const QAudioFormat& format,
							   const QAudioDeviceInfo& device,
							   size_t bufSize,
							   QObject* parent):
	AudioFilter(parent),
	mBuffer(bufSize),
	mOut(new QAudioOutput(device, format)),
	mMode(ON_INPUT)
{
	mBuffer.open(QIODevice::ReadWrite);
}

inline PlaybackFilter::PlaybackFilter(const QSharedPointer<QAudioOutput>& out, size_t bufSize, QObject* parent):
	AudioFilter(parent),
	mBuffer(bufSize),
	mOut(out),
	mMode(ON_INPUT)
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

inline PlaybackFilter::PlayMode PlaybackFilter::getPlayMode() const
{
	return mMode;
}

inline void PlaybackFilter::setPlayMode(PlaybackFilter::PlayMode mode)
{
	mMode = mode;
}

inline void PlaybackFilter::input(AudioBuffer buf)
{
	if (buf.getFormat() == mOut->format()) {
		mBuffer.write(buf.bytes());
		if ((mMode == ON_INPUT) && (mOut->state() != QAudio::ActiveState)) {
			mOut->start(&mBuffer);
		}
	}
}

inline void PlaybackFilter::start()
{
	mOut->start(&mBuffer);
}

inline void PlaybackFilter::stop()
{
	mOut->stop();
}

}

#endif // PLAYBACKFILTER_H
