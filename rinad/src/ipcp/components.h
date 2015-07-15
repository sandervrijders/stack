/*
 * Common interfaces and constants of the IPC Process components
 *
 *    Bernat Gaston         <bernat.gaston@i2cat.net>
 *    Eduard Grasa          <eduard.grasa@i2cat.net>
 *    Francesco Salvestrini <f.salvestrini@nextworks.it>
 *    Vincenzo Maffione <v.maffione@nextworks.it>
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

#ifndef IPCP_COMPONENTS_HH
#define IPCP_COMPONENTS_HH

#ifdef __cplusplus

#include <list>
#include <vector>
#include <string>

#include <librina/ipc-process.h>
#include <librina/internal-events.h>
#include <librina/irm.h>
#include <librina/security-manager.h>

#include "common/encoder.h"

namespace rinad {

const unsigned int max_sdu_size_in_bytes = 10000;

enum IPCProcessOperationalState {
	NOT_INITIALIZED,
	INITIALIZED,
	ASSIGN_TO_DIF_IN_PROCESS,
	ASSIGNED_TO_DIF
};

class IPCProcess;

/// IPC process component
class IPCProcessComponent {
public:
        IPCProcessComponent() : ipcp(NULL) { };
        virtual ~IPCProcessComponent() { };
        virtual void set_dif_configuration(const rina::DIFConfiguration& dif_configuration) = 0;

        IPCProcess * ipcp;
};

/// Interface
/// Delimits and undelimits SDUs, to allow multiple SDUs to be concatenated in the same PDU
class IDelimiter
{
public:
  virtual ~IDelimiter() { };

  /// Takes a single rawSdu and produces a single delimited byte array, consisting in
  /// [length][sdu]
  /// @param rawSdus
  /// @return
  virtual char* getDelimitedSdu(char rawSdu[]) = 0;

  /// Takes a list of raw sdus and produces a single delimited byte array, consisting in
  /// the concatenation of the sdus followed by their encoded length: [length][sdu][length][sdu] ...
  /// @param rawSdus
  /// @return
  virtual char* getDelimitedSdus(const std::list<char*>& rawSdus) = 0;

  /// Assumes that the first length bytes of the "byteArray" are an encoded varint (encoding an integer of 32 bytes max), and returns the value
  /// of this varint. If the byteArray is still not a complete varint, doesn't start with a varint or it is encoding an
  /// integer of more than 4 bytes the function will return -1.
  ///  @param byteArray
  /// @return the value of the integer encoded as a varint, or -1 if there is not a valid encoded varint32, or -2 if
  ///this may be a complete varint32 but still more bytes are needed
  virtual int readVarint32(char byteArray[],
                           int  length) = 0;

  /// Takes a delimited byte array ([length][sdu][length][sdu] ..) and extracts the sdus
  /// @param delimitedSdus
  /// @return
  virtual std::list<char*>& getRawSdus(char delimitedSdus[]) = 0;
};

class IEnrollmentStateMachine;

/// Interface that must be implementing by classes that provide
/// the behavior of an enrollment task
class IPCPEnrollmentTask : public IPCProcessComponent,
			   public rina::IEnrollmentTask {
public:
	IPCPEnrollmentTask() : IEnrollmentTask() { };
	virtual ~IPCPEnrollmentTask(){};
	virtual IEnrollmentStateMachine * getEnrollmentStateMachine(int portId, bool remove) = 0;
	virtual void deallocateFlow(int portId) = 0;
	virtual void add_enrollment_state_machine(int portId, IEnrollmentStateMachine * stateMachine) = 0;
};

/// Policy set of the IPCP enrollment task
class IPCPEnrollmentTaskPS : public rina::IPolicySet {
public:
        virtual ~IPCPEnrollmentTaskPS() {};
        virtual void connect_received(const rina::CDAPMessage& cdapMessage,
        			      rina::CDAPSessionDescriptor * session_descriptor) = 0;
        virtual void connect_response_received(int result,
        				       const std::string& result_reason,
        				       rina::CDAPSessionDescriptor * session_descriptor) = 0;
        virtual void process_authentication_message(const rina::CDAPMessage& message,
        					    rina::CDAPSessionDescriptor * session_descriptor) = 0;
        virtual void initiate_enrollment(const rina::NMinusOneFlowAllocatedEvent & event,
        				 rina::EnrollmentRequest * request) = 0;
        virtual void inform_ipcm_about_failure(IEnrollmentStateMachine * state_machine) = 0;
        virtual void set_dif_configuration(const rina::DIFConfiguration& dif_configuration) = 0;
};

/// The object that contains all the information
/// that is required to initiate an enrollment
/// request (send as the objectvalue of a CDAP M_START
/// message, as specified by the Enrollment spec)
class EnrollmentInformationRequest {
public:
	EnrollmentInformationRequest() : address_(0),
		allowed_to_start_early_(false) {};

	/// The address of the IPC Process that requests
	///to join a DIF
	unsigned int address_;
	std::list<rina::ApplicationProcessNamingInformation> supporting_difs_;
	bool allowed_to_start_early_;
};

/// Encapsulates all the information required to manage a Flow
class Flow {
public:
	enum IPCPFlowState {
		EMPTY,
		ALLOCATION_IN_PROGRESS,
		ALLOCATED,
		WAITING_2_MPL_BEFORE_TEARING_DOWN,
		DEALLOCATED
	};

	Flow();
	Flow(const Flow& flow);
	~Flow();
	rina::Connection * getActiveConnection();
	std::string toString();

	/// The application that requested the flow
	rina::ApplicationProcessNamingInformation source_naming_info;

	/// The destination application of the flow
	rina::ApplicationProcessNamingInformation destination_naming_info;

	/// The port-id returned to the Application process that requested the flow. This port-id is used for
	/// the life of the flow.
	unsigned int source_port_id;

	/// The port-id returned to the destination Application process. This port-id is used for
	// the life of the flow
	unsigned int destination_port_id;

	/// The address of the IPC process that is the source of this flow
	unsigned int source_address;

	/// The address of the IPC process that is the destination of this flow
	unsigned int destination_address;

	/// All the possible connections of this flow
	std::list<rina::Connection*> connections;

	/// The index of the connection that is currently Active in this flow
	unsigned int current_connection_index;

	/// The status of this flow
	IPCPFlowState state;

	/// The list of parameters from the AllocateRequest that generated this flow
	rina::FlowSpecification flow_specification;

	/// TODO this is just a placeHolder for this piece of data
	char* access_control;

	/// Maximum number of retries to create the flow before giving up.
	unsigned int max_create_flow_retries;

	/// The current number of retries
	unsigned int create_flow_retries;

	/// While the search rules that generate the forwarding table should allow for a
	/// natural termination condition, it seems wise to have the means to enforce termination.
	unsigned int hop_count;

	///True if this IPC process is the source of the flow, false otherwise
	bool source;
};

class IFlowAllocatorInstance;

class IFlowAllocatorPs : public rina::IPolicySet {
// This class is used by the IPCP to access the plugin functionalities
public:
        virtual Flow *newFlowRequest(IPCProcess * ipc_process,
                        const rina::FlowRequestEvent& flowRequestEvent) = 0;

        virtual ~IFlowAllocatorPs() {}
};

/// Interface that must be implementing by classes that provide
/// the behavior of a Flow Allocator task
class IFlowAllocator : public IPCProcessComponent, public rina::ApplicationEntity {
public:
	static const std::string FLOW_ALLOCATOR_AE_NAME;

	IFlowAllocator() : rina::ApplicationEntity(FLOW_ALLOCATOR_AE_NAME) { };
	virtual ~IFlowAllocator(){};

	virtual IFlowAllocatorInstance * getFAI(int portId) = 0;

	/// The Flow Allocator is invoked when an Allocate_Request.submit is received.  The source Flow
	/// Allocator determines if the request is well formed.  If not well-formed, an Allocate_Response.deliver
	/// is invoked with the appropriate error code.  If the request is well-formed, a new instance of an
	/// FlowAllocator is created and passed the parameters of this Allocate_Request to handle the allocation.
	/// It is a matter of DIF policy (AllocateNoificationPolicy) whether an Allocate_Request.deliver is invoked
	/// with a status of pending, or whether a response is withheld until an Allocate_Response can be delivered
	/// with a status of success or failure.
	/// @param allocateRequest the characteristics of the flow to be allocated.
	/// to honour the request
	virtual void submitAllocateRequest(rina::FlowRequestEvent& flowRequestEvent) = 0;

	virtual void processCreateConnectionResponseEvent(const rina::CreateConnectionResponseEvent& event) = 0;

	/// Forward the allocate response to the Flow Allocator Instance.
	/// @param portId the portId associated to the allocate response
	/// @param AllocateFlowResponseEvent - the response from the application
	virtual void submitAllocateResponse(const rina::AllocateFlowResponseEvent& event) = 0;

	virtual void processCreateConnectionResultEvent(const rina::CreateConnectionResultEvent& event) = 0;

	virtual void processUpdateConnectionResponseEvent(const rina::UpdateConnectionResponseEvent& event) = 0;

	/// Forward the deallocate request to the Flow Allocator Instance.
	/// @param the flow deallocate request event
	/// @throws IPCException
	virtual void submitDeallocate(const rina::FlowDeallocateRequestEvent& event) = 0;

	/// When an Flow Allocator receives a Create_Request PDU for a Flow object,
	/// it consults its local Directory to see if it has an entry. If there is an
	/// entry and the address is this IPC Process, it creates an FAI and passes
	/// the Create_request to it.If there is an entry and the address is not this IPC
	/// Process, it forwards the Create_Request to the IPC Process designated by the address.
	/// @param cdapMessage
	/// @param underlyingPortId
	virtual void createFlowRequestMessageReceived(Flow * flow, const std::string& object_name,
			int invoke_id) = 0;

	/// Called by the flow allocator instance when it finishes to cleanup the state.
	/// @param portId
	virtual void removeFlowAllocatorInstance(int portId) = 0;

        // Plugin support
	virtual std::list<rina::QoSCube*> getQoSCubes() = 0;
	virtual Flow * createFlow() = 0;
	virtual void destroyFlow(Flow *) = 0;
};

class IRoutingPs : public rina::IPolicySet {
	// This class is used by the IPCP to access the plugin functionalities
public:
	virtual ~IRoutingPs() {};
	virtual void set_dif_configuration(const rina::DIFConfiguration& dif_configuration) = 0;
};

class IRoutingComponent : public IPCProcessComponent, public rina::ApplicationEntity {
public:
	static const std::string ROUTING_COMPONENT_AE_NAME;
	IRoutingComponent() : rina::ApplicationEntity(ROUTING_COMPONENT_AE_NAME) { };
	virtual ~IRoutingComponent(){};
};

class RoutingComponent: public IRoutingComponent {
public:
	RoutingComponent() : IRoutingComponent() { };
	void set_application_process(rina::ApplicationProcess * ap);
	void set_dif_configuration(const rina::DIFConfiguration& dif_configuration);
        ~RoutingComponent() {};
};

/// Namespace Manager Interface
class INamespaceManagerPs : public rina::IPolicySet {
// This class is used by the IPCP to access the plugin functionalities
public:
	/// Decides if a given address is valid or not
	/// @param address
	///	@return true if valid, false otherwise
	virtual bool isValidAddress(unsigned int address, const std::string& ipcp_name,
			const std::string& ipcp_instance) = 0;

	/// Return a valid address for the IPC process that
	/// wants to join the DIF
	virtual unsigned int getValidAddress(const std::string& ipcp_name,
				const std::string& ipcp_instance) = 0;

	virtual ~INamespaceManagerPs() {}
};

class INamespaceManager : public IPCProcessComponent, public rina::ApplicationEntity {
public:
	static const std::string NAMESPACE_MANAGER_AE_NAME;
	INamespaceManager() : rina::ApplicationEntity(NAMESPACE_MANAGER_AE_NAME) { };
	virtual ~INamespaceManager(){};

	/// Returns the address of the IPC process where the application process is, or
	/// null otherwise
	/// @param apNamingInfo
	/// @return
	virtual unsigned int getDFTNextHop(const rina::ApplicationProcessNamingInformation& apNamingInfo) = 0;

	/// Returns the IPC Process id (0 if not an IPC Process) of the registered
	/// application, or -1 if the app is not registered
	/// @param apNamingInfo
	/// @return
	virtual unsigned short getRegIPCProcessId(const rina::ApplicationProcessNamingInformation& apNamingInfo) = 0;

	/// Add an entry to the directory forwarding table
	/// @param entry
	virtual void addDFTEntry(rina::DirectoryForwardingTableEntry * entry) = 0;

	/// Get an entry from the application name
	/// @param apNamingInfo
	/// @return
	virtual rina::DirectoryForwardingTableEntry * getDFTEntry(
			const rina::ApplicationProcessNamingInformation& apNamingInfo) = 0;

	/// Remove an entry from the directory forwarding table
	/// @param apNamingInfo
	virtual void removeDFTEntry(const rina::ApplicationProcessNamingInformation& apNamingInfo) = 0;

	/// Process an application registration request
	/// @param event
	virtual void processApplicationRegistrationRequestEvent(
			const rina::ApplicationRegistrationRequestEvent& event) = 0;

	/// Process an application unregistration request
	/// @param event
	virtual void processApplicationUnregistrationRequestEvent(
			const rina::ApplicationUnregistrationRequestEvent& event) = 0;

	virtual unsigned int getAdressByname(const rina::ApplicationProcessNamingInformation& name) = 0;
};

///N-1 Flow Manager interface
class INMinusOneFlowManager : public rina::IPCResourceManager {
public:
	INMinusOneFlowManager() : rina::IPCResourceManager(true) { };
	virtual ~INMinusOneFlowManager(){};

	virtual void set_ipc_process(IPCProcess * ipc_process) = 0;

	virtual void set_dif_configuration(const rina::DIFConfiguration& dif_configuration) = 0;

	/// The IPC Process has been unregistered from or registered to an N-1 DIF
	/// @param evet
	/// @throws IPCException
	virtual void processRegistrationNotification(const rina::IPCProcessDIFRegistrationEvent& event) = 0;

	virtual std::list<int> getNMinusOneFlowsToNeighbour(unsigned int address) = 0;

	virtual int getManagementFlowToNeighbour(unsigned int address) = 0;

	virtual unsigned int numberOfFlowsToNeighbour(const std::string& apn,
			const std::string& api) = 0;
};

/// PDUFT Generator Policy Set Interface
class IPDUFTGeneratorPs : public rina::IPolicySet {
// This class is used by the IPCP to access the plugin functionalities
public:
	/// The routing table has been updated; decide if
	/// the PDU Forwarding Table has to be updated and do it
	///	@return true if valid, false otherwise
	virtual void routingTableUpdated(const std::list<rina::RoutingTableEntry*>& routing_table) = 0;

	virtual ~IPDUFTGeneratorPs() {}
};

/// Resource Allocator Interface
/// The Resource Allocator (RA) is the core of management in the IPC Process.
/// The degree of decentralization depends on the policies and how it is used. The RA has a set of meters
/// and dials that it can manipulate. The meters fall in 3 categories:
/// 	Traffic characteristics from the user of the DIF
/// 	Traffic characteristics of incoming and outgoing flows
/// 	Information from other members of the DIF
/// The Dials:
///     Creation/Deletion of QoS Classes
///     Data Transfer QoS Sets
///     Modifying Data Transfer Policy Parameters
///     Creation/Deletion of RMT Queues
///     Modify RMT Queue Servicing
///     Creation/Deletion of (N-1)-flows
///     Assignment of RMT Queues to (N-1)-flows
///     Forwarding Table Generator Output
class IResourceAllocator: public IPCProcessComponent, public rina::ApplicationEntity {
public:
	static const std::string RESOURCE_ALLOCATOR_AE_NAME;
	static const std::string PDUFT_GEN_COMPONENT_NAME;

	IResourceAllocator() : rina::ApplicationEntity(RESOURCE_ALLOCATOR_AE_NAME),
			pduft_gen_ps(NULL){ };
	virtual ~IResourceAllocator(){};
	virtual INMinusOneFlowManager * get_n_minus_one_flow_manager() const = 0;
	int set_pduft_gen_policy_set(const std::string& name);

	IPDUFTGeneratorPs * pduft_gen_ps;
};

/// Security Management � A DIF requires three security functions:
///  1) Authentication to ensure that an IPC-Process wishing to join the DIF is who it
///  says it is and is an allowable member of the DIF (Enrollment);
///  2) Confidentiality and integrity of all PDUs; and
///  3) Access control to determine whether application processes requesting an IPC flow
///  with a remote application has the necessary permissions to establish
///  communication.
/// DAF Management performs authentication of new members, as well as key management
/// and other security management functions required for the security measures. Access
/// Control is performed by the Flow Allocator. The particular security procedures used for
/// these security functions are a matter of policy. SDU Protection provides confidentiality
/// and integrity
class ISecurityManagerPs : public rina::IPolicySet {
// This class is used by the IPCP to access the plugin functionalities
public:
	/// Decide if an IPC Process is allowed to join a DIF
	virtual bool isAllowedToJoinDIF(const rina::Neighbor& newMember) = 0;

	/// Decide if a new flow to the IPC process should be accepted
	virtual bool acceptFlow(const Flow& newFlow) = 0;

        virtual ~ISecurityManagerPs() {}
};

class IPCPSecurityManager: public rina::ISecurityManager, public IPCProcessComponent {
// Used by IPCP to access the functionalities of the security manager
public:
	IPCPSecurityManager(){ };
	void set_application_process(rina::ApplicationProcess * ap);
	void set_dif_configuration(const rina::DIFConfiguration& dif_configuration);
	~IPCPSecurityManager() {};
};

class IPCPRIBDaemon;

/// Base RIB Object. API for the create/delete/read/write/start/stop RIB
/// functionality for certain objects (identified by objectNames)
class BaseIPCPRIBObject: public rina::BaseRIBObject {
public:
	virtual ~BaseIPCPRIBObject(){};
	BaseIPCPRIBObject(IPCProcess* ipc_process,
                      const std::string& object_class,
                      long object_instance,
                      const std::string& object_name);

	IPCProcess * ipc_process_;
	IPCPRIBDaemon * rib_daemon_;
};

/// Interface that provides the RIB Daemon API
class IPCPRIBDaemon : public rina::RIBDaemon, public IPCProcessComponent {
public:
	IPCPRIBDaemon() : wmpi(0) { };
	virtual ~IPCPRIBDaemon(){};

	rina::WireMessageProviderInterface *wmpi;

	/// Process a Query RIB Request from the IPC Manager
	/// @param event
	virtual void processQueryRIBRequestEvent(const rina::QueryRIBRequestEvent& event) = 0;
	virtual void generateCDAPResponse(int invoke_id,
			rina::CDAPSessionDescriptor * cdapSessDescr,
			rina::CDAPMessage::Opcode opcode,
			const std::string& obj_class,
			const std::string& obj_name,
			rina::RIBObjectValue& robject_value) = 0;
};

/// IPC Process interface
class IPCProcess : public rina::ApplicationProcess {
public:
	static const std::string MANAGEMENT_AE;
	static const std::string DATA_TRANSFER_AE;
	static const int DEFAULT_MAX_SDU_SIZE_IN_BYTES;

	IDelimiter * delimiter_;
	rina::IMasterEncoder * encoder_;
	rina::CDAPSessionManagerInterface* cdap_session_manager_;
	rina::InternalEventManager * internal_event_manager_;
	IPCPEnrollmentTask * enrollment_task_;
	IFlowAllocator * flow_allocator_;
	INamespaceManager * namespace_manager_;
	IResourceAllocator * resource_allocator_;
	IPCPSecurityManager * security_manager_;
	IRoutingComponent * routing_component_;
	IPCPRIBDaemon * rib_daemon_;

	IPCProcess(const std::string& name, const std::string& instance);
	virtual ~IPCProcess(){};
	virtual unsigned short get_id() = 0;
	virtual void set_address(unsigned int address) = 0;
	virtual const IPCProcessOperationalState& get_operational_state() const = 0;
	virtual void set_operational_state(const IPCProcessOperationalState& operational_state) = 0;
	virtual const rina::DIFInformation& get_dif_information() const = 0;
	virtual void set_dif_information(const rina::DIFInformation& dif_information) = 0;
	virtual const std::list<rina::Neighbor*> get_neighbors() const = 0;
};

/// A simple RIB object that just acts as a wrapper. Represents an object in the RIB that just
/// can be read or written, and whose read/write operations have no side effects other than
/// updating the value of the object
class SimpleIPCPRIBObject: public BaseIPCPRIBObject {
public:
        SimpleIPCPRIBObject(IPCProcess* ipc_process,
                        const std::string& object_class,
			const std::string& object_name,
                        const void* object_value);
	virtual const void* get_value() const;
	virtual void        writeObject(const void* object);

	/// Create has the semantics of update
	virtual void createObject(const std::string& objectClass,
                                  const std::string& objectName,
                                  const void* objectValue);

private:
	const void* object_value_;
};

/// Class SimpleSetRIBObject. A RIB object that is a set and has no side effects
class SimpleSetIPCPRIBObject: public SimpleIPCPRIBObject {
public:
        SimpleSetIPCPRIBObject(IPCProcess * ipc_process,
                           const std::string& object_class,
                           const std::string& set_member_object_class,
                           const std::string& object_name);
	void createObject(const std::string& objectClass,
                          const std::string& objectName,
                          const void* objectValue);

private:
	std::string set_member_object_class_;
};

/// Class SimpleSetMemberRIBObject. A RIB object that is member of a set
class SimpleSetMemberIPCPRIBObject: public SimpleIPCPRIBObject {
public:
        SimpleSetMemberIPCPRIBObject(IPCProcess* ipc_process,
                                 const std::string& object_class,
                                 const std::string& object_name,
                                 const void* object_value);
	virtual void deleteObject(const void* objectValue);
};

class IPCMCDAPSessDesc : public rina::CDAPSessionDescriptor {
public:
	IPCMCDAPSessDesc(unsigned int seqnum) : rina::CDAPSessionDescriptor(),
						 req_seqnum(seqnum) { }
	unsigned int req_seqnum;
};

}

#endif

#endif
