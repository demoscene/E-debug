#pragma once


// CPage3 对话框

class CPage3 : public CDialog
{
	DECLARE_DYNAMIC(CPage3)

public:
	CPage3(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPage3();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
