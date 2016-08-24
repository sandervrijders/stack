#include "confm.h"
#include "agent.h"

#define RINA_PREFIX "ipcm.mad.conf"
#include <librina/logs.h>
#include <librina/common.h>
#include <debug.h>

#include <librina/json/json.h>

#include <fstream>
#include <sstream>

namespace rinad{
namespace mad{

//Singleton instance
Singleton<ConfManager> ConfManager;

//Public constructor destructor
ConfManager::ConfManager(const rinad::RINAConfiguration& config){

	Json::Reader    reader;
	Json::Value     root;
        Json::Value     mad_conf;
	std::ifstream   fin;

	fin.open(config.configuration_file.c_str());
	if (fin.fail()) {
		LOG_ERR("Failed to open config file");
		return;
	}

	if (!reader.parse(fin, root, false)) {
		std::stringstream ss;

		ss << "Failed to parse JSON configuration" << std::endl
			<< reader.getFormatedErrorMessages() << std::endl;
		FLUSH_LOG(ERR, ss);

		return;
	}

	fin.close();

        // Set default values for MAD configuration variables
        app_name = rina::ApplicationProcessNamingInformation("rina.apps.mad",
                                                             "1");

        // Parse MAD configuration from JSON, overwriting default configuration
        // variables
        mad_conf = root["addons"]["mad"];
        if (mad_conf != 0) {
                Json::Value nms_difs_conf = mad_conf["NMSDIFs"];
                Json::Value mad_conns_conf = mad_conf["managerConnections"];
                std::string app_name_enc;

                app_name_enc = mad_conf.get("managerAppName",
                                        app_name_enc).asString();
                if (app_name_enc != std::string()) {
                        app_name = rina::decode_apnameinfo(app_name_enc);
                }

                if (mad_conns_conf != 0) {
                        for (unsigned i = 0; i< mad_conns_conf.size(); i++) {
                                ManagerConnInfo mci;
                                std::string app_name_enc;
                                std::string dif;

                                app_name_enc = mad_conns_conf[i]
                                               .get("managerAppName",
                                               app_name_enc).asString();
                                dif = mad_conns_conf[i].get("DIF", dif)
                                                       .asString();

                                mci.manager_name =
                                        rina::decode_apnameinfo(app_name_enc);
                                mci.manager_dif = dif;
                                manager_connections.push_back(mci);
                        }
                }
        }

        LOG_INFO("MAD application name will be %s",
                 app_name.toString().c_str());

        for (std::list<std::string>::iterator it = nms_difs.begin();
                                        it != nms_difs.end(); it++) {
                LOG_INFO("MAD NMS DIF %s", it->c_str());
        }

        for (std::list<ManagerConnInfo>::iterator
                        it = manager_connections.begin();
                                it != manager_connections.end(); it++) {
                LOG_INFO("MAD Manager connection Name=%s DIF=%s",
                                it->manager_name.toString().c_str(),
                                it->manager_dif.c_str());
        }

	LOG_DBG("Initialized");
}

ConfManager::~ConfManager(){
}

//Module configuration routine
void ConfManager::configure(ManagementAgent& agent){

	//Configure the AP name and instance ID
	rina::ApplicationProcessNamingInformation info("rina.apps.mad", "1");
	agent.setAPInfo(app_name);

	//Configure Manager connections
        for (std::list<ManagerConnInfo>::iterator
                        it = manager_connections.begin();
                                it != manager_connections.end(); it++) {
                AppConnection ap_con;

                ap_con.flow_info.remoteAppName = it->manager_name;
                ap_con.flow_info.difName =
                        rina::ApplicationProcessNamingInformation(
                                        it->manager_dif, std::string());

	        agent.addManagerConnection(ap_con);
        }

	//TODO
}

}; //namespace mad
}
;
//namespace rinad
