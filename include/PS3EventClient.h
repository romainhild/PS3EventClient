#ifndef __PS3EVENTCLIENT_H__
#define __PS3EVENTCLIENT_H__

#include <linux/input.h>

#include "xbmcclient.h"

class PS3EventClient
{
private:
    std::ofstream M_out;
    std::string M_handlerName;
    int M_handler;
    std::string M_controllerName;
    std::string M_keymap;
    CAddress M_addr;
    int M_sockfd;
    struct input_event M_event;
    CPacketBUTTON M_button;
    bool M_hasGrabed;

public:
    PS3EventClient();
    std::string handlerName() const { return M_handlerName; }
    void setHandlerName( std::string s ) { M_handlerName = s; }
    std::string controllerName() const { return M_controllerName; }
    std::string keymap() const { return M_keymap; }
    void setKeymap( std::string s ) { M_keymap = s; }
    void setAddr( CAddress s ) { M_addr = s; }
    bool hasGrabed() const { return M_hasGrabed; }
    
    void grabDevice();
    void releaseDevice();
    int connectToKodi();
    void readEvent();
    void processEvent( input_event );
};

#endif
