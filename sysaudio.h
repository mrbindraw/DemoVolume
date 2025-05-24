#ifndef SYSAUDIO_H
#define SYSAUDIO_H

#include <initguid.h>
#include <Windows.h>
#include <cguid.h>
#include <atlbase.h>

#include <mmdeviceapi.h>
#include <devicetopology.h>
#include <Ksmedia.h>

#include <QString>
#include <QDebug>

class SysAudio
{
private:
    SysAudio();

    CComPtr<IMMDeviceEnumerator> deviceEnumerator;
    CComPtr<IConnector> connectorFrom;
    CComPtr<IConnector> connectorTo;

public:
    static SysAudio &getInstance()
    {
        static SysAudio instance;
        return instance;
    }

    SysAudio(SysAudio const &) = delete;
    SysAudio &operator = (SysAudio const &) = delete;

    ~SysAudio();

    void init();

    CComPtr<IMMDevice> getDefaultDevice(EDataFlow dataFlow);
    CComPtr<IDeviceTopology> getDeviceTopology(const CComPtr<IMMDevice> &device);
    CComPtr<IPart> getPartRoot() const;
    CComPtr<IPart> getPart(CComPtr<IPart> &part, const QString &partName) const;
    bool isPartExist(const QString &partName, CComPtr<IPart> &outPart) const;
    bool isPartExist(const QString &partName) const;
    QString getPartName(const CComPtr<IPart> &part) const;

    CComPtr<IAudioVolumeLevel> getAudioVolumeLevel(const CComPtr<IPart> &part) const;
    CComPtr<IAudioVolumeLevel> getAudioVolumeLevel(const QString &partName) const;
    int getPartVolume(const QString &partName);
    int getPartVolume(CComPtr<IAudioVolumeLevel> &audioVolumeLevel);

    bool setPartVolume(const QString &partName, int volume);
    bool setPartVolume(const CComPtr<IAudioVolumeLevel> &audioVolumeLevel, int volume);

    // utils
    // formula convert percent->db and db->percent
    // http://www.sengpielaudio.com/calculator-thd.htm
    // percent->db valueDB = 20 * lg(valueSliderFloat/100)
    // db->percent valuePercent = 10^(valueDB/20) * 100
    inline static int dbToPercent(float value)
    {
        return std::powf(10.0f, (value/20.0f)) * 100.0f;
    }

    inline static float percentToDb(float value)
    {
        return 20 * std::log10(value/100);
    }
};

#endif // SYSAUDIO_H
