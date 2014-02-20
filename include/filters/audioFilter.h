#ifndef AUDIOFILTER_H
#define AUDIOFILTER_H

#include <QObject>

#include "audioBuffer.h"

namespace triksound {

/**
 * @brief The AudioFilter class is an abstract class for all classes that
 *        can process, produce, capture or playback audio data.
 *        Each AudioFilter has input slot that provide audio data to filter and output signal,
 *        which emitted when processing is over.
 */
class TRIKSOUNDSHARED_EXPORT AudioFilter : public QObject
{
	Q_OBJECT
public:
	explicit AudioFilter(QObject *parent = 0):
		QObject(parent)
	{}
	virtual ~AudioFilter() {}

signals:
	void output(const AudioBuffer& buf);
public slots:
	virtual void input(AudioBuffer buf) = 0;
};

}

#endif // AUDIOFILTER_H
