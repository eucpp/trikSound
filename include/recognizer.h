#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include <QObject>

#include "../soundRecorder.h"
#include "pocketsphinxWrap.h"

class Recognizer : public QObject
{
	Q_OBJECT
public:
	Recognizer(const QString& pathToHmm,
			   const QString& pathToLm,
			   const QString& pathToDict,
			   QObject* parent = 0);

public slots:
	void start();
	void stop();

private:
	TrikCapture::SoundRecorder mRecorder;
	PocketsphinxDecoder mPS;

	typedef TrikCapture::AudioBuffer AudioBuffer;

private slots:
	void handleFrame(AudioBuffer buffer);
};

#endif // RECOGNIZER_H
