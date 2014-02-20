#include "include/pocketsphinxDecoder.h"

#include <QDebug>

using namespace triksound;

const QString PocketsphinxDecoder::defaultHmm = "";
const QString PocketsphinxDecoder::defaultLm = "";
const QString PocketsphinxDecoder::defaultDict = "";

PocketsphinxDecoder::PocketsphinxDecoder(QString pathToHmm, QString pathToLm, QString pathToDict):
	mDecoder(NULL),
	mConfig(NULL),
	mIsReady(false)
{
	mConfig = cmd_ln_init(NULL, ps_args(), TRUE,
						 "-hmm", pathToHmm.toAscii().data(),
						 "-fsg", pathToLm.toAscii().data(),
						 "-dict", pathToDict.toAscii().data(),
						 NULL);
	if (mConfig == NULL) {
		mIsReady = false;
		mInitError = INCORRECT_ARGUMENTS;
		qDebug() << "Incorrect arguments for pocketsphinx initialization.";
	}
	mDecoder = ps_init(mConfig);
	if (mDecoder == NULL) {
		mIsReady = false;
		mInitError = DECODER_INIT_ERROR;
		qDebug() << "Can't initialize pocketsphinx.";
	}

	mIsReady = true;
}

PocketsphinxDecoder::~PocketsphinxDecoder()
{
	ps_free(mDecoder);
}

PocketsphinxDecoder::Command PocketsphinxDecoder::recognize(AudioBuffer buffer)
{
	char const *uttid = NULL;
	int uttReturn = ps_start_utt(mDecoder, uttid);
	if (uttReturn < 0) {
		mRecognitionError = START_UTT_ERROR;
		qDebug() << "Pocketsphinx error. Can't start utterance.";
		emit error(mRecognitionError);
	}

	int decodeReturn = ps_process_raw(mDecoder, (short*)buffer.data(),
								  buffer.samplesNum(), 0, 0);
	if (decodeReturn < 0) {
		mRecognitionError = PROCESS_ERROR;
		qDebug() << "Pocketsphinx error. Can't process audio data.";
		emit error(mRecognitionError);
	}

	uttReturn = ps_end_utt(mDecoder);
	if (uttReturn < 0) {
		mRecognitionError = END_UTT_ERROR;
		qDebug() << "Pocketsphinx error. Can't end utterance.";
		emit error(mRecognitionError);
	}

	const char* str = NULL;
	QString qstr = "";
	double accuracy = 0;
	int score;
	str = ps_get_hyp(mDecoder, &score, &uttid);
	if (str != NULL) {
		qstr = str;
		accuracy = - static_cast<double>(ps_get_prob(mDecoder, &uttid)) / MAX_INT16;
	}
	Command cmd(qstr, accuracy);

	emit recognized(cmd);
	return cmd;
}
