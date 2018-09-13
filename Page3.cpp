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
}


BEGIN_MESSAGE_MAP(CPage3, CDialog)
END_MESSAGE_MAP()


// CPage3 消息处理程序

BOOL CPage3::OnInitDialog() {
	CDialog::OnInitDialog();

	WindowInfo m_WindowInfo;
	DWORD EipAddr = pEAnalysisEngine->pEnteyInfo->dwEWindow; //当前解析数据结构的地址,真实地址
	m_WindowInfo.WindowCount =*(unsigned long*)pEAnalysisEngine->R_O2V(EipAddr)>>3;
	if (m_WindowInfo.WindowCount == 0) {
		return TRUE;
	}

	for (int i = 0;i < m_WindowInfo.WindowCount;i++) {
		EipAddr = EipAddr + 4;
		m_WindowInfo.WindowId.push_back(*(DWORD*)(pEAnalysisEngine->R_O2V(EipAddr)));
		pMaindlg->outputInfo("%X", m_WindowInfo.WindowId[i]);
	}
	for (int i = 0;i < m_WindowInfo.WindowCount;i++) {
		EipAddr = EipAddr + 4;
		//m_WindowInfo.WindowAddr.push_back(*(DWORD*)(pEAnalysisEngine->R_O2V(EipAddr))); //没有什么用的数据
	}

	EipAddr = EipAddr + 20;
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
	}

	return TRUE;
}


//窗口个数
//窗口ID
//对应内存地址
//Unknow1
//Unknow2
//0000000
//0000000
//控件个数
//控件大小?
//控件ID
//00000000
//对应控件偏移

//第一个控件空间大小
//01 00 01 00