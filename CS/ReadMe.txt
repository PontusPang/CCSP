m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
CString strOnlyOffFJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuOnlyOffFJ"));
strDelay3 = "SELECT * FROM PropCHSH WHERE Item = '��ʱ' AND Num = 3 ORDER BY ID ASC";
strSql.Format("UPDATE PropSomeKu SET [ID] = %d WHERE NO = %d", nTotalSenconds+nSomeKuIndex, nNO);
strSql.Format("DELETE FROM PropSomeKu WHERE NO >= %d", nDelay3NO);


//********************************����********************************
//0-�ʼǱ�
//#define CS_SQL "CS_QXJC_141014"	//ʵ��CAN
////#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"�ʼǱ�"
//#define LOGIN_PWD	"xsak168"
//#define CS_NAME	"�ൺ��ʥ���ػ���Ƽ�"
//#define CS_WORKER	"������"
//#define CS_PHONE	"185-6066-8933"
////////ThinkPad
////#define NumVolumeSerial 0xC24D10B1
////#define StrHDSerial		"AA3H4061"	
////////Dell
//#define NumVolumeSerial 0xAE8CCFB6
//#define StrHDSerial		"5VG46WG4"

//0-��ʥ����
//#define CS_SQL "CS_QXJC_141014"	//ʵ��CAN
////#define CS_SQL "CS_ZW17_150313"
////#define CS_SQL "CS_QXYX_141012"	//��ʱ2
//#define LOGIN_NAME	"��ʥ����"
////#define LOGIN_NAME	"��ϼԴ��"
//#define LOGIN_PWD	"xsak168"
//#define CS_NAME	"�ൺ��ʥ���ػ���Ƽ�"
//#define CS_WORKER	"������"
//#define CS_PHONE	"185-6066-8933"
//#define FLAG_EN_MUSIC FALSE
//
//#define NumVolumeSerial 0xC24D10B1
//#define StrHDSerial		"Z6E0SV0G"

//********************************��ʽ********************************

//1-��ϼԴ��1-8[15253591031]
//#define CS_SQL "CS_QXYX_141012"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"��ϼԴ��"
//#define LOGIN_PWD	"yuanxin168"
//#define CS_NAME	"��̨Դ�ι������"
//#define CS_WORKER	"����"
//#define CS_PHONE	"138-5357-7173"
//
//#define NumVolumeSerial 0xCAEDB8B2
//#define StrHDSerial		"W3TCZNDL"

//2-��ϼ����2-13[15166899019]
//#define CS_SQL "CS_QXJC_141014"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"��ϼ����"
//#define LOGIN_PWD	"770610"
//#define CS_NAME	"��ϼ���̹������޹�˾"
//#define CS_WORKER	"������"
//#define CS_PHONE	"137-9259-5368"
//
//#define NumVolumeSerial 0xCAEDB8B2
//#define StrHDSerial	"W3TCTXL1"
//
//3-�ӱ��Խ���1-7[15933119379]
//#define CS_SQL "CS_ZXJH_141211"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"��������"
//#define LOGIN_PWD	"zxxx168"
//#define CS_NAME	"���������ʹ���"
//#define CS_WORKER	"�Խ���"
//#define CS_PHONE	"139-3314-9888"
//
//#define NumVolumeSerial 0xCAEDB8B2		
//#define StrHDSerial		"W3TCZN9Y"

//4-�ӱ���ΰ2-7[13785163749]
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"��ΰ"
//#define LOGIN_PWD	"zhw168"
//#define CS_NAME	"���Ӫ����ΰ"
//#define CS_WORKER	"��ΰ"
//#define CS_PHONE	"130-1158-5590"
//
//#define NumVolumeSerial 0xC24D10B1
//#define StrHDSerial		"W4X0651X"

//5-�ӱ�����1-7[13833174441]
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"����"
//#define LOGIN_PWD	"xw168"
//#define CS_NAME	"���ش�������ҵ"
//#define CS_WORKER	"������"
//#define CS_PHONE	"137-8435-4899"
//////
//#define NumVolumeSerial 0xC24D10B1
//#define StrHDSerial		"W53015ZG"

//6-�ӱ��߾���A-1-8[15075189081]
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"����"
//#define LOGIN_PWD	"jh168"
//#define CS_NAME	"����¥��߾���"
//#define CS_WORKER	"�߾���"
//#define CS_PHONE	"150-3117-1798"
////
//#define NumVolumeSerial 0xC24D10B1
//#define StrHDSerial		"W530DY2M"

//7-�ӱ�����ƽ2-10
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"��ƽ"
//#define LOGIN_PWD	"xp168"
//#define CS_NAME	"���ؼ�չ"
//#define CS_WORKER	"����ƽ"
//#define CS_PHONE	"189-3197-6208"
////
//#define NumVolumeSerial 0xCAEDB8B2
//#define StrHDSerial		"W4Y2K413"

//8-�ӱ��߾���B-1-9
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"����"
//#define LOGIN_PWD	"jh168"
//#define CS_NAME	"����¥��߾���"
//#define CS_WORKER	"�߾���"
//#define CS_PHONE	"150-3117-1798"
////
//#define NumVolumeSerial 0xC24D10B1
//#define StrHDSerial		"W4X063K3"

//9-�ӱ�������2-9
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"����"
//#define LOGIN_PWD	"gq168"
//#define CS_NAME	"����"
//#define CS_WORKER	"����"
//#define CS_PHONE	"136-0311-1097"
//
//#define NumVolumeSerial 0x00B005EB
//#define StrHDSerial		"S3T29THK"
	
