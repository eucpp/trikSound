#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QObject>
#include <QAudioOutput>

#include "soundRecorder.h"
//#include "ringModulator.h"

/**
 * @brief The AudioProcessor class capture audio data from mic,
 *		  processes it via ring modutlation and outputs result to the dynamic.
 */
class AudioProcessor : public QObject
{
	Q_OBJECT
public:
	AudioProcessor(int freq, QObject *parent = 0);

public slots:
	void start();
	void stop();

private:
	triksound::SoundRecorder mRecorder;
	QAudioOutput mOut;
//	RingModulator mModulator;
	int mDelay;
	triksound::CircularBuffer mOutBuffer;
	triksound::AudioBuffer mSine;

	typedef triksound::AudioBuffer AudioBuffer;

private slots:
	void handleFrame(AudioBuffer buffer);
	void startPlaying();
};

#endif // AUDIOPROCESSOR_H
