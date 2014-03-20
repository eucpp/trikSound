#ifndef RECOGNITIONACCURACYTESTER_H
#define RECOGNITIONACCURACYTESTER_H

#include <QFile>

#include <trikSound/pocketsphinxDecoder.h>

class RecognitionAccuracyTester
{
public:
	RecognitionAccuracyTester();

	void test();

private:
	struct Report {
		int initTime;
		int recTime;
		QList< QPair<QString, float> > wordsAccur;
	};

	Report getReport(triksound::PocketsphinxDecoder::InitParams params);

	QFile mFiles;

	QScopedPointer<triksound::PocketsphinxDecoder> mDecoder;
	QString mHmm;
	QString mFsg;
	QString mDic;
};

#endif // RECOGNITIONACCURACYTESTER_H
