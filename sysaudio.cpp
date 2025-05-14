#include "sysaudio.h"

SysAudio::SysAudio() :
    deviceEnumerator(nullptr),
    connectorFrom(nullptr),
    connectorTo(nullptr)
{

}

SysAudio::~SysAudio()
{
    CoUninitialize();
}

void SysAudio::init()
{
    CoInitialize(nullptr);

    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    if (hr != S_OK)
    {
        qDebug() << "hr != S_OK: CoCreateInstance(__uuidof(MMDeviceEnumerator): " << Q_FUNC_INFO;
        return;
    }

    CComPtr<IMMDevice> device = getDefaultDevice(EDataFlow::eRender); // need default device in mmsys.cpl Speakers
    if(!device)
    {
        return;
    }

    CComPtr<IDeviceTopology> deviceTopology = getDeviceTopology(device);
    if(!deviceTopology)
    {
        return;
    }

    hr = deviceTopology->GetConnector(0, &connectorFrom);
    if (hr != S_OK)
    {
        qDebug() << "hr != S_OK: deviceTopology->GetConnector(0, &connectorFrom): " << Q_FUNC_INFO;
        return;
    }

    hr = connectorFrom->GetConnectedTo(&connectorTo);
    if (hr != S_OK)
    {
        qDebug() << "hr != S_OK: connectorFrom->GetConnectedTo(&connectorTo): " << Q_FUNC_INFO;
        return;
    }
}

CComPtr<IMMDevice> SysAudio::getDefaultDevice(EDataFlow dataFlow)
{
    CComPtr<IMMDevice> device;
    HRESULT hr = deviceEnumerator->GetDefaultAudioEndpoint(dataFlow, eConsole, &device);
    if(hr != S_OK)
    {
        qDebug() << "hr != S_OK: deviceEnumerator->GetDefaultAudioEndpoint: " << Q_FUNC_INFO;
        return nullptr;
    }

    return device;
}

CComPtr<IDeviceTopology> SysAudio::getDeviceTopology(const CComPtr<IMMDevice> &device)
{
    if(!device)
    {
        qDebug() << "ERROR!: !device: " << Q_FUNC_INFO;
        return nullptr;
    }

    CComPtr<IDeviceTopology> deviceTopology;
    HRESULT hr = device->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (PVOID *)&deviceTopology);
    if (hr != S_OK)
    {
        qDebug() << "hr != S_OK: device->Activate(__uuidof(IDeviceTopology): " << Q_FUNC_INFO;
        return nullptr;
    }

    return deviceTopology;
}

CComPtr<IPart> SysAudio::getPartRoot() const
{
    if(!connectorTo)
    {
        qDebug() << "ERROR!: !connectorTo: " << Q_FUNC_INFO;
        return nullptr;
    }

    CComPtr<IPart> part;
    HRESULT hr = connectorTo->QueryInterface(__uuidof(IPart), (PVOID *)&part);
    if (hr != S_OK)
    {
        qDebug() << "hr != S_OK: connectorTo->QueryInterface(__uuidof(IPart), (PVOID *)&part): " << Q_FUNC_INFO;
        return nullptr;
    }

    return part;
}

CComPtr<IPart> SysAudio::getPart(CComPtr<IPart> &part, const QString &partName) const
{
    if(!part)
    {
        qDebug() << "ERROR!: !part: " << Q_FUNC_INFO;
        return nullptr;
    }

    const QString currPartName = getPartName(part);
    qDebug() << "getPartName: " << currPartName;
    if(currPartName == partName)
    {
        return part;
    }

    // Find GUID/KSNODE in Ksmedia.h and see what interface you can init
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dd371442(v=vs.85).aspx
    //GUID guid;
    //part->GetSubType(&guid);  // KSNODETYPE_MUTE part->Activate(IAudioMute) (can control Master Mute of Realtek HD Audio output)
                                // KSNODETYPE_VOLUME part->Activate(IAudioVolumeLevel) (can control Master Volume of Realtek HD Audio output)
                                // KSNODETYPE_SUM (Front, Rear, Subwoofer, Center, Side, ...)


    // When guid KSNODETYPE_MUTE
    /*
    CComPtr<IAudioMute> audioMute = nullptr;
    part->Activate(CLSCTX_INPROC, __uuidof(IAudioMute), (void**)&audioMute);
    part->SetMute(FALSE, nullptr); // TRUE - off / FALSE - on
    */

    CComPtr<IPartsList> partList = nullptr;
    HRESULT hr = part->EnumPartsIncoming(&partList);
    if(hr != S_OK)
    {
        qDebug() << "hr != S_OK: part->EnumPartsIncoming(&partList): " << Q_FUNC_INFO;
        return nullptr;
    }

    UINT count;
    hr = partList->GetCount(&count);
    if(hr != S_OK)
    {
        qDebug() << "hr != S_OK: partList->GetCount(&count): " << Q_FUNC_INFO;
        return nullptr;
    }

    qDebug() << "count: " << count;
    CComPtr<IPart> partNext;
    if(count > 1)
    {
        for(UINT i=0; i < count; i++)
        {
            CComPtr<IPart> partIdx;
            hr = partList->GetPart(i, &partIdx);
            if(hr != S_OK)
            {
                qDebug() << "hr != S_OK: partList->GetPart(i, &partIdx): " << Q_FUNC_INFO;
                return nullptr;
            }

            const QString currPartIdxName = getPartName(partIdx);
            qDebug() << i << "-> getPartName: " << currPartIdxName;
            if(currPartIdxName == partName)
            {
                return partIdx;
            }
        }
    }
    else
    {
        hr = partList->GetPart(0, &partNext);
        if(hr != S_OK)
        {
            qDebug() << "hr != S_OK: partList->GetPart(0, &partNext): " << Q_FUNC_INFO;
            return nullptr;
        }
    }

    return getPart(partNext, partName);
}

QString SysAudio::getPartName(const CComPtr<IPart> &part) const
{
    if(!part)
    {
        qDebug() << "ERROR!: !part: " << Q_FUNC_INFO;
        return QString();
    }

    LPWSTR wstrPartName = nullptr;
    part->GetName(&wstrPartName);
    const QString partName = QString::fromWCharArray(wstrPartName);
    CoTaskMemFree(wstrPartName);
    wstrPartName = nullptr;

    return partName;
}

CComPtr<IAudioVolumeLevel> SysAudio::getAudioVolumeLevel(const CComPtr<IPart> &part) const
{
    if(!part)
    {
        qDebug() << "ERROR!: !part: " << Q_FUNC_INFO;
        return nullptr;
    }

    CComPtr<IAudioVolumeLevel> audioVolumeLevel;
    HRESULT hr = part->Activate(CLSCTX_INPROC, __uuidof(IAudioVolumeLevel), (PVOID *)&audioVolumeLevel);
    if(hr != S_OK)
    {
        qDebug() << "hr != S_OK: part->Activate(CLSCTX_INPROC, __uuidof(IAudioVolumeLevel): " << Q_FUNC_INFO;
        return nullptr;
    }

    return audioVolumeLevel;
}

int SysAudio::getPartVolume(const QString &partName)
{
    if(partName.isEmpty())
    {
        return 0;
    }

    CComPtr<IPart> partRoot = getPartRoot();
    if(!partRoot)
    {
        return 0;
    }

    CComPtr<IPart> part = getPart(partRoot, partName);
    if(!part)
    {
        return 0;
    }

    CComPtr<IAudioVolumeLevel> audioVolumeLevel = getAudioVolumeLevel(part);
    if(!audioVolumeLevel)
    {
        return 0;
    }

    return getPartVolume(audioVolumeLevel);
}

int SysAudio::getPartVolume(CComPtr<IAudioVolumeLevel> &audioVolumeLevel)
{
    if(!audioVolumeLevel)
    {
        return 0;
    }

    UINT count;
    HRESULT hr = audioVolumeLevel->GetChannelCount(&count);
    if(hr != S_OK)
    {
        qDebug() << "hr != S_OK: audioVolumeLevel->GetChannelCount: " << Q_FUNC_INFO;
        return 0;
    }

    qDebug() << "channel count: " << count;
    float volume = 0.0f;
    if(count > 1)
    {
        float volumeRight;
        float volumeLeft;
        audioVolumeLevel->GetLevel(0, &volumeRight);
        audioVolumeLevel->GetLevel(1, &volumeLeft);
        volume = qMax(volumeRight, volumeLeft); // get max volume from right or left channels
    }
    else
    {
        audioVolumeLevel->GetLevel(0, &volume);
    }

    return dbToPercent(volume);
}

bool SysAudio::setPartVolume(const QString &partName, int volume)
{
    if(partName.isEmpty())
    {
        return false;
    }

    CComPtr<IPart> partRoot = getPartRoot();
    if(!partRoot)
    {
        return false;
    }

    CComPtr<IPart> part = getPart(partRoot, partName);
    if(!part)
    {
        return false;
    }

    CComPtr<IAudioVolumeLevel> audioVolumeLevel = getAudioVolumeLevel(part);
    if(!audioVolumeLevel)
    {
        return false;
    }

    int currentVolume = getPartVolume(audioVolumeLevel);
    if(currentVolume == volume)
    {
        qDebug() << "WARNING!: currentVolume == volume: " << volume << Q_FUNC_INFO;
        return false;
    }

    return setPartVolume(audioVolumeLevel, volume);
}

bool SysAudio::setPartVolume(const CComPtr<IAudioVolumeLevel> &audioVolumeLevel, int volume)
{
    if(!audioVolumeLevel)
    {
        return false;
    }

    UINT count;
    HRESULT hr = audioVolumeLevel->GetChannelCount(&count);
    if(hr != S_OK)
    {
        qDebug() << "hr != S_OK: audioVolumeLevel->GetChannelCount: " << Q_FUNC_INFO;
        return 0;
    }

    qDebug() << "channel count: " << count;

    float valueDB = percentToDb(volume);
    if(count > 1)
    {
        float valDBs[2] = { valueDB, valueDB };

        HRESULT hr = audioVolumeLevel->SetLevelAllChannels(valDBs, 2, nullptr);
        if(hr != S_OK)
        {
            qDebug() << "hr != S_OK: audioVolumeLevel->SetLevelAllChannels: " << Q_FUNC_INFO;
            return false;
        }
    }
    else
    {
        HRESULT hr = audioVolumeLevel->SetLevel(0, valueDB, nullptr);
        if(hr != S_OK)
        {
            qDebug() << "hr != S_OK: audioVolumeLevel->SetLevel: " << Q_FUNC_INFO;
            return false;
        }
    }

    return true;
}
