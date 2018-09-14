// Page3.cpp : 实现文件
//

#include "stdafx.h"
#include "E-Debug.h"
#include "Page3.h"
#include "afxdialogex.h"
#include "MainWindow.h"
#include "EAnalyEngine.h"

extern 	CMainWindow *pMaindlg;
extern  EAnalysis	*pEAnalysisEngine;

// CPage3 对话框

IMPLEMENT_DYNAMIC(CPage3, CDialog)

CPage3::CPage3(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PAGE3, pParent)
{

}

CPage3::~CPage3()
{
}

void CPage3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
}


BEGIN_MESSAGE_MAP(CPage3, CDialog)
END_MESSAGE_MAP()


// CPage3 消息处理程序

BOOL CPage3::OnInitDialog() {
	CDialog::OnInitDialog();



	m_Tree.SetLineColor(RGB(0, 0, 255));



	//————————————————
	WindowInfo m_WindowInfo;
	DWORD EipAddr = pEAnalysisEngine->pEnteyInfo->dwEWindow; //Eip地址为当前解析地址
	if (EipAddr == 0) {
		return TRUE;
	}
	m_WindowInfo.WindowCount =*(unsigned long*)pEAnalysisEngine->R_O2V(EipAddr)>>3;
	if (m_WindowInfo.WindowCount == 0) {
		return TRUE;
	}

	CString WinID;
	CString ControlID;

	for (int i = 0;i < m_WindowInfo.WindowCount;i++) {
		EipAddr = EipAddr + 4;
		WinID.Format(L"0x%X", *(DWORD*)(pEAnalysisEngine->R_O2V(EipAddr)));
		m_WindowInfo.WindowId.push_back(m_Tree.InsertItem(WinID));
	}

	EipAddr = EipAddr + 4 * (m_WindowInfo.WindowCount+1); //跳过WindowAddr

	WindowPropery m_WindowPropery;

	for (int i = 0;i < m_WindowInfo.WindowCount;i++) {   //窗口依次解析
		EipAddr = EipAddr + 16;
		m_WindowPropery.ControlCount = *(unsigned long*)pEAnalysisEngine->R_O2V(EipAddr);
		if (m_WindowPropery.ControlCount == 0) {
			return true;
		}
		EipAddr = EipAddr + 4;
		m_WindowPropery.ControlSize= *(unsigned long*)pEAnalysisEngine->R_O2V(EipAddr);
		for (int j = 0;j < m_WindowPropery.ControlCount;j++) {
			EipAddr = EipAddr + 4;
			m_WindowPropery.ControlID.push_back(*(unsigned long*)pEAnalysisEngine->R_O2V(EipAddr));
			ControlID.Format(L"0x%X",m_WindowPropery.ControlID[j]);
			m_Tree.InsertItem(ControlID, m_WindowInfo.WindowId[i]);
		}
		EipAddr = 4 + (EipAddr - m_WindowPropery.ControlCount * 4) + m_WindowPropery.ControlSize;
	}

	/*EipAddr = EipAddr + 20;
	m_WindowInfo.ControlCount = *(unsigned long*)pEAnalysisEngine->R_O2V(EipAddr);
	if (m_WindowInfo.ControlCount == 0) {      //其实是默认自带一个控件ID的
		return true;  
	}

	EipAddr = EipAddr + 4;
	m_WindowInfo.ControlSize = *(unsigned long*)pEAnalysisEngine->R_O2V(EipAddr);

	for (int i = 0;i < m_WindowInfo.ControlCount;i++) {
		EipAddr = EipAddr + 4;
		m_WindowInfo.ControlID.push_back(*(DWORD*)(pEAnalysisEngine->R_O2V(EipAddr)));
		pMaindlg->outputInfo("%X", m_WindowInfo.ControlID[i]);
	}

	EipAddr = EipAddr + 4; 

	for (int i = 0;i < m_WindowInfo.ControlCount;i++) {
		EipAddr = EipAddr + 4;
		m_WindowInfo.Controloffset.push_back(*(DWORD*)(pEAnalysisEngine->R_O2V(EipAddr)));
	}*/

	return TRUE;
}

