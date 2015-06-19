#include <ipcp_obj.h>

#define RINA_PREFIX "ipcm.mad.ribd_v1"
#include <librina/logs.h>
#include <librina/exceptions.h>

#include "../agent.h"
#include "../../../ipcm.h"

namespace rinad {
namespace mad {
namespace rib_v1 {

//Instance generator
extern Singleton<rina::ConsecutiveUnsignedIntegerGenerator> inst_gen;

//Static class names
const std::string IPCPObj::class_name = "IPCProcess";

//Class
IPCPObj::IPCPObj(std::string name, long instance, int ipcp_id)
	: RIBObject<mad_manager::structures::ipcp_t>(class_name, instance, name,
				(mad_manager::structures::ipcp_t*) NULL,
				&encoder),
				processID_(ipcp_id){

}

IPCPObj::IPCPObj(std::string name, long instance,
		const rina::cdap_rib::SerializedObject &object_value)
	: RIBObject<mad_manager::structures::ipcp_t>(class_name, instance, name,
					(mad_manager::structures::ipcp_t*) NULL,
					&encoder){
        (void) object_value;
}

rina::cdap_rib::res_info_t* IPCPObj::remoteRead(
				const std::string& name,
				rina::cdap_rib::SerializedObject &obj_reply){

	(void) name;
	rina::cdap_rib::res_info_t* r = new rina::cdap_rib::res_info_t;
	r->result_ = 0;

	mad_manager::structures::ipcp_t info;
	info.process_id = processID_;
	info.name = IPCManager->get_ipcp_name(processID_);
	//TODO: Add missing stuff...

	encoder_->encode(info, obj_reply);
	return r;
}

//We only support deletion
rina::cdap_rib::res_info_t* IPCPObj::remoteDelete(const std::string& name){

	(void) name;
	rina::cdap_rib::res_info_t* r = new rina::cdap_rib::res_info_t;

	//Fill in the response
	r->result_ = 0;

	//Call the IPCManager and return
	if (IPCManager->destroy_ipcp(ManagementAgent::inst, processID_) != IPCM_SUCCESS) {
		LOG_ERR("Unable to destroy IPCP with id %d", processID_);
		r->result_ = -1;
	}

	return r;
}
};//namespace rib_v1
};//namespace mad
};//namespace rinad
