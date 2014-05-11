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
 *        Instead, filter sends results of recognition in the signal.
 */
class CommandRecognitionFilter : public AudioFilter
{
	Q_OBJECT
public:
	/**
	 * @param decoder Pointer to decoder object. If NULL is passed, new one will be created.
	 */
	explicit CommandRecognitionFilter(const QSharedPointer<PocketsphinxDecoder>& decoder
										= QSharedPointer<PocketsphinxDecoder>(NULL),
									  QObject *parent = 0);

public slots:
	void input(AudioBuffer buf);

signals:
	void command(QString cmd);

private:
	QSharedPointer<PocketsphinxDecoder> mDecoder;
};

inline CommandRecognitionFilter::CommandRecognitionFilter(const QSharedPointer<PocketsphinxDecoder>& decoder,
												   QObject* parent):
	AudioFilter(parent),
	mDecoder(decoder)
{
	if (!mDecoder) {
		mDecoder = QSharedPointer<PocketsphinxDecoder>(new PocketsphinxDecoder());
	}
}

inline void CommandRecognitionFilter::input(AudioBuffer buf)
{
	clock_t c1 = clock();
	QString cmd = mDecoder->recognize(buf).getText();
	clock_t c2 = clock();
	emit command(cmd);
}

}

#endif // COMMANDRECOGNITIONFILTER_H
