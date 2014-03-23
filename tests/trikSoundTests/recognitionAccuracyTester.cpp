#include "recognitionAccuracyTester.h"

#include <ctime>

#include <QDebug>
#include <qmath.h>
#include <QtTest>

#include <trikSound/wavFile.h>

using namespace triksound;

RecognitionAccuracyTester::RecognitionAccuracyTester():
	QObject()
{
}

void RecognitionAccuracyTester::test()
{

}

void RecognitionAccuracyTester::runTests()
{

}

QList<RecognitionAccuracyTester::ParamRange> RecognitionAccuracyTester::extractParams()
{
	QList<ParamRange> res;
	QByteArray bytes;
	while ((bytes = mParamsFile.readLine()).size() != 0) {
		QString line = QString::fromUtf8(bytes.data(), bytes.size());
		ParamRange paramRange;
		QStringList list = line.split(' ', QString::SkipEmptyParts);
		paramRange.paramName = list.at(0);
		paramRange.from = list.at(1);
		paramRange.to = list.at(2);
		paramRange.step = list.at(3);
		res.append(paramRange);
	}
	return res;
}

void RecognitionAccuracyTester::runTestsForRange(const RecognitionAccuracyTester::ParamRange& range)
{
	PocketsphinxDecoder::InitParams params = PocketsphinxDecoder::getDefaultInitParams();
	if ((range.paramName == "ds") || (range.paramName == "topn") ||
			(range.paramName == "maxwpf") || (range.paramName == "maxhmmpf")) {
		bool b = false;
		int from = range.from.toInt(&b);
		if (!b) {
			qDebug() << "Recognition accuracy test error. Param " << range.paramName
					 << " has incorrect 'from' value = " << range.from;
		}
		int to = range.to.toInt(&b);
		if (!b) {
			qDebug() << "Recognition accuracy test error. Param " << range.paramName
					 << " has incorrect 'to' value = " << range.to;
		}
		int step = range.step.toInt(&b);
		if (!b) {
			qDebug() << "Recognition accuracy test error. Param " << range.paramName
					 << " has incorrect 'step' value = " << range.step;
		}
		for (int i = from; i <= to; i += step) {
			if (range.paramName == "ds") {
				params.ds = QString().setNum(i);
			}
			else if (range.paramName == "topn") {
				params.topn = QString().setNum(i);
			}
			else if (range.paramName == "maxwpf") {
				params.maxwpf = QString().setNum(i);
			}
			else if (range.paramName == "maxhmmpf") {
				params.maxhmmpf = QString().setNum(i);
			}
			Report rep = getReport(params);
			printReport(rep, params);
		}
	}
	else if ((range.paramName == "lpbeam") || (range.paramName == "lponlybeam")) {
		bool b = false;
		float from = range.from.toFloat(&b);
		if (!b) {
			qDebug() << "Recognition accuracy test error. Param " << range.paramName
					 << " has incorrect 'from' value = " << range.from;
		}
		float to = range.to.toFloat(&b);
		if (!b) {
			qDebug() << "Recognition accuracy test error. Param " << range.paramName
					 << " has incorrect 'to' value = " << range.to;
		}
		int step = range.step.toFloat(&b);
		if (!b) {
			qDebug() << "Recognition accuracy test error. Param " << range.paramName
					 << " has incorrect 'step' value = " << range.step;
		}
		for (float i = from; i <= to; i += step) {
			if (range.paramName == "lpbeam") {
				params.lpbeam = QString().setNum(i);
			}
			else if (range.paramName == "lponlybeam") {
				params.lponlybeam = QString().setNum(i);
			}
			Report rep = getReport(params);
			printReport(rep, params);
		}
	}
}

RecognitionAccuracyTester::Report RecognitionAccuracyTester::getReport(PocketsphinxDecoder::InitParams params)
{
	Report rep;

	clock_t c1 = clock();
	mDecoder.reset(new PocketsphinxDecoder(mHmm, mFsg, mDic, params));
	clock_t c2 = clock();

	rep.initTime = (c2 - c1) / (CLOCKS_PER_SEC / 1000);
	rep.recTime = 0;

	// count of all files, which used in test.
	// need for computation of average recognition time
	int countFiles = 0;
	QByteArray bytes;

	while ((bytes = mPhrasesFile.readLine()).size() != 0) {
		QString line = QString::fromUtf8(bytes.data(), bytes.size());
		QPair<QString, AudioBuffer> phrase = extractPhraseData(line);

		QPair<int, int> pair;
		if (!rep.wordsAccur.contains(phrase.first)) {
			pair = QPair<int, int>(1, 0);
		}
		else {
			pair = rep.wordsAccur.value(phrase.first);
			pair.first += 1;
		}

		c1 = clock();
		QString actualPhrase = (mDecoder->recognize(phrase.second)).getText();
		c2 = clock();
		rep.recTime += (c2 - c1) / (CLOCKS_PER_SEC / 1000);

		if (actualPhrase == phrase.first) {
			pair.second += 1;
		}
		rep.wordsAccur[phrase.first] = pair;

		countFiles++;
	}

	rep.recTime /= countFiles;

	return rep;
}

QPair<QString, AudioBuffer> RecognitionAccuracyTester::extractPhraseData(const QString& line)
{
	QPair<QString, AudioBuffer> res;

	QStringList list = line.split(':', QString::SkipEmptyParts);
	res.first = list.at(0).trimmed();
	QString filename = list.at(1).trimmed();

	WavFile file(mPhrasesDir + filename);
	file.open(WavFile::ReadOnly);
	if ((file.getHeader().sampleRate() != 16000) && (file.getHeader().sampleSize() != 16)) {
		qDebug() << "Recognition accuracy test warning. Unstandard format of wav file.";
	}
	res.second = file.readAll();

	return res;
}

void RecognitionAccuracyTester::printReport(const Report& rep, const PocketsphinxDecoder::InitParams& params)
{
	mOut << "\n";
	mOut << "--------------------- Recognition Accuracy & Performance -------------------------- \n";
	if (params == PocketsphinxDecoder::getDefaultInitParams()) {
		mOut << "------------------------------ Default Params ------------------------------------- \n";
	}
	mOut << "Params: \n";
	mOut << "-ds = " << params.ds << "\n";
	mOut << "-lpbeam = " << params.lpbeam << "\n";
	mOut << "-lponlybeam = " << params.lponlybeam << "\n";
	mOut << "-maxhmmpf = " << params.maxhmmpf << "\n";
	mOut << "-maxwpf = " << params.maxwpf << "\n";
	mOut << "-topn = " << params.topn << "\n";

	mOut << "---                                                                            ---  \n";

	mOut << "Initialization time = " << rep.initTime << " msec \n";
	mOut << "Average recognition time = " << rep.recTime << " msec \n";
	float totalRatio = 0;
	int n = 0;
	QHash< QString, QPair<int, int> >::const_iterator itr;
	for (itr = rep.wordsAccur.begin(); itr != rep.wordsAccur.end(); itr++) {
		float ratio = static_cast<float>(itr.value().second) / itr.value().first;
		totalRatio += ratio;
		n++;
		int percent = qFloor(ratio * 100);
		mOut << "Word: " << itr.key() << "; Accuracy = " << percent << "% \n";
	}
	int totalPercent = qFloor((totalRatio / n) * 100);
	mOut << "Total Accuracy = " << totalPercent << "% \n";

	mOut << "----------------------------------------------------------------------------------- \n";
	mOut.flush();
}


void RecognitionAccuracyTester::testExtractPhraseData()
{
	mPhrasesDir = "unit_test_data/audio_data/";
	QString line = "rus_stop : stop1.wav";
	QPair<QString, AudioBuffer> actual = extractPhraseData(line);

	WavFile file("unit_test_data/audio_data/stop1.wav");
	file.open(WavFile::ReadOnly);
	AudioBuffer buf = file.readAll();
	QPair<QString, AudioBuffer> expected("rus_stop", buf);

	QCOMPARE(actual, expected);
}

void RecognitionAccuracyTester::testGetReport()
{
	mHmm = "unit_test_data/model/acoustic";
	mFsg = "unit_test_data/model/grammar.fsg";
	mDic = "unit_test_data/model/trik.dic";
	mPhrasesDir = "unit_test_data/audio_data/";
	mPhrasesFile.setFileName("unit_test_data/audio_data/phrases");
	mPhrasesFile.open(QIODevice::ReadOnly);

	Report rep = getReport(PocketsphinxDecoder::getDefaultInitParams());

	QPair<int, int> stopData = rep.wordsAccur.value("rus_stop");
	QPair<int, int> backData = rep.wordsAccur.value("rus_back");
	QPair<int, int> forwardData = rep.wordsAccur.value("rus_forward");

	QCOMPARE(stopData.first, 1);
	QCOMPARE(backData.first, 1);
	QCOMPARE(forwardData.first, 1);
	QCOMPARE(rep.wordsAccur.size(), 3);
}

void RecognitionAccuracyTester::testPrintReport()
{
	mHmm = "unit_test_data/model/acoustic";
	mFsg = "unit_test_data/model/grammar.fsg";
	mDic = "unit_test_data/model/trik.dic";
	mPhrasesDir = "unit_test_data/audio_data/";
	mPhrasesFile.setFileName("unit_test_data/audio_data/phrases");
	mPhrasesFile.open(QIODevice::ReadOnly);
	mOutFile.setFileName("unit_test_data/printReportOut.txt");
	mOutFile.open(QIODevice::WriteOnly);
	mOut.setDevice(&mOutFile);


	Report rep = getReport(PocketsphinxDecoder::getDefaultInitParams());
	printReport(rep, PocketsphinxDecoder::getDefaultInitParams());
}
