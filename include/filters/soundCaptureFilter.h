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
	explicit SoundCaptureFilter(SoundRecorder* recorder, QObject* parent = NULL):
		AudioFilter(parent)
	{
		connect(recorder, SIGNAL(captured(AudioBuffer)), this, SIGNAL(output(AudioBuffer)));
	}

public slots:
	void input(AudioBuffer buf);
};

inline void SoundCaptureFilter::input(AudioBuffer buf)
{
	Q_UNUSED(buf);
	return;
}

}

#endif // SOUNDCAPTUREFILTER_H
