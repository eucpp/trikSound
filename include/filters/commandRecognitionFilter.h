#ifndef COMMANDRECOGNITIONFILTER_H
#define COMMANDRECOGNITIONFILTER_H

#include <cstdio>
#include <ctime>

#include <QTextStream>
#include <QSharedPointer>
#include <QDebug>

#include "audioFilter.h"
#include "../pocketsphinxDecoder.h"

namespace triksound {

/**
 * @brief The CommandRecognitionFilter class provides AudioFilter interface for Pocketsphinx decoder.
 *        This filter has input, but hasn't audio output.
 *        Instead, filter puts results of recognition to text stream defined by user of this class.
 */
class CommandRecognitionFilter : public AudioFilter
{
	Q_OBJECT
public:
	/**
	 * @param decoder Pointer to decoder object. If NULL is passed, new one will be created.
	 * @param stream Pointer to the text stream for text output. By default, this is stdout.
	 */
	explicit CommandRecognitionFilter(const QSharedPointer<PocketsphinxDecoder>& decoder
										= QSharedPointer<PocketsphinxDecoder>(NULL),
									  const QSharedPointer<QTextStream>& stream
										= QSharedPointer<QTextStream>(NULL),
									  QObject *parent = 0);

public slots:
	void input(AudioBuffer buf);

private:
	QSharedPointer<PocketsphinxDecoder> mDecoder;
	QSharedPointer<QTextStream> mStream;
};

inline CommandRecognitionFilter::CommandRecognitionFilter(const QSharedPointer<PocketsphinxDecoder>& decoder,
												   const QSharedPointer<QTextStream>& stream,
												   QObject* parent):
	AudioFilter(parent),
	mDecoder(decoder),
	mStream(stream)
{
	if (!mDecoder) {
		mDecoder = QSharedPointer<PocketsphinxDecoder>(new PocketsphinxDecoder());
	}
	if (!mStream) {
		mStream = QSharedPointer<QTextStream>(new QTextStream(stdout));
	}
}

inline void CommandRecognitionFilter::input(AudioBuffer buf)
{
	clock_t c1 = clock();
	QString cmd = mDecoder->recognize(buf).getText();
	clock_t c2 = clock();
//	qDebug() << "PS time = " << (c2 - c1) / (CLOCKS_PER_SEC / 1000);
	if (cmd == "") {
		(*mStream) << "\n Unrecognized command \n";
	}
	else {
		(*mStream) << "\n" << cmd << "\n";
	}
	mStream->flush();
}

}

#endif // COMMANDRECOGNITIONFILTER_H
