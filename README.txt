This Demo show how to change volume speakers of each levels
for surround 7.1 system mmsys.cpl (Front, Rear, Subwoofer, Center, Side)
without thirdparty libs.

COM Interfaces: IMMDevice, IMMDeviceEnumerator, IDeviceTopology, IConnector, IPart, IAudioVolumeLevel


-------------
Correct test:
-------------
OS: Windows 10 x64
IDE: Qt5.5.0-msvc2013-x64 
http://download.qt.io/official_releases/qt/5.5/5.5.0/qt-opensource-windows-x86-msvc2013_64-5.5.0.exe.mirrorlist
WindowsSDK 8.1
Realtek HD Audio surround 7.1 system
see mmsys.cpl Playback->Speakers set as default


--------------
Debug output:
--------------
pPart->GetName =  "Realtek HD Audio output"
 pPartItem1->GetName =  "Master Mute"
  pPartItem2->GetName =  "Speakers"
   pPartItem3->GetName =  "Sum"
    Sum = 14
    pPartItemFront->GetName =  "Front"
    pPartItemRear->GetName =  "Rear"
    pPartItemSubwoofer->GetName =  "Subwoofer"
    pPartItemCenter->GetName =  "Center"
    pPartItemSide->GetName =  "Side"

	
-------------
Info:
-------------
Find GUID/KSNODE in Ksmedia.h and see what interface you can init
https://msdn.microsoft.com/en-us/library/windows/desktop/dd371442(v=vs.85).aspx

02b223c0-c557-11d0-8a2b-00a0c9255ac1		KSNODETYPE_MUTE
dff21ce1-f70f-11d0-b917-00a0c9223196	 	KSNODETYPE_SPEAKER
3a5acc00-c557-11d0-8a2b-00a0c9255ac1		KSNODETYPE_VOLUME
da441a60-c556-11d0-8a2b-00a0c9255ac1		KSNODETYPE_SUM
6994ad04-93ef-11d0-a3cc-00a0c9223196		KSCATEGORY_AUDIO

Formula convert percent->db and db->percent
http://www.sengpielaudio.com/calculator-thd.htm
percent->db valueDB = 20 * lg(valueSliderFloat/100)
db->percent valuePercent = 10^(valueDB/20) * 100
