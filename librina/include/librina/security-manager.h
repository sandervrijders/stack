/*
 * Security Manager
 *
 *    Eduard Grasa          <eduard.grasa@i2cat.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */

#ifndef LIBRINA_CACEP_H
#define LIBRINA_CACEP_H

#ifdef __cplusplus

#include "librina/application.h"
#include "librina/rib.h"
#include "librina/internal-events.h"
#include "librina/timer.h"

namespace rina {

//Contains data related to a particular security context
class ISecurityContext {
public:
	ISecurityContext(int id_) : id(id_) { };
	virtual ~ISecurityContext() { };

	int id;
};

class IAuthPolicySet : public IPolicySet {
public:
	enum AuthStatus {
		IN_PROGRESS, SUCCESSFULL, FAILED
	};

	static const std::string AUTH_NONE;
	static const std::string AUTH_PASSWORD;
	static const std::string AUTH_SSHRSA;
	static const std::string AUTH_SSHDSA;

	static const std::string cdapTypeToString(CDAPMessage::AuthTypes type);
	static CDAPMessage::AuthTypes stringToCDAPType(const std::string& type);

	IAuthPolicySet(CDAPMessage::AuthTypes type_);
	virtual ~IAuthPolicySet() { };

	/// get credentials of application process required for authentication
	virtual AuthValue get_my_credentials(int session_id) = 0;

	/// initiate the authentication of a remote AE. Any values originated
	/// from authentication such as sesion keys will be stored in the
	/// corresponding security context
	virtual AuthStatus initiate_authentication(AuthValue credentials, int session_id) = 0;

	/// Process an incoming CDAP message
	virtual int process_incoming_message(const CDAPMessage& message, int session_id) = 0;

	// The type of authentication policy
	std::string type;
};

class AuthNonePolicySet : public IAuthPolicySet {
public:
	AuthNonePolicySet() : IAuthPolicySet(rina::CDAPMessage::AUTH_NONE) { };
	virtual ~AuthNonePolicySet() { };
	rina::AuthValue get_my_credentials(int session_id);
	AuthStatus initiate_authentication(rina::AuthValue credentials, int session_id);
	int process_incoming_message(const CDAPMessage& message, int session_id);
	int set_policy_set_param(const std::string& name,
	                         const std::string& value);
};

class AuthPasswordPolicySet;

class CancelPasswdAuthTimerTask : public TimerTask {
public:
	CancelPasswdAuthTimerTask(AuthPasswordPolicySet * ps_,
			int session_id_) : ps(ps_),
			session_id(session_id_) { };
	~CancelPasswdAuthTimerTask() throw() { };
	void run();

	AuthPasswordPolicySet * ps;
	int session_id;
};

class AuthPasswordSessionInformation {
public:
	AuthPasswordSessionInformation(CancelPasswdAuthTimerTask * task,
			std::string * chall) : timer_task(task),
			challenge(chall) { };
	~AuthPasswordSessionInformation() {
		if (challenge) {
			delete challenge;
		}
	};

	// Owned by a timer
	CancelPasswdAuthTimerTask * timer_task;
	std::string * challenge;
};

/// As defined in PRISTINE's D4.1, online at
/// https://wiki.ict-pristine.eu/wp4/d41/Authentication-mechanisms#The-AuthNPassword-Authentication-Mechanism
class AuthPasswordPolicySet : public IAuthPolicySet {
public:
	static const std::string PASSWORD;
	static const std::string CHALLENGE_REQUEST;
	static const std::string CHALLENGE_REPLY;
	static const std::string DEFAULT_CIPHER;
	static const int DEFAULT_TIMEOUT;

	AuthPasswordPolicySet(const std::string password_,
			int challenge_length_, IRIBDaemon * ribd);
	rina::AuthValue get_my_credentials(int session_id);
	AuthStatus initiate_authentication(rina::AuthValue credentials, int session_id);
	int process_incoming_message(const CDAPMessage& message, int session_id);
	int set_policy_set_param(const std::string& name,
	                         const std::string& value);
	void remove_session_info(int session_id);

private:
	std::string * generate_random_challenge();
	std::string encrypt_challenge(const std::string& challenge);
	std::string decrypt_challenge(const std::string& encrypted_challenge);
	int process_challenge_request(const std::string& challenge,
			 	      int session_id);
	int process_challenge_reply(const std::string& encrypted_challenge,
			 	    int session_id);

	std::string password;
	int challenge_length;
	IRIBDaemon * rib_daemon;
	std::string cipher;
	ThreadSafeMapOfPointers<int, AuthPasswordSessionInformation> pending_sessions;
	Timer timer;
	int timeout;
	Lockable lock;
};

class ISecurityManager: public ApplicationEntity, public InternalEventListener {
public:
	ISecurityManager() : rina::ApplicationEntity(SECURITY_MANAGER_AE_NAME) { };
        virtual ~ISecurityManager();
        int add_auth_policy_set(const std::string& auth_type);
        int set_policy_set_param(const std::string& path,
                                 const std::string& name,
                                 const std::string& value);
        IAuthPolicySet * get_auth_policy_set(const std::string& auth_type);
        ISecurityContext * get_security_context(int context_id);
        void eventHappened(InternalEvent * event);

private:
        /// The authentication policy sets, by type
        ThreadSafeMapOfPointers<std::string, IAuthPolicySet> auth_policy_sets;

        /// The security contexts, by session-id (the port-id of the flow
        /// used for enrollment=
        ThreadSafeMapOfPointers<int, ISecurityContext> security_contexts;
};

}

#endif

#endif
