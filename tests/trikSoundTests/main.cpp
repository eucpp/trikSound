#include <QCoreApplication>

#include <QtTest>
#include <QHash>
#include <QDebug>

#include "wavFileTest.h"
#include "circularBufferTest.h"
#include "playbackFilterTest.h"
#include "recognitionAccuracyTester.h"

void myMessageHandler(QtMsgType type, const char *msg)
{
	QString txt;
	switch (type) {
		case QtDebugMsg:
			txt = QString("Debug: %1").arg(msg);
		break;
		case QtWarningMsg:
			txt = QString("Warning: %1").arg(msg);
		break;
		case QtCriticalMsg:
			txt = QString("Critical: %1").arg(msg);
		break;
		case QtFatalMsg:
			txt = QString("Fatal: %1").arg(msg);
			abort();
	}
	QFile outFile("log");
	outFile.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream ts(&outFile);
	ts << txt << endl;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
//	qInstallMsgHandler(myMessageHandler);

	WavFileTest test1;
	QTest::qExec(&test1);

	CircularBufferTest test2;
	QTest::qExec(&test2);

	PlaybackFilterTest test3(argv[1]);
	QTimer::singleShot(1000, &test3, SLOT(playbackTest()));

//	if (argc != 7) {
//		qDebug() << "Incorrect params num";
//	}
//	else {
//		RecognitionAccuracyTester tester(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
//		tester.run();
//	}

//	QTest::qExec(&tester);

	return a.exec();
}
