#ifndef SOUNDCAPTUREFILTER_H
#define SOUNDCAPTUREFILTER_H

#include <QObject>

#include "audioFilter.h"
#include "../soundRecorder.h"

namespace triksound {

/**
 * @brief The SoundCaptureFilter class provides AudioFilter interface for SoundRecorder class.
 *        This filter has output, but has not input.
 */
class TRIKSOUNDSHARED_EXPORT SoundCaptureFilter : public AudioFilter
{
	Q_OBJECT
public:
	explicit SoundCaptureFilter(const QSharedPointer<SoundRecorder>& recorder, QObject* parent = NULL):
		AudioFilter(parent),
		mRecorder(recorder)
	{
		connect(mRecorder.data(), SIGNAL(captured(AudioBuffer)), this, SIGNAL(output(AudioBuffer)));
	}

	QSharedPointer<SoundRecorder> getRecorder() const;
	void setRecorder(const QSharedPointer<SoundRecorder>& recorder);
public slots:
	void input(AudioBuffer buf);
private:
	QSharedPointer<SoundRecorder> mRecorder;
};

inline QSharedPointer<SoundRecorder> SoundCaptureFilter::getRecorder() const
{
	return mRecorder;
}

inline void SoundCaptureFilter::setRecorder(const QSharedPointer<SoundRecorder>& recorder)
{
	mRecorder->stop();
	mRecorder = recorder;
	connect(mRecorder.data(), SIGNAL(captured(AudioBuffer)), this, SIGNAL(output(AudioBuffer)));
}

inline void SoundCaptureFilter::input(AudioBuffer buf)
{
	Q_UNUSED(buf);
	return;
}

}

#endif // SOUNDCAPTUREFILTER_H
