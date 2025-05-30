# DemoVolume

This Qt App can control the volume of a 5.1 surround sound system in Windows OS for each channel: Front, Rear, Subwoofer, Center, Side (mmsys.cpl).  
Used COM and WASAPI without third party libs.  

**COM Interfaces:** `IMMDevice, IMMDeviceEnumerator, IDeviceTopology, IConnector, IPart, IAudioVolumeLevel`.  

> [!NOTE]
> 
> These formulas convert percent->db and db->percent  
> https://sengpielaudio.com/calculator-thd.htm  
> percent->db: `valueDB = 20 * lg(valueSliderFloat/100)`  
> db->percent: `valuePercent = 10^(valueDB/20) * 100`
>
> Find GUID/KSNODE in Ksmedia.h and see what interface is possible init:  
> https://learn.microsoft.com/en-us/windows/win32/api/devicetopology/nf-devicetopology-ipart-getsubtype
> - 02b223c0-c557-11d0-8a2b-00a0c9255ac1 -> `KSNODETYPE_MUTE`
> - dff21ce1-f70f-11d0-b917-00a0c9223196 -> `KSNODETYPE_SPEAKER`
> - 3a5acc00-c557-11d0-8a2b-00a0c9255ac1 -> `KSNODETYPE_VOLUME`
> - da441a60-c556-11d0-8a2b-00a0c9255ac1 -> `KSNODETYPE_SUM`
> - 6994ad04-93ef-11d0-a3cc-00a0c9223196 -> `KSCATEGORY_AUDIO`
