#pragma once

#ifdef _WINDOWS
#include <Windows.h>

using FILE_HANDLE = HANDLE;
#define TcFileOpen(fileName,readWrite,sharing,atts)			CreateFileW(fileName.GetConstantBuffer().getBuffer(), readWrite, sharing, nullptr, atts, FILE_ATTRIBUTE_NORMAL, nullptr)


#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))

using FILE_HANDLE = int 
#define DWORD ssize_t 
#define TcFileOpen(fileName,readWrite,sharing,atts)			open(fileName.GetRegString().c_str(), readWrite | sharing | atts, S_IRGRP)


#endif

void TcFileRead(FILE_HANDLE fileName, UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead);
void TcFileWrite(FILE_HANDLE fileName, const UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead);
void TcFileClose(FILE_HANDLE& fileName);