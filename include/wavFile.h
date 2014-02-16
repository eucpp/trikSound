#pragma once


#include <iostream>
#include <cstring>
#include <QObject>

#ifdef TRIK
#include <QtMultimedia/QAudioInput>
#else
#include <QtMultimediaKit/QAudioInput>
#endif

#include <QFile>

#include "audioBuffer.h"

class WavFile
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

	void open(OpenModes mode);
	/**
	  * @param format This parameter is used if file opened in WriteOnly mode.
	  *				  Must set sample size and sample rate.
	  */
	void open(OpenModes mode, const QAudioFormat& format);

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
	  *
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
	  * @brief Returns file size in samples.
	  */
	int size() const;

	triksound::AudioBuffer read(int length) throw(ReadExc);
	triksound::AudioBuffer readAll() throw(ReadExc);

	/**
	  * @brief Write audio buffer to the file.
	  *
	  * @param buffer Buffer for writing.
	  * @param length Length of writing samples (equal to audio buffer size by default)
	  * @return Num of written bytes.
	  * @throw WriteDisabledExc Thrown if file is opened in ReadOnly mode.
	  * @throw FormatMismatchExc Thrown if formats of buffer and file are not equal.
	  */
	int write(triksound::AudioBuffer buffer, int length = -1) throw(WriteExc, FormatMismatchExc);
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


