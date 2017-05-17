#pragma once

#include "Pacqets.h"

namespace Networking {

	class IServer {
	public:
		virtual void UserConnectedCallback(uchar id) = 0;
		virtual void UserInputCallback(KeyPress info, uchar id) = 0;
		virtual void SomeoneLeftCallback(uchar id) = 0;

	};

}
