#ifndef AUDIOPIPE_H
#define AUDIOPIPE_H

#include <QObject>

#include "filters/audioFilter.h"
#include "trikSound_global.h"

namespace triksound {

/**
 * @brief The AudioPipe class provides a connection between two audio filters.
 *        Each time the input filter finishes processing audio data they will be sent to output filter.
 */
class TRIKSOUNDSHARED_EXPORT AudioPipe : public QObject
{
	Q_OBJECT
public:
	explicit AudioPipe(AudioFilter* in, AudioFilter* out, QObject *parent = 0);
	~AudioPipe()
	{
		disconnect(mIn, SIGNAL(output(AudioBuffer)), mOut, SLOT(input(AudioBuffer)));
	}

private:
	AudioFilter* mIn;
	AudioFilter* mOut;
};

inline AudioPipe::AudioPipe(AudioFilter* in, AudioFilter* out, QObject* parent):
	mIn(in),
	mOut(out),
	QObject(parent)
{

	connect(in, SIGNAL(output(AudioBuffer)), out, SLOT(input(AudioBuffer)));
}

}


#endif // AUDIOPIPE_H
