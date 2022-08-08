#pragma once

#ifdef _WINDOWS

#ifndef _TC_GRAPH
#define _TC_GRAPH __declspec(dllexport)
#else
#define _TC_GRAPH __declspec(dllimport)
#endif

#else
#define _TC_GRAPH
#endif
