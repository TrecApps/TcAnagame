#pragma once

#ifdef _WINDOWS
#define ANAGAME_CODE_EX __declspec(dllexport)
#else
#define ANAGAME_CODE_EX
#endif // _WINDOWS