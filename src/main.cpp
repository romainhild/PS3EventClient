#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "PS3EventClient.h"

#define PID_FILE "/var/run/ps3event_clientd.pid"
#define LOG_FILE "/var/log/ps3event_clientd.log"

int main()
{
    std::ofstream fp;
    pid_t process_id = 0;
    pid_t sid = 0;

    // Create child process
    process_id = fork();
    // Indication of fork() failure
    if (process_id < 0)
    	exit(1);

    // PARENT PROCESS. Need to write the child pid and kill itself.
    if (process_id > 0)
    {
	fp.open( PID_FILE );
	fp << process_id << std::endl;
	fp.close();
    	exit(0);
    }

    //unmask the file mode
    umask(0);
    //set new session
    sid = setsid();
    if(sid < 0)
    	exit(1);

    // Change the current working directory to root.
    chdir("/");
    // Close stdin. stdout and stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Open a log file in write mode.
    fp.open ( LOG_FILE );

    PS3EventClient client;
    do
    {
	fp << "tempting to grab " << client.controllerName() << " at " << client.handlerName() << " ... ";
	client.grabDevice();
	fp << strerror(errno) << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(10));
    } while ( !client.hasGrabed() );

    std::string msg = "PS3EventClient use " + client.keymap();
    client.SendLOG( LOGINFO, msg.c_str());

    while (1)
    {
    	client.readEvent();
    	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    client.releaseDevice();
    fp << "stop daemon" << std::endl;
    fp.close();

    return (0);
}
