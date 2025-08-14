#ifndef __OSCINPUTDEVICE_SAMPLE_WINSOCKET_H__
#define __OSCINPUTDEVICE_SAMPLE_WINSOCKET_H__

#include <string>

#include <fbsdk/fbsdk.h>

// Windows Sockets API
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class OSCInputDeviceSampleDevice; // Forward declaration

struct BoneTransform
{
    FBVector3d position;   // Translation vector
    FBQuaternion rotation; // Rotation quaternion
};

class OSCInputDeviceSampleWinSocket
{
public:
    // Constructor & Destructor
    OSCInputDeviceSampleWinSocket();
    ~OSCInputDeviceSampleWinSocket();

    // Socket management
    bool CreateSampleSocket();
    bool BindSocket(int port, unsigned int ipAddr);
    void CloseSampleSocket();

    std::string GetSocketStatus() const { return mSocketStatus; }    // Get the status
    BoneTransform *PassData();                                       // Pass the data to the device
    int ReceiveData();                                               // Receive data from the socket
    void SetupDeviceConnection(OSCInputDeviceSampleDevice *pParent); // Embed this hardware in the plugin device

private:
    OSCInputDeviceSampleDevice *mOSCInputDevice = nullptr; // Handle to the device

    WSADATA wsadata;
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    std::string mSocketStatus; // Socket status message

    // Received data
    BoneTransform mData;
};

#endif /* __OSCINPUTDEVICE_SAMPLE_WINSOCKET_H__ */