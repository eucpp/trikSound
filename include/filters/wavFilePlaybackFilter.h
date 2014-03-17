#ifndef WAVFILEPLAYBACKFILTER_H
#define WAVFILEPLAYBACKFILTER_H

#include <QSharedPointer>

#include "audioFilter.h"
#include "../wavFile.h"

namespace triksound {

class TRIKSOUNDSHARED_EXPORT WavFilePlaybackFilter : public AudioFilter
{
	Q_OBJECT
public:
	explicit WavFilePlaybackFilter(const QString& filename,
								   const QAudioFormat& format,
								   QObject *parent = 0);

	QSharedPointer<WavFile> getFile() const;
public slots:
	void input(AudioBuffer buf);
private:
	QSharedPointer<WavFile> mFile;
};

inline WavFilePlaybackFilter::WavFilePlaybackFilter(const QString& filename,
													const QAudioFormat& format,
													QObject* parent):
	AudioFilter(parent)
{
	mFile = QSharedPointer<WavFile>(new WavFile(filename));
	mFile->open(WavFile::WriteOnly, format);
}

inline QSharedPointer<WavFile> WavFilePlaybackFilter::getFile() const
{
	return mFile;
}

inline void WavFilePlaybackFilter::input(AudioBuffer buf)
{
	mFile->write(buf);
}


}

#endif // WAVFILEPLAYBACKFILTER_H
