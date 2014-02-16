#pragma once

#include <QObject>
#include <QFile>

#ifdef TRIK
#include <QtMultimedia/QAudioInput>
#else
#include <QtMultimediaKit/QAudioInput>
#endif

#include "trikSound_global.h"
#include "audioBuffer.h"

namespace triksound {

class TRIKSOUNDSHARED_EXPORT WavFile
{
public:

	class OpenFileExc {};
	class WriteExc {};
	class ReadExc {};
	class FormatMismatchExc {};

	enum OpenModes
	{
		ReadOnly,
		WriteOnly,
		Append,
		NotOpen
	};

	WavFile();
	WavFile(const QString& filename);

	bool open(OpenModes mode);
	/**
	  * @param format This parameter is used if file opened in WriteOnly mode.
	  *				  Must set sample size and sample rate.
	  */
	bool open(OpenModes mode, const QAudioFormat& format);

	void close();
	/**
	  * @brief Set new filename (if another file is opened it will be closed).
	  */
	void setFileName(const QString& filename);
	QString fileName() const;


	bool isHeaderSet() const;
	/**
	  * @brief Set the header of file (for not opened files).
	  *		   If file will be opened for writing, this header will be used for saving the audio format.
	  * @param format New format information.
	  */
	void setHeader(const QAudioFormat& format);
	QAudioFormat getHeader() const;

	bool isOpen() const;

	/**
	 * @brief Seek to the sample in pos.
	 */
	bool seek(int pos);
	/**
	 * @brief Return current position (in samples)
	 */
	int pos() const;

	/**
	  * @brief Returns file size in bytes (including size of header).
	  */
	int size() const;
	/**
	  * @brief Returns file size in samples.
	  */
	int samplesNum() const;

	/**
	 * @brief Read max length samples from file.
	 */
	AudioBuffer read(int length);
	/**
	 * @brief Read all file into audio buffer.
	 */
	AudioBuffer readAll();

	/**
	  * @brief Write audio buffer to the file.
	  *
	  * @param buffer Buffer for writing. Must have the same audio format as file.
	  *        Otherwise data will not be written.
	  * @param length Length of writing samples (equal to audio buffer size by default)
	  * @return Num of written bytes or -1 if an error occured.
	  */
	int write(AudioBuffer buffer, int length = -1);
private:
	class UncorrectHeader {};
	struct Header
	{
		char chunkId[4];
		unsigned int chunkSize;
		char format[4];

		char subchunk1Id[4];
		unsigned int subchunk1Size;

		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned int sampleRate;
		unsigned int byteRate;
		unsigned short blockAlign;
		unsigned short bitsPerSample;

		char subchunk2Id[4];
		unsigned long subchunk2Size;
	};

	QAudioFormat readHeader();
	void writeHeader(const QAudioFormat& format);
	void setDataSize(int size);
	bool isCorrectFormat(const QAudioFormat& format);

	QFile mFile;
	OpenModes mOpenmode;
	QAudioFormat mHeader;
	bool mHeaderSetFlag;
};

}

