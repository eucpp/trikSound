#include "include/circularBuffer.h"

#include <QDebug>
#include <QSignalSpy>

using namespace triksound;

CircularBuffer::CircularBuffer(size_t n, QObject *parent) :
	QIODevice(parent),
	mBuffer(n),
	//mStartReadingPos(mBuffer.begin()),
	mPos(0),
	mLostBytesNum(0),
	mBytesAvailable(0)
{
}

bool CircularBuffer::isSequential() const
{
	return true;
}

bool CircularBuffer::open(QIODevice::OpenMode mode)
{
	setOpenMode(mode);
	return true;
}

void CircularBuffer::close()
{
	emit aboutToClose();
	setErrorString("");
	mBuffer.clear();
	setOpenMode(NotOpen);
}

qint64 CircularBuffer::pos() const
{
	return 0;
}

qint64 CircularBuffer::size() const
{
	return mBuffer.size();
}

bool CircularBuffer::seek(qint64 pos)
{
	Q_UNUSED(pos);
	return false;
}

bool CircularBuffer::atEnd() const
{
	// circular buffer has no end
	return false;
}

bool CircularBuffer::reset()
{
	return false;
}

size_t CircularBuffer::capacity()
{
	return mBuffer.capacity();
}

void CircularBuffer::setCapacity(size_t n)
{
	mBuffer.set_capacity(n);
}

qint64 CircularBuffer::bytesAvailable() const
{
	return mBytesAvailable + QIODevice::bytesAvailable() /*- mLostBytesNum*/;
}

qint64 CircularBuffer::bytesToWrite() const
{
	return 0;
}

bool CircularBuffer::canReadLine() const
{
	return false;
}

bool CircularBuffer::waitForReadyRead(int msecs)
{
	Q_UNUSED(msecs);
	return true;
}

bool CircularBuffer::waitForBytesWritten(int msecs)
{
	Q_UNUSED(msecs);
	return true;
}

qint64 CircularBuffer::reserve() const
{
	return mBuffer.reserve();
}

void CircularBuffer::clear()
{
	mBuffer.clear();
	mBytesAvailable = 0;
	mPos = 0;
	QIODevice::seek(QIODevice::pos() + QIODevice::bytesAvailable());
}

qint64 CircularBuffer::readData(char* data, qint64 maxlen)
{
	qint64 len = qMin(maxlen, mBytesAvailable);
	std::uninitialized_copy(mBuffer.begin() + mPos, mBuffer.begin() + mPos + len, data);
	mPos += len;
	mBytesAvailable -= len;
	return len;
}

qint64 CircularBuffer::readLineData(char* data, qint64 maxlen)
{
	int counter = 0;
	char* ch = data;
	boost::circular_buffer<char>::iterator itr = mBuffer.begin() + mPos;
	for (; itr != mBuffer.end(), ch < data + maxlen; ++ch, ++itr) {
		if (*itr == '\n') {
			break;
		}
		*ch = *itr;
		counter++;
	}
	//mStartReadingPos = itr;
	mBytesAvailable -= counter;
	mPos += counter - 1;
	return counter;
}

qint64 CircularBuffer::writeData(const char *data, qint64 len)
{
	int reserve = 0;
	if (mBuffer.reserve() == 0) {
		std::distance((mBuffer.begin() + mPos), mBuffer.end());
	}
	else {
		reserve = mBuffer.reserve();
	}
	if (len > reserve) {
		size_t overflow = len - reserve;
		overflow %= mBuffer.capacity();
		if (overflow == 0) {
			mPos = 0;
		}
		else {
			mPos -= overflow;
			if (mPos < 0) {
				mPos = 0;
			}
		}
	}
//	if (len > mBuffer.capacity() - bytesAvailable()) {
//		mLostBytesNum += QIODevice::bytesAvailable() - mLostBytesNum;
//	}

	// if len > size of buffer, drop all elements cached in QIODevice
	if (len > mBuffer.capacity()) {
		QIODevice::seek(QIODevice::pos() + QIODevice::bytesAvailable());
	}
	// else if len > size of non-cached part of buffer, drop only rewrited elements
	else if (len > mBuffer.capacity() - QIODevice::bytesAvailable()) {
		QIODevice::seek(QIODevice::pos() + (len - (mBuffer.capacity() - QIODevice::bytesAvailable())));
	}
	for (int i = 0; i < len; i++) {
		mBuffer.push_back(data[i]);
	}
	mBytesAvailable = qMin(mBytesAvailable + len, (qint64)(mBuffer.capacity()));
	emit readyRead();
	return qMin((qint64)mBuffer.capacity(), len);
}

/********************************************************************************
 * TEST SLOTS
 ********************************************************************************/

void CircularBuffer::writeTest()
{
	const size_t size = 10;
	char arr[size];
	for (int i = 0; i < size; i++) {
		arr[i] = (char)i;
	}

	CircularBuffer buffer(size);
	buffer.open(WriteOnly);
	buffer.write(arr, size);

	for (int i = 0; i < size; i++) {
		QCOMPARE(buffer.mBuffer[i], arr[i]);
	}
}

void CircularBuffer::writeOverflowTest()
{
	const size_t size = 15;
	char arr[size];
	for (int i = 0; i < size; i++) {
		arr[i] = (char)i;
	}

	const size_t bufSize = 10;
	CircularBuffer buffer(bufSize);
	buffer.open(WriteOnly);
	buffer.write(arr, size);

	int i = 0;
	int j = 0;
	for (i = 0, j = size - bufSize; i < size - bufSize; i++, j++) {
		QCOMPARE(buffer.mBuffer[i], arr[j]);
	}
	for (i = size - bufSize; i < bufSize; i++, j++) {
		QCOMPARE(buffer.mBuffer[i], arr[j]);
	}
}

void CircularBuffer::bytesAvailableTest()
{
	const size_t size = 10;
	char arr[size];
	for (int i = 0; i < size; i++) {
		arr[i] = (char)i;
	}


	CircularBuffer buffer(size);
	buffer.open(ReadWrite);

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

void CircularBuffer::readTest()
{
	const size_t size = 10;
	char expected[size];
	for (int i = 0; i < size; i++) {
		expected[i] = (char)i;
	}
	CircularBuffer buffer(size);
	buffer.open(ReadWrite);
	buffer.write(expected, size);

	char actual[size];
	buffer.read(actual, (quint64)size);

	for (int i = 0; i < size; i++) {
		QCOMPARE(actual[i], expected[i]);
	}
}

void CircularBuffer::readTest2()
{
	const size_t size = 10;
	char expected[size];
	for (int i = 0; i < size; i++) {
		expected[i] = (char)i;
	}
	CircularBuffer buffer(size);
	buffer.open(ReadWrite);
	buffer.write(expected, 4);

	char actual[size];
	buffer.read(actual, (quint64)3);
	buffer.read(&actual[3], (qint64)1);

	for (int i = 0; i < 4; i++) {
		QCOMPARE(actual[i], expected[i]);
	}
}

void CircularBuffer::readOverflowTest()
{
	const size_t size = 18;
	char expected[size];
	for (int i = 0; i < size; i++) {
		expected[i] = (char)i;
	}
	const size_t bufSize = 10;
	CircularBuffer buffer(bufSize);
	buffer.open(ReadWrite);
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



void CircularBuffer::readOverflowTest2()
{
	const size_t size = 10;
	char expected[size];
	for (int i = 0; i < size; i++) {
		expected[i] = 'A' + i;
	}
	const size_t bufSize = 10;
	CircularBuffer buffer(bufSize);
	buffer.open(ReadWrite);

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

void CircularBuffer::readQIODeviceCacheTest()
{
	const size_t size = 10;
	char expected[size];
	for (int i = 0; i < size; i++) {
		expected[i] = 'A' + i;
	}
	const size_t bufSize = 10;
	CircularBuffer buffer(bufSize);
	buffer.open(ReadWrite);

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

void CircularBuffer::readAllTest()
{
	const size_t size = 10;
	char arr[size];
	for (int i = 0; i < size; i++) {
		arr[i] = (char)i;
	}
	const size_t bufSize = 10;
	CircularBuffer buffer(bufSize);
	buffer.open(ReadWrite);
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

void CircularBuffer::readyReadSignalTest()
{
	const size_t size = 10;
	char arr[size];
	for (int i = 0; i < size; i++) {
		arr[i] = (char)i;
	}
	const size_t bufSize = 10;
	CircularBuffer buffer(bufSize);

	QSignalSpy spy(&buffer, SIGNAL(readyRead()));
	buffer.open(ReadWrite);
	buffer.write(arr, 3);
	buffer.write(arr, 3);

	QCOMPARE(spy.count(), 2);
}

void CircularBuffer::writeBenchmark()
{
	size_t size = 16 * 1024 * 1024;
	char* arr = new char[size];
	memset((void*)arr, 0, size);

	CircularBuffer buffer(size);

	QBENCHMARK {
		buffer.write(arr, 1000);
	}
}

void CircularBuffer::readBenchmark()
{
	size_t size = 16 * 1024 * 1024;
	char* arr = new char[size];
	memset((void*)arr, 0, size);

	CircularBuffer buffer(size);
	buffer.write(arr, size);

	QBENCHMARK {
		buffer.read(1000);
	}
}

