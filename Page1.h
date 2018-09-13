#pragma once
#include "afxcmn.h"
#include "stdafx.h"

#include <string>
#include <map>

using namespace std;

typedef struct LIBMAP
{
	vector<string>  Command_name;
	vector<DWORD>  Command_addr;
}*LibMap;


#define MAX_ESIZE 256
typedef struct
{
	char	m_CommandName[64];				//命令名称
	int  	m_CallType;					    //函数类型,0代表无重复CALL,1代表需要判断第二个CALL，2代表需要判断IAT函数的CALL
											//3代表判断CALL之前有一段特殊代码
	ULONG   m_CallOffset;			        //记录需要判断的call的偏移
	int		m_size;							//程序一阶函数的字节大小
	UCHAR   m_opcode[MAX_ESIZE];	        //匹配的字节
	int     m_size2;                        //程序二阶函数的字节大小
	UCHAR	m_opcode2[MAX_ESIZE];	        //类型1或3时的opcode
	char    m_IATEAT[128];                   //call为类型2时为IAT与EAT
}ESTATICLIBOPCODE, *PESTATICLIBOPCODE;

// CPage1 对话框

class CPage1 : public CDialog
{
	DECLARE_DYNAMIC(CPage1)

public:
	CPage1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPage1();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	PESTATICLIBOPCODE ReadMap(const char *lpMapPath, int *nCount);
	bool MatchCode(unsigned char* pSrc1, unsigned char* pSrc2, int nLen);
	bool MatchCode_UnEx(unsigned char* pSrc1, unsigned char* pSrc2, int nLen);//
	CListCtrl m_lib;
	CListCtrl m_command;
	map<int, LIBMAP> m_map;
	afx_msg void OnNMClickListlib(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListcommand(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnNMDblclkListcommand(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListcommand(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void On32771();
};
