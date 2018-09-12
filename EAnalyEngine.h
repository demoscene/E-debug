#pragma once

typedef struct _ENTRYINFO // 易语言入口信息
{
	DWORD	dwMagic;		//<- 未知
	DWORD	dwUnkown1;		//+4 未知
	DWORD	dwUnkown2;		//+8 未知
	DWORD	dwUserCodeStart;//+c 用户代码开始
	DWORD	dwEString;		//+10 字符串 资源       如果没有字符串资源,则为0
	DWORD	dwEStringSize;	//+14 字符串 资源大小   如果没有字符串资源,则为0
	DWORD	dwEWindow;		//+18 创建组件信息      包括窗口、标签等控件
	DWORD	dwEWindowSize;	//+1c 大小
	DWORD	dwLibNum;		//+20 支持库数量
	DWORD	pLibEntey;		//+24 支持库信息入口
	DWORD	dwApiCount;     //+28 Api数量
	DWORD	pLibName;		//+2C 指向库名称
	DWORD	pApiName;		//+30 指向Api名称

}*PEENTRYINFO;

class EAnalysis
{
public:
	EAnalysis(ULONG, ULONG);
	~EAnalysis();

	BOOL EStaticLibInit(); //静态编译--初始化
	BOOL GetUserEntryPoint();  //静态编译--取用户结束地址

	HANDLE GethProcess();

	DWORD   QuerySearch(HANDLE h_gprocess,byte *pSrc,DWORD StartAddr);//内存模糊搜索,参数一为进程句柄,参数二搜寻字节,参数三为起始地址

	DWORD	Search_Bin(byte *pSrc, byte *pTrait, int nSrcLen, int nTraitLen);
	DWORD   Search_BinEx(byte *pSrc, byte *pTrait, int nSrcLen, int nTraitLen);
	DWORD	T_O2V(DWORD dwVaddr);//origin addr to virtual addr
	DWORD	T_V2O(DWORD dwOaddr);
	DWORD	R_O2V(DWORD dwVaddr);//origin addr to virtual addr
	DWORD	R_V2O(DWORD dwOaddr);
	DWORD	GetPoint(DWORD dwAddr);
	DWORD	R_GetOriginPoint(DWORD dwAddr);
	DWORD dwUsercodeStart; //用户代码的起始地址
	DWORD dwUsercodeEnd;   //用户代码的结束地址

	BYTE	*textSection; // 申请的代码段空间
	BYTE    *rdataSection;// 申请的输入表段空间
	ULONG	text_dwBase;		// 
	ULONG	text_dwSize;		// 
	ULONG	rdata_dwBase;		// 
	ULONG	rdata_dwSize;		//

	PEENTRYINFO pEnteyInfo; // entry info
private:
protected:
};