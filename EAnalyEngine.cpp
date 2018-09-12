#include "stdafx.h"
#include "MainWindow.h"
#include "EAnalyEngine.h"

extern CMainWindow *pMaindlg;


EAnalysis::EAnalysis(ULONG dwVBase, ULONG dwVSize)
{
	text_dwBase = dwVBase;
	text_dwSize = dwVSize;
	textSection = NULL;
	rdataSection = NULL;
}

EAnalysis::~EAnalysis()
{
	if (textSection != NULL)
	{
		VirtualFree((LPVOID)textSection, 0, MEM_RELEASE);// free buf
	}
}

BOOL EAnalysis::EStaticLibInit() {    //易语言静态编译 识别初始化

	DWORD	dwResult;
	DWORD	dwCalc;
	_MEMORY_BASIC_INFORMATION MB;

	BYTE SearchCode[17] = { 0x50,0x64,0x89,0x25,0x00,0x00,0x00,0x00,
						  0x81,0xEC,0xAC,0x01,0x00,0x00,0x53,0x56,0x57 };

	textSection = (BYTE *)VirtualAlloc(NULL, text_dwSize, MEM_COMMIT, PAGE_READWRITE); //申请一块代码段空间
	
	if (textSection == NULL)
	{
		pMaindlg->outputInfo("申请代码段内存失败!");
		return FALSE;
	}

	Readmemory(textSection, text_dwBase, text_dwSize, MM_RESILENT);

	dwResult = Search_Bin(textSection, SearchCode, text_dwSize, sizeof(SearchCode));

	if (dwResult == 0)
	{
		return FALSE;
	}

	dwCalc = dwResult + 0x26 + text_dwBase;

	if (GetPoint(T_O2V(dwCalc)) - text_dwBase >text_dwSize)  //在区段外面
	{
		if (!VirtualQueryEx(GethProcess(), (LPCVOID*)GetPoint(T_O2V(dwCalc)), &MB, sizeof(MEMORY_BASIC_INFORMATION))) {
			pMaindlg->outputInfo("查询内存失败!");
			return FALSE;
		}

		rdata_dwBase =(ULONG)MB.BaseAddress;
		rdata_dwSize = MB.RegionSize;
		rdataSection = (BYTE *)VirtualAlloc(NULL, rdata_dwSize, MEM_COMMIT, PAGE_READWRITE);
		if (rdataSection == NULL) {
			pMaindlg->outputInfo("申请输入表段内存失败!");
			return FALSE;
		}

		Readmemory(rdataSection, rdata_dwBase, rdata_dwSize, MM_RESILENT);
	}
	else {
		rdataSection = textSection;
		rdata_dwBase = text_dwBase;
		rdata_dwSize = text_dwSize;
	}

	pEnteyInfo = (PEENTRYINFO)R_O2V(GetPoint(T_O2V(dwCalc)));


	dwUsercodeStart = pEnteyInfo->dwUserCodeStart;

	return TRUE;
}

BOOL EAnalysis::GetUserEntryPoint() {
	BYTE data[21] = {
		0x55, 0x8B, 0xEC, 0x51, 0x53, 0x56, 0x8B, 0xF1, 0x57, 0x8B, 0x4E, 0x68,
		0x8D, 0x86, 0xD8, 0x00, 0x00, 0x00, 0x50, 0x51, 0xE8 };

	DWORD	dwResult;
	DWORD	dwStartEntry;

	dwResult = Search_Bin(textSection, data, text_dwSize, sizeof(data));
	if (dwResult == 0)
	{
		return false;
	}
	dwStartEntry = dwResult + 0x37 + text_dwBase; //call addr
	dwUsercodeEnd = GetPoint(T_O2V(dwStartEntry) + 1) + dwStartEntry + 5;

	return true;
}

HANDLE EAnalysis::GethProcess() {
	return (HANDLE)*(DWORD*)0x4D5A68;
}



DWORD EAnalysis::T_O2V(DWORD dwOaddr)  //实际地址到虚拟地址
{
	return dwOaddr - text_dwBase + (DWORD)textSection;
}
DWORD EAnalysis::T_V2O(DWORD dwVaddr) //虚拟地址到实际地址
{
	return dwVaddr - (DWORD)textSection + text_dwBase;
}
DWORD EAnalysis::R_O2V(DWORD dwOaddr)  //实际地址到虚拟地址
{
	return dwOaddr - rdata_dwBase + (DWORD)rdataSection;
}
DWORD EAnalysis::R_V2O(DWORD dwVaddr) //虚拟地址到实际地址
{
	return dwVaddr - (DWORD)rdataSection + rdata_dwBase;
}
DWORD EAnalysis::GetPoint(DWORD dwAddr)
{
	return *(DWORD*)dwAddr;
}
DWORD EAnalysis::R_GetOriginPoint(DWORD dwAddr)
{
	DWORD	pDwData;
	pDwData = GetPoint(R_O2V(dwAddr));
	return pDwData;
}

DWORD EAnalysis::Search_Bin(byte *pSrc, byte *pTrait, int nSrcLen, int nTraitLen)
{
	if (IsBadReadPtr(pSrc, 4) == TRUE)
	{
		return 0;
	}
	int i, j, k;
	for (i = 0; i <= (nSrcLen - nTraitLen); i++)
	{
		if (pSrc[i] == pTrait[0])
		{
			k = i;
			j = 0;
			while (j < nTraitLen)
			{
				k++; j++;
				if (pTrait[j] == 0x90)
				{
					continue;
				}
				if (pSrc[k] != pTrait[j])
				{
					break;
				}
			}

			if (j == nTraitLen)
			{
				return i;
			}

		}

	}
	return 0;
}
DWORD EAnalysis::Search_BinEx(byte *pSrc, byte *pTrait, int nSrcLen, int nTraitLen) //精确版本,无模糊匹配
{
	if (IsBadReadPtr(pSrc, 4) == TRUE)
	{
		return 0;
	}
	int i, j, k;
	for (i = 0; i <= (nSrcLen - nTraitLen); i++)
	{
		if (pSrc[i] == pTrait[0])
		{
			k = i;
			j = 0;
			while (j < nTraitLen)
			{
				k++; j++;
				if (pSrc[k] != pTrait[j])
				{
					break;
				}
			}

			if (j == nTraitLen)
			{
				return i;
			}

		}

	}
	return 0;
}

DWORD EAnalysis::QuerySearch(HANDLE h_gprocess, byte *pSrc, DWORD StartAddr){

	return 1;
}