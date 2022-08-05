
#include "FileFunctions.hpp"

#ifdef _WINDOWS

void TcFileRead(FILE_HANDLE fileName, UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead)
{
	if (!ReadFile(fileName, bytes, sizeToRead, &bytesRead, nullptr))
		bytesRead = -1;
}

void TcFileWrite(FILE_HANDLE fileName, const UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead)
{
	if (!WriteFile(fileName, bytes, sizeToRead, &bytesRead, nullptr))
		bytesRead = -1;
}

void TcFileClose(FILE_HANDLE& fileName)
{
	if (fileName)
		CloseHandle(fileName);
	fileName = nullptr;
}


#elif defined(__linux__) || (defined (__APPLE__) && defined (__MACH__))
void TcFileRead(FILE_HANDLE fileName, UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead)
{
	bytesRead = read(fileName, bytes, sizeToRead);
}
void TcFileWrite(FILE_HANDLE fileName, UCHAR* bytes, DWORD sizeToRead, DWORD& bytesRead)
{
	bytesRead = write(fileName, bytes, sizeToRead);
}

void TcFileClose(FILE_HANDLE& fileName)
{
	if (fileName > 2)
		close(fileName);
	fileName = -1;
}
#endif