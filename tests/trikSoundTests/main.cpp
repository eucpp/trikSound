#include <QCoreApplication>

#include <QtTest>

#include "wavFileTest.h"
#include "circularBufferTest.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	WavFileTest test1;
	QTest::qExec(&test1);

	CircularBufferTest test2;
	QTest::qExec(&test2);

	return a.exec();
}
