#pragma once

#include "Pacqets.h"

namespace Networking{

	class IClient {

	public:
		virtual void EntityUpdateCallback(EntityUpdate info) = 0;
		virtual void DeathCallback(Death info) = 0;
		virtual void KillCallback(Kill info) = 0;

	};

}
