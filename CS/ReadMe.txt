m_pRs = m_pCon->Execute(_bstr_t(strSql),NULL,adCmdText);
CString strOnlyOffFJ = (LPCSTR)_bstr_t(m_pRs->GetCollect("KuOnlyOffFJ"));
strDelay3 = "SELECT * FROM PropCHSH WHERE Item = '延时' AND Num = 3 ORDER BY ID ASC";
strSql.Format("UPDATE PropSomeKu SET [ID] = %d WHERE NO = %d", nTotalSenconds+nSomeKuIndex, nNO);
strSql.Format("DELETE FROM PropSomeKu WHERE NO >= %d", nDelay3NO);


//********************************测试********************************
//0-笔记本
//#define CS_SQL "CS_QXJC_141014"	//实地CAN
////#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"笔记本"
//#define LOGIN_PWD	"xsak168"
//#define CS_NAME	"青岛新圣安控机电科技"
//#define CS_WORKER	"逄淑松"
//#define CS_PHONE	"185-6066-8933"
////////ThinkPad
////#define NumVolumeSerial 0xC24D10B1
////#define StrHDSerial		"AA3H4061"	
////////Dell
//#define NumVolumeSerial 0xAE8CCFB6
//#define StrHDSerial		"5VG46WG4"

//0-新圣安控
//#define CS_SQL "CS_QXJC_141014"	//实地CAN
////#define CS_SQL "CS_ZW17_150313"
////#define CS_SQL "CS_QXYX_141012"	//延时2
//#define LOGIN_NAME	"新圣安控"
////#define LOGIN_NAME	"栖霞源鑫"
//#define LOGIN_PWD	"xsak168"
//#define CS_NAME	"青岛新圣安控机电科技"
//#define CS_WORKER	"逄淑松"
//#define CS_PHONE	"185-6066-8933"
//#define FLAG_EN_MUSIC FALSE
//
//#define NumVolumeSerial 0xC24D10B1
//#define StrHDSerial		"Z6E0SV0G"

//********************************正式********************************

//1-栖霞源鑫1-8[15253591031]
//#define CS_SQL "CS_QXYX_141012"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"栖霞源鑫"
//#define LOGIN_PWD	"yuanxin168"
//#define CS_NAME	"烟台源鑫果蔬冷藏"
//#define CS_WORKER	"王金福"
//#define CS_PHONE	"138-5357-7173"
//
//#define NumVolumeSerial 0xCAEDB8B2
//#define StrHDSerial		"W3TCZNDL"

//2-栖霞锦程2-13[15166899019]
//#define CS_SQL "CS_QXJC_141014"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"栖霞锦程"
//#define LOGIN_PWD	"770610"
//#define CS_NAME	"栖霞锦程果蔬有限公司"
//#define CS_WORKER	"程绍龙"
//#define CS_PHONE	"137-9259-5368"
//
//#define NumVolumeSerial 0xCAEDB8B2
//#define StrHDSerial	"W3TCTXL1"
//
//3-河北赵江辉1-7[15933119379]
//#define CS_SQL "CS_ZXJH_141211"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"赵县新星"
//#define LOGIN_PWD	"zxxx168"
//#define CS_NAME	"赵县新星鲜果库"
//#define CS_WORKER	"赵江辉"
//#define CS_PHONE	"139-3314-9888"
//
//#define NumVolumeSerial 0xCAEDB8B2		
//#define StrHDSerial		"W3TCZN9Y"

//4-河北振伟2-7[13785163749]
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"振伟"
//#define LOGIN_PWD	"zhw168"
//#define CS_NAME	"秀才营村振伟"
//#define CS_WORKER	"振伟"
//#define CS_PHONE	"130-1158-5590"
//
//#define NumVolumeSerial 0xC24D10B1
//#define StrHDSerial		"W4X0651X"

//5-河北祥卫1-7[13833174441]
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"祥卫"
//#define LOGIN_PWD	"xw168"
//#define CS_NAME	"赵县大安三康果业"
//#define CS_WORKER	"李祥卫"
//#define CS_PHONE	"137-8435-4899"
//////
//#define NumVolumeSerial 0xC24D10B1
//#define StrHDSerial		"W53015ZG"

//6-河北高京华A-1-8[15075189081]
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"京华"
//#define LOGIN_PWD	"jh168"
//#define CS_NAME	"贤门楼村高京华"
//#define CS_WORKER	"高京华"
//#define CS_PHONE	"150-3117-1798"
////
//#define NumVolumeSerial 0xC24D10B1
//#define StrHDSerial		"W530DY2M"

//7-河北李新平2-10
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"新平"
//#define LOGIN_PWD	"xp168"
//#define CS_NAME	"赵县冀展"
//#define CS_WORKER	"李新平"
//#define CS_PHONE	"189-3197-6208"
////
//#define NumVolumeSerial 0xCAEDB8B2
//#define StrHDSerial		"W4Y2K413"

//8-河北高京华B-1-9
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"京华"
//#define LOGIN_PWD	"jh168"
//#define CS_NAME	"贤门楼村高京华"
//#define CS_WORKER	"高京华"
//#define CS_PHONE	"150-3117-1798"
////
//#define NumVolumeSerial 0xC24D10B1
//#define StrHDSerial		"W4X063K3"

//9-河北戴广勤2-9
//#define CS_SQL "CS_ZW17_150313"
//#define FLAG_EN_MUSIC FALSE
//#define LOGIN_NAME	"广勤"
//#define LOGIN_PWD	"gq168"
//#define CS_NAME	"广勤"
//#define CS_WORKER	"广勤"
//#define CS_PHONE	"136-0311-1097"
//
//#define NumVolumeSerial 0x00B005EB
//#define StrHDSerial		"S3T29THK"
	
