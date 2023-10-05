#pragma once
#include <string>
#include <signal.h>

#include "Utils/Dark.h"

#define ASSERT(x) if(!x) raise(SIGABRT)

#define GLEC(x) clearGLErrors();\
				x;\
				ASSERT(displayGLErrors(#x, DARK_FILE, DARK_LINE));

extern void clearGLErrors();
extern bool displayGLErrors(const char* functionName, const char* fileName, int line);

namespace Dark
{
	extern void error(const std::string& errorMessage, const char* file, int line);
}