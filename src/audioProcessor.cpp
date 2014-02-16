#include <QTimer>

//#include "sineOscillator.h"

#include "include/audioProcessor.h"

using namespace triksound;

AudioProcessor::AudioProcessor(int freq, QObject *parent) :
	QObject(parent),
	mOut(mRecorder.getFormat()),
	mDelay(2),
	mOutBuffer(mDelay * mRecorder.getFormat().sampleRate() + 10 * mRecorder.getFrameLength())
{
	mOutBuffer.open(QIODevice::ReadWrite);
	mSine = SineOscillator::generate(freq, 32768,
							 mRecorder.getFrameLength() / (mRecorder.getFormat().sampleSize() / 8),
							 mRecorder.getFormat());
}

void AudioProcessor::start()
{
	connect(&mRecorder, SIGNAL(captured(AudioBuffer)), this, SLOT(handleFrame(AudioBuffer)));
	QTimer::singleShot(mDelay * 1000, this, SLOT(startPlaying()));
	mRecorder.start();
}

void AudioProcessor::stop()
{
	mRecorder.stop();
	mOut.stop();
	mOutBuffer.clear();
}

void AudioProcessor::handleFrame(AudioBuffer buffer)
{
	mOutBuffer.write(mModulator.ringModulation(buffer, mSine).bytes());
}

void AudioProcessor::startPlaying()
{
	mOut.start(&mOutBuffer);
}
