#ifndef __OSCINPUTDEVICE_SAMPLE_DEVICE_H__
#define __OSCINPUTDEVICE_SAMPLE_DEVICE_H__

// include hardware - socket
#include "OSCInputDeviceSample_socket.h"

#include <string>

#include <fbsdk/fbsdk.h>

class OSCInputDeviceSampleDevice : public FBDevice
{
    // NOTE: This macro must be used INSIDE the custom device class
    FBDeviceDeclare(OSCInputDeviceSampleDevice, FBDevice);

public:
    virtual bool FBCreate();  // FBComponent Constructor
    virtual void FBDestroy(); // FBComponent Destructor

    // Essential FBDevice/Animation functions
    virtual bool AnimationNodeNotify(FBAnimationNode *pAnimationNode, FBEvaluateInfo *pEvaluateInfo);
    virtual void DeviceIONotify(kDeviceIOs pAction, FBDeviceNotifyInfo &pDeviceNotifyInfo);
    virtual bool DeviceOperation(kDeviceOperations pOperation);

    // Called by DeviceOperation()
    bool Init();
    bool Start();
    bool Stop();
    bool Reset();
    bool Done();

    // Recording animation data
    void RecordData(FBDeviceNotifyInfo &pDeviceNotifyInfo);

    // Network Configuration
    int GetPort() const { return mPort; }
    unsigned int GetIPAddress() const { return mIPAddress; }
    void SetPort(int port);
    void SetIPAddress(const char *ipAddrStr);

    // Message Getter to show in the device layout
    std::string GetDeviceMessage() const { return mDeviceMessage; }
    std::string PassSocketStatus() const { return sampleWinSocket.GetSocketStatus(); }

private:
    // hardware - UDP Socket for receiving data
    OSCInputDeviceSampleWinSocket sampleWinSocket;

    // Retarget Source Skeleton model template
    FBModelTemplate *mTemplateRoot;
    FBModelTemplate *mTemplateBone;
    FBAnimationNode *mNodeT;
    FBAnimationNode *mNodeR;

private:
    bool mBindSocketNeedsUpdate = false;  // Flag to check if the socket needs to be updated
    int mPort = 49983;                    // Default port
    unsigned int mIPAddress = INADDR_ANY; // Default address: all NIC

    // Original String to show in the device layout
    // NOTE: FBDevice::Status (FBPropertyString) is displayed at the left bottom of Device Layout UI,
    //       but it is too narrow to show long messages. So we use a custom string and custom display.
    //
    std::string mDeviceMessage;
};

#endif /* __OSCINPUTDEVICE_SAMPLE_DEVICE_H__ */