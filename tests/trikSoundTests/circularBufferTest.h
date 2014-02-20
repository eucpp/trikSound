#ifndef CIRCULARBUFFERTEST_H
#define CIRCULARBUFFERTEST_H

#include <QObject>
#include <QtTest>

#include "circularBuffer.h"

class CircularBufferTest : public QObject
{
	Q_OBJECT
private slots:
	void writeTest()
	{
		const size_t size = 10;
		char arr[size];
		for (int i = 0; i < size; i++) {
			arr[i] = (char)i;
		}

		triksound::CircularBuffer buffer(size);
		buffer.open(QIODevice::WriteOnly);
		buffer.write(arr, size);

		for (int i = 0; i < size; i++) {
			QCOMPARE(buffer.getBoostBuffer()->operator [](i), arr[i]);
		}
	}

	void writeOverflowTest()
	{
		const size_t size = 15;
		char arr[size];
		for (int i = 0; i < size; i++) {
			arr[i] = (char)i;
		}

		const size_t bufSize = 10;
		triksound::CircularBuffer buffer(bufSize);
		buffer.open(QIODevice::WriteOnly);
		buffer.write(arr, size);

		int i = 0;
		int j = 0;
		for (i = 0, j = size - bufSize; i < size - bufSize; i++, j++) {
			QCOMPARE(buffer.getBoostBuffer()->operator [](i), arr[j]);
		}
		for (i = size - bufSize; i < bufSize; i++, j++) {
			QCOMPARE(buffer.getBoostBuffer()->operator [](i), arr[j]);
		}
	}

	void bytesAvailableTest()
	{
		const size_t size = 10;
		char arr[size];
		for (int i = 0; i < size; i++) {
			arr[i] = (char)i;
		}


		triksound::CircularBuffer buffer(size);
		buffer.open(QIODevice::ReadWrite);

		QCOMPARE(buffer.bytesAvailable(), (qint64)0);

		buffer.write(arr, 4);

		QCOMPARE(buffer.bytesAvailable(), (qint64)4);

		buffer.read(3);

		QCOMPARE(buffer.bytesAvailable(), (qint64)1);

		buffer.write(arr, 2);

		QCOMPARE(buffer.bytesAvailable(), (qint64)3);

		buffer.write(arr, 2);

		QCOMPARE(buffer.bytesAvailable(), (qint64)5);

		buffer.write(arr, (qint64)size);

		QCOMPARE(buffer.bytesAvailable(), (qint64)size);

		buffer.write(arr, (qint64)size);
		buffer.write(arr, (qint64)4);

		QCOMPARE(buffer.bytesAvailable(), (qint64)size);
	}

	void readTest()
	{
		const size_t size = 10;
		char expected[size];
		for (int i = 0; i < size; i++) {
			expected[i] = (char)i;
		}
		triksound::CircularBuffer buffer(size);
		buffer.open(QIODevice::ReadWrite);
		buffer.write(expected, size);

		char actual[size];
		buffer.read(actual, (quint64)size);

		for (int i = 0; i < size; i++) {
			QCOMPARE(actual[i], expected[i]);
		}
	}

	void readTest2()
	{
		const size_t size = 10;
		char expected[size];
		for (int i = 0; i < size; i++) {
			expected[i] = (char)i;
		}
		triksound::CircularBuffer buffer(size);
		buffer.open(QIODevice::ReadWrite);
		buffer.write(expected, 4);

		char actual[size];
		buffer.read(actual, (quint64)3);
		buffer.read(&actual[3], (qint64)1);

		for (int i = 0; i < 4; i++) {
			QCOMPARE(actual[i], expected[i]);
		}
	}

	void readOverflowTest()
	{
		const size_t size = 18;
		char expected[size];
		for (int i = 0; i < size; i++) {
			expected[i] = (char)i;
		}
		const size_t bufSize = 10;
		triksound::CircularBuffer buffer(bufSize);
		buffer.open(QIODevice::ReadWrite);
		buffer.write(expected, size);

		char actual[size];
		buffer.read(actual, size);

		int i = 0;
		int j = 0;
		for (i = 0, j = size - bufSize; i < size - bufSize; i++, j++) {
			QCOMPARE(actual[i], expected[j]);
		}
		for (i = size - bufSize; i < bufSize; i++, j++) {
			QCOMPARE(actual[i], expected[j]);
		}
	}



	void readOverflowTest2()
	{
		const size_t size = 10;
		char expected[size];
		for (int i = 0; i < size; i++) {
			expected[i] = 'A' + i;
		}
		const size_t bufSize = 10;
		triksound::CircularBuffer buffer(bufSize);
		buffer.open(QIODevice::ReadWrite);

		buffer.write(expected, 3);
		buffer.read(3);
		buffer.write(expected, 8);
		char actual[size];
		int bytesReaded = buffer.read(actual, bufSize);

	//	for (int i = 0; i < size; i++) {
	//		std::cout << "actual[" << i << "] " << (char)actual[i] << std::endl;
	//		std::cout << "expected[" << i << "] " << (char)expected[i] << std::endl;
	//	}

		for (int i = 0; i < bytesReaded; i++) {

			QCOMPARE(actual[i], expected[i]);
		}
	}

	void readQIODeviceCacheTest()
	{
		const size_t size = 10;
		char expected[size];
		for (int i = 0; i < size; i++) {
			expected[i] = 'A' + i;
		}
		const size_t bufSize = 10;
		triksound::CircularBuffer buffer(bufSize);
		buffer.open(QIODevice::ReadWrite);

		buffer.write(expected, size);
		buffer.read(3);
		buffer.write(expected, size);
		char actual[size];
		int bytesReaded = buffer.read(actual, bufSize);

	//	for (int i = 0; i < size; i++) {
	//		std::cout << "actual[" << i << "] " << (char)actual[i] << std::endl;
	//		std::cout << "expected[" << i << "] " << (char)expected[i] << std::endl;
	//	}

		for (int i = 0; i < bytesReaded; i++)
		{
			QCOMPARE(actual[i], expected[i]);
		}
	}

	void readAllTest()
	{
		const size_t size = 10;
		char arr[size];
		for (int i = 0; i < size; i++) {
			arr[i] = (char)i;
		}
		const size_t bufSize = 10;
		triksound::CircularBuffer buffer(bufSize);
		buffer.open(QIODevice::ReadWrite);
		buffer.write(arr, 3);
		buffer.write(arr, 3);

		int available = buffer.bytesAvailable();
		QByteArray test = buffer.readAll();

		QCOMPARE(test.size(), 6);
		QCOMPARE(test.size(), available);

		buffer.write(arr, 3);
		buffer.write(arr, 10);

		available = buffer.bytesAvailable();
		test = buffer.readAll();

		QCOMPARE(test.size(), 10);
		QCOMPARE(test.size(), available);
	}

	void readyReadSignalTest()
	{
		const size_t size = 10;
		char arr[size];
		for (int i = 0; i < size; i++) {
			arr[i] = (char)i;
		}
		const size_t bufSize = 10;
		triksound::CircularBuffer buffer(bufSize);

		QSignalSpy spy(&buffer, SIGNAL(readyRead()));
		buffer.open(QIODevice::ReadWrite);
		buffer.write(arr, 3);
		buffer.write(arr, 3);

		QCOMPARE(spy.count(), 2);
	}

	void writeBenchmark()
	{
		size_t size = 16 * 1024 * 1024;
		char* arr = new char[size];
		memset((void*)arr, 0, size);

		triksound::CircularBuffer buffer(size);

		QBENCHMARK {
			buffer.write(arr, 1000);
		}
	}

	void readBenchmark()
	{
		size_t size = 16 * 1024 * 1024;
		char* arr = new char[size];
		memset((void*)arr, 0, size);

		triksound::CircularBuffer buffer(size);
		buffer.write(arr, size);

		QBENCHMARK {
			buffer.read(1000);
		}
	}
};

#endif // CIRCULARBUFFERTEST_H
