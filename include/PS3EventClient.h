#ifndef __PS3EVENTCLIENT_H__
#define __PS3EVENTCLIENT_H__

#include <fcntl.h> /* O_RDONLY O_NONBLOCKING */
#include <linux/input.h>

#include "xbmcclient.h"

// use udev rule to symlink
#define HANDLER "/dev/ps3controller"

class PS3EventClient : public CXBMCClient
{
private:
    std::string M_handlerName;
    int M_handler;
    std::string M_controllerName;
    std::string M_keymap;
    bool M_hasGrabed;

public:
    PS3EventClient(const char *IP = "127.0.0.1", int Port = 9777, int Socket = -1, unsigned int UID = 0)
        : CXBMCClient(IP, Port, Socket, UID)
    {
	M_handlerName = HANDLER;
	M_handler = open( M_handlerName.c_str(), O_RDONLY | O_NONBLOCK );
	M_hasGrabed = false;
	char name[128];
	if (ioctl(M_handler, EVIOCGNAME(sizeof(name)), name) < 0)
	    strncpy(name, "Unknown", sizeof(name));
	M_controllerName = std::string(name);
	M_keymap = "JS0:" + M_controllerName;
    }
    std::string handlerName() const { return M_handlerName; }
    void setHandlerName( std::string s ) { M_handlerName = s; }
    std::string controllerName() const { return M_controllerName; }
    std::string keymap() const { return M_keymap; }
    void setKeymap( std::string s ) { M_keymap = s; }
    bool hasGrabed() const { return M_hasGrabed; }
    
    void grabDevice();
    void releaseDevice();
    void readEvent();
};

#endif
