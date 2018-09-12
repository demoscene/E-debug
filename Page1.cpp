// Page1.cpp : 实现文件
//

#include "stdafx.h"
#include "E-Debug.h"
#include "afxdialogex.h"
#include "Page1.h"
#include "EAnalyEngine.h"
#include "Elib.h"
#include "MainWindow.h"
#include <vector>

// CPage1 对话框
extern  EAnalysis	*pEAnalysisEngine;
extern 	CMainWindow *pMaindlg;

char DIRECTORY[MAX_PATH];
static int addrtype;
static int nametype;

IMPLEMENT_DYNAMIC(CPage1, CDialog)

CPage1::CPage1(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PAGE1, pParent)
{
	
}

CPage1::~CPage1()
{
	m_map.clear();
}

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTLib, m_lib);
	DDX_Control(pDX, IDC_LISTCommand, m_command);
}


BEGIN_MESSAGE_MAP(CPage1, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_LISTLib, &CPage1::OnNMClickListlib)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTCommand, &CPage1::OnLvnColumnclickListcommand)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCommand, &CPage1::OnNMDblclkListcommand)
	ON_NOTIFY(NM_RCLICK, IDC_LISTCommand, &CPage1::OnNMRClickListcommand)
	ON_COMMAND(ID_32771, &CPage1::On32771)
END_MESSAGE_MAP()


// CPage1 消息处理程序
BOOL CPage1::OnInitDialog() {
	CDialog::OnInitDialog();
	
	LONG lStyle;

	lStyle = GetWindowLong(m_lib.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_lib.m_hWnd, GWL_STYLE, lStyle);//设置style

	lStyle = GetWindowLong(m_command.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_command.m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = m_lib.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
									//dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	m_lib.SetExtendedStyle(dwStyle); //设置扩展风格

	dwStyle = m_command.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	m_command.SetExtendedStyle(dwStyle); //设置扩展风格


	m_lib.InsertColumn(0, _T("支持库信息"), LVCFMT_LEFT, 250);

	m_command.InsertColumn(0, L"地址", LVCFMT_LEFT, 65);
	m_command.InsertColumn(1, L"命令名称", LVCFMT_CENTER, 205);

	int			nPos = 0;
	DWORD		pFirst = pEAnalysisEngine->pEnteyInfo->pLibEntey;

	PLIB_INFO	pLibInfo = NULL;
	CString		strLib, strGuid;
	CString		str;

	for (int i = 0; i < pEAnalysisEngine->pEnteyInfo->dwLibNum; i++)  //对于解析出来的每个支持库
	{
		pLibInfo = (PLIB_INFO)pEAnalysisEngine->R_O2V(pEAnalysisEngine->R_GetOriginPoint(pFirst));
		strLib.Format(L"---->%s (Ver:%1d.%1d)",
			(CString)(char*)pEAnalysisEngine->R_O2V((DWORD)pLibInfo->m_szName),
			pLibInfo->m_nMajorVersion,
			pLibInfo->m_nMinorVersion);
		strGuid.Format(L"        %s", (CString)(char*)pEAnalysisEngine->R_O2V((DWORD)pLibInfo->m_szGuid));

		m_lib.InsertItem(nPos, strLib); nPos++;   //显示Lib名称(Ver:版本号)
		m_lib.InsertItem(nPos, strGuid); nPos++;  //显示Lib的GUID

		str.Empty();
		str.Format(L"   -> 调用命令 (总数:%d)", pLibInfo->m_nCmdCount); //显示调用命令总数

		int	nCountCmd = 0;    // MAP中的命令个数
		DWORD		pFunc = pEAnalysisEngine->R_O2V((DWORD)pLibInfo->m_pCmdsFunc);
		DWORD		dwAddress;

		char szLibVer[12] = { 0 };
		wsprintfA(szLibVer, "\\%1d.%1d", pLibInfo->m_nMajorVersion, pLibInfo->m_nMinorVersion);

		char szDirectory[MAX_PATH] = {};
		StrCpyA(szDirectory, DIRECTORY);
		
		strcat_s(szDirectory, "\\Plugin\\EMap\\");strcat_s(szDirectory, (char*)pEAnalysisEngine->R_O2V((DWORD)pLibInfo->m_szGuid));
		strcat_s(szDirectory, szLibVer);strcat_s(szDirectory, ".Emap");
		
		PESTATICLIBOPCODE pMap = ReadMap(szDirectory, &nCountCmd); //返回MAP中的命令个数
		LIBMAP m_Libmap;

		m_Libmap.Command_addr.clear();
		m_Libmap.Command_name.clear();
		if (pMap == NULL) {    //如果读取不到MAP文件
			for (int n = 0;n < pLibInfo->m_nCmdCount;n++) {
				dwAddress = pEAnalysisEngine->GetPoint(pFunc);
				m_Libmap.Command_addr.push_back(dwAddress);
				m_Libmap.Command_name.push_back("Emap Not Founded");
				pFunc += sizeof(int);
			}
		}
		else {
			for (int n = 0;n < pLibInfo->m_nCmdCount;n++) {     //对于程序中的每个命令,进行一次精确匹配
				dwAddress = pEAnalysisEngine->GetPoint(pFunc);
				m_Libmap.Command_addr.push_back(dwAddress);
				PESTATICLIBOPCODE pTmpMap = pMap;
				BOOL bMatchCom = false;
				for (int i = 0;i < nCountCmd;i++) {    //精确匹配 
					if (MatchCode((unsigned char*)pEAnalysisEngine->T_O2V(dwAddress), (unsigned char*)pTmpMap->m_opcode, pTmpMap->m_size))
					{
						if (pTmpMap->m_CallType == 0) {
							pTmpMap->m_CallType = -1;//不再引用
							bMatchCom = true;
							break;
						}
						else if (pTmpMap->m_CallType == 1) {
							unsigned long SecondFuncAddr = *(long*)(pEAnalysisEngine->T_O2V(dwAddress) + pTmpMap->m_CallOffset + 1) + pEAnalysisEngine->T_O2V(dwAddress) + pTmpMap->m_CallOffset + 5;
							if (MatchCode((unsigned char*)SecondFuncAddr, (unsigned char*)pTmpMap->m_opcode2, pTmpMap->m_size2)) {
								pTmpMap->m_CallType = -1;
								bMatchCom = true;
								break;
							}
						}
						else if (pTmpMap->m_CallType == 2) {
							char IAT[256] = { 0 };
							unsigned long IATFuncAddr = *(long*)(pEAnalysisEngine->T_O2V(dwAddress) + pTmpMap->m_CallOffset + 2);
							char Func[MAX_ESIZE];
							strcpy(Func, pTmpMap->m_IATEAT);
							char *IATFunc = strtok(Func, "||");
							char *EATFunc = strtok(NULL, "||");
							if (Findname(IATFuncAddr, NM_IMPORT, IAT) != 0 && strcmp(IAT, IATFunc) == 0) {    //IAT探寻尝试
								pTmpMap->m_CallType = -1;
								bMatchCom = true;
								break;
							}
							else if (Findname(*(long*)pEAnalysisEngine->R_O2V(IATFuncAddr), NM_EXPORT, IAT) != 0 && strcmp(IAT, EATFunc) == 0) { //EAT探寻尝试	
								pTmpMap->m_CallType = -1;
								bMatchCom = true;
								break;
							}
						}
						else if (pTmpMap->m_CallType == 3) {
							unsigned long JmpAddr = *(long*)(pEAnalysisEngine->T_O2V(dwAddress + pTmpMap->m_size + 1)) + pEAnalysisEngine->T_O2V(dwAddress) + pTmpMap->m_size + 5;
							if (MatchCode((unsigned char*)JmpAddr, (unsigned char*)pTmpMap->m_opcode2, pTmpMap->m_size2)) {
								pTmpMap->m_CallType = -1;
								bMatchCom = true;
								break;
							}
						}
					}
					pTmpMap++;
				}
			if (!bMatchCom)   //对于匹配失败的命令,进行相似度匹配
			{
				m_Libmap.Command_name.push_back("Error");
				Insertname(dwAddress, NM_LABEL, "未知命令");
			}
			else
			{
				m_Libmap.Command_name.push_back(pTmpMap->m_CommandName);
				Insertname(dwAddress, NM_LABEL, pTmpMap->m_CommandName);
			}
			pFunc += sizeof(int);
		}
	}

		USES_CONVERSION;
		pFunc = pEAnalysisEngine->R_O2V((DWORD)pLibInfo->m_pCmdsFunc);  //精确匹配结束后,再进行一次模糊匹配
		for (int m = 0;m < m_Libmap.Command_name.size();m++) {
			dwAddress = pEAnalysisEngine->GetPoint(pFunc);
			PESTATICLIBOPCODE pTmpMap = pMap;
			if (m_Libmap.Command_name[m] == "Error") {      //匹配失败
				for (int n = 0;n < nCountCmd;n++) {
					if (MatchCode_UnEx((unsigned char*)pEAnalysisEngine->T_O2V(dwAddress), (unsigned char*)pTmpMap->m_opcode, pTmpMap->m_size)) {
						if (pTmpMap->m_CallType == 0) {
							pTmpMap->m_CallType = -1;
							strcat(pTmpMap->m_CommandName, "(模糊)");
							m_Libmap.Command_name[m] = pTmpMap->m_CommandName;
							Insertname(dwAddress, NM_LABEL, pTmpMap->m_CommandName);
							break;
						}
						else if (pTmpMap->m_CallType == 1 || pTmpMap->m_CallType==2) {
							unsigned long SecondFuncAddr = *(long*)(pEAnalysisEngine->T_O2V(dwAddress) + pTmpMap->m_CallOffset + 1) + pEAnalysisEngine->T_O2V(dwAddress) + pTmpMap->m_CallOffset + 5;
							if (MatchCode_UnEx((unsigned char*)SecondFuncAddr, (unsigned char*)pTmpMap->m_opcode2, pTmpMap->m_size2)) {
								pTmpMap->m_CallType = -1;
								strcat(pTmpMap->m_CommandName, "(模糊)");
								m_Libmap.Command_name[m] = pTmpMap->m_CommandName;								
								Insertname(dwAddress, NM_LABEL, pTmpMap->m_CommandName);
								break;
							}
						}
					}
					pTmpMap++;
				}
			}
			pFunc += sizeof(int);
		}

		if (pMap) {
			free(pMap);
		}
		
		m_map.insert(map<int,LIBMAP>::value_type(nPos,m_Libmap));
		m_lib.InsertItem(nPos, str);nPos++;
		m_lib.InsertItem(nPos, L"============================="); nPos++;
		pFirst += sizeof(DWORD);
	}

	pMaindlg->outputInfo("->  分析易语言<KrnlLibCmd>&&<LibCmd>完毕...");
	return true;
}


bool CPage1::MatchCode(unsigned char* pSrc1, unsigned char* pSrc2, int nLen)
{
	if (nLen == 0)
	{
		return FALSE;
	}
	for (int i = 0; i < nLen; i++)
	{
		if (pSrc2[i] == 0x90)//模糊匹配
			continue;
		if (pSrc1[i] != pSrc2[i])
			return FALSE;
	}
	return TRUE;
}

bool CPage1::MatchCode_UnEx(unsigned char* pSrc1, unsigned char* pSrc2, int nLen)
{
	float Count = 0;
	if (nLen == 0)
	{
		return FALSE;
	}
	for (int i = 0; i < nLen; i++)
	{
		if (pSrc2[i] == 0x90)//模糊匹配
		{
			Count++;
			continue;
		}
		if (pSrc1[i] != pSrc2[i])
		{
			continue;
		}
		Count++;
	}
	if (Count / (float)nLen > 0.75f) {   
		return TRUE;
	}
	else {
		return false;
	}
}

PESTATICLIBOPCODE CPage1::ReadMap(const char *lpMapPath, int *nCount)
{
	HANDLE hFile = CreateFileA(lpMapPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		*nCount = 0;
		return NULL;
	}
	DWORD	dwHitSize = 0;
	DWORD	dwSize = GetFileSize(hFile, &dwHitSize);
	DWORD	dwReadSize;
	*nCount = dwSize / sizeof(ESTATICLIBOPCODE);//得到命令个数
	PESTATICLIBOPCODE pMap = (PESTATICLIBOPCODE)calloc(dwSize, sizeof(char));
	ReadFile(hFile, pMap, dwSize, &dwReadSize, NULL);
	CloseHandle(hFile);
	return pMap;
}

void CPage1::OnNMClickListlib(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	USES_CONVERSION;
	CString		strAddress;
	int	nPos= m_lib.GetSelectionMark();
	int nCmdCount = m_map[nPos].Command_addr.size();

	if (nCmdCount) {
		m_command.DeleteAllItems();
		for (int n = 0;n < nCmdCount;n++) {
			strAddress.Format(L"%08X", m_map[nPos].Command_addr[n]);
			m_command.InsertItem(n, strAddress);
			m_command.SetItemData(n, m_map[nPos].Command_addr[n]);
			m_command.SetItemText(n, 1,A2W(m_map[nPos].Command_name[n].c_str()));
		}
	}

	addrtype = 0;
	nametype = 0;
	*pResult = 0;
}

static int CALLBACK CompareAddr(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	if (lParamSort == 0) {
		return lParam1 > lParam2;
	}
	else if (lParamSort == 1) {
		return lParam2 > lParam1;
	}
	return 0;
}

static int CALLBACK CompareName(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	if (lParamSort == 0) {          //按照文本ASCII比较
		UCHAR x1 = *(UCHAR*)pMaindlg->m_page1.m_command.GetItemText(static_cast<int>(lParam1), 1).GetBuffer();
		UCHAR x2 = *(UCHAR*)pMaindlg->m_page1.m_command.GetItemText(static_cast<int>(lParam2), 1).GetBuffer();
		return x1 > x2;
	}
	else if (lParamSort == 1) {     //按照文本长度比较
		return pMaindlg->m_page1.m_command.GetItemText(static_cast<int>(lParam1), 1).GetLength() > pMaindlg->m_page1.m_command.GetItemText(static_cast<int>(lParam2), 1).GetLength();
	}
	return 0;
}

void CPage1::OnLvnColumnclickListcommand(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iSubItem == 0) {
		m_command.SortItems(CompareAddr, (DWORD_PTR)addrtype);
		if (addrtype == 0) {  //正向排序
			addrtype = 1;
		}
		else {                //逆向排序
			addrtype = 0;
		}
	}
	else if (pNMLV->iSubItem == 1) {
		m_command.SortItemsEx(CompareName, (DWORD_PTR)nametype);
		if (nametype == 0) {       //按照ASCII排序
			nametype = 1;
		}
		else {                    //按照长度排序
			nametype = 0;
		}
	}
	*pResult = 0;
}



void CPage1::OnNMDblclkListcommand(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (m_command.GetSelectedCount() <= 0) {    //说明未选取到命令
		return;
	}
	int nPos = m_command.GetSelectionMark();
	if (nPos == -1) {
		return;
	}
	int dwData = m_command.GetItemData(nPos);
	if (!dwData)
		return;

	Setcpu(0, dwData, 0, 0, CPU_NOFOCUS);

	*pResult = 0;
}


void CPage1::OnNMRClickListcommand(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CMenu Menu;
	CMenu *pSubMenu;
	if (pNMItemActivate->iItem != -1) {
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));
		Menu.LoadMenu(IDR_MENU1);
		pSubMenu = Menu.GetSubMenu(0);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	*pResult = 0;
}

void CPage1::On32771()   //查找引用按钮
{
	USES_CONVERSION;
	int nPos = m_command.GetSelectionMark();
	if (nPos == -1) {
		return;
	}
	DWORD dwData = m_command.GetItemData(nPos);
	if (!dwData) {
		return;
	}

	CString	strCom= m_command.GetItemText(nPos, 1);

	pMaindlg->m_output.ResetContent();
	pMaindlg->outputInfo("-> 执行命令   --==查找引用==--");
	byte ComCall[5] = { 0xBB, 0, 0, 0, 0 };
	memcpy(&ComCall[1], &dwData, sizeof(DWORD));
	byte *pTmp = (byte*)pEAnalysisEngine->T_O2V(pEAnalysisEngine->dwUsercodeStart);

	DWORD	dwSecSize = pEAnalysisEngine->dwUsercodeEnd - pEAnalysisEngine->dwUsercodeStart;
	DWORD	dwResult = pEAnalysisEngine->dwUsercodeStart;    //搜寻结果地址
	DWORD   dwCount = 0;   //已搜索字节数
	while (true) {
		DWORD offset = pEAnalysisEngine->Search_BinEx(pTmp, ComCall, dwSecSize, sizeof(ComCall));//得到偏移地址
		if (offset == 0)
			break;
		dwResult += offset;
		int index= pMaindlg->outputInfo("%08X    mov ebx,%08X    //%s", dwResult, dwData, W2A(strCom)); //显示出结果地址
		pMaindlg->m_output.SetItemData(index, dwResult);
		dwResult += sizeof(ComCall);
		pTmp += offset+sizeof(ComCall);		
		dwSecSize -= offset + sizeof(ComCall);
	}
}
