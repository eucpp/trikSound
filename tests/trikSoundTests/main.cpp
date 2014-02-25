#include <QCoreApplication>

#include <QtTest>

#include "wavFileTest.h"
#include "circularBufferTest.h"
#include "playbackFilterTest.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	WavFileTest test1;
	QTest::qExec(&test1);

	CircularBufferTest test2;
	QTest::qExec(&test2);

	PlaybackFilterTest test3(argv[1]);
	QTimer::singleShot(1000, &test3, SLOT(playbackTest()));


	return a.exec();
}
