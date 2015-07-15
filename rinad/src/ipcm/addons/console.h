/*
 * IPC Manager console
 *
 *    Vincenzo Maffione     <v.maffione@nextworks.it>
 *    Eduard Grasa          <eduard.grasa@i2cat.net>
 *    Marc Sune             <marc.sune (at) bisdn.de>
 *
 * This program is free software; you can redistribute it and/or modify
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __IPCM_CONSOLE_H__
#define __IPCM_CONSOLE_H__

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sstream>

#include <librina/concurrency.h>
#include <librina/common.h>
#include <librina/ipc-manager.h>

#include "rina-configuration.h"
#include "../addon.h"


namespace rinad {

class IPCMConsole : public Addon {
                static const unsigned int CMDBUFSIZE = 120;
                static const int CMDRETCONT = 0;
                static const int CMDRETSTOP = 1;

                typedef int (IPCMConsole::*ConsoleCmdFunction)
                            (std::vector<std::string>& args);

                struct ConsoleCmdInfo {
                        ConsoleCmdFunction fun;
                        const char *usage;

                        ConsoleCmdInfo() : fun(NULL), usage(NULL) { }
                        ConsoleCmdInfo(ConsoleCmdFunction f, const char *u)
                                                        : fun(f), usage(u) { }
                };

                rina::Thread *worker;

                std::map<std::string, ConsoleCmdInfo> commands_map;
                std::ostringstream outstream;

                int init(void);
                int process_command(int cfd, char *cmdbuf, int size);
                int flush_output(int cfd);
                int plugin_load_unload(std::vector<std::string>& args,
                                       bool load);

                // Console commands functions
                int quit(std::vector<std::string>& args);
                int help(std::vector<std::string>& args);
                int create_ipcp(std::vector<std::string>& args);
                int destroy_ipcp(std::vector<std::string>& args);
                int list_ipcps(std::vector<std::string>& args);
                int list_ipcp_types(std::vector<std::string>& args);
                int query_rib(std::vector<std::string>& args);
                int assign_to_dif(std::vector<std::string>& args);
                int register_at_dif(std::vector<std::string>& args);
                int unregister_from_dif(std::vector<std::string>& args);
                int update_dif_config(std::vector<std::string>& args);
                int enroll_to_dif(std::vector<std::string>& args);
                int select_policy_set(std::vector<std::string>& args);
                int set_policy_set_param(std::vector<std::string>& args);
                int plugin_load(std::vector<std::string>& args);
                int plugin_unload(std::vector<std::string>& args);
                int plugin_get_info(std::vector<std::string>& args);
                int show_dif_templates(std::vector<std::string>& args);
                int read_ipcp_ribobj(std::vector<std::string>& args);
                int show_catalog(std::vector<std::string>& args);

        public:
                IPCMConsole(const unsigned int port);
                void body();
                virtual ~IPCMConsole() throw();
		static const std::string NAME;
	private:
		//Local console port
		const unsigned int port;
};

}
#endif  /* __IPCM_CONSOLE_H__ */
