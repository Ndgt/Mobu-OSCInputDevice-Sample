#include "OSCInputDeviceSample_UI.h"

// These macros must be used to register the custom device layout class.
FBDeviceLayoutImplementation(OSCInputDeviceSampleLayout); // Class
FBRegisterDeviceLayout(
    OSCInputDeviceSampleLayout,   // Class
    "OSCInputDeviceSampleDevice", // IMPORTANT: Name of the device class
    FB_DEFAULT_SDK_ICON);         // Default Icon

// FBComponent Constructor
bool OSCInputDeviceSampleLayout::FBCreate()
{
    FBDevice *baseDevice = this->Device;
    mOSCInputDevice = dynamic_cast<OSCInputDeviceSampleDevice *>(baseDevice); // Get a handle on the device.
    UIConfigure();                                                            // Configure the UI
    return true;
}

// FBComponent Destructor
void OSCInputDeviceSampleLayout::FBDestroy()
{
    //  Remove the callback
    OnIdle.Remove(this, (FBCallback)&OSCInputDeviceSampleLayout::GetStatusMessages);
}

void OSCInputDeviceSampleLayout::UIConfigure()
{
    // Configure the port label and edit field
    mLabelPort.Caption = "Port:";
    mEditPort.Text = std::to_string(mOSCInputDevice->GetPort()).c_str();

    // Configure the IP address label and edit field
    mLabelIPAddress.Caption = "IP Address:";
    struct in_addr addr;
    addr.s_addr = mOSCInputDevice->GetIPAddress();
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr, str, sizeof(str));
    mEditIPAddress.Text = str;

    // Configure the message labels
    mLabelDeviceMessage.Caption = "";
    mLabelSocketStatus.Caption = "";

    // Register Event Callback
    mEditPort.OnChange.Add(this, (FBCallback)&OSCInputDeviceSampleLayout::EventEditPortChange);
    mEditIPAddress.OnChange.Add(this, (FBCallback)&OSCInputDeviceSampleLayout::EventEditIPAddressChange);
    this->OnIdle.Add(this, (FBCallback)&OSCInputDeviceSampleLayout::GetStatusMessages);

    int lS = 4;   // Space
    int lH = 25;  // Height
    int lW = 100; // Width

    const char *lRegionNameLabelPort = "LabelPort";
    const char *lRegionNameEditPort = "EditPort";
    const char *lRegionNameLabelIPAddress = "LabelIPAddress";
    const char *lRegionNameEditIPAddress = "EditIPAddress";
    const char *lRegionNameLabelDeviceMessage = "LabelDeviceMessage";
    const char *lRegionNameLabelSocketStatus = "LabelSocketStatus";

    // Create regions within the main layout for each UI element
    AddRegion(lRegionNameLabelPort, lRegionNameLabelPort,
              lS, kFBAttachLeft, "", 1.0,
              lS, kFBAttachTop, "", 1.0,
              lW, kFBAttachNone, "", 1.0,
              lH, kFBAttachNone, "", 1.0);
    AddRegion(lRegionNameEditPort, lRegionNameEditPort,
              lS, kFBAttachRight, lRegionNameLabelPort, 1.0,
              lS, kFBAttachTop, "", 1.0,
              lW, kFBAttachNone, "", 1.0,
              lH, kFBAttachNone, "", 1.0);
    AddRegion(lRegionNameLabelIPAddress, lRegionNameLabelIPAddress,
              lS, kFBAttachLeft, "", 1.0,
              lS, kFBAttachBottom, lRegionNameLabelPort, 1.0,
              lW, kFBAttachNone, "", 1.0,
              lH, kFBAttachNone, "", 1.0);
    AddRegion(lRegionNameEditIPAddress, lRegionNameEditIPAddress,
              lS, kFBAttachRight, lRegionNameLabelIPAddress, 1.0,
              lS, kFBAttachBottom, lRegionNameEditPort, 1.0,
              lW, kFBAttachNone, "", 1.0,
              lH, kFBAttachNone, "", 1.0);
    AddRegion(lRegionNameLabelDeviceMessage, lRegionNameLabelDeviceMessage,
              lS, kFBAttachLeft, "", 1.0,
              lS * 3, kFBAttachBottom, lRegionNameLabelIPAddress, 1.0,
              0, kFBAttachRight, "", 1.0,
              lH * 4, kFBAttachNone, "", 1.0);
    AddRegion(lRegionNameLabelSocketStatus, lRegionNameLabelSocketStatus,
              lS, kFBAttachLeft, "", 1.0,
              lS * 2, kFBAttachBottom, lRegionNameLabelDeviceMessage, 1.0,
              0, kFBAttachRight, "", 1.0,
              lH * 4, kFBAttachNone, "", 1.0);

    // Give UI components Controls to the regions name specified
    SetControl(lRegionNameLabelPort, mLabelPort);
    SetControl(lRegionNameEditPort, mEditPort);
    SetControl(lRegionNameLabelIPAddress, mLabelIPAddress);
    SetControl(lRegionNameEditIPAddress, mEditIPAddress);
    SetControl(lRegionNameLabelDeviceMessage, mLabelDeviceMessage);
    SetControl(lRegionNameLabelSocketStatus, mLabelSocketStatus);
}

// Called when the port edit field changes
void OSCInputDeviceSampleLayout::EventEditPortChange(HISender pSender, HKEvent pEvent)
{
    const char *text = mEditPort.Text.AsString();
    try
    {
        int lPort = std::stoi(text);
        mOSCInputDevice->SetPort(lPort);
    }
    catch (std::exception &e)
    {
        mLabelDeviceMessage.Caption = (std::string("Invalid port number specified: ") + e.what()).c_str();
    }
}

// Called when the IP address edit field changes
void OSCInputDeviceSampleLayout::EventEditIPAddressChange(HISender pSender, HKEvent pEvent)
{
    const char *ipStr = mEditIPAddress.Text.AsString();
    mOSCInputDevice->SetIPAddress(ipStr);
}

// Called when the device layout is idle
void OSCInputDeviceSampleLayout::GetStatusMessages(HISender pSender, HKEvent pEvent)
{
    // Update the device & socket message label with the latest message
    mLabelDeviceMessage.Caption = std::string("== Device Message ==\n\n" + mOSCInputDevice->GetDeviceMessage()).c_str();
    mLabelSocketStatus.Caption = std::string("== Socket Status ==\n\n" + mOSCInputDevice->PassSocketStatus()).c_str();
}