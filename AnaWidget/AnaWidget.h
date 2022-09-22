#pragma once
#ifdef _WINDOWS

#ifndef _ANA_WIDGET
#define _ANA_WIDGET __declspec(dllexport)
#else
#define _ANA_WIDGET __declspec(dllimport)
#endif

#else
#define _ANA_WIDGET
#endif