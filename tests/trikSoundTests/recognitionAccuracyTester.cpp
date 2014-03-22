#include "recognitionAccuracyTester.h"

#include <ctime>

#include <QDebug>
#include <qmath.h>

#include <trikSound/wavFile.h>

using namespace triksound;

RecognitionAccuracyTester::RecognitionAccuracyTester()
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
		if (line != "--") {
			ParamRange paramRange;
			QStringList list = line.split(' ', QString::SkipEmptyParts);
			paramRange.paramName = list.at(0);
			if (list.size() == 2) {
				paramRange.from = list.at(1);
				paramRange.to = "";
				paramRange.step = "";
			}
			else if (list.size() == 4) {
				paramRange.from = list.at(1);
				paramRange.to = list.at(2);
				paramRange.step = list.at(3);
			}
			else {
				qDebug() << "Recognition accuracy test error. Uncorrect params file.";
			}
			res.append(paramRange);
		}
		else {
			return res;
		}
	}
	return res;
}

RecognitionAccuracyTester::Report RecognitionAccuracyTester::getReport(PocketsphinxDecoder::InitParams params)
{
	Report rep;

	clock_t c1 = clock();
	mDecoder.reset(new PocketsphinxDecoder(mHmm, mFsg, mDic, params));
	clock_t c2 = clock();

	rep.initTime = (c2 - c1) / (CLOCKS_PER_SEC / 1000);

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
	QString filename = list.at(0).trimmed();
	res.first = list.at(1).trimmed();

	WavFile file(filename);
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
	mOut << "Params: \n";
	mOut << "-ds = " << params.ds << "\n";
	mOut << "-lpbeam" << params.lpbeam << "\n";
	mOut << "-lponlybeam" << params.lponlybeam << "\n";
	mOut << "-maxhmmpf" << params.maxhmmpf << "\n";
	mOut << "-maxwpf" << params.maxwpf << "\n";
	mOut << "-topn" << params.topn << "\n";

	mOut << "---                                                                            ---  \n";

	mOut << "Initialization time = " << rep.initTime << " msec \n";
	mOut << "Average recognition time = " << rep.recTime << " msec \n";
	QHash< QString, QPair<int, int> >::const_iterator itr;
	for (itr = rep.wordsAccur.begin(); itr != rep.wordsAccur.end(); itr++) {
		float ratio = static_cast<float>(itr.value().second) / itr.value().first;
		int percent = qFloor(ratio * 100);
		mOut << "Word: " << itr.key() << "; Accuracy = " << percent << "% \n";
	}

	mOut << "----------------------------------------------------------------------------------- \n";
}
