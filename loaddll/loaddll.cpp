// loaddll.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "windows.h"
#include "cstring"
using namespace std;

typedef int(*func)(int a, int b, HINSTANCE his);
//#define MAX_PATH 256
//#define MakePtr(cast,ptr,AddValue)(cast)((DWORD)(ptr)+(DWORD)(AddValue))
//#define GetImgDirEntryRVA( pNTHdr, IDE ) \
//	(pNTHdr->OptionalHeader.DataDirectory[IDE].VirtualAddress)
//#define GetImgDirEntrySize( pNTHdr, IDE ) \
//	(pNTHdr->OptionalHeader.DataDirectory[IDE].Size)
//typedef UINT (CALLBACK *LPENTRYPOINT)(HANDLE hinstance, DWORD reason, LPVOID reserved);
//
//
//DWORD LoadPbDllFromMemory(LPVOID lpRawDll, LPVOID &lpImageDll)
//{
//	SYSTEM_INFO sSysInfo;
//	PIMAGE_DOS_HEADER dosHeader;
//	PIMAGE_NT_HEADERS pNTHeader;
//	PIMAGE_SECTION_HEADER section;
//	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
//	PIMAGE_IMPORT_BY_NAME pOrdinalName;
//	PIMAGE_BASE_RELOCATION baseReloc;
//	PDWORD lpLink;
//	unsigned char Protection[4096];
//	HINSTANCE hDll;
//	WORD i;
//	DWORD ImagePages, fOldProtect, j, MaxLen, HdrLen, Addr1, Addr2, Pg, Pg1, Pg2;
//	char *sDllName;
//	PBYTE pImageBase;
//	LPVOID lpImageDll2;
//	LPENTRYPOINT EntryPoint;
//
//	if (NULL == lpRawDll) return 1;
//
//	dosHeader = (PIMAGE_DOS_HEADER)lpRawDll;
//
//	// Is this the MZ header?   
//	if ((TRUE == IsBadReadPtr(dosHeader, sizeof(IMAGE_DOS_HEADER))) ||
//		(IMAGE_DOS_SIGNATURE != dosHeader->e_magic))
//		return 2;
//
//	// Get the PE header.   
//	pNTHeader = MakePtr(PIMAGE_NT_HEADERS, dosHeader, dosHeader->e_lfanew);
//
//	// Is this a real PE image?   
//	if ((TRUE == IsBadReadPtr(pNTHeader, sizeof(IMAGE_NT_HEADERS))) ||
//		(IMAGE_NT_SIGNATURE != pNTHeader->Signature))
//		return 3;
//
//	if ((pNTHeader->FileHeader.SizeOfOptionalHeader !=
//		sizeof(pNTHeader->OptionalHeader)) ||
//		(pNTHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC))
//		return 4;
//
//	if (pNTHeader->FileHeader.NumberOfSections < 1) return 5;
//
//	section = IMAGE_FIRST_SECTION(pNTHeader);
//	int HeaderSize = sizeof(IMAGE_SECTION_HEADER);
//
//	// 节头长度   
//	HdrLen = (DWORD)section - (DWORD)dosHeader +
//		HeaderSize * pNTHeader->FileHeader.NumberOfSections;
//
//	// 找出最大的节的长度,此节一般是代码所在的节(.text 节)   
//	MaxLen = HdrLen;
//	int ii = 0;
//
//	for (i = 0; i<(DWORD)pNTHeader->FileHeader.NumberOfSections; i++)// find MaxLen   
//	{
//		if (MaxLen < section[i].VirtualAddress + section[i].SizeOfRawData)
//		{
//			MaxLen = section[i].VirtualAddress + section[i].SizeOfRawData;
//		}
//		if (strcmp((const char *)section[i].Name, ".rsrc") == 0) ii = i;
//	}
//
//	GetSystemInfo(&sSysInfo);
//	ImagePages = MaxLen / sSysInfo.dwPageSize;
//	if (MaxLen % sSysInfo.dwPageSize) ImagePages++;
//
//	// 分配所需的内存   
//	DWORD NeededMemory = ImagePages * sSysInfo.dwPageSize;
//	lpImageDll = VirtualAlloc(NULL, NeededMemory, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
//
//	if (lpImageDll == NULL) return 6; // 分配内存失败   
//
//	MoveMemory(lpImageDll, lpRawDll, HdrLen); // 复制节头   
//
//	DWORD OrgAddr = 0;
//	DWORD NewAddr = 0;
//	DWORD Size = 0;
//
//	// 复制 .text 节数据   
//	for (i = 0; i<pNTHeader->FileHeader.NumberOfSections; i++)
//	{
//		OrgAddr = (DWORD)lpImageDll + (DWORD)section[i].VirtualAddress;
//		NewAddr = (DWORD)lpRawDll + (DWORD)section[i].PointerToRawData;
//		Size = (DWORD)section[i].SizeOfRawData;
//		MoveMemory((void *)OrgAddr, (void *)NewAddr, Size);
//	}
//
//	// 把指针指向新的 DLL 映像   
//	dosHeader = (PIMAGE_DOS_HEADER)lpImageDll; // Switch to new image   
//	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)dosHeader + dosHeader->e_lfanew);
//	section = (PIMAGE_SECTION_HEADER)((DWORD)pNTHeader + sizeof(IMAGE_NT_HEADERS));
//	pImageBase = (PBYTE)dosHeader;
//
//	if ((ii != 0))// && (IsNT() == TRUE))
//	{
//		section[ii].VirtualAddress = section[ii].VirtualAddress + (DWORD)lpRawDll;
//		section[ii].PointerToRawData = section[ii].PointerToRawData + (DWORD)lpRawDll;
//	}
//
//	DWORD importsStartRVA;
//
//	// Look up where the imports section is (normally in the .idata section)   
//	// but not necessarily so. Therefore, grab the RVA from the data dir.   
//	importsStartRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);
//	if (!importsStartRVA)
//	{
//		VirtualFree(dosHeader, 0, MEM_RELEASE);
//		return 7;
//	}
//
//	pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)pNTHeader->
//		OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
//
//	if (pImportDesc != 0)
//		pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pImportDesc + (DWORD)dosHeader);
//	else
//	{
//		VirtualFree(dosHeader, 0, MEM_RELEASE);
//		return 8;
//	}
//
//	while (1) // 处理各入口表中的 DLL   
//	{
//		// 检查是否遇到了空的 IMAGE_IMPORT_DESCRIPTOR   
//		if ((pImportDesc->TimeDateStamp == 0) && (pImportDesc->Name == 0)) break;
//
//		// 从磁盘载入必须的 Dll,   
//		// 注意,载入的 DLL 是合并的 DLL 的入口表中的 DLL,   
//		// 不是合并到 EXE 中的 DLL   
//		sDllName = (char *)(pImportDesc->Name + (DWORD)pImageBase);
//		cout << sDllName << endl;
//		size_t len = strlen(sDllName) + 1;
//		size_t converted = 0;
//		wchar_t *WStr;
//		WStr = (wchar_t*)malloc(len * sizeof(wchar_t));
//		mbstowcs_s(&converted, WStr, len, sDllName, _TRUNCATE);
//		hDll = GetModuleHandle(WStr);
//
//		if (hDll == 0) 
//			hDll = LoadLibrary(WStr);
//
//		if (hDll == 0)
//		{
//			cout << "Can''t find required Dll"<< "Error in LoadPbDllFromMemory()" << endl;
//			VirtualFree(dosHeader, 0, MEM_RELEASE);
//			return 9;
//		}
//
//		DWORD *lpFuncNameRef = (DWORD *)(pImportDesc->OriginalFirstThunk +
//			(DWORD)dosHeader);
//		DWORD *lpFuncAddr = (DWORD *)(pImportDesc->FirstThunk +
//			(DWORD)dosHeader);
//
//		while (*lpFuncNameRef != 0)
//		{
//			pOrdinalName = (PIMAGE_IMPORT_BY_NAME)(*lpFuncNameRef +
//				(DWORD)dosHeader);
//			DWORD pIMAGE_ORDINAL_FLAG = 0x80000000;
//
//			if (*lpFuncNameRef & pIMAGE_ORDINAL_FLAG)
//				*lpFuncAddr = (DWORD)GetProcAddress(hDll,
//				(const char *)(*lpFuncNameRef & 0xFFFF));
//			else
//				*lpFuncAddr = (DWORD)GetProcAddress(hDll,
//				(const char *)pOrdinalName->Name);
//
//			if (lpFuncAddr == 0)
//			{
//				VirtualFree(dosHeader, 0, MEM_RELEASE);
//				return 10;// Can''t GetProcAddress   
//			}
//
//			lpFuncAddr++;
//			lpFuncNameRef++;
//		}
//		pImportDesc++;
//	}
//
//	DWORD TpOffset;
//	baseReloc = (PIMAGE_BASE_RELOCATION)((DWORD)pNTHeader->
//		OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
//
//	if (baseReloc != 0)
//	{
//		baseReloc = (PIMAGE_BASE_RELOCATION)((DWORD)baseReloc + (DWORD)dosHeader);
//		while (baseReloc->VirtualAddress != 0)
//		{
//			PWORD lpTypeOffset = (PWORD)((DWORD)baseReloc +
//				sizeof(IMAGE_BASE_RELOCATION));
//			while (lpTypeOffset < (PWORD)((DWORD)baseReloc +
//				(DWORD)baseReloc->SizeOfBlock))
//			{
//				TpOffset = *lpTypeOffset & 0xF000;
//				if (TpOffset == 0x3000)
//				{
//					lpLink = (PDWORD)((DWORD)dosHeader +
//						baseReloc->VirtualAddress +
//						(*lpTypeOffset & 0xFFF));
//					*lpLink = (DWORD)dosHeader +
//						(*lpLink) - pNTHeader->OptionalHeader.ImageBase;
//				}
//				else
//				{
//					if (TpOffset != 0)
//					{
//						VirtualFree(dosHeader, 0, MEM_RELEASE);
//						return 10;
//					}
//				}
//				lpTypeOffset++;
//			}
//			baseReloc = (PIMAGE_BASE_RELOCATION)((DWORD)baseReloc +
//				(DWORD)baseReloc->SizeOfBlock);
//		}
//	}
//
//	// 取得原始的内存状态   
//	memset(Protection, 0, 4096);
//	for (i = 0; i <= pNTHeader->FileHeader.NumberOfSections; i++)
//	{
//		if (i == pNTHeader->FileHeader.NumberOfSections)
//		{
//			Addr1 = 0;
//			Addr2 = HdrLen;
//			j = 0x60000000;
//		}
//		else
//		{
//			Addr1 = section[i].VirtualAddress;
//			Addr2 = section[i].SizeOfRawData;
//			j = section[i].Characteristics;
//		}
//		Addr2 += Addr1 - 1;
//
//		Pg1 = Addr1 / sSysInfo.dwPageSize;
//		Pg2 = Addr2 / sSysInfo.dwPageSize;
//		for (Pg = Pg1; Pg <= Pg2; Pg++)
//		{
//			if (j & 0x20000000) Protection[Pg] |= 1; // Execute   
//			if (j & 0x40000000) Protection[Pg] |= 2; // Read   
//			if (j & 0x80000000) Protection[Pg] |= 4; // Write   
//		}
//	}
//
//	// 恢复原始的内存状态   
//	Addr1 = (DWORD)dosHeader;
//	for (Pg = 0; Pg <= ImagePages; Pg++)
//	{
//		switch (Protection[Pg])
//		{
//		case 2:
//			fOldProtect = PAGE_READONLY;
//			break;
//		case 3:
//			fOldProtect = PAGE_EXECUTE_READ;
//			break;
//		case 6:
//			fOldProtect = PAGE_READWRITE;
//			break;
//		default:
//			// Ignore strange combinations  
//			fOldProtect = PAGE_EXECUTE_READWRITE;
//			break;
//		}
//
//		if (fOldProtect != PAGE_EXECUTE_READWRITE)
//		{
//			if (VirtualProtect((void *)Addr1,
//				sSysInfo.dwPageSize,
//				fOldProtect,
//				&fOldProtect) == 0)
//			{
//				VirtualFree(dosHeader, 0, MEM_RELEASE);
//				return 11;
//			}
//		}
//		Addr1 += sSysInfo.dwPageSize;
//	}
//
//	EntryPoint = (LPENTRYPOINT)((DWORD)pNTHeader->OptionalHeader.AddressOfEntryPoint + (DWORD)dosHeader);
//	LPVOID lpReserved = 0;
//	EntryPoint((HINSTANCE)dosHeader, DLL_PROCESS_ATTACH, lpReserved);
//	lpImageDll2 = lpImageDll;
//	return 0;
//}
//
//PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD rva, PIMAGE_NT_HEADERS pNTHeader)
//{
//	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
//	unsigned i;
//
//	for (i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++)
//	{
//		DWORD size = section->Misc.VirtualSize;
//		if (0 == size) size = section->SizeOfRawData;
//		if ((rva >= section->VirtualAddress) && (rva < (section->VirtualAddress + size))) return section;
//	}
//	return 0;
//}
//
//LPVOID GetPtrFromRVA(DWORD rva, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase)
//{
//	PIMAGE_SECTION_HEADER pSectionHdr;
//	INT delta;
//
//	pSectionHdr = GetEnclosingSectionHeader(rva, pNTHeader);
//	if (!pSectionHdr) return 0;
//
//	delta = (INT)(pSectionHdr->VirtualAddress - pSectionHdr->PointerToRawData);
//	return (PVOID)(imageBase + rva - delta);
//}
//
//
//DWORD GetProcAddressDirectly(PIMAGE_DOS_HEADER dosHeader, char * FuncName)
//{
//	PIMAGE_NT_HEADERS pNTHeader;
//	PIMAGE_EXPORT_DIRECTORY pExportDir;
//	PWORD lpNameOrdinals;
//	LPDWORD lpFunctions;
//	DWORD * lpName;
//	char * lpExpFuncName;
//	DWORD i;
//	DWORD j;
//	char * lpFuncName;
//	PBYTE pImageBase;
//	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) return 0;
//
//	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)dosHeader + dosHeader->e_lfanew);
//
//	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE) return 0;
//
//	if ((pNTHeader->FileHeader.SizeOfOptionalHeader != sizeof(pNTHeader->OptionalHeader)) ||
//		(pNTHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC))
//		return 0;
//
//	DWORD exportsStartRVA, exportsEndRVA;
//	pImageBase = (PBYTE)dosHeader;
//
//	// Make pointers to 32 and 64 bit versions of the header.    
//	pNTHeader = MakePtr(PIMAGE_NT_HEADERS, dosHeader, dosHeader->e_lfanew);
//
//	exportsStartRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_EXPORT);
//	exportsEndRVA = exportsStartRVA +
//		GetImgDirEntrySize(pNTHeader, IMAGE_DIRECTORY_ENTRY_EXPORT);
//
//	// Get the IMAGE_SECTION_HEADER that contains the exports. This is    
//	// usually the .edata section, but doesn''t have to be.    
//	PIMAGE_SECTION_HEADER header;
//	header = GetEnclosingSectionHeader(exportsStartRVA, pNTHeader);
//	if (!header) return 0;
//
//	INT delta;
//	delta = (INT)(header->VirtualAddress - header->PointerToRawData);
//	pExportDir = (PIMAGE_EXPORT_DIRECTORY)GetPtrFromRVA(exportsStartRVA,
//		pNTHeader, pImageBase);
//
//
//	pExportDir = (PIMAGE_EXPORT_DIRECTORY)(pNTHeader->
//		OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
//
//	if (pExportDir == 0)
//	{
//		cout << "Error in GetProcAddressDirectly()" << endl;
//		return 0;
//	}
//
//	pExportDir = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pExportDir + (DWORD)dosHeader);
//	lpNameOrdinals = (PWORD)((DWORD)pExportDir->AddressOfNameOrdinals + (DWORD)dosHeader);
//	lpName = (LPDWORD)(pExportDir->AddressOfNames + (DWORD)dosHeader);
//	lpFunctions = (LPDWORD)(pExportDir->AddressOfFunctions + (DWORD)dosHeader);
//	lpFuncName = FuncName;
//
//	if (HIWORD(lpFuncName) != 0)
//	{
//		for (i = 0; i <= pExportDir->NumberOfFunctions - 1; i++)
//		{
//			DWORD entryPointRVA = *lpFunctions;
//
//			// Skip over gaps in exported function    
//			if (entryPointRVA == 0) continue;
//			for (j = 0; j <= pExportDir->NumberOfNames - 1; j++)
//			{
//				if (lpNameOrdinals[j] == i)
//				{
//					lpExpFuncName = (char *)(lpName[j] +
//						(DWORD)dosHeader);
//					if (strcmp((char *)lpExpFuncName, (char *)FuncName) == 0)
//						return (DWORD)(lpFunctions[i] +
//						(DWORD)dosHeader);
//				}
//			}
//		}
//	}
//	else
//	{
//		for (i = 0; i <= pExportDir->NumberOfFunctions - 1; i++)
//		{
//			if (lpFuncName == (char *)(pExportDir->Base + i))
//			{
//				if (lpFunctions[i]) return (unsigned long)(lpFunctions[i] +
//					dosHeader);
//			}
//		}
//	}
//	return 0;
//}
//
//
//LPVOID hinstace2;

int main()
{
	int a = 1;
	int b = 2;
	//HINSTANCE hInstance = NULL;
	//LPWSTR lib_name = (wchar_t *)malloc(sizeof(wchar_t)*MAX_PATH);
	//GetModuleFileName(hInstance, lib_name, MAX_PATH);
	//HMODULE hLibrary;
	//HRSRC hRes;
	//HGLOBAL hResourceLoaded;
	//LPVOID sRawDll;


	//size_t len = wcslen(lib_name) + 1;
	//size_t converted = 0;
	//char *CStr;
	//CStr = (char*)malloc(len * sizeof(char));
	//wcstombs_s(&converted, CStr, len, lib_name, _TRUNCATE);
	//cout << CStr << endl;
	//hLibrary = LoadLibrary(lib_name);

	//if (NULL != hLibrary)
	//{  
	//	cout << hLibrary << endl;
	//	hRes = FindResource(hLibrary, MAKEINTRESOURCE(IDR_MYTYPE1), L"MYTYPE");
	//	if (NULL != hRes)
	//	{
	//		hResourceLoaded = LoadResource(hLibrary, hRes);
	//		if (NULL != hResourceLoaded)
	//		{
	//			SizeofResource(hLibrary, hRes); 
	//			sRawDll = (LPVOID)LockResource(hResourceLoaded);
	//			cout << sRawDll << endl;
	//			cout << LoadPbDllFromMemory(sRawDll,hinstace2) << endl;
	//			cout << hinstace2 << endl;
	//			func add = (func)GetProcAddressDirectly((PIMAGE_DOS_HEADER)hinstace2, "fun");
	//			cout << add(1, 2) << endl;

	//		}
	//	}
	//	else
	//		cout << "error 1" << endl;
	//	FreeLibrary(hLibrary);
	//}
	//else
	//	cout << "error 2" << endl;


	HINSTANCE his = LoadLibraryA("loadfromMemory.dll");
	func add = (func)GetProcAddress(his, "fun");
	
	cout << add(4, 6, his) << endl;
	getchar();
    return 0;
}

