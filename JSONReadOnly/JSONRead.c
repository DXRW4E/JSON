/*
 * Copyright (C) 2022 Denis X Alb (DXRW4E)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//#include "JSONRead.h"
//#include "/storage/emulated/0/JSON/JSONRead.h"

#ifndef _JSONRead_H_
#define _JSONRead_H_

#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FALSE
	#define FALSE	0
#endif
#ifndef TRUE
	#define TRUE	1
#endif
#ifndef CONST
	#define CONST	const
#endif
#ifndef NULL
	#ifdef __cplusplus
		#define NULL 0
	#else
		#define NULL ((void *)0)
	#endif
#endif
#ifndef CONST
	#define CONST  const
#endif
#ifndef VOID
	#define VOID void
#endif

#ifndef MemAlloc
	#define MemAlloc(cbSize) malloc(cbSize)
#endif
#ifndef MemAllocEx
	#define MemAllocEx(cbSize) calloc(cbSize, 1)
#endif
#ifndef MemReAlloc
	#define MemReAlloc(lpMem, cbSize) realloc(lpMem, cbSize)
#endif
#ifndef MemFree
	#define MemFree(lpMem) free(lpMem)
#endif
#ifndef CopyMemoryEx
	#define CopyMemoryEx(Destination,Source,Length) wmemcpy((Destination),(Source),(Length))
#endif
#ifndef EqualMemoryEx
	#define EqualMemoryEx(Destination,Source,Length) (!wmemcmp((Destination),(Source),(Length)))
#endif
#ifndef ZeroMemory
	#define ZeroMemory(Destination, Length) memset((Destination),0,(Length))
#endif


#ifndef _MINWINDEF_
	typedef wchar_t		    WCHAR, * PWCHAR, * LPWSTR;
	typedef const		    WCHAR* PCWSTR, * LPCWSTR;
	typedef unsigned int	UINT, * PUINT, * LPUINT;
	typedef long			LONG, * PLONG, * LPLONG;
	typedef int			    INT, * PINT, * LPINT;
	typedef float		    FLOAT, * PFLOAT, * LPFLOAT;
	typedef unsigned short  WORD, * PWORD, * LPWORD;
	typedef int				BOOL, * PBOOL, * LPBOOL;
	typedef unsigned char   BOOLEAN, * PBOOLEAN, * LPBOOLEAN;
	typedef unsigned char   BYTE, * PBYTE, * LPBYTE;
	typedef const           BYTE* PCBYTE, * LPCBYTE;
	typedef void*           LPVOID;
	typedef const void*     LPCVOID;
	typedef size_t		    SIZE_T, * PSIZE_T, * LPSIZE_T;
#ifdef _MSC_VER
	typedef unsigned long	  DWORD, * PDWORD, * LPDWORD;  ////typedef unsigned long   DWORD,  *PDWORD,  *LPDWORD;
	typedef  __int64		  INT64, * PINT64, * LPINT64;
	typedef unsigned __int64  UINT64, * PUINT64, * LPUINT64, QWORD;
#else
	typedef unsigned int	  DWORD, * PDWORD, * LPDWORD;  ////typedef unsigned long   DWORD,  *PDWORD,  *LPDWORD;
	typedef int64_t			  INT64, * PINT64, * LPINT64;
	typedef uint64_t		  UINT64, * PUINT64, * LPUINT64;
#endif
	typedef double			  DOUBLE, * PDOUBLE, * LPDOUBLE;
#endif //_MINWINDEF_

#ifndef StrLen
#define StrLen(s) (wcslen((s)))
#endif
#ifndef IsCharXDigit
#define IsCharXDigit(wch) (iswxdigit(wch))
#endif 

#ifndef _JSON_STRING_COMPARE_FUNCTION
#define _JSON_STRING_COMPARE_FUNCTION
#ifdef _MSC_VER
#define StringEqualN(a, b, n) (!wcsncmp((a), (b), (n)))
#define StringEqualNI(a, b, n) (!_wcsnicmp((a), (b), (n)))
#define StrCmpN(a, b, n) (wcsncmp((a), (b), (n)))
#define StrCmpNI(a, b, n) (_wcsnicmp((a), (b), (n)))
#else
#define StringEqualN(a, b, n) (!wcsncmp((a), (b), (n)))
#define StringEqualNI(a, b, n) (!wcsncasecmp((a), (b), (n)))
#define StrCmpN(a, b, n) (wcsncmp((a), (b), (n)))
#define StrCmpNI(a, b, n) (wcsncasecmp((a), (b), (n)))
#endif
#define StringCompareN   StrCmpN
#define StringCompareNI  StrCmpNI
#endif //_JSON_STRING_COMPARE_FUNCTION

#ifndef ERROR_SUCCESS
	#define ERROR_SUCCESS 0 // NOERROR 0dif
#endif
#ifndef NO_ERROR
	#define NO_ERROR 0 // NOERROR 0
#endif
#ifndef ERROR_NOT_ENOUGH_MEMORY
	#define ERROR_NOT_ENOUGH_MEMORY 8 // ENOMEM 12
#endif 
#ifndef ERROR_INVALID_PARAMETER
	#define ERROR_INVALID_PARAMETER 87 // EINVAL 22
#endif
#ifndef ERROR_INVALID_HANDLE
	#define ERROR_INVALID_HANDLE 6 // EFAULT 14
#endif
#ifndef ERROR_INVALID_DATA
	#define ERROR_INVALID_DATA 13 // EINVAL 22
#endif 
#ifndef ERROR_FILE_NOT_FOUND
	#define ERROR_FILE_NOT_FOUND 2 // ENOFILE 2
#endif 
#ifndef ERROR_INVALID_INDEX
	#define ERROR_INVALID_INDEX 1413  // ENOENT 2
#endif 
#ifndef ERROR_DELETE_PENDING
	#define ERROR_DELETE_PENDING 303  // EBUSY 16 - The file cannot be opened because it is in the process of being deleted.
#endif 
//#ifndef ERROR_NO_MORE_FILES
//	#define ERROR_NO_MORE_FILES 18 // ENMFILE 89
//#endif
#ifndef ERROR_NO_MORE_ITEMS
	#define ERROR_NO_MORE_ITEMS 259 // ENMFILE 89
#endif


//#ifndef _MSC_VER
#define GetLastError() errno
#define SetLastError(dwErrCode)  errno = dwErrCode
//#endif

#ifndef _HANDLE_JSON_
#define _HANDLE_JSON_
	typedef struct _SJSON {
		LPWSTR			lpszKeyName;
		union {
			struct _SJSON* lpMembers;
			LPWSTR		   lpszValue;
		};
		union {
			struct {
				DWORD cchUnEscapeStr;
				DWORD dwNestedLevel;
			};
			DWORD	 dwValue;
			UINT	 uValue;
			int		 iValue;
			float	 fValue;
			INT64	 illValue;
			DOUBLE	 dValue;
			UINT64	 ullValue;
			BOOL	 bValue;
		};
		DWORD cchKeyName;
		DWORD dwMemBuffer;
		union {
			DWORD dwMemCount;	// Count - Data is in name/value pairs ////dwArrayCount; // Count of Array Elements
			DWORD cchValue;
		};
		union {
			union {
				struct {
					WORD IsObject : 1;		  // 0x0001 - Value is Object
					WORD IsArray : 1;		  // 0x0002 - Value is Array
					WORD IsString : 1;		  // 0x0004 - Value is String
					WORD IsBoolean : 1;	      // 0x0008 - Value is Boolean (true or false literals in lower case)
					WORD IsNull : 1;		  // 0x0010 - Value is Null (null literal in lower case)
					WORD IsNumber : 1;		  // 0x0020 - Value is Number
					WORD IsBoolTrue : 1;	  // 0x0040 - Value Boolean is true
					WORD IsSigned : 1;		  // 0x0080 - Number is Signed Integer
					WORD IsFloat : 1;		  // 0x0100 - Number is Float
					WORD IsHexDigit : 1;	  // 0x0200 - Number is HexDigit
					WORD IsNumInit : 1;	      // 0x0400 - StrToNum is Initialized, illValue, ullValue, dValue, fValue contain Number
					WORD IsEscapeStr : 1;	  // 0x0800 - Value String is Escaped = \" or \/ or \\ or \b or \f or \n or \r or \t
					WORD IsSingleQuote : 1;   // 0x1000 - Value String is Single quote 'String'
					WORD IsKeyNameSQ : 1;	  // 0x2000 - KeyName String is Single quote 'String'
					WORD IsDeletePending : 1; // 0x4000 - Is Delete Pending
					WORD IsHandleLock : 1;	  // 0x8000 - Handle is Lock, Edit Pending
					//if ((ullValueullValue & 0xFFFFFFFF00000000) || (ValNum >> 32)) Number is Int64
				};
				struct {
					WORD IsContainer : 2;
					WORD IsStrValue : 4;
				};
				struct { WORD ValueType : 6; };
				struct { WORD ValueTypeEx : 9; };
			};
			WORD wValueType;
		};
		union {
			struct {
				WORD IsHeader : 1;			  // 0x0001 - Handle is Header
				WORD Reserved : 1;			  // 0x0002 - Reserved
				WORD cHandle : 14;			  // 0x0004 - Count Open Handle, MaxSize 0x3FFF - 16383, or 13 bit 0x1FFF - 8191, or 12 bit 0xFFF --> 4095
			};
			WORD wHandleType;
		};
	} SJSON, *PJSON, **PPJSON;
	typedef const SJSON *PCJSON, **PPCJSON;
	typedef PJSON HJSON, *PHJSON;
	typedef PCJSON HCJSON;

	typedef struct _JSON_PARSE {
		PJSON pJSON;
	}JSON_PARSE, *PJSON_PARSE;
#endif // _HANDLE_JSON_

#ifndef _JSON_NUMBER_
#define _JSON_NUMBER_
	typedef union _JSON_NUMBER {
		DWORD  dwValue;
		UINT   uValue;
		int	   iValue;
		float  fValue;
		INT64  illValue;
		DOUBLE dValue;
		UINT64 ullValue;
		BOOL   bValue;
	} JSON_NUMBER, * PJSON_NUMBER;
	////static JSON_NUMBER JSONNum = { 0 };
#endif // _JSON_NUMBER_

#define JSON_IS_OBJECT		  0x00000001  // Value is Object
#define JSON_IS_ARRAY		  0x00000002  // Value is Array
#define JSON_IS_OBJECT_ARRAY  0x00000003  // (JSON_IS_OBJECT | JSON_IS_ARRAY)
#define JSON_IS_CONTAINER	  0x00000003  // (JSON_IS_OBJECT | JSON_IS_ARRAY)
#define JSON_IS_STRING		  0x00000004  // Value is String
#define JSON_IS_BOOLEAN	      0x00000008  // Value is Boolean (true or false literals in lower case)
#define JSON_IS_NULL		  0x00000010  // Value is Null (null literal in lower case)
#define JSON_IS_NUMBER		  0x00000020  // Value is Number
#define JSON_IS_STRVALUE	  0x0000003C  // (JSON_IS_STRING | JSON_IS_BOOLEAN | JSON_IS_NULL | JSON_IS_NUMBER)
#define JSON_VALUE_TYPE	      0x0000003F  // (JSON_IS_OBJECT | JSON_IS_ARRAY | JSON_IS_STRING | JSON_IS_BOOLEAN | JSON_IS_NULL | JSON_IS_NUMBER)
#define JSON_IS_BOOLTRUE	  0x00000040  // Value Boolean is true
#define JSON_IS_SIGNED		  0x00000080  // Number is Signed Integer
#define JSON_IS_FLOAT		  0x00000100  // Number is Float
#define JSON_VALUETYPE_MASK   0x000001FF  // (JSON_VALUE_TYPE | JSON_IS_BOOLTRUE | JSON_IS_SIGNED | JSON_IS_FLOAT)
#define JSON_IS_HEXDIGIT	  0x00000200  // Number is HexDigit
#define JSON_IS_NUMINIT	      0x00000400  // StrToNum is Initialized, illValue, ullValue, dValue, fValue contain Number
#define JSON_IS_ESCAPESTR	  0x00000800  // Value String is Escaped = \" or \/ or \\ or \b or \f or \n or \r or \t
#define JSON_IS_SINGLEQUOTE   0x00001000  // Value String is Single quote 'String'
#define JSON_IS_KEYNAMESQ	  0x00002000  // KeyName String is Single quote 'String'
#define JSON_IS_DELETEPENDING 0x00004000  // Handle is Delete Pending
#define JSON_IS_HANDLELOCK	  0x00008000  // Handle is Lock, Edit Pending
//// Example - if (pJSON->wHandleType & JSON_IS_HEADER) { Handle is Header }
#define JSON_IS_HEADER		  0x00000001 // Handle is Header

#define JSONPtr(pJSON) pJSON
#define JSONContainer(pJSON) (pJSON->IsContainer ? pJSON->lpMembers : NULL)
#define JSONContainerLength(pJSON) (pJSON->IsContainer ? pJSON->dwMemCount : 0)
#define JSONContainerSize(pJSON) (pJSON->IsContainer ? pJSON->dwMemBuffer : 0)
#define JSONMembers JSONContainer
#define JSONMembersLength JSONContainerLength
#define JSONMembersSize JSONContainerSize
#define JSONValueData(pJSON) (pJSON->IsStrValue ? pJSON->lpszValue : NULL)
#define JSONValueLength(pJSON) (pJSON->IsStrValue ? pJSON->cchValue : 0)

VOID JSONFree(PJSON pJSON);
LPWSTR JSONEscapeStr(LPCWSTR lpcString, DWORD cchString, PJSON pJSON);
LPWSTR JSONUnEscapeStr(LPCWSTR lpcString, DWORD cchString, DWORD cchUnEscapeStr);
DWORD JSONToNumber(PCJSON pJSON, PJSON_NUMBER pJSONNum);
PJSON JSONParse(LPCWSTR lpSrc, DWORD cSrc, LPCWSTR* lpcszEnd);
DWORD _JSONGetStringifySize(PCJSON pJSON, DWORD cTab);
DWORD JSONGetStringifySize(PCJSON pJSON);
LPWSTR _JSONStringify(PCJSON pJSON, LPWSTR lpStr, DWORD cTab);
LPWSTR JSONStringify(PCJSON pJSON);
DWORD JSONOpenKey(PCJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPCJSON ppJSONResult);
#define JSONGetKey JSONOpenKey
DWORD JSONOpenKeyEx(PCJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPCJSON ppJSONResult);
#define JSONGetKeyEx JSONOpenKeyEx
DWORD JSONEnumKey(PCJSON pJSON, DWORD dwIndex, PPCJSON ppJSONResult);
#define JSONOpenKeyByIndex JSONEnumKey
#define JSONOpenElement JSONEnumKey

#ifdef __cplusplus
}
#endif

#endif  // _JSONRead_H_

#define IsCharSpaceEx(wch) (iswspace(wch) || wch == L'\x200B' || wch == L'\xFEFF')
//#define IsJSONSpace(wch) ((wch == L'\x0009' || wch == L'\x0020' || wch == L'\x000D' || wch == L'\x000A' || wch == L'\x2028' || wch == L'\x2029'))
#define IsJSONSpaceEx(wch) (wch == L'\0' || IsCharSpaceEx(wch))
//#define IsJSONUnEscapeSpace(wch) (wch != L'\x0020' && (wch == L'\x0000' || wch < L'\x0020' || wch == L'\x00A0' || wch == L'\x0085' || wch == L'\x2028' || wch == L'\x2029' || wch == L'\x202F' \
//																	  || wch == L'\x1680' || wch == L'\x180E' || (wch > L'\x1FFF' && wch < L'\x200C') || wch == L'\x3000' || wch == L'\xFEFF' || wch == L'\x205F'))
#define IsJSONUnEscapeSpaceEx(wch) (wch != L'\x0020' && (wch < L'\x0020' || IsCharSpaceEx(wch)))

#ifndef RTL_INTEGER_CHARS
#define RTL_INTEGER_CHARS
//static const CHAR RtlIntegerChars[]   = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
static const WCHAR RtlIntegerWChars[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };
#endif // RTL_INTEGER_CHARS

VOID JSONFree(PJSON pJSON) {
	if (!pJSON) return;
	if (pJSON->lpszKeyName) MemFree(pJSON->lpszKeyName);
	if (pJSON->IsContainer) { for (PJSON pMember = pJSON->lpMembers; pJSON->dwMemCount; --pJSON->dwMemCount, ++pMember) { JSONFree(pMember); }; }
	if (pJSON->lpMembers) MemFree(pJSON->lpMembers); ///lpMembers == lpszValue
	if (pJSON->IsHeader) MemFree(pJSON);
	return;
}

LPWSTR JSONEscapeStr(LPCWSTR lpcString, DWORD cchString, PJSON pJSON) {
	LPCWSTR lpStr = NULL; LPWSTR lpszString = NULL, lpXChr = NULL; DWORD cchEscapeStr = 0, cStr = 0, cCh = 0; WCHAR wChar = 0;
	////if (pJSON && pJSON->IsHandleLock && !pJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	if (!lpcString) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
	else if (!cchString) cchString = StrLen(lpcString); ////else if (!cchString && !(cchString = StrLen(lpcString))) { SetLastError(ERROR_INVALID_DATA); return NULL; }
	lpszString = (LPWSTR)MemAlloc(((cchString * 6) + 1) * sizeof(wchar_t));
	if (!lpszString) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	else if (pJSON && pJSON->IsString) { pJSON->cchValue = cchString; pJSON->cchUnEscapeStr = 0; pJSON->IsEscapeStr = 0; }
	for (lpXChr = lpszString, lpStr = lpcString, cStr = cchString; cStr; --cStr, ++lpStr, ++lpXChr) {
		if (*lpStr == L'"' || *lpStr == L'\\' || *lpStr == L'/') { *lpXChr = L'\\'; *++lpXChr = *lpStr; }
		else if (*lpStr == L'\x0008') { *lpXChr = L'\\'; *++lpXChr = L'b'; } // Backspace (BS)
		else if (*lpStr == L'\x0009') { *lpXChr = L'\\'; *++lpXChr = L't'; } // Horizontal tab (HT)
		else if (*lpStr == L'\x000A') { *lpXChr = L'\\'; *++lpXChr = L'n'; } // Linefeed (LF)
		else if (*lpStr == L'\x000B') { *lpXChr = L'\\'; *++lpXChr = L'v'; } // Vertical tab (VT)
		else if (*lpStr == L'\x000C') { *lpXChr = L'\\'; *++lpXChr = L'f'; } // Form feed (FF)
		else if (*lpStr == L'\x000D') { *lpXChr = L'\\'; *++lpXChr = L'r'; } // Carriage return (CR)
		else if (IsJSONUnEscapeSpaceEx(*lpStr)) { *lpXChr = L'\\'; *++lpXChr = L'u'; for (wChar = *lpStr, cCh = 5; --cCh; wChar >>= 4) { lpXChr[cCh] = RtlIntegerWChars[(wChar & 0xF)]; } lpXChr += 4; }
		else { *lpXChr = *lpStr; }
	}
	*lpXChr = L'\0'; cchEscapeStr = (DWORD)(lpXChr - lpszString); //lpszString[cchEscapeStr] = L'\0';
	if (cchEscapeStr > cchString) {
		lpXChr = (LPWSTR)MemReAlloc(lpszString, (cchEscapeStr + 1) * sizeof(wchar_t));
		if (lpXChr) lpszString = lpXChr;
		if (pJSON && pJSON->IsString) { pJSON->cchValue = cchEscapeStr; pJSON->cchUnEscapeStr = cchString; pJSON->IsEscapeStr = TRUE; }
	}
	return lpszString;
}

LPWSTR JSONUnEscapeStr(LPCWSTR lpcString, DWORD cchString, DWORD cchUnEscapeStr) {
	LPCWSTR lpStr = NULL; LPWSTR lpszString = NULL, lpXChr = NULL; DWORD cStr = 0, cCh = 0;
	if (!lpcString) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
	else if (!cchString && !(cchString = StrLen(lpcString))) { SetLastError(ERROR_INVALID_DATA); return NULL; }
	else if (!cchUnEscapeStr) cchUnEscapeStr = cchString;
	lpszString = (LPWSTR)MemAlloc((cchUnEscapeStr + 1) * sizeof(wchar_t));
	if (!lpszString) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	for (lpXChr = lpszString, lpStr = lpcString, cStr = cchString; cStr; --cStr, ++lpStr, ++lpXChr) {
		if (*lpStr != L'\\') { *lpXChr = *lpStr; }
		else if (!--cStr) { goto ReturnError; }
		else if (*++lpStr == L'"' || *lpStr == L'\\' || *lpStr == L'/') { *lpXChr = *lpStr; }
		else if (*lpStr == L'b') { *lpXChr = L'\x0008'; } // Backspace (BS)
		else if (*lpStr == L't') { *lpXChr = L'\x0009'; } // Horizontal tab (HT)
		else if (*lpStr == L'n') { *lpXChr = L'\x000A'; } // Linefeed (LF)
		else if (*lpStr == L'v') { *lpXChr = L'\x000B'; } // Vertical tab (VT)
		else if (*lpStr == L'f') { *lpXChr = L'\x000C'; } // Form feed (FF)
		else if (*lpStr == L'r') { *lpXChr = L'\x000D'; } // Carriage return (CR)
		else if (*lpStr == L'u') {
			if (cStr < 5) goto ReturnError; cStr -= 5;
			for (cCh = 4, *lpXChr = L'\0', ++lpStr; cCh; --cCh, ++lpStr) {
				if (*lpStr >= L'A') {
					if (*lpStr > L'F') {
						if (*lpStr < L'a' || *lpStr > L'f') goto ReturnError; //break;
						*lpXChr = (*lpXChr << 4) + (*lpStr - L'a') + 10;
					}
					else *lpXChr = (*lpXChr << 4) + (*lpStr - L'A') + 10;
				}
				else if (*lpStr < L'0' || *lpStr > L'9') goto ReturnError; //break;
				else *lpXChr = (*lpXChr << 4) + (*lpStr - L'0');
			}
		}
		else { *lpXChr = *lpStr; }
	}
	*lpXChr = L'\0'; cchUnEscapeStr = (DWORD)(lpXChr - lpszString); //lpszString[cchUnEscapeStr] = L'\0';
	if (cchString > cchUnEscapeStr) {
		lpXChr = (LPWSTR)MemReAlloc(lpszString, (cchUnEscapeStr + 1) * sizeof(wchar_t));
		if (lpXChr) lpszString = lpXChr; ////if (!lpXChr) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); goto ReturnError; }
	}
	return lpszString;
ReturnError:
	MemFree(lpszString);
	return NULL;
}

DWORD JSONToNumber(PCJSON pJSON, PJSON_NUMBER pJSONNum) {
	if (!pJSON || !pJSON->IsNumber || !pJSONNum) return ERROR_INVALID_PARAMETER;
	LPWSTR lpEnd = NULL; //JSON_NUMBER jsonNumber = { 0 };
	if (pJSON->IsFloat) { pJSONNum->dValue = wcstod(pJSON->lpszValue, &lpEnd); }
	else if (pJSON->IsSigned) { pJSONNum->illValue = wcstoll(pJSON->lpszValue, &lpEnd, 10); }
	else { pJSONNum->ullValue = wcstoull(pJSON->lpszValue, &lpEnd, 10); }
	return NO_ERROR;
}

PJSON JSONParse(LPCWSTR lpSrc, DWORD cSrc, LPCWSTR* lpcszEnd) {
	if (!lpSrc || (!cSrc && !(cSrc = StrLen(lpSrc)))) { SetLastError(ERROR_INVALID_DATA); return NULL; }
	LPCWSTR lpStr = NULL; LPWSTR lpEnd = NULL; PJSON pHeader = NULL, pJSON = NULL, pMember = NULL;
	PJSON_PARSE pJSONParse = NULL, pParse = NULL; DWORD cCh = 0, dwIndex = 0, dwParseBuffer = 32, dwLastError = ERROR_INVALID_DATA, dwNestedLevel = 1, dwMaxNL = 1; WCHAR wCh = 0; BOOLEAN bContinue = FALSE;
	pJSONParse = (PJSON_PARSE)MemAllocEx(dwParseBuffer * sizeof(JSON_PARSE));
	if (!pJSONParse) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pParse = pJSONParse; ++dwIndex; pHeader = (PJSON)MemAllocEx(sizeof(SJSON));
	if (!pHeader) { MemFree(pJSONParse); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pParse->pJSON = pHeader; pJSON = pHeader; pJSON->IsHeader = TRUE; pJSON->IsObject = TRUE; //pJSON->Header = pJSON;
	pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer += 8) * sizeof(SJSON));
	if (!pJSON->lpMembers) { MemFree(pJSON); MemFree(pJSONParse); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pMember = pJSON->lpMembers; while (cSrc && IsJSONSpaceEx(*lpSrc)) { --cSrc; ++lpSrc; }
	if (!cSrc || *lpSrc != L'{') { goto ReturnError; }
	do {
		bContinue = FALSE; do { ++lpSrc; --cSrc; } while (cSrc && IsJSONSpaceEx(*lpSrc));
		if (pJSON->dwMemCount == pJSON->dwMemBuffer) {
			pMember = (PJSON)MemReAlloc(pJSON->lpMembers, (pJSON->dwMemBuffer *= 2) * sizeof(SJSON));
			if (!pMember) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
			pJSON->lpMembers = pMember; pMember += pJSON->dwMemCount; ////ZeroMemory(&pMember[pJSON->dwMemCount], pJSON->dwMemCount * sizeof(SJSON));
		}
		ZeroMemory(pMember, sizeof(SJSON)); //pMember->Header = pJSON->Header;
		if (!pJSON->IsArray) {
			if (!cSrc || (*lpSrc != L'"' && *lpSrc != L'\'')) goto CloseContainer; //{ if (pJSON->dwMemCount) goto ReturnError; goto CloseContainer; }
			else if (*lpSrc == L'\'') pMember->IsKeyNameSQ = TRUE;
			for (wCh = *lpSrc, lpStr = ++lpSrc, --cSrc; cSrc && *lpSrc != wCh; --cSrc, ++lpSrc) {};
			if (!cSrc || *lpSrc != wCh) goto ReturnError;
			pMember->cchKeyName = (DWORD)(lpSrc - lpStr);
			pMember->lpszKeyName = (LPWSTR)MemAlloc((pMember->cchKeyName + 1) * sizeof(wchar_t));
			if (!pMember->lpszKeyName) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
			if (pMember->cchKeyName) CopyMemoryEx(pMember->lpszKeyName, lpStr, pMember->cchKeyName); ////MemCopy(pMember->lpszKeyName, lpStr, pMember->cchKeyName * sizeof(wchar_t));
			pMember->lpszKeyName[pMember->cchKeyName] = L'\0';
			do { ++lpSrc; --cSrc; } while (cSrc && IsJSONSpaceEx(*lpSrc));
			if (!cSrc || *lpSrc != L':') { MemFree(pMember->lpszKeyName); ZeroMemory(pMember, sizeof(SJSON)); goto ReturnError; }
			do { ++lpSrc; --cSrc; } while (cSrc && IsJSONSpaceEx(*lpSrc));
		}
		if (!cSrc) { goto ReturnError; }
		else if (*lpSrc == L'"' || *lpSrc == L'\'') {
			pMember->IsString = TRUE; wCh = *lpSrc; if (*lpSrc == L'\'') pMember->IsSingleQuote = TRUE;
			for (lpStr = ++lpSrc, --cSrc, cCh = 0; cSrc && *lpStr != wCh; --cSrc, ++lpStr, ++cCh) {
				if (*lpStr == L'\\') {
					pMember->IsEscapeStr = TRUE;
					if (!--cSrc) { ++cSrc; goto ReturnError; }
					else if (*++lpStr == L'u') { if (cSrc < 5 || !IsCharXDigit(lpStr[1]) || !IsCharXDigit(lpStr[2]) || !IsCharXDigit(lpStr[3]) || !IsCharXDigit(lpStr[4])) { goto ReturnError; }; cSrc -= 4; lpStr += 4; }
				}
			}
			if (!cSrc || *lpStr != wCh) { lpSrc = lpStr; goto ReturnError; }
			pMember->cchValue = (DWORD)(lpStr - lpSrc); if (pMember->IsEscapeStr) pMember->cchUnEscapeStr = cCh;
			pMember->lpszValue = (LPWSTR)MemAlloc((pMember->cchValue + 1) * sizeof(wchar_t));
			if (!pMember->lpszValue) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
			CopyMemoryEx(pMember->lpszValue, lpSrc, pMember->cchValue);
			pMember->lpszValue[pMember->cchValue] = L'\0'; lpSrc = ++lpStr; --cSrc;
		}
		else if (*lpSrc == L't' || *lpSrc == L'f') { ////if (cSrc < 5 || (*lpSrc == L't' && StrCmpN(lpSrc, L"true", 4)) || (*lpSrc == L'f' && StrCmpN(lpSrc, L"false", 5))) goto ReturnError;
			if (*lpSrc == L't' && cSrc > 3 && lpSrc[1] == L'r' && lpSrc[2] == L'u' && lpSrc[3] == L'e') { pMember->IsBoolTrue = TRUE; pMember->cchValue = 4; }
			else if (*lpSrc == L'f' && cSrc > 4 && lpSrc[1] == L'a' && lpSrc[2] == L'l' && lpSrc[3] == L's' && lpSrc[4] == L'e') { pMember->cchValue = 5; }
			else { goto ReturnError; }
			pMember->IsBoolean = TRUE; lpSrc += pMember->cchValue; cSrc -= pMember->cchValue;
			pMember->lpszValue = (LPWSTR)MemAlloc((pMember->cchValue + 1) * sizeof(wchar_t));
			if (!pMember->lpszValue) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
			if (pMember->cchValue == 4) { CopyMemoryEx(pMember->lpszValue, L"true", 4); } ////MemCopy(pMember->lpszValue, L"true", 4 * sizeof(wchar_t));
			else { CopyMemoryEx(pMember->lpszValue, L"false", 5); } ////MemCopy(pMember->lpszValue, L"false", 5 * sizeof(wchar_t));
			pMember->lpszValue[pMember->cchValue] = L'\0'; //while (cSrc && (IsJSONSpaceEx(*lpSrc))) { --cSrc; ++lpSrc; }
		}
		else if (*lpSrc == L'n' || (*lpSrc == L',' && pJSON->IsArray)) { //if (cSrc < 3 || StrCmpN(lpSrc, L"null", 4)) goto ReturnError;
			if (*lpSrc == L',' || (cSrc > 3 && lpSrc[1] == L'u' && lpSrc[2] == L'l' && lpSrc[3] == L'l')) pMember->cchValue = 4;
			else { goto ReturnError; }
			pMember->IsNull = TRUE; if (*lpSrc == L'n') { lpSrc += 4; cSrc -= 4; };
			pMember->lpszValue = (LPWSTR)MemAlloc((pMember->cchValue + 1) * sizeof(wchar_t));
			if (!pMember->lpszValue) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
			CopyMemoryEx(pMember->lpszValue, L"null", 4); ////MemCopy(pMember->lpszValue, L"null", 4 * sizeof(wchar_t));
			pMember->lpszValue[pMember->cchValue] = L'\0';
		}
		else if (*lpSrc == L'-' || *lpSrc == L'+' || !(*lpSrc < L'0' || *lpSrc > L'9')) {
			pMember->IsNumber = TRUE; lpStr = lpSrc;
			if (*lpSrc == L'-') { pMember->IsSigned = TRUE; ++lpSrc; --cSrc; }
			else if (*lpSrc == L'+') { ++lpSrc; --cSrc; }
			if (*lpSrc == L'0' && cSrc > 1 && (lpSrc[1] == L'x' || lpSrc[1] == L'X')) { pMember->IsHexDigit = TRUE; lpSrc += 2; cSrc -= 2; }
			while (cSrc && (*lpSrc == L'.' || *lpSrc == L'-' || *lpSrc == L'+' || IsCharXDigit(*lpSrc))) { if (*lpSrc == L'.') pMember->IsFloat = TRUE; --cSrc; ++lpSrc; }
			pMember->cchValue = (DWORD)(lpSrc - lpStr); //pMember->illValue = 0;
			pMember->lpszValue = (LPWSTR)MemAlloc((pMember->cchValue + 1) * sizeof(wchar_t));
			if (!pMember->lpszValue) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
			CopyMemoryEx(pMember->lpszValue, lpStr, pMember->cchValue); ////MemCopy(pMember->lpszValue, lpStr, pMember->cchValue * sizeof(wchar_t));
			pMember->lpszValue[pMember->cchValue] = L'\0';
			//if (pMember->IsFloat) { pMember->dValue = wcstod(lpStr, &lpEnd); }
			//else if (pMember->IsSigned) { pMember->illValue = wcstoll(lpStr, &lpEnd, 10); }
			//else { pMember->ullValue = wcstoull(lpStr, &lpEnd, 10); }
			if (pMember->IsFloat) { pMember->dValue = wcstod(lpStr, &lpEnd); } //if (lpSrc != lpEnd) ?????
			else {
				cCh = pMember->cchValue; if (*lpStr == L'-' || *lpStr == L'+') { ++lpStr; --cCh; }
				if (pMember->IsHexDigit) {
					for (cCh -= 2, lpStr += 2; cCh; --cCh, ++lpStr) {
						if (*lpStr >= L'A') {
							if (*lpStr > L'F') {
								if (*lpStr < L'a' || *lpStr > L'f') goto ReturnError; //break;
								pMember->illValue = (pMember->illValue << 4) + (*lpStr - L'a') + 10;
							}
							else pMember->illValue = (pMember->illValue << 4) + (*lpStr - L'A') + 10;
						}
						else if (*lpStr < L'0' || *lpStr > L'9') goto ReturnError; //break;
						else pMember->illValue = (pMember->illValue << 4) + (*lpStr - L'0');
					}
				}
				else { while (!(*lpStr < L'0' || *lpStr > L'9')) { pMember->illValue *= 10; pMember->illValue += *lpStr - L'0'; ++lpStr; } }
				if (pMember->IsSigned) pMember->illValue = -pMember->illValue;
			}
			pMember->IsNumInit = TRUE;
		}
		else if (*lpSrc == L'{' || *lpSrc == L'[') {
			if (*lpSrc == L'[') { pMember->IsArray = TRUE; wCh = L']'; }
			else { pMember->IsObject = TRUE; wCh = L'}'; }
			if (dwNestedLevel == dwMaxNL) ++dwMaxNL;
			do { ++lpSrc; --cSrc; } while (cSrc && IsJSONSpaceEx(*lpSrc));
			if (cSrc && *lpSrc == wCh) { ++lpSrc; --cSrc; }
			else {
				if (dwIndex == dwParseBuffer) {
					pParse = (PJSON_PARSE)MemReAlloc(pJSONParse, (dwParseBuffer *= 2) * sizeof(JSON_PARSE));
					if (!pParse) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
					pJSONParse = pParse; pParse += (dwIndex - 1);
				}
				++pParse; ++dwIndex; pParse->pJSON = pMember; pJSON = pMember; pJSON->dwMemBuffer = 8;
				pJSON->lpMembers = (PJSON)MemAlloc(pJSON->dwMemBuffer * sizeof(SJSON));
				if (!pJSON->lpMembers) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
				pMember = pJSON->lpMembers; --lpSrc; ++cSrc; ++dwNestedLevel; bContinue = TRUE; continue;
			}
		}
		else if (pJSON->IsArray) goto CloseContainer;
		else { goto ReturnError; }
		while (cSrc && IsJSONSpaceEx(*lpSrc)) { --cSrc; ++lpSrc; }
		++pMember; ++pJSON->dwMemCount;
		while (cSrc && (*lpSrc == L'}' || *lpSrc == L']')) {
		CloseContainer:
			if (pJSON->IsArray) { if (*lpSrc != L']') goto ReturnError; }
			else if (*lpSrc != L'}') { goto ReturnError; }
			do { ++lpSrc; --cSrc; } while (cSrc && IsJSONSpaceEx(*lpSrc));
			if (!pJSON->dwMemCount) { if (pJSON->lpMembers) MemFree(pJSON->lpMembers); pJSON->lpMembers = NULL; pJSON->dwMemBuffer = 0; }
			else if (pJSON->dwMemBuffer > pJSON->dwMemCount) {
				pMember = (PJSON)MemReAlloc(pJSON->lpMembers, pJSON->dwMemCount * sizeof(SJSON));
				if (pMember) { pJSON->lpMembers = pMember; pJSON->dwMemBuffer = pJSON->dwMemCount; }
			}
			--dwNestedLevel; if (!--dwIndex) break;
			--pParse; ++pParse->pJSON->dwMemCount; pJSON = pParse->pJSON; pMember = &pJSON->lpMembers[pJSON->dwMemCount];
		}
	} while (cSrc && (bContinue || *lpSrc == L','));
	if (lpcszEnd) *lpcszEnd = lpSrc;
	if (dwNestedLevel) goto ReturnError;
	MemFree(pJSONParse); pJSON->dwNestedLevel = dwMaxNL; return pJSON;
ReturnError:
	JSONFree(pHeader); MemFree(pJSONParse);
	if (lpcszEnd) *lpcszEnd = lpSrc; SetLastError(dwLastError); return NULL;
}

DWORD _JSONGetStringifySize(PCJSON pJSON, DWORD cTab) { // cTab is for Internal Use Only
	PJSON pMember = NULL; DWORD cchBuffer = 0, dwMemCount = 0; //, cchOffset = 0;
	//// JSONGetStringifySize() check this
	//if (!pJSON || !pJSON->IsContainer || pJSON->IsDeletePending) { SetLastError(ERROR_INVALID_PARAMETER); return 0; } //or ERROR_INVALID_PARAMETER or ERROR_DELETE_PENDING
	if (!pJSON->dwMemCount) { return 2; }
	++cTab; cchBuffer += 3;
	for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwMemCount; --dwMemCount, ++pMember) {
		if (pMember->IsDeletePending) continue;
		cchBuffer += cTab;
		if (pMember->lpszKeyName) { cchBuffer += pMember->cchKeyName + 4; } //L": " = 2
		if (pMember->IsContainer) { //(pMember->IsObject || pMember->IsArray)
			if (!pMember->dwMemCount) { cchBuffer += 2; }
			else { cchBuffer += _JSONGetStringifySize(pMember, cTab); }
		}
		else if (pMember->IsString) { cchBuffer += pMember->cchValue + 2; }
		else if (pMember->IsStrValue) { cchBuffer += pMember->cchValue; } //(pMember->IsString || pMember->IsBoolean || pMember->IsNull || pMember->IsNumber)
		if (dwMemCount > 1) { cchBuffer += 3; }
	}
	if ((--pMember)->IsDeletePending) { cchBuffer -= 3; if (!cchBuffer) return 2; }
	cchBuffer += 3 + --cTab;
	return cchBuffer;
}

DWORD JSONGetStringifySize(PCJSON pJSON) {
	if (!pJSON || !pJSON->IsContainer || pJSON->IsDeletePending) { SetLastError(ERROR_INVALID_PARAMETER); return 0; } //or ERROR_DELETE_PENDING
	DWORD cchBuffer = 0, cTab = 0, cchRoot = 0;
	if (!pJSON->IsHeader) { ++cTab; cchRoot = 7 + pJSON->cchKeyName + 4; }
	cchBuffer = _JSONGetStringifySize(pJSON, cTab); if (!cchBuffer) return 0; //use GetLastError()
	return (cchRoot + cchBuffer);
}

LPWSTR _JSONStringify(PCJSON pJSON, LPWSTR lpStr, DWORD cTab) {
	PJSON pMember = NULL; LPWSTR lpStart = lpStr; DWORD cStr = 0, dwMemCount = 0;
	//// JSONStringify() check this
	//if (!pJSON || !pJSON->IsContainer || pJSON->IsDeletePending || !lpStr) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; } //or ERROR_INVALID_PARAMETER or ERROR_DELETE_PENDING
	*lpStr = (pJSON->IsArray ? L'[' : L'{'); if (!pJSON->dwMemCount) { ++lpStr; goto EndObj; }
	*++lpStr = L'\r'; *++lpStr = L'\n'; ++lpStr; ++cTab;
	for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwMemCount; --dwMemCount, ++pMember) {
		if (pMember->IsDeletePending) continue;
		for (cStr = cTab; cStr; --cStr, ++lpStr) { *lpStr = L'\t'; }
		if (pMember->lpszKeyName) { //pMember->IsKeyNameSQ - KeyName is Single quote ---> 'KeyName'
			*lpStr = (pMember->IsKeyNameSQ ? L'\'' : L'"'); CopyMemoryEx(++lpStr, pMember->lpszKeyName, pMember->cchKeyName); lpStr += pMember->cchKeyName; *lpStr = (pMember->IsKeyNameSQ ? L'\'' : L'"'); ////*lpStr = _DSQM[pMember->IsKeyNameSQ];
			CopyMemoryEx(++lpStr, L": ", 2); lpStr += 2;
		}
		if (pMember->IsContainer) { //(pMember->IsObject || pMember->IsArray)
			if (!pMember->dwMemCount) { CopyMemoryEx(lpStr, (pMember->IsObject ? L"{}" : L"[]"), 2); lpStr += 2; }
			else if (!(lpStr = _JSONStringify(pMember, lpStr, cTab))) { goto ReturnError; }
		}
		else if (pMember->IsString) { *lpStr = (pMember->IsSingleQuote ? L'\'' : L'"'); CopyMemoryEx(++lpStr, pMember->lpszValue, pMember->cchValue); lpStr += pMember->cchValue; *lpStr = (pMember->IsSingleQuote ? L'\'' : L'"'); ++lpStr; } //// *lpStr = _DSQM[pMember->IsSingleQuote]; -- //pMember->IsEscapeStr; pMember->IsSingleQuote;
		else if (pMember->IsStrValue) { CopyMemoryEx(lpStr, pMember->lpszValue, pMember->cchValue); lpStr += pMember->cchValue; } //(pMember->IsBoolean || pMember->IsNull || pMember->IsNumber) ---> pMember->IsSigned; pMember->IsFloat; pMember->IsHexDigit;
		else { goto ReturnError; } /// ERROR??? It should never happen.
		if (dwMemCount > 1) { CopyMemoryEx(lpStr, L",\r\n", 3); lpStr += 3; }
	}
	if ((--pMember)->IsDeletePending) { lpStr -= 3; if (lpStart == lpStr) { ++lpStr; goto EndObj; }; }
	CopyMemoryEx(lpStr, L"\r\n", 2); lpStr += 2;
	for (cStr = --cTab; cStr; --cStr, ++lpStr) { *lpStr = L'\t'; }
EndObj:
	*lpStr = (pJSON->IsArray ? L']' : L'}'); ++lpStr;
	return lpStr;
ReturnError:
	SetLastError(ERROR_INVALID_DATA);
	return NULL;
}

LPWSTR JSONStringify(PCJSON pJSON) {
	if (!pJSON || !pJSON->IsContainer || pJSON->IsDeletePending) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; } //or ERROR_DELETE_PENDING
	LPWSTR lpszOffset = NULL, lpszBuffer = NULL, lpStr = NULL; DWORD cchBuffer = 0, cTab = 0; //, cchJSONStringify = 0; 
	cchBuffer = JSONGetStringifySize(pJSON); if (!cchBuffer) return NULL;
	if (cchBuffer > 2) cchBuffer += 8; // 1 for NULL, plus 7 char, just for safety, they are not needed
	lpszBuffer = (LPWSTR)MemAlloc((cchBuffer) * sizeof(wchar_t));
	if (!lpszBuffer) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	else if (cchBuffer == 2) { CopyMemoryEx(lpszBuffer, (pJSON->IsObject ? L"{}\0" : L"[]\0"), 3); return lpszBuffer; }
	lpszOffset = lpszBuffer; if (!pJSON->IsHeader) { ++cTab; lpszOffset = &lpszBuffer[(pJSON->cchKeyName + 8)]; }
	lpszOffset = _JSONStringify(pJSON, lpszOffset, cTab);
	if (!lpszOffset) { MemFree(lpszBuffer); return NULL; }
	if (!pJSON->IsHeader) {
		lpStr = lpszBuffer; CopyMemoryEx(lpStr, L"{\r\n\t", 4); lpStr += 4;
		*lpStr = (pJSON->IsKeyNameSQ ? L'\'' : L'"'); CopyMemoryEx(++lpStr, pJSON->lpszKeyName, pJSON->cchKeyName); lpStr += pJSON->cchKeyName; *lpStr = (pJSON->IsKeyNameSQ ? L'\'' : L'"'); *++lpStr = L':'; *++lpStr = L' ';
		CopyMemoryEx(lpszOffset, L"\r\n}\0", 4); lpszOffset += 3;
	}
	*lpszOffset = L'\0'; //cchBuffer = (DWORD)(lpszOffset - lpszBuffer); lpszBuffer[cchBuffer] = L'\0';
	return lpszBuffer;
}

DWORD JSONOpenKey(PCJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPCJSON ppJSONResult) {
	if (!pJSON || !ppJSONResult || !pJSON->IsObject || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; // ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	PCJSON pMember = NULL; DWORD dwMemCount = 0;
	for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwMemCount; --dwMemCount, ++pMember) { ////if (pMember->cchKeyName != cchKeyName || CompareMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) continue;
		if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
			if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
			*ppJSONResult = pMember; return NO_ERROR; //++pMember->cHandle; 
		}
	}
	return ERROR_FILE_NOT_FOUND; //ENOENT
}

DWORD JSONOpenKeyEx(PCJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPCJSON ppJSONResult) {
	if (!pJSON || !ppJSONResult || !pJSON->IsContainer || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; //or ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	PCJSON pMember = pJSON; LPCWSTR lpSubKey = lpszKeyName, lpIndex = NULL; DWORD dwMemCount = 1, cchSubKey = 0, i = 0;
	if (*lpszKeyName == L'[') { cchSubKey = cchKeyName; goto GetKeyIndex; }
	do {
		lpSubKey = wmemchr(lpszKeyName, L'.', cchKeyName); //wcschr(lpszKeyName, L'.');
		lpIndex = wmemchr(lpszKeyName, L'[', (lpSubKey ? (lpSubKey - lpszKeyName) : cchKeyName));
		if (lpIndex) { lpSubKey = lpIndex; } cchSubKey = 0;
		if (lpSubKey) { cchSubKey = cchKeyName - (DWORD)(lpSubKey - lpszKeyName); cchKeyName -= cchSubKey; if (!lpIndex) { ++lpSubKey; --cchSubKey; }; } //if (*lpSubKey == L'.')
		for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwMemCount; --dwMemCount, ++pMember) {
			if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
				if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
				while (cchSubKey && *lpSubKey == L'[') {
				GetKeyIndex:
					i = 0; while (--cchSubKey && !(*++lpSubKey < L'0' || *lpSubKey > L'9')) { i *= 10; i += *lpSubKey - L'0'; }
					if (*lpSubKey != L']') return ERROR_INVALID_PARAMETER; if (--cchSubKey && *++lpSubKey == L'.') { --cchSubKey; ++lpSubKey; };
					if (!pMember->IsContainer || i >= pMember->dwMemCount) return ERROR_INVALID_INDEX;
					pMember = &pMember->lpMembers[i]; if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
				}
				if (cchSubKey) { pJSON = pMember; lpszKeyName = lpSubKey; cchKeyName = cchSubKey; break; }
				else { *ppJSONResult = pMember; return NO_ERROR; } //++pMember->cHandle;
			}
		}
	} while (dwMemCount && cchSubKey && pJSON->IsObject && pJSON->dwMemCount);
	return ERROR_FILE_NOT_FOUND; //ENOENT
}

DWORD JSONEnumKey(PCJSON pJSON, DWORD dwIndex, PPCJSON ppJSONResult) {
	if (!pJSON || !ppJSONResult) return ERROR_INVALID_PARAMETER;
	else if (!pJSON->IsContainer || dwIndex >= pJSON->dwMemCount) return ERROR_NO_MORE_ITEMS;
	*ppJSONResult = &pJSON->lpMembers[dwIndex]; return NO_ERROR; //++pJSON->lpMembers[dwIndex].cHandle;
}


