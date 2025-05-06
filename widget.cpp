#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    _pDeviceEnumerator(nullptr),
    _pDefaultDevice(nullptr),
    _pDeviceTopology(nullptr),
    _pConnectorFrom(nullptr),
    _pConnectorTo(nullptr),
    _pPart(nullptr),
    _pAudioVolumeLevelFront(nullptr),
    _pAudioVolumeLevelRear(nullptr),
    _pAudioVolumeLevelSubwoofer(nullptr),
    _pAudioVolumeLevelCenter(nullptr),
    _pAudioVolumeLevelSide(nullptr),
    _isAppLoading(true)
{
    ui->setupUi(this);

    CoInitialize(nullptr);

    _hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&_pDeviceEnumerator);
    if (_hr != S_OK)
        return;

    _hr = _pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_pDefaultDevice); // need default device in mmsys.cpl Speakers
    if (_hr != S_OK)
        return;

    _hr = _pDefaultDevice->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, NULL, (PVOID *)&_pDeviceTopology);
    if (_hr != S_OK)
        return;

    _hr = _pDeviceTopology->GetConnector(0, &_pConnectorFrom);
    if (_hr != S_OK)
        return;

    _hr = _pConnectorFrom->GetConnectedTo(&_pConnectorTo);
    if (_hr != S_OK)
        return;

    _hr = _pConnectorTo->QueryInterface(__uuidof(IPart), (PVOID *)&_pPart);
    if (_hr != S_OK)
        return;



    LPWSTR PartName = nullptr;
    _pPart->GetName(&PartName);
    qDebug() << "pPart->GetName = " << QString::fromWCharArray(PartName); // Realtek HD Audio output
    CoTaskMemFree(PartName);
    PartName = nullptr;

    IPartsList *pPartList = nullptr;
    _hr = _pPart->EnumPartsIncoming(&pPartList);
    if(_hr != S_OK)
        return;

    IPart *pPartItem1 = nullptr;
    _hr = pPartList->GetPart(0, &pPartItem1);
    if(_hr != S_OK)
        return;

    LPWSTR PartNameItem1 = nullptr;
    pPartItem1->GetName(&PartNameItem1);
    qDebug() << " pPartItem1->GetName = " << QString::fromWCharArray(PartNameItem1); // Master Mute
    CoTaskMemFree(PartNameItem1);
    PartNameItem1 = nullptr;

    // Find GUID/KSNODE in Ksmedia.h and see what interface you can init
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dd371442(v=vs.85).aspx
    GUID guidPartItem1;
    _hr = pPartItem1->GetSubType(&guidPartItem1); // KSNODETYPE_MUTE pPartItem1->Activate(IAudioMute) (can control Master Mute of Realtek HD Audio output)
    if(_hr != S_OK)
        return;

    IPartsList *pPartList2 = nullptr;
    IPart *pPartItem2 = nullptr;
    _hr = pPartItem1->EnumPartsIncoming(&pPartList2);
    if(_hr != S_OK)
        return;

    _hr = pPartList2->GetPart(0, &pPartItem2);
    if(_hr != S_OK)
        return;

    LPWSTR PartNameItem2 = nullptr;
    pPartItem2->GetName(&PartNameItem2);
    qDebug() << "  pPartItem2->GetName = " << QString::fromWCharArray(PartNameItem2); // Speakers
    CoTaskMemFree(PartNameItem2);
    PartNameItem2 = nullptr;

    GUID guidPartItem2;
    _hr = pPartItem2->GetSubType(&guidPartItem2); // KSNODETYPE_VOLUME pPartItem2->Activate(IAudioVolumeLevel) (can control Master Volume of Realtek HD Audio output)
    if(_hr != S_OK)
        return;

    IPartsList *pPartList3 = nullptr;
    _hr = pPartItem2->EnumPartsIncoming(&pPartList3);
    if(_hr != S_OK)
        return;

    IPart *pPartItem3 = nullptr;
    pPartList3->GetPart(0, &pPartItem3);

    LPWSTR PartNameItem3;
    pPartItem3->GetName(&PartNameItem3);
    qDebug() << "   pPartItem3->GetName = " << QString::fromWCharArray(PartNameItem3); // Sum
    CoTaskMemFree(PartNameItem3);
    PartNameItem3 = nullptr;

    GUID guidPartItem3;
    _hr = pPartItem3->GetSubType(&guidPartItem3); // KSNODETYPE_SUM (Front, Rear, Subwoofer, Center, Side, ...)
    if(_hr != S_OK)
        return;


    IPartsList *pPartList4 = nullptr;
    _hr = pPartItem3->EnumPartsIncoming(&pPartList4);
    if(_hr != S_OK)
        return;

    UINT CountLevels;
    pPartList4->GetCount(&CountLevels); // PartList4Count


    // (Front, Rear, Subwoofer, Center, Side, ...)
    // -------
    // Front
    IPart *pPartItemFront = nullptr;
    pPartList4->GetPart(CountLevels-1, &pPartItemFront);

    LPWSTR PartItem4Name;
    pPartItemFront->GetName(&PartItem4Name);
    qDebug() << "    Sum =" << CountLevels;
    qDebug() << "    pPartItemFront->GetName = " << QString::fromWCharArray(PartItem4Name); // Front
    CoTaskMemFree(PartItem4Name);
    PartItem4Name = nullptr;

    GUID guidPartItem4;
    _hr = pPartItemFront->GetSubType(&guidPartItem4); // KSNODETYPE_VOLUME pPartItemLast->Activate(IAudioVolumeLevel)
    if(_hr != S_OK)
        return;

    _hr = pPartItemFront->Activate(CLSCTX_INPROC, __uuidof(IAudioVolumeLevel), (PVOID *)&_pAudioVolumeLevelFront);
    if(_hr != S_OK)
        return;

    if(pPartItemFront)
    {
        pPartItemFront->Release();
        pPartItemFront = nullptr;
    }


    // -------
    // Rear
    IPart *pPartItemRear = nullptr;
    pPartList4->GetPart(CountLevels-2, &pPartItemRear);

    LPWSTR PartItem5Name;
    pPartItemRear->GetName(&PartItem5Name);
    qDebug() << "    pPartItemRear->GetName = " << QString::fromWCharArray(PartItem5Name); // Rear
    CoTaskMemFree(PartItem5Name);
    PartItem5Name = nullptr;

    _hr = pPartItemRear->Activate(CLSCTX_INPROC, __uuidof(IAudioVolumeLevel), (PVOID *)&_pAudioVolumeLevelRear);
    if(_hr != S_OK)
        return;

    if(pPartItemRear)
    {
        pPartItemRear->Release();
        pPartItemRear = nullptr;
    }


    // -------
    // Subwoofer
    IPart *pPartItemSubwoofer = nullptr;
    pPartList4->GetPart(CountLevels-3, &pPartItemSubwoofer);

    LPWSTR PartItem6Name;
    pPartItemSubwoofer->GetName(&PartItem6Name);
    qDebug() << "    pPartItemSubwoofer->GetName = " << QString::fromWCharArray(PartItem6Name); // Subwoofer
    CoTaskMemFree(PartItem6Name);
    PartItem6Name = nullptr;

    _hr = pPartItemSubwoofer->Activate(CLSCTX_INPROC, __uuidof(IAudioVolumeLevel), (PVOID *)&_pAudioVolumeLevelSubwoofer);
    if(_hr != S_OK)
        return;

    if(pPartItemSubwoofer)
    {
        pPartItemSubwoofer->Release();
        pPartItemSubwoofer = nullptr;
    }


    // -------
    // Center
    IPart *pPartItemCenter = nullptr;
    pPartList4->GetPart(CountLevels-4, &pPartItemCenter);

    LPWSTR PartItem7Name;
    pPartItemCenter->GetName(&PartItem7Name);
    qDebug() << "    pPartItemCenter->GetName = " << QString::fromWCharArray(PartItem7Name); // Center
    CoTaskMemFree(PartItem7Name);
    PartItem7Name = nullptr;

    _hr = pPartItemCenter->Activate(CLSCTX_INPROC, __uuidof(IAudioVolumeLevel), (PVOID *)&_pAudioVolumeLevelCenter);
    if(_hr != S_OK)
        return;

    if(pPartItemCenter)
    {
        pPartItemCenter->Release();
        pPartItemCenter = nullptr;
    }


    // -------
    // Side
    IPart *pPartItemSide = nullptr;
    pPartList4->GetPart(CountLevels-5, &pPartItemSide);

    LPWSTR PartItem8Name;
    pPartItemSide->GetName(&PartItem8Name);
    qDebug() << "    pPartItemSide->GetName = " << QString::fromWCharArray(PartItem8Name); // Side
    CoTaskMemFree(PartItem8Name);
    PartItem8Name = nullptr;

    _hr = pPartItemSide->Activate(CLSCTX_INPROC, __uuidof(IAudioVolumeLevel), (PVOID *)&_pAudioVolumeLevelSide);
    if(_hr != S_OK)
        return;

    if(pPartItemSide)
    {
        pPartItemSide->Release();
        pPartItemSide = nullptr;
    }


/*
    // when guid = KSNODETYPE_MUTE

    IAudioMute *pAudioMute = nullptr;
    hr = pPartItem->Activate(CLSCTX_INPROC, __uuidof(IAudioMute), (void**)&pAudioMute);
    if(hr != S_OK)
        return;

    pAudioMute->SetMute(FALSE, NULL); // TRUE - off / FALSE - on
*/
}

void Widget::showEvent(QShowEvent *)
{
    // -------
    // Front
    float ch0[1], ch1[1];
    if(_pAudioVolumeLevelFront)
    {
        _pAudioVolumeLevelFront->GetLevel(0, ch0);
        _pAudioVolumeLevelFront->GetLevel(1, ch1);
        float volumeFront = this->dbToPercent(ch1[0]); // get db from right channel
        ui->hSlider_Front->setValue(volumeFront);
        ui->lbl_VolFront->setText(QString::number(static_cast<uint>(volumeFront)));
    }


    // -------
    // Rear
    if(_pAudioVolumeLevelRear)
    {
        _pAudioVolumeLevelRear->GetLevel(0, ch0);
        _pAudioVolumeLevelRear->GetLevel(1, ch1);
        float volumeRear = this->dbToPercent(ch1[0]); // get db from right channel
        ui->hSlider_Rear->setValue(volumeRear);
        ui->lbl_VolRear->setText(QString::number(static_cast<uint>(volumeRear)));
    }


    // -------
    // Subwoofer
    if(_pAudioVolumeLevelSubwoofer)
    {
        _pAudioVolumeLevelSubwoofer->GetLevel(0, ch0);
        float volumeSub = this->dbToPercent(ch0[0]); // subwoofer has only 1 channel
        ui->hSlider_Sub->setValue(volumeSub);
        ui->lbl_VolSub->setText(QString::number(static_cast<uint>(volumeSub)));
    }


    // -------
    // Center
    if(_pAudioVolumeLevelCenter)
    {
        _pAudioVolumeLevelCenter->GetLevel(0, ch0);
        float volumeCenter = this->dbToPercent(ch0[0]); // center has only 1 channel
        ui->hSlider_Center->setValue(volumeCenter);
        ui->lbl_VolCenter->setText(QString::number(static_cast<uint>(volumeCenter)));
    }


    // -------
    // Side
    if(_pAudioVolumeLevelSide)
    {
        _pAudioVolumeLevelSide->GetLevel(0, ch0);
        _pAudioVolumeLevelSide->GetLevel(1, ch1);
        float volumeSide = this->dbToPercent(ch1[0]); // get db from right channel
        ui->hSlider_Side->setValue(volumeSide);
        ui->lbl_VolSide->setText(QString::number(static_cast<uint>(volumeSide)));
    }


    // if volume = slider-value slot slider_valueChanged don't call!
    // this flag fix twice set volume on load app (when get-volume != slider-value)
    _isAppLoading = false;
}


// formula convert percent->db and db->percent
// http://www.sengpielaudio.com/calculator-thd.htm
// percent->db valueDB = 20 * lg(valueSliderFloat/100)
// db->percent valuePercent = 10^(valueDB/20) * 100
float Widget::dbToPercent(float value)
{
    return std::powf(10.0f, (value/20.0f)) * 100.0f;
}

float Widget::percentToDb(float value)
{
    return 20 * std::log10(value/100);
}

void Widget::on_hSlider_Front_valueChanged(int value)
{
    if(_isAppLoading)
        return;

    qDebug() << Q_FUNC_INFO;

    if(_pAudioVolumeLevelFront)
    {
        float valueDB = this->percentToDb(value);
        float valDBs[2] = { valueDB, valueDB };
        _pAudioVolumeLevelFront->SetLevelAllChannels(valDBs, 2, NULL);
    }

    ui->lbl_VolFront->setText(QString::number(value));
}

void Widget::on_hSlider_Rear_valueChanged(int value)
{
    if(_isAppLoading)
        return;

    qDebug() << Q_FUNC_INFO;

    if(_pAudioVolumeLevelRear)
    {
        float valueDB = this->percentToDb(value);
        float valDBs[2] = { valueDB, valueDB };
        _pAudioVolumeLevelRear->SetLevelAllChannels(valDBs, 2, NULL);
    }

    ui->lbl_VolRear->setText(QString::number(value));
}

void Widget::on_hSlider_Sub_valueChanged(int value)
{
    if(_isAppLoading)
        return;

    qDebug() << Q_FUNC_INFO;

    if(_pAudioVolumeLevelSubwoofer)
    {
        float valueDB = this->percentToDb(value);
        _pAudioVolumeLevelSubwoofer->SetLevel(0, valueDB, NULL);
    }

    ui->lbl_VolSub->setText(QString::number(value));
}

void Widget::on_hSlider_Center_valueChanged(int value)
{
    if(_isAppLoading)
        return;

    qDebug() << Q_FUNC_INFO;

    if(_pAudioVolumeLevelCenter)
    {
        float valueDB = this->percentToDb(value);
        _pAudioVolumeLevelCenter->SetLevel(0, valueDB, NULL);
    }

    ui->lbl_VolCenter->setText(QString::number(value));
}

void Widget::on_hSlider_Side_valueChanged(int value)
{
    if(_isAppLoading)
        return;

    qDebug() << Q_FUNC_INFO;

    if(_pAudioVolumeLevelSide)
    {
        float valueDB = this->percentToDb(value);
        float valDBs[2] = { valueDB, valueDB };
        _pAudioVolumeLevelSide->SetLevelAllChannels(valDBs, 2, NULL);
    }

    ui->lbl_VolSide->setText(QString::number(value));
}


Widget::~Widget()
{
    if(_pAudioVolumeLevelFront)
    {
        _pAudioVolumeLevelFront->Release();
        _pAudioVolumeLevelFront = nullptr;
    }

    if(_pAudioVolumeLevelRear)
    {
        _pAudioVolumeLevelRear->Release();
        _pAudioVolumeLevelRear = nullptr;
    }

    if(_pAudioVolumeLevelSubwoofer)
    {
        _pAudioVolumeLevelSubwoofer->Release();
        _pAudioVolumeLevelSubwoofer = nullptr;
    }

    if(_pAudioVolumeLevelCenter)
    {
        _pAudioVolumeLevelCenter->Release();
        _pAudioVolumeLevelCenter = nullptr;
    }

    if(_pAudioVolumeLevelSide)
    {
        _pAudioVolumeLevelSide->Release();
        _pAudioVolumeLevelSide = nullptr;
    }

    if(_pPart)
    {
        _pPart->Release();
        _pPart = nullptr;
    }

    if(_pConnectorTo)
    {
        _pConnectorTo->Release();
        _pConnectorTo = nullptr;
    }

    if(_pConnectorFrom)
    {
        _pConnectorFrom->Release();
        _pConnectorFrom = nullptr;
    }

    if(_pDeviceTopology)
    {
        _pDeviceTopology->Release();
        _pDeviceTopology = nullptr;
    }

    if(_pDefaultDevice)
    {
        _pDefaultDevice->Release();
        _pDefaultDevice = nullptr;
    }

    if(_pDeviceEnumerator)
    {
        _pDeviceEnumerator->Release();
        _pDeviceEnumerator = nullptr;
    }

    CoUninitialize();

    delete ui;
}


