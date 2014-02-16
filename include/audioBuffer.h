	#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <QObject>
#include <QAudioFormat>
#include <QByteArray>
#include <QSysInfo>
#include <QDebug>

#ifdef TRIK
	#include <QtMultimedia/QAudioFormat>
#else
	#include <QtMultimediaKit/QAudioFormat>
#endif

namespace triksound {

/**
 * @brief The AudioBuffer class is used for storage raw audio data along with their format.
 */
class AudioBuffer
{
public:
	AudioBuffer(const QAudioFormat& format = getDefaultFormat());
	AudioBuffer(size_t n, const QAudioFormat& format = getDefaultFormat());
	AudioBuffer(const QByteArray& bytes, const QAudioFormat& format = getDefaultFormat());

	void* data();
	const void* data() const;
	const void* constData() const;

	QByteArray bytes() const;

	/**
	 * @brief Buffer size in bytes
	 */
	size_t size() const;
	/**
	 * @brief Buffer size in samples
	 */
	size_t samplesNum() const;

	/**
	 * @brief Returns format of audio data
	 */
	QAudioFormat getFormat() const;

	/**
	 * @brief Append buffer to this buffer if both of them have the same format. Otherwise do nothing.
	 * @param buffer Appended buffer.
	 */
	void append(const AudioBuffer& buffer);

	int	sampleRate() const;
	int	sampleSize() const;
	QAudioFormat::SampleType sampleType() const;

	/**
	 * @brief Returns part of this buffer started from pos sample and with passed length
	 *		  (to the end of file by default).
	 * @param pos
	 * @param length
	 * @return
	 */
	AudioBuffer subBuffer(int pos, int length = -1);

	bool operator==(const AudioBuffer& other) const;

	static QAudioFormat getDefaultFormat();
private:
	QByteArray mBytes;
	QAudioFormat mFormat;
};

}


#endif // AUDIOBUFFER_H
