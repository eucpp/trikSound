#include "trikInputDeviceManager.h"

const int TrikInputDeviceManager::mMaxVol = 119;

TrikInputDeviceManager::TrikInputDeviceManager(const QAudioDeviceInfo& device):
	DeviceManager(device),
	mVolume(0),
	mIsReady(false)
{}

void TrikInputDeviceManager::init()
{
	bool err = false;
	int res = 0;
	res = QProcess::execute("amixer", "set \"Right PGA Mixer Mic3R\" on");
	if (res < 0) {
		err = true;
	}
	res = QProcess::execute("amixer", "set \"Left PGA Mixer Mic3R\" on");
	if (res < 0) {
		err = true;
	}
	res = QProcess::execute("amixer", "set \"Left PGA Mixer Line2L\" on");
	if (res < 0) {
		err = true;
	}
	res = QProcess::execute("amixer", "set AGC on");
	if (res < 0) {
		err = true;
	}
	res = QProcess::execute("amixer", "set PGA " + QString().setNum(mMaxVol));
	if (res < 0) {
		err = true;
	}
	if (err) {
		qDebug() << "TrikInputDeviceManager construct error. Can't run amixer";
	}
	mIsReady = !err;
	return mIsReady;
}

void TrikInputDeviceManager::setVolume(float vol)
{
	mVolume = vol;
	QString arg = "set PGA " + QString().setNum(static_cast<int>(vol * mMaxVol));
	QStringList args(arg);
	int res = QProcess::execute("amixer", args);
	if (res < 0) {
		qDebug() << "TrikInputDeviceManager setVolume() error. Can't run amixer";
	}
}
