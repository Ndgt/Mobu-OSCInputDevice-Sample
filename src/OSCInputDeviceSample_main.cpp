#include "OSCInputDeviceSample_main.h"

// These macros must be used to register the custom device class.
FBDeviceImplementation(OSCInputDeviceSampleDevice); // Class
FBRegisterDevice(
    "OSCInputDeviceSampleDevice",                    // Class Name Str
    OSCInputDeviceSampleDevice,                      // Class
    "OSCInputDevice Sample",                         // Name displayed in the Asset Browser of the Resoures Window
    "This is the sample of device using OSC input.", // Device Description - only acccessed by FBObject_GetEntryDescription()
    FB_DEFAULT_SDK_ICON);                            // Icon displayed in the Asset Browser of the Resources Window

// FBComponent Constructor
bool OSCInputDeviceSampleDevice::FBCreate()
{
    // Connect hardware
    sampleWinSocket.SetupDeviceConnection(this);

    // Initialize model template
    // NOTE: You can specify any name(recommended to use the unique name for the custom device)
    //       for the first parameter 'pPrefix' of FBModelTemplate.
    mTemplateRoot = new FBModelTemplate("OSCInputDeviceSample", "Root", kFBModelTemplateRoot);
    mTemplateBone = new FBModelTemplate("OSCInputDeviceSample", "Bone", kFBModelTemplateSkeleton);

    // Setup animation nodes of the bone
    mNodeT = AnimationNodeOutCreate(0, "Translation", ANIMATIONNODE_TYPE_LOCAL_TRANSLATION);
    mNodeR = AnimationNodeOutCreate(1, "Rotation", ANIMATIONNODE_TYPE_LOCAL_ROTATION);
    mTemplateBone->Bindings.Add(mNodeT);
    mTemplateBone->Bindings.Add(mNodeR);

    // Setup the model hierarchy
    this->ModelTemplate.Children.Add(mTemplateRoot);
    mTemplateRoot->Children.Add(mTemplateBone);

    // You can expilicitly set the sampling period for the device.
    // NOTE: If not specified, the period will be set to the value defined in Application.txt.
    //       You can specify higher fps, but the actual process might be limited your environment.
    //
    // this->SamplingPeriod = 1.0 / 60.0; // 60 FPS

    return true;
}

// FBComponent Destructor
void OSCInputDeviceSampleDevice::FBDestroy()
{
}

// Device operation.
bool OSCInputDeviceSampleDevice::DeviceOperation(kDeviceOperations pOperation)
{
    switch (pOperation)
    {
    case kOpInit:
    {
        // Called once when the device is dropped into the Viewer
        return Init();
    }
    case kOpStart:
    {
        // Called
        // - after DeviceOperation(kOpInit) is called
        // - when push the Online button to make the device online
        return Start();
    }
    case kOpAutoDetect:
        break;
    case kOpStop:
    {
        // Called when push the Online button to make the device offline
        return Stop();
    }
    case kOpReset:
    {
        // This seemes not to be called usually.
        // Some user might explicitly call "DeviceOperation(kOpReset)" to reset the device.
        return Reset();
    }
    case kOpDone:
    {
        // Called when the device is deleted - removed from the scene
        return Done();
    }
    }
    return FBDevice::DeviceOperation(pOperation);
}

// DeviceOperation - init
bool OSCInputDeviceSampleDevice::Init()
{
    // Create the socket
    if (!sampleWinSocket.CreateSampleSocket())
    {
        FBMessageBox("Warning", "Error: Failed to create socket", "OK");
        mDeviceMessage = "The Device is not initialized.";
        return false;
    }

    // Bind the socket to the address and port
    if (!sampleWinSocket.BindSocket(mPort, mIPAddress))
    {
        FBMessageBox("Warning", "Error: Failed to bind socket or retrieve address info", "OK");
        mDeviceMessage = "The Device is not initialized.";
        return false;
    }

    this->Status = "Initialized";
    return true;
}

// DeviceOperation - online
bool OSCInputDeviceSampleDevice::Start()
{
    if (mBindSocketNeedsUpdate)
    {
        sampleWinSocket.CloseSampleSocket(); // Close the socket

        // Reinitialize the socket with the new settings
        if (Init())
            mDeviceMessage = "Connection updated.";
        else
            return false; // return false to block the device from going online
    }
    this->Status = "Online";
    return true;
}

// DeviceOperation - offline
bool OSCInputDeviceSampleDevice::Stop()
{
    this->Status = "Offline";
    return true;
}

// DeviceOperation - reset
bool OSCInputDeviceSampleDevice::Reset()
{
    return true;
}

// DeviceOperation - remove
bool OSCInputDeviceSampleDevice::Done()
{
    // Close the socket
    sampleWinSocket.CloseSampleSocket();
    return true;
}

// MotionBuilder Input/Output thread notification function
// Place receive/send & key add function here.
void OSCInputDeviceSampleDevice::DeviceIONotify(kDeviceIOs pAction, FBDeviceNotifyInfo &pDeviceNotifyInfo)
{
    // NOTE: Using pDeviceNotifyInfo, you can get or set
    //       - Local Time
    //       - System Time
    //       - Sync Count
    //       - Evaluate Info (for animation nodes)

    switch (pAction)
    {
    // INPUT state of engine
    case kIOStopModeRead: // transport is stopped
    case kIOPlayModeRead: // transport is playing
    {
        {
            // Receive data from socket - from other services
            int recvLen = sampleWinSocket.ReceiveData();

            // If we received data, show the state in the device layout UI as "sample(s)/s"
            if (recvLen > 0)
            {
                AckOneSampleReceived();
            }

            // This function collects only data needed to be recorded.
            // Only works if the transport is recording
            RecordData(pDeviceNotifyInfo);
        }
    }
    break;

    // OUTPUT state of engine - for output device
    case kIOStopModeWrite:
    case kIOPlayModeWrite:
    {
    }
    }
}

// MotionBuilder Evaluation thread notification function
// Place evaluation(applying animation) function here.
// Note: AnimationNodeNotify() of FBDevice is called when Online and "Live" is checked
bool OSCInputDeviceSampleDevice::AnimationNodeNotify(FBAnimationNode *pAnimationNode, FBEvaluateInfo *pEvaluateInfo)
{
    // Pull data from the socket
    BoneTransform *bone = sampleWinSocket.PassData();

    // Write the data to the animation nodes - apply the animation to the model
    mNodeT->WriteData(bone->position, pEvaluateInfo);
    mNodeR->WriteData(bone->rotation, pEvaluateInfo);

    return true;
}

// Record animation data
void OSCInputDeviceSampleDevice::RecordData(FBDeviceNotifyInfo &pDeviceNotifyInfo)
{
    // Get the current time from the device
    FBTime lTime;
    lTime = pDeviceNotifyInfo.GetLocalTime();

    // Get the current transform
    BoneTransform *bone = sampleWinSocket.PassData();

    // IMPORTANT: Get the animation node to collect data to record
    FBAnimationNode *nodeT = mNodeT->GetAnimationToRecord();
    FBAnimationNode *nodeR = mNodeR->GetAnimationToRecord();

    // Add keys to the animation nodes at the current time
    // NOTE: This is simplified for the sample of UDP input device.
    //       You can consider FBDevice::SamplingMode to determine how to add keys
    if (nodeT)
        nodeT->KeyAdd(lTime, bone->position);
    if (nodeR)
        nodeR->KeyAdd(lTime, bone->rotation);
}

void OSCInputDeviceSampleDevice::SetPort(int port)
{
    if (port < 0 || port > 65535)
    {
        mDeviceMessage = "Error: Invalid port number specified. Currently connected to port: " + std::to_string(mPort);
        return;
    }

    mPort = port;
    mDeviceMessage = "Port set to: " + std::to_string(mPort); // Update the device status message
    mDeviceMessage += "\nPlease toggle the device offline and then online again to apply the new port.";
    mBindSocketNeedsUpdate = true; // Set the flag to update the socket
}

void OSCInputDeviceSampleDevice::SetIPAddress(const char *ipAddrStr)
{
    struct in_addr addr;
    if (inet_pton(AF_INET, ipAddrStr, &addr) == 1)
    {
        mIPAddress = addr.s_addr;
        mDeviceMessage = "IP Address set to: " + std::string(ipAddrStr); // Update the device status message
        mDeviceMessage += "\nPlease toggle the device offline and then online again. to apply the new IP address.";
        mBindSocketNeedsUpdate = true; // Set the flag to update the socket
    }
    else
    {
        // Retrieve the current connected IP address and show it in the device message
        addr.s_addr = mIPAddress;
        char str[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &addr, str, sizeof(str)) != NULL)
            mDeviceMessage = "Error: Invalid IP address specified. Currently connected to IP: " + std::string(str);
        else
            mDeviceMessage = "Error: Invalid IP address specified.";
    }
}