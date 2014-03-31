#ifndef SOUNDRECORDER_H
#define SOUNDRECORDER_H

#include <QObject>

#ifdef TRIK
	#include <QtMultimedia/QAudioInput>
#else
	#include <QtMultimediaKit/QAudioInput>
#endif

#include "trikSound_global.h"
#include "circularBuffer.h"
#include "filters/audioFilter.h"
#include "deviceManager.h"

namespace triksound {

/**
 * @brief The SoundRecorder class used for real-time audio capturing from device.
 *		  After an object creation and calling the start() slot object will emit captured() signal
 *		  every time when new bytes available.
 *		  This signal passes the buffer, which contains bytes of last frame.
 *		  You can specify the size of this buffer by passing its length in constructor or
 *		  by calling the setFrameLength() method.
 */
class TRIKSOUNDSHARED_EXPORT SoundRecorder : public QObject
{
	Q_OBJECT
public:
	enum Error {
		DEVICE_INIT_ERROR
	};

	/**
	 * @brief Construct new SoundRecorder with default audio input device.
	 * @param format Capture format
	 */
	SoundRecorder(const QAudioFormat& format = AudioBuffer::getDefaultFormat(), size_t frameLength = 1024);
	/**
	 * @brief Construct new SoundRecorder with giving device and format
	 * @param device
	 * @param format
	 */
	SoundRecorder(const QAudioDeviceInfo& device,
				  const QAudioFormat& format = AudioBuffer::getDefaultFormat(),
				  size_t frameLength = 1024);

	/**
	 * @return Size of captured frame.
	 */
	size_t getFrameLength() const;
	/**
	 * @brief Set the capture frame length (in bytes).
	 *		  You can call this method if the capturing is stopped, otherwise it do nothing.
	 * @param length
	 */
	void setFrameLength(size_t length);
	/**
	  *
	  */
	QAudioFormat getFormat() const;
	/**
	  *
	  */
	QAudioDeviceInfo getDevice() const;
signals:
	/**
	 * @brief This signal is emited when the new frame is captured.
	 * @param frame Buffer containing new frame.
	 */
	void captured(const AudioBuffer& frame);
	void error(Error);
public slots:
	/**
	 * @brief Start audio capture
	 */
	void start();
	/**
	 * @brief Stop capture if it activated now
	 */
	void stop();
private slots:
	void readyReadHandler();

private:
	QScopedPointer<DeviceManager> mDevice;
	QScopedPointer<QAudioInput> mAudioInput;
	CircularBuffer mBuffer;

	size_t mFrameLength;
};

}

#endif // SOUNDRECORDER_H
