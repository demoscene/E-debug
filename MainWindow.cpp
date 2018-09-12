// MainWindow.cpp : 实现文件
//

#include "stdafx.h"
#include "E-Debug.h"
#include "afxdialogex.h"
#include "MainWindow.h"
#include <vector>
#include "EAnalyEngine.h"



extern HINSTANCE g_hInstace;
CMainWindow *pMaindlg;
EAnalysis	*pEAnalysisEngine;

// CMainWindow 对话框

IMPLEMENT_DYNAMIC(CMainWindow, CDialog)

CMainWindow::CMainWindow(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MainWindow, pParent)
{

}

CMainWindow::~CMainWindow()
{

}

void CMainWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	DDX_Control(pDX, IDC_LOG, m_output);
}


BEGIN_MESSAGE_MAP(CMainWindow, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMainWindow::OnTcnSelchangeTab1)
	ON_LBN_SELCHANGE(IDC_LOG, &CMainWindow::OnLbnSelchangeLog)
END_MESSAGE_MAP()


// CMainWindow 消息处理程序
BOOL CMainWindow::OnInitDialog() {
	CDialog::OnInitDialog();
	pMaindlg = this;
	
	
	HICON hIcon = LoadIcon(g_hInstace, MAKEINTRESOURCE(IDI_ICON));
	SetIcon(hIcon, FALSE);
	SetIcon(hIcon, TRUE);

	m_Tab.InsertItem(0, _T("支持库命令"));
	m_Tab.InsertItem(1, _T("API命令"));
	m_Tab.InsertItem(2, _T("窗口组件"));

	if (Getcputhreadid() == 0) {
		return true;
	}

	ULONG uBase, uSize;
	BOOL bInit;
	INT nPos;
	
	Getdisassemblerrange(&uBase, &uSize);
	
	outputInfo("->开始分析当前区段....  分析地址: % 08X  内存大小: % 08X", uBase, uSize);

	pEAnalysisEngine = new EAnalysis(uBase, uSize);

	bInit = pEAnalysisEngine->EStaticLibInit();

	if (bInit) {      //针对静态编译的易语言程序

		std::vector<std::string> krnlCmd =
		{ "错误回调", "DLL命令", "三方支持库命令", "核心支持库命令",
			"读取组件属性", "设置组件属性", "分配内存", "重新分配内存",
			"释放内存", "结束", "窗口消息循环", "载入启动窗口", "初始化" };

		nPos = outputInfo("->  用户自写代码段开始 : %08X     ", pEAnalysisEngine->dwUsercodeStart);
		m_output.SetItemData(nPos, pEAnalysisEngine->dwUsercodeStart);
		Insertname(pEAnalysisEngine->dwUsercodeStart, NM_COMMENT, "用户代码段开始");

		if (!pEAnalysisEngine->GetUserEntryPoint()) {
			pEAnalysisEngine->dwUsercodeEnd = uBase + uSize -1;
			nPos = outputInfo("->  用户自写代码段结束 : %08X     ", pEAnalysisEngine->dwUsercodeEnd);
		}
		else {
			nPos = outputInfo("->  用户自写代码段结束 : %08X     ", pEAnalysisEngine->dwUsercodeEnd);
			m_output.SetItemData(nPos, pEAnalysisEngine->dwUsercodeEnd);
			Insertname(pEAnalysisEngine->dwUsercodeEnd, NM_COMMENT, "易语言程序入口");
		}

		DWORD	dwKrnlEntry = pEAnalysisEngine->pEnteyInfo->dwEString;
		if (dwKrnlEntry == 0) {
			dwKrnlEntry = pEAnalysisEngine->pEnteyInfo->dwEWindow;
		}
		DWORD	dwPoint;


		if ( dwKrnlEntry > (pEAnalysisEngine->rdata_dwBase+pEAnalysisEngine->rdata_dwSize)) {     //部分加壳程序,区段被切割
			ULONG tempSection=(DWORD)LocalAlloc(LMEM_ZEROINIT, krnlCmd.size()*4);
			Readmemory((DWORD*)tempSection,dwKrnlEntry - krnlCmd.size() * 4, krnlCmd.size() * 4, MM_RESILENT);
			for (int i = 0; i < krnlCmd.size(); i++) {
				dwPoint = pEAnalysisEngine->GetPoint((DWORD)tempSection);
				Insertname(dwPoint, NM_LABEL, (char*)krnlCmd[i].c_str());
				tempSection +=sizeof(DWORD);
			}
			LocalFree((HLOCAL)tempSection);
		}
		else {
			for (int i = krnlCmd.size() - 1; i >= 0; i--)
			{
				dwKrnlEntry -= sizeof(DWORD);
				dwPoint = pEAnalysisEngine->GetPoint(pEAnalysisEngine->R_O2V(dwKrnlEntry));
				Insertname(dwPoint, NM_LABEL, (char*)krnlCmd[i].c_str());
			}

		}





		outputInfo("->  枚举易语言<_krnl_fn_>核心支持命令成功...");
	}

	else {
		outputInfo("->  该程序有可能不是易语言程序...");
		return true;
	}


	CRect rc;
	m_Tab.GetClientRect(&rc);
	rc.top += 26;
	rc.left += 4;
	rc.right -= 6;
	rc.bottom -= 6;

	m_page1.Create(IDD_PAGE1, &m_Tab);
	m_page1.MoveWindow(&rc);

    m_page2.Create(IDD_PAGE2, &m_Tab);
	m_page2.MoveWindow(&rc);

	m_page3.Create(IDD_PAGE3, &m_Tab);
	m_page3.MoveWindow(&rc);


	m_page1.ShowWindow(true);

	return true;
}

void CMainWindow::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nCursel = m_Tab.GetCurSel();
	switch (nCursel)
	{
	case 0:
		m_page1.ShowWindow(true);
		m_page2.ShowWindow(false);
		m_page3.ShowWindow(false);
		break;
	case 1:
		m_page1.ShowWindow(false);
		m_page2.ShowWindow(true);
		m_page3.ShowWindow(false);
		break;
	case 2:
		m_page1.ShowWindow(false);
		m_page2.ShowWindow(false);
		m_page3.ShowWindow(true);
		break;
	default:
		break;
	}
	*pResult = 0;
}

int CMainWindow::outputInfo(char *formatText, ...)
{
	USES_CONVERSION;
	char buf[MAX_PATH] = { 0 };
	va_list st;
	va_start(st, formatText);
	vsprintf_s(buf, formatText, st);
	va_end(st);
	return m_output.InsertString(-1, A2W(buf));
}

DWORD Search_Bin(byte *pSrc, byte *pTrait, int nSrcLen, int nTraitLen)
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

void CMainWindow::OnLbnSelchangeLog()
{
	int		nCurSel;
	DWORD	dwItemData;
	nCurSel = m_output.GetCurSel();
	dwItemData = m_output.GetItemData(nCurSel);
	if (dwItemData != 0)
	{
		Setcpu(NULL, dwItemData, NULL, NULL, CPU_NOFOCUS);
	}
}

void ScanCode(byte* pSrc,byte* pTrait,int nStrlen,int nTraitlen ) {
	Search_Bin(pSrc,pTrait,nStrlen,nTraitlen);

}