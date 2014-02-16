#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <QIODevice>
#include <QByteArray>
#include <QTest>

#include <boost/circular_buffer.hpp>

namespace triksound {

class CircularBuffer : public QIODevice
{
	Q_OBJECT
public:
	explicit CircularBuffer(size_t n = 1024, QObject *parent = 0);

	bool isSequential() const;

	bool open(OpenMode mode);
	void close();

	virtual qint64 pos() const;
	virtual qint64 size() const;
	bool seek(qint64 pos);
	bool atEnd() const;
	bool reset();

	size_t capacity();
	void setCapacity(size_t n);

	qint64 bytesAvailable() const;
	qint64 bytesToWrite() const;

	bool canReadLine() const;

	bool waitForReadyRead(int msecs);
	bool waitForBytesWritten(int msecs);

	qint64 reserve() const;

	void clear();

signals:

private slots:
	qint64 readData(char *data, qint64 maxlen);
	qint64 readLineData(char *data, qint64 maxlen);
	qint64 writeData(const char *data, qint64 len);

	/********************************************************************************
	 * TEST SLOTS
	 ********************************************************************************/

	// test writing to the buffer without overflow
	void writeTest();
	// test writing to the buffer with overflow (circlular buffer must start write to the begining after overflow)
	void writeOverflowTest();
	// test bytesAvailable() method
	void bytesAvailableTest();
	// test reading from buffer (which isn't overflowed yet)
	void readTest();
	// test readint from buffer (not all available bytes)
	void readTest2();
	// test reading from buffer which was overflowed
	void readOverflowTest();
	// test more complicated case of reading from overflowed buffer
	void readOverflowTest2();
	// test skip of QIODevice cache (from overflowed buffer)
	void readQIODeviceCacheTest();
	// test readAll()
	void readAllTest();
	// test the emiting of readyRead() signal after writing to buffer
	void readyReadSignalTest();
	// write() benchmark test
	void writeBenchmark();
	// read() benchmark test
	void readBenchmark();
private:
	typedef boost::circular_buffer<char>::iterator cbIterator;

	boost::circular_buffer<char> mBuffer;
	//cbIterator mStartReadingPos;
	qint64 mPos;
	qint64 mLostBytesNum;
	qint64 mBytesAvailable;
};

}

#endif // CIRCULARBUFFER_H
