#if !defined(AFX_STDAFX_H__23EBD760_53D8_444B_AEE5_DB2E80975B5E__INCLUDED_)
#define AFX_STDAFX_H__23EBD760_53D8_444B_AEE5_DB2E80975B5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#define NO_WARN_MBCS_MFC_DEPRECATION 
#endif // _MSC_VER > 1000

#define WINVER 0x0501	//XP VC6¡úVS2005
#define _WIN32_WINNT 0x0501

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include <Afxmt.h>

#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename_namespace("ADOBS") rename("EOF","adoEOF")
#include <afxdisp.h>
using namespace ADOBS;

#include "excel9.h"
#include"tlhelp32.h"


#endif
