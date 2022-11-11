#pragma once
#ifdef _WINDOWS

#ifndef _ANA_CODE
#define _ANA_CODE __declspec(dllexport)
#else
#define _ANA_CODE __declspec(dllimport)
#endif

#else
#define _ANA_CODE
#endif