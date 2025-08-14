#ifndef __OSCINPUTDEVICE_SAMPLE_LAYOUT_H__
#define __OSCINPUTDEVICE_SAMPLE_LAYOUT_H__

#include "OSCInputDeviceSample_main.h"

class OSCInputDeviceSampleLayout : public FBDeviceLayout
{
    // NOTE: This macro must be used INSIDE the custom device layout class
    FBDeviceLayoutDeclare(OSCInputDeviceSampleLayout, FBDeviceLayout);

public:
    virtual bool FBCreate();  // FBComponent Constructor
    virtual void FBDestroy(); // FBComponent Destructor

    // UI Configuration
    void UIConfigure();
    void EventEditPortChange(HISender pSender, HKEvent pEvent);
    void EventEditIPAddressChange(HISender pSender, HKEvent pEvent);
    void GetStatusMessages(HISender pSender, HKEvent pEvent);

private:
    // Handle to the device
    OSCInputDeviceSampleDevice *mOSCInputDevice;

    // UI Components
    FBLabel mLabelPort;
    FBEdit mEditPort;
    FBLabel mLabelIPAddress;
    FBEdit mEditIPAddress;
    FBLabel mLabelDeviceMessage;
    FBLabel mLabelSocketStatus;
};

#endif /* __OSCINPUTDEVICE_SAMPLE_LAYOUT_H__ */