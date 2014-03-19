#ifndef POCKETSPHINXWRAP_H
#define POCKETSPHINXWRAP_H

#include <QObject>

#include <limits>
#include <pocketsphinx/pocketsphinx.h>

#include "trikSound_global.h"
#include "audioBuffer.h"

namespace triksound {

/**
  * The PocketsphinxWrap class provide a Qt-based wrap for pocketsphinx library.
  */
class TRIKSOUNDSHARED_EXPORT PocketsphinxDecoder : public QObject
{
	Q_OBJECT
public:
	/**
	  * The Command class provides an voice command recognized by Pockesphinx.
	  */
	class TRIKSOUNDSHARED_EXPORT Command
	{
	public:
		Command(const QString& cmd):
			mText(cmd)
		{}

		QString getText() const;

	private:
		QString mText;
	};

	enum InitError {
		INCORRECT_ARGUMENTS,
		DECODER_INIT_ERROR
	};

	enum RecognitionError {
		START_UTT_ERROR,
		END_UTT_ERROR,
		PROCESS_ERROR
	};

	/**
	  * @param pathToHmm Path to acoustic model file.
	  * @param pathToLm Path to grammar file.
	  * @param pathToDict Path to dictionary file.
	  */
	PocketsphinxDecoder(QString pathToHmm = defaultHmm,
						QString pathToLm = defaultLm,
						QString pathToDict = defaultDict);
	~PocketsphinxDecoder();

	/**
	 * @brief Checks was the decoder initialized correctly and ready to recognition.
	 * @return true if decoder is ready to recognition.
	 */
	bool isReady() const;
	/**
	 * @brief If there was some error during the initialization of Decoder this method will return it.
	 */
	InitError initError() const;
	/**
	 * @brief Returns the type of error that last occurred during the recognition.
	 */
	RecognitionError recognitionError() const;

	static const QString defaultHmm;
	static const QString defaultLm;
	static const QString defaultDict;
public slots:
	/**
	  * @brief Tries to recognize command in audio data. Emits recognized signal when finished.
	  */
	Command recognize(AudioBuffer buffer);
signals:
	/**
	  * Emitted whenever a recognition of command finished.
	  */
	void recognized(Command);
	/**
	  * This signal is emitted after an error of recognition occurred.
	  */
	void error(RecognitionError);
private:
	ps_decoder_t* mDecoder;
	cmd_ln_t* mConfig;
	bool mIsReady;
	InitError mInitError;
	RecognitionError mRecognitionError;
};

inline bool PocketsphinxDecoder::isReady() const
{
	return mIsReady;
}

inline PocketsphinxDecoder::InitError PocketsphinxDecoder::initError() const
{
	return mInitError;
}

inline PocketsphinxDecoder::RecognitionError PocketsphinxDecoder::recognitionError() const
{
	return mRecognitionError;
}

inline QString PocketsphinxDecoder::Command::getText() const
{
	return mText;
}

}

#endif // POCKETSPHINXWRAP_H
