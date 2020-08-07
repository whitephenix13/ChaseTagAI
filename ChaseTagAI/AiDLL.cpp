#include "pch.h"
#include "AiDLL.h"

//extern "C" is necessary to export the reference from our functions to the DLL
extern "C" {
	bool Negate(bool val) {
		return !val;
	}
}