/*
 * IPC Manager
 *
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

#ifndef __DIF_VALIDATOR_H__
#define __DIF_VALIDATOR_H__

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <utility>

#include <librina/common.h>
#include <librina/ipc-manager.h>
#include <librina/patterns.h>

#include "rina-configuration.h"

namespace rinad{

//
// @brief Validates the DIF configurations
//
// FIXME: Convert this into an abstract class and make all the subtypes
// derive from the DIF abstract class (e.g. shimEth, ShimHV..)
//
class DIFConfigValidator {
public:
        enum Types{
                NORMAL,
                SHIM_ETH,
                SHIM_DUMMY,
                SHIM_TCP_UDP,
                SHIM_HV,
                SHIM_NOT_DEFINED
        };
        DIFConfigValidator(const rina::DIFConfiguration &dif_config,
                        const rina::DIFInformation &dif_info, std::string type);
        bool validateConfigs();
private:
        Types type_;
        const rina::DIFConfiguration &dif_config_;
        const rina::DIFInformation &dif_info_;

	bool validateShimEth();
	bool validateShimHv();
	bool validateShimDummy();
        bool validateShimTcpUdp();
        bool validateNormal();
	bool validateBasicDIFConfigs();
	bool validateConfigParameters(const std::vector< std::string >&
                                      expected_params);
	bool dataTransferConstants();
	bool qosCubes();
	bool knownIPCProcessAddresses();
	bool pdufTableGeneratorConfiguration();
};


}//rinad namespace

#endif  /* __DIF_VALIDATOR_H__ */
