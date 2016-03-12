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

void PS3EventClient::grabDevice()
{
    if ( ioctl(M_handler, EVIOCGRAB, 1) )
	M_hasGrabed = false;
    else
    	M_hasGrabed = true;
}

void PS3EventClient::releaseDevice()
{
    ioctl(M_handler, EVIOCGRAB, 0);
}

void PS3EventClient::readEvent()
{
    struct input_event e;
    unsigned short flag;
    unsigned short code;
    unsigned short amout;
    int rc = read(M_handler, &e, sizeof(e));
    if ( rc > 0 )
    {
        switch ( e.type ) {
        case 1:
            switch ( e.value ) {
            case 0:
                flag = BTN_UP;
                break;
            case 1:
                flag = BTN_DOWN;
                break;
            default:
                break;
            }
            code = e.code - 287;
            SendButton(code, M_keymap.c_str(), flag);
            break;
        // case 3:
        //     flag = BTN_USE_AMOUNT | BTN_AXIS;
        //     switch ( e.code ) {
        //     case 0:
        //         code = 1;
        //         amount = (e.value + 127.0)/127*65
        //         SendButton(code, M_keymap.c_str(), flag, e.value);
        //         break;
        //     case 1:
        //         code = 2;
        //         SendButton(code, M_keymap.c_str(), flag, e.value);
        //         break;
        //     case 2:
        //         code = 3;
        //         SendButton(code, M_keymap.c_str(), flag, e.value);
        //         break;
        //     case 3:
        //         code = 4;
        //         SendButton(code, M_keymap.c_str(), flag, e.value);
        //         break;
        //     default:
        //         break;
        //     }
        //     break;
        default:
            break;
        }
    }
}
