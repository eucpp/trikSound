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
	explicit AudioPipe(const AudioFilter& in, const AudioFilter& out, QObject *parent = 0);
};

inline AudioPipe::AudioPipe(const AudioFilter& in, const AudioFilter& out, QObject* parent):
	QObject(parent)
{
	connect(&in, SIGNAL(output(AudioBuffer)), &out, SLOT(input(AudioBuffer)));
}

}


#endif // AUDIOPIPE_H
