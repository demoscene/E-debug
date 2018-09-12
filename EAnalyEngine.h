#pragma once

typedef struct _ENTRYINFO // �����������Ϣ
{
	DWORD	dwMagic;		//<- δ֪
	DWORD	dwUnkown1;		//+4 δ֪
	DWORD	dwUnkown2;		//+8 δ֪
	DWORD	dwUserCodeStart;//+c �û����뿪ʼ
	DWORD	dwEString;		//+10 �ַ��� ��Դ       ���û���ַ�����Դ,��Ϊ0
	DWORD	dwEStringSize;	//+14 �ַ��� ��Դ��С   ���û���ַ�����Դ,��Ϊ0
	DWORD	dwEWindow;		//+18 ���������Ϣ      �������ڡ���ǩ�ȿؼ�
	DWORD	dwEWindowSize;	//+1c ��С
	DWORD	dwLibNum;		//+20 ֧�ֿ�����
	DWORD	pLibEntey;		//+24 ֧�ֿ���Ϣ���
	DWORD	dwApiCount;     //+28 Api����
	DWORD	pLibName;		//+2C ָ�������
	DWORD	pApiName;		//+30 ָ��Api����

}*PEENTRYINFO;

class EAnalysis
{
public:
	EAnalysis(ULONG, ULONG);
	~EAnalysis();

	BOOL EStaticLibInit(); //��̬����--��ʼ��
	BOOL GetUserEntryPoint();  //��̬����--ȡ�û�������ַ

	HANDLE GethProcess();

	DWORD   QuerySearch(HANDLE h_gprocess,byte *pSrc,DWORD StartAddr);//�ڴ�ģ������,����һΪ���̾��,��������Ѱ�ֽ�,������Ϊ��ʼ��ַ

	DWORD	Search_Bin(byte *pSrc, byte *pTrait, int nSrcLen, int nTraitLen);
	DWORD   Search_BinEx(byte *pSrc, byte *pTrait, int nSrcLen, int nTraitLen);
	DWORD	T_O2V(DWORD dwVaddr);//origin addr to virtual addr
	DWORD	T_V2O(DWORD dwOaddr);
	DWORD	R_O2V(DWORD dwVaddr);//origin addr to virtual addr
	DWORD	R_V2O(DWORD dwOaddr);
	DWORD	GetPoint(DWORD dwAddr);
	DWORD	R_GetOriginPoint(DWORD dwAddr);
	DWORD dwUsercodeStart; //�û��������ʼ��ַ
	DWORD dwUsercodeEnd;   //�û�����Ľ�����ַ

	BYTE	*textSection; // ����Ĵ���οռ�
	BYTE    *rdataSection;// �����������οռ�
	ULONG	text_dwBase;		// 
	ULONG	text_dwSize;		// 
	ULONG	rdata_dwBase;		// 
	ULONG	rdata_dwSize;		//

	PEENTRYINFO pEnteyInfo; // entry info
private:
protected:
};