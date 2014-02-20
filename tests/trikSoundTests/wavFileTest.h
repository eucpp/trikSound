#pragma once

#include <QObject>
#include <QtTest/QtTest>
#include <QDebug>

#include "wavFile.h"


class WavFileTest : public QObject
{
	Q_OBJECT
private slots:
	// создаём тестовый файл и записываем в него хэдер и сигнал
	void init()
	{
		mHeader = QAudioFormat();
		mHeader.setCodec("audio/pcm");
		mHeader.setSampleRate(16000);
		mHeader.setSampleSize(16);
		mHeader.setChannels(1);
		mHeader.setSampleType(QAudioFormat::SignedInt);
		mFile.setFileName("wavFileTest.wav");
		mFile.open(triksound::WavFile::WriteOnly, mHeader);

		short* signal = new short[4];
		signal[0] = 32767;
		signal[1] = 16384;
		signal[2] = -256;
		signal[3] = -32768;
		mBuffer = triksound::AudioBuffer(QByteArray(reinterpret_cast<char*>(signal), 8), mHeader);

		mFile.close();
	}
	void cleanup()
	{
		mFile.close();
	}

	// тестируем запись/чтение заголовка wav файла
	void readWriteHeaderTest()
	{
		mFile.open(triksound::WavFile::ReadOnly);

		QAudioFormat readFormat = mFile.getHeader();

		QCOMPARE(readFormat, mHeader);
	}
	// тестируем кол-во записанных байт
	void readWriteSignalTest1()
	{
		mFile.open(triksound::WavFile::WriteOnly, mHeader);

		int samplesWritten = mFile.write(mBuffer);

		QCOMPARE(samplesWritten, 4);
	}

	// тестируем запись/чтение сигнала в файл
	void readWriteSignalTest2()
	{
		mFile.open(triksound::WavFile::WriteOnly, mHeader);

		mFile.write(mBuffer);
		mFile.close();
		mFile.open(triksound::WavFile::ReadOnly);
		triksound::AudioBuffer readSignal = mFile.readAll();

		QCOMPARE(readSignal, mBuffer);
	}
	// тестируем запись чтение части сигнала
	void readWriteSignalTest3()
	{
		mFile.open(triksound::WavFile::WriteOnly, mHeader);

		mFile.write(mBuffer, 3);
		mFile.close();
		mFile.open(triksound::WavFile::ReadOnly);
		triksound::AudioBuffer readSignal(mFile.read(2));

		QCOMPARE(readSignal, mBuffer.subBuffer(0, 2));
	}
	void samplesNumTest()
	{
		mFile.open(triksound::WavFile::WriteOnly, mHeader);
		mFile.write(mBuffer);

		QCOMPARE(mFile.samplesNum(), 4);
	}
	void sizeTest()
	{
		mFile.open(triksound::WavFile::WriteOnly, mHeader);
		mFile.write(mBuffer);

		QCOMPARE(mFile.size(), 4 * 2 + 44);
	}
	void seekPosTest()
	{
		mFile.open(triksound::WavFile::WriteOnly, mHeader);
		mFile.write(mBuffer);
		mFile.close();
		mFile.open(triksound::WavFile::ReadOnly);

		mFile.seek(1);

		QCOMPARE(mFile.pos(), 1);

		triksound::AudioBuffer signal(mFile.read(1));
		short* data = reinterpret_cast<short*>(signal.data());

		QCOMPARE(data[0], (short)16384);
	}

	// проверяем запись "с наложением" данных.
	void writeTest()
	{
		mFile.open(triksound::WavFile::WriteOnly, mHeader);

		mFile.write(mBuffer);
		mFile.seek(1);
		mFile.write(mBuffer);

		QCOMPARE(mFile.samplesNum(), 5);
	}

	// проверяем добавление в существующий файл.
	void appendTest()
	{
		mFile.open(triksound::WavFile::WriteOnly, mHeader);
		mFile.write(mBuffer);
		mFile.close();

		mFile.open(triksound::WavFile::Append);
		short* signal2 = new short[2];
		signal2[0] = 0;
		signal2[1] = 0;
		triksound::AudioBuffer buff(QByteArray(reinterpret_cast<char*>(signal2), 4), mHeader);

		mFile.write(buff);

		QCOMPARE(mFile.samplesNum(), 6);
	}
	// проверяем, сгенерируется ли исключение при попытке открыть некорректный wav файл.
	void UncorrectFileTest()
	{
		QFile badFile("testUncorrectWavFile.txt");
		badFile.open(QIODevice::WriteOnly);
		for (int i = 0; i < 20; i++)
			badFile.write("Lorem ipsum dolar sit amet ");
		badFile.close();

		mFile.setFileName("testUncorrectWavFile.txt");
		bool isOpen = mFile.open(triksound::WavFile::ReadOnly);

		QCOMPARE(isOpen, false);
	}
private:
	triksound::WavFile mFile;
	QAudioFormat mHeader;
	triksound::AudioBuffer mBuffer;
};
