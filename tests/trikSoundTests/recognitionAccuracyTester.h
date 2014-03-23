#ifndef RECOGNITIONACCURACYTESTER_H
#define RECOGNITIONACCURACYTESTER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

#include <trikSound/pocketsphinxDecoder.h>

class RecognitionAccuracyTester : public QObject
{
	Q_OBJECT
public:
	RecognitionAccuracyTester();

	void test();

private:
	// report of recognition accuracy and performance
	struct Report {
		// initialization time
		int initTime;
		// average time of recognition
		int recTime;
		// hash table contains all phrases in this test case
		// phrase is a key
		// first int is a count of this phrase
		// second int is a count of correct recognition of this phrase
		QHash< QString, QPair<int, int> > wordsAccur;
	};

	struct ParamRange {
		QString paramName;
		QString from;
		QString to;
		QString step;
	};

	void runTests();
	// extracts param ranges from input file
	QList<ParamRange> extractParams();
	//
	void runTestsForRange(const ParamRange& range);
	//
	Report getReport(triksound::PocketsphinxDecoder::InitParams params);

	// extracts phrase text and audio data binded with this text from string,
	// which must contains text and wav filename devided by ':'
	QPair<QString, triksound::AudioBuffer> extractPhraseData(const QString& line);
	// print report about testing to output file
	void printReport(const Report& rep, const triksound::PocketsphinxDecoder::InitParams& params);

	QFile mParamsFile;
	QFile mPhrasesFile;
	QString mPhrasesDir;
	QFile mOutFile;
	QTextStream mOut;

	QScopedPointer<triksound::PocketsphinxDecoder> mDecoder;
	QString mHmm;
	QString mFsg;
	QString mDic;

	// some unit tests of inner logic
private slots:
	void testExtractPhraseData();
	void testGetReport();
	void testPrintReport();
};

#endif // RECOGNITIONACCURACYTESTER_H
