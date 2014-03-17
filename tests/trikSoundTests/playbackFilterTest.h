#ifndef PLAYBACKFILTERTEST_H
#define PLAYBACKFILTERTEST_H

#include <QObject>
#include <QtTest>
#include <QTimer>
#include <QDebug>


#include "filters/playbackFilter.h"
#include "wavFile.h"

class PlaybackFilterTest : public QObject
{
	Q_OBJECT
public:
	PlaybackFilterTest(const QString& pathToFile):
		file(pathToFile),
		playback(NULL)
	{
		file.open(triksound::WavFile::ReadOnly);
		playback.reset(new triksound::PlaybackFilter(file.getHeader(),
					   QAudioDeviceInfo::defaultOutputDevice(),
					   file.getHeader().sampleRate() * 10));
		playback->setPlayMode(triksound::PlaybackFilter::ON_INPUT);
		// 2 sec - interval of reading from file
		timer.setInterval(2 * 1000);
		connect(&timer, SIGNAL(timeout()), this, SLOT(play()));
		connect(playback->getAudioOutput().data(), SIGNAL(stateChanged(QAudio::State)), this, SLOT(printState(QAudio::State)));
		playback->start();
	}

public slots:
	// this test just plays wav file to test playback
	void playbackTest()
	{
		timer.start();
	}

private slots:
	void play()
	{
		static int n = 1;
		qDebug() << "Read " << n++ << " frame";

		triksound::AudioBuffer buf = file.read(file.getHeader().sampleRate());
		if (buf.size() == 0) {
			timer.stop();
		}
		playback->input(buf);
	}
	void printState(QAudio::State state)
	{
		qDebug() << "State changed: " << state;
	}

private:
	triksound::WavFile file;
	QScopedPointer<triksound::PlaybackFilter> playback;
	QTimer timer;
};

#endif // PLAYBACKFILTERTEST_H
