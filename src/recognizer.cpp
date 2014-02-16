#include "include/recognizer.h"

#include <QFile>

#include "iostream"

using namespace std;
using namespace TrikCapture;

Recognizer::Recognizer(const QString& pathToHmm,
					   const QString& pathToLm,
					   const QString& pathToDict,
					   QObject* parent) :
	QObject(parent),
	mPS(pathToHmm, pathToLm, pathToDict)
{
	// try to recognize word every 2 second
	mRecorder.setFrameLength(4 * mRecorder.getFormat().sampleRate());
}

void Recognizer::start()
{
	connect(&mRecorder, SIGNAL(captured(AudioBuffer)), this, SLOT(handleFrame(AudioBuffer)));
	mRecorder.start();
}

void Recognizer::stop()
{
	mRecorder.stop();
}

void Recognizer::handleFrame(AudioBuffer buffer)
{
	PocketsphinxDecoder::Command cmd = mPS.recognize(buffer);

	cout << "word: " ;
	if (cmd.command == QString()) {
		cout << "..." << endl;
	}
	else {
		cout << (cmd.command + "\n").toAscii().data() << endl;
	}
}
