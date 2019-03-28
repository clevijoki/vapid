#pragma once
#include "BufOps.h"
#include "DebugLog.h"
#include "NetworkUtil.h"
#include <thread>

#include "GeneratedCommon.h"
#include "GeneratedMaster.inl"

namespace master
{

	// start the listener thread
	void RunServer(SOCKET listener_socket){

		RunServerListener("Vapid Monitor", listener_socket, 
			[](SOCKET connection_socket, const SOCKADDR* /*connection_addr*/)
			{
				while (master::Update(connection_socket));
			}
		);
	}


}