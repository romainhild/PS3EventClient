/*
 * This file is part of the libCEC(R) library.
 *
 * libCEC(R) is Copyright (C) 2011-2015 Pulse-Eight Limited.  All rights reserved.
 * libCEC(R) is an original work, containing original code.
 *
 * libCEC(R) is a trademark of Pulse-Eight Limited.
 *
 * This program is dual-licensed; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 *
 *
 * Alternatively, you can license this library under a commercial license,
 * please contact Pulse-Eight Licensing for more information.
 *
 * For more information contact:
 * Pulse-Eight Licensing       <license@pulse-eight.com>
 *     http://www.pulse-eight.com/
 *     http://www.pulse-eight.net/
 */

#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <signal.h>
#include <stdlib.h>
#include <boost/format.hpp>
#include <p8-platform/os.h>
#include <p8-platform/util/StringUtils.h>
#include <p8-platform/threads/threads.h>
#include <libcec/cec.h>
#include <libcec/cectypes.h>

using namespace CEC;
using namespace P8PLATFORM;

#include <libcec/cecloader.h>

#ifdef UNUSED
#elif defined(__GNUC__)
#define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
#define UNUSED(x) /**/ x
#else
#define UNUSED(x) x
#endif

ICECCallbacks        g_callbacks;
libcec_configuration g_config;
int                  g_cecLogLevel(3);
std::ofstream        g_logOutput;
std::string          g_strPort;
ICECAdapter*         g_parser;

int CecLogMessage(void *UNUSED(cbParam), const cec_log_message message)
{
  if ((message.level & g_cecLogLevel) == message.level)
  {
    std::string strLevel;
    switch (message.level)
    {
    case CEC_LOG_ERROR:
      strLevel = "ERROR:   ";
      break;
    case CEC_LOG_WARNING:
      strLevel = "WARNING: ";
      break;
    case CEC_LOG_NOTICE:
      strLevel = "NOTICE:  ";
      break;
    case CEC_LOG_TRAFFIC:
      strLevel = "TRAFFIC: ";
      break;
    case CEC_LOG_DEBUG:
      strLevel = "DEBUG:   ";
      break;
    default:
      break;
    }

    if (g_logOutput.is_open())
    {
        g_logOutput << boost::format( "%1%[%2%]\t%3%" ) % strLevel.c_str() % message.time % message.message << std::endl;
    }
  }

  return 0;
}

int CecKeyPress(void *UNUSED(cbParam), const cec_keypress UNUSED(key))
{
  return 0;
}

int CecCommand(void *UNUSED(cbParam), const cec_command UNUSED(command))
{
  return 0;
}

int CecAlert(void *UNUSED(cbParam), const libcec_alert type, const libcec_parameter UNUSED(param))
{
  return 0;
}
 
int main (int argc, char *argv[])
{
    pid_t process_id = 0;
    pid_t sid = 0;
    // Create child process
    process_id = fork();
    // Indication of fork() failure
    if (process_id < 0)
    	exit(1);

    // PARENT PROCESS. Need to write the child pid and kill itself.
    if (process_id > 0)
        exit(0);

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

    g_config.Clear();
    g_callbacks.Clear();
    snprintf(g_config.strDeviceName, 13, "CECTurn");
    g_config.clientVersion       = LIBCEC_VERSION_CURRENT;
    g_config.bActivateSource     = 0;
    g_callbacks.CBCecLogMessage  = &CecLogMessage;
    g_callbacks.CBCecKeyPress    = &CecKeyPress;
    g_callbacks.CBCecCommand     = &CecCommand;
    g_callbacks.CBCecAlert       = &CecAlert;
    g_config.callbacks           = &g_callbacks;
    g_logOutput.open("/var/log/cecTurn.log");
    g_config.deviceTypes.Add(CEC_DEVICE_TYPE_RECORDING_DEVICE);

    g_logOutput << "pre init" << std::endl;
    g_parser = LibCecInitialise(&g_config);
    g_logOutput << "post init" << std::endl;
    if (!g_parser)
    {
        g_logOutput << "Cannot load libcec.so" << std::endl;

        if (g_parser)
            UnloadLibCec(g_parser);

        return 1;
    }

    // init video on targets that need this
    g_logOutput << "pre init video" << std::endl;
    g_parser->InitVideoStandalone();
    g_logOutput << "post init video" << std::endl;

    if (g_strPort.empty())
    {
        cec_adapter devices[10];
        uint8_t iDevicesFound = g_parser->FindAdapters(devices, 10, NULL);
        if (iDevicesFound <= 0)
        {
            UnloadLibCec(g_parser);
            return 1;
        }
        else
        {
            g_strPort = devices[0].comm;
        }
    }

    g_logOutput << "opening a connection to the CEC adapter..." << std::endl;

    if (!g_parser->Open(g_strPort.c_str()))
    {
        g_logOutput << boost::format("unable to open the device on port %s") %  g_strPort.c_str() << std::endl;
        UnloadLibCec(g_parser);
        return 1;
    }

    if ( argc < 2 )
        g_logOutput << "trying to launch without argument" << std::endl;
    else
    {
        if ( strcmp(argv[1], "start") == 0 )
            g_parser->PowerOnDevices();
        else if ( strcmp(argv[1], "stop") == 0 )
            g_parser->StandbyDevices();
    }

    g_parser->Close();
    UnloadLibCec(g_parser);

    if (g_logOutput.is_open())
        g_logOutput.close();

    return 0;
}
