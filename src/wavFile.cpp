#include "include/wavFile.h"

#include <QDebug>

using namespace triksound;

WavFile::WavFile():
	mFile(),
	mOpenmode(NotOpen),
	mHeaderSetFlag(false)
{}
WavFile::WavFile(const QString &filename):
	mFile(filename),
	mOpenmode(NotOpen),
	mHeaderSetFlag(false)
{}

bool WavFile::open(OpenModes mode)
{
	return open(mode, QAudioFormat());
}

bool WavFile::open(OpenModes mode, const QAudioFormat& format)
{
	try
	{
		mOpenmode = mode;
		if (mOpenmode == ReadOnly)
		{
			if (!mFile.open(QIODevice::ReadOnly)) {
				qDebug() << "Can't open wav file " << mFile.fileName() << " in ReadOnly mode";
				return false;
			}
			if (mFile.size() > 0 && mFile.size() < 44) {
				qDebug() << "File " << mFile.fileName() << " has incorrect wav header";
				return false;
			}
			seek(0);
			mHeader = readHeader();
			mHeaderSetFlag = true;
			return true;
		}
		else if (mOpenmode == WriteOnly)
		{
			if (!mFile.open(QIODevice::WriteOnly)) {
				qDebug() << "Can't open wav file " << mFile.fileName() << " in WriteOnly mode";
				return false;
			}
			if (isCorrectFormat(format)) {
				mHeader = format;
				writeHeader(mHeader);
				mHeaderSetFlag = true;
			}
			else {
				qDebug() << "Pass incorrect format when open wav file " << mFile.fileName();
			}
			seek(0);
			return true;
		}
		else if (mOpenmode == Append)
		{
			if (!mFile.open(QIODevice::ReadOnly)) {
				qDebug() << "Can't read header of wav file " << mFile.fileName();

				return false;
			}
			if (mFile.size() > 0 && mFile.size() < 44) {
				qDebug() << "File " << mFile.fileName() << " has incorrect wav header";
				return false;
			}
			mHeader = readHeader();
			mHeaderSetFlag = true;
			mFile.close();
			if (!mFile.open(QIODevice::Append)) {
				qDebug() << "Can't open wav file " << mFile.fileName() << " in Append mode";
				return false;
			}
			seek(size());
			return true;
		}
	}
	catch (UncorrectHeader exc)
	{
		qDebug() << "File " << mFile.fileName() << " has incorrect wav header";
		return false;
	}

	/*
	else if (openmode == ReadWrite)
	{
		if (!file.open(QIODevice::ReadWrite))
			throw OpenFileExc();
		// проверяем размер заголовка файла
		if (file.size() > 0 && file.size() < 44)
			throw OpenFileExc();
		header = readHeader();
		// перекодируем содержимое файла, если был передан новый формат, и он отличается от старого
		if (isCorrectFormat(format) && header != format)
		{
			Signal fileData = readAll();
			writeHeader(format);
			header = format;
			seek(0);
			write(fileData);
			seek(size());
		}
		return;
	}
	*/
}

void WavFile::close()
{
	mFile.close();
	mOpenmode = NotOpen;
	mHeaderSetFlag = false;
}

void WavFile::setFileName(const QString &filename)
{
	if (mOpenmode != NotOpen)
		mFile.close();
	mFile.setFileName(filename);
}

QString WavFile::fileName() const
{
	return mFile.fileName();
}

bool WavFile::isHeaderSet() const
{
	return mHeaderSetFlag;
}

void WavFile::setHeader(const QAudioFormat &format)
{
	if (mOpenmode == NotOpen)
	{
		mHeader = format;
		mHeaderSetFlag = true;
	}
}

QAudioFormat WavFile::getHeader() const
{
	if (mHeaderSetFlag)
		return mHeader;
	else
		return QAudioFormat();
}

bool WavFile::isOpen() const
{
	return mFile.isOpen();
}

bool WavFile::seek(int pos)
{
	return (mFile.seek(pos * (mHeader.sampleSize() / 8) + 44));
}

int WavFile::pos() const
{
	if (mFile.pos() < 44)
		return 0;
	if (isHeaderSet())
		return ((mFile.pos() - 44) / (mHeader.sampleSize() / 8));
	else
		return 0;
}

int WavFile::size() const
{
	return mFile.size();
}

int WavFile::samplesNum() const
{
	if (mOpenmode == NotOpen)
		return 0;
	return (mFile.size() - 44) / (mHeader.sampleSize() / 8);
}

AudioBuffer WavFile::read(int length)
{
	if (mOpenmode == NotOpen) {
		return AudioBuffer();
	}
	if (mOpenmode == WriteOnly) {
		return AudioBuffer();
	}
	int bytesNum = length * mHeader.sampleSize() / 8;
	QByteArray bytes = mFile.read(bytesNum);
	return AudioBuffer(bytes, mHeader);
}

AudioBuffer WavFile::readAll()
{
	if (mOpenmode == NotOpen) {
		return AudioBuffer();
	}
	if (mOpenmode == WriteOnly) {
		return AudioBuffer();
	}
	int currPos = pos();
	seek(0);
	QByteArray bytes = mFile.readAll();
	seek(currPos);
	return AudioBuffer(bytes, mHeader);
}

int WavFile::write(AudioBuffer buffer, int length)
{
	if ((mOpenmode == NotOpen) || !mHeaderSetFlag) {
		return -1;
	}
	if (mOpenmode == ReadOnly) {
		return -1;
	}
	if (mHeader != buffer.getFormat()) {
		return -1;
	}
	AudioBuffer subBuffer = buffer.subBuffer(0, length);
	int n = mFile.write(reinterpret_cast<const char*>(subBuffer.constData()), subBuffer.size());
	setDataSize(size() * (mHeader.sampleSize() / 8));
	return n / (mHeader.sampleSize() / 8);
}


QAudioFormat WavFile::readHeader()
{
	qint64 currPos = pos();
	mFile.seek(0);
	QByteArray headerArrayBytes = mFile.read(sizeof(Header));
	const char* headerBytes = headerArrayBytes.constData();
	seek(currPos);

	const Header* pHeader = reinterpret_cast<const Header*>(headerBytes);
	// проверяем, что это действительно заголовок wav файла
	if (    (pHeader->chunkId[0] != 'R') ||
			(pHeader->chunkId[1] != 'I') ||
			(pHeader->chunkId[2] != 'F') ||
			(pHeader->chunkId[3] != 'F'))
		throw UncorrectHeader();
	if (    (pHeader->format[0] != 'W') ||
			(pHeader->format[1] != 'A') ||
			(pHeader->format[2] != 'V') ||
			(pHeader->format[3] != 'E'))
		throw UncorrectHeader();
	if (    (pHeader->subchunk1Id[0] != 'f') ||
			(pHeader->subchunk1Id[1] != 'm') ||
			(pHeader->subchunk1Id[2] != 't') ||
			(pHeader->subchunk1Id[3] != ' '))
		throw UncorrectHeader();


	QAudioFormat format;
	format.setCodec("audio/pcm");
	format.setSampleRate(pHeader->sampleRate);
	format.setChannels(pHeader->numChannels);
	format.setSampleSize(pHeader->bitsPerSample);
	if (pHeader->bitsPerSample == 8)
		format.setSampleType(QAudioFormat::UnSignedInt);
	else
		format.setSampleType(QAudioFormat::SignedInt);
	return format;
}

void WavFile::writeHeader(const QAudioFormat& format)
{
	Header header;
	header.chunkId[0]='R';
	header.chunkId[1]='I';
	header.chunkId[2]='F';
	header.chunkId[3]='F';

	header.chunkSize = 36;

	header.format[0] = 'W';
	header.format[1] = 'A';
	header.format[2] = 'V';
	header.format[3] = 'E';

	header.subchunk1Id[0] = 'f';
	header.subchunk1Id[1] = 'm';
	header.subchunk1Id[2] = 't';
	header.subchunk1Id[3] = ' ';

	header.subchunk1Size = 16;
	header.audioFormat = 1;
	header.numChannels = format.channels();
	header.sampleRate = format.sampleRate();
	header.byteRate = header.sampleRate * header.numChannels * format.sampleSize() / 8;
	header.blockAlign = header.numChannels * format.sampleSize() / 8;
	header.bitsPerSample = format.sampleSize();

	header.subchunk2Id[0] = 'd';
	header.subchunk2Id[1] = 'a';
	header.subchunk2Id[2] = 't';
	header.subchunk2Id[3] = 'a';

	header.subchunk2Size = 0;

	char* bytes = reinterpret_cast<char*>(&header);
	qint64 currPos = pos();
	mFile.seek(0);
	mFile.write(bytes, sizeof(Header));
	seek(currPos);
}

void WavFile::setDataSize(int size)
{
	int currPos = pos();
	if (mOpenmode == Append)
	{
		mFile.close();
		mFile.open(QIODevice::ReadWrite);
	}
	mFile.seek(40);
	mFile.write(reinterpret_cast<char*>(&size), 4);
	if (mOpenmode == Append)
	{
		mFile.close();
		mFile.open(QIODevice::Append);
	}
	seek(currPos);
}

bool WavFile::isCorrectFormat(const QAudioFormat &format)
{
	return !((format.sampleRate() == -1) || (format.sampleSize() == -1) || (format.channelCount() == -1));
}

/*
unsigned int WavFile::readDataSize()
{
	unsigned int currPos = pos();
	file.seek(40);
	char* charSize = new char[5];
	file.read(charSize, 4);
	charSize[4] = '\0';
	seek(currPos);
	unsigned int* pSize = reinterpret_cast<unsigned int*>(charSize);
	unsigned int size = *pSize;
	delete[] charSize;
	return size;
}
*/
