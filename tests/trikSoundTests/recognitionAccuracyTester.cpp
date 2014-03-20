#include "recognitionAccuracyTester.h"

#include <ctime>

#include <QDebug>

#include <trikSound/wavFile.h>

using namespace triksound;

RecognitionAccuracyTester::RecognitionAccuracyTester()
{
}

void RecognitionAccuracyTester::test()
{

}

RecognitionAccuracyTester::Report RecognitionAccuracyTester::getReport(PocketsphinxDecoder::InitParams params)
{
	Report rep;

	clock_t c1 = clock();
	mDecoder.reset(new PocketsphinxDecoder(mHmm, mFsg, mDic, params));
	clock_t c2 = clock();

	rep.initTime = (c2 - c1) / (CLOCKS_PER_SEC / 1000);

	// hash table with all phrases in this test case
	// phrase is a key
	// first int is a count of this phrase
	// second int is a count of correct recognition of this phrase
	QHash< QString, QPair<int, int> > hash;

	int countFiles = 0;
	QByteArray bytes;
	while ((bytes = mFiles.readLine()).size() != 0) {
		QString line = QString::fromUtf8(bytes.data(), bytes.size());
		QStringList list = line.split(':', QString::SkipEmptyParts);
		QString filename = list.at(0).trimmed();
		QString phrase = list.at(1).trimmed();

		if (!hash.contains(phrase)) {
			hash[phrase] = QPair<int, int>(1, 0);
		}
		else {
			QPair<int, int> pair = hash.value(phrase);
			pair.first += 1;
			hash[phrase] = pair;
		}

		WavFile file(filename);
		file.open(WavFile::ReadOnly);
		if ((file.getHeader().sampleRate() != 16000) && (file.getHeader().sampleSize() != 16)) {
			qDebug() << "Recognition accuracy test warning. Unstandard format of wav file.";
		}
		AudioBuffer buf = file.readAll();

		c1 = clock();
		QString actualPhrase = (mDecoder->recognize(buf)).getText();
		c2 = clock();
		rep.recTime += (c2 - c1) / (CLOCKS_PER_SEC / 1000);

		if (actualPhrase == phrase) {
			QPair<int, int> pair = hash.value(phrase);
			pair.second += 1;
			hash[phrase] = pair;
		}

		countFiles++;
	}

	rep.recTime /= countFiles;


}
