#include <fcntl.h> /* O_RDONLY O_NONBLOCKING */
#include <sys/ioctl.h> /* ioctl */
#include <unistd.h>

#include "PS3EventClient.h"

/* Map :
type 0 not used

button type 1 value 0 released 1 pressed
select : 288
L3 : 289
R3 : 290
start : 291
dpad up : 292
dpad right : 293
dpad down : 294
dpad left : 295
L2 : 296
R2 : 297
L1 : 298
L2 : 299
triangle : 300
rond : 301
cross : 302
square : 303
ps : 304

stick type : 3 value [-127,127]
left horizontal : 0
left vertical: 1
right horizontal : 2
right vertical : 3
*/

PS3EventClient::PS3EventClient( )
{
    M_out.open( "/home/xbian/ps3.out");
    M_handlerName = "/dev/input/event3";
    M_handler = open( M_handlerName.c_str(), O_RDONLY | O_NONBLOCK );
    M_hasGrabed = false;
    char name[128];
    if (ioctl(M_handler, EVIOCGNAME(sizeof(name)), name) < 0)
    	strncpy(name, "Unknown", sizeof(name));
    M_controllerName = std::string(name);
    M_keymap = "JS0:" + M_controllerName;
}

void PS3EventClient::grabDevice()
{
    if ( ioctl(M_handler, EVIOCGRAB, 1) )
	M_hasGrabed = false;
    else
    	M_hasGrabed = true;
    M_out << "has grabed " << M_hasGrabed << " error : " << strerror(errno) << std::endl;
}

void PS3EventClient::releaseDevice()
{
    ioctl(M_handler, EVIOCGRAB, 0);
}

int PS3EventClient::connectToKodi()
{
    M_addr; // Address => localhost on 9777
    M_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (M_sockfd < 0)
    	return -1;

    M_addr.Bind(M_sockfd);

    CPacketHELO HeloPackage("PS3EventClient", ICON_NONE);
    HeloPackage.Send(M_sockfd, M_addr);
    M_out << "say hello" << std::endl;
    return 0;
}

void PS3EventClient::readEvent()
{
    struct input_event e;
    int rc = read(M_handler, &e, sizeof(e));
    if ( e.type == 1 )
	M_out << "read event of type " << e.type << " for code " << e.code << " and value " << e.value << std::endl;
    processEvent( e );
    M_button.Send(M_sockfd, M_addr);
}

void PS3EventClient::processEvent( input_event e )
{
    unsigned short flag;
    switch ( e.type ) {
	case 1:
	    switch ( e.value ) {
		case 0:
		    flag = 0x04;
		case 1:
		    flag = 0x02;
		default:
		    break;
		}
	    M_button = CPacketBUTTON( (int)e.code, M_keymap.c_str(), flag );
	default:
	    break;
	}
}
