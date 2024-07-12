/*
 * Copyright (C) 2024 Denis X Alb (DXRW4E)
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



#ifndef _JSON_H_
#define _JSON_H_

#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <errno.h>

 ////#define _JSON_DELETE_ON_CLOSE

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
#ifndef CopyMemory
	#define CopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#endif
#ifndef CopyMemoryEx
	#define CopyMemoryEx(Destination,Source,Length) wmemcpy((Destination),(Source),(Length))
#endif
//#ifndef EqualMemory
//	#define EqualMemory(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
//#endif
#ifndef EqualMemoryEx
	#define EqualMemoryEx(Destination,Source,Length) (!wmemcmp((Destination),(Source),(Length)))
#endif
//#ifndef CompareMemory
//	#define CompareMemory(Destination,Source,Length) memcmp((Destination),(Source),(Length))
//#endif
#ifndef CompareMemoryEx
	#define CompareMemoryEx(Destination,Source,Length) wmemcmp((Destination),(Source),(Length))
#endif
#ifndef MoveMemory
	#define MoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#endif
//#ifndef MoveMemoryEx
//	#define MoveMemoryEx(Destination,Source,Length) wmemmove((Destination),(Source),(Length))
//#endif
//#ifndef _CopyMemory
//	#define _CopyMemory(Destination,Source,Length) (Destination > Source ? memmove((Destination),(Source),(Length)) : memcpy((Destination),(Source),(Length)))
//#endif
#ifndef ZeroMemory
	#define ZeroMemory(Destination, Length) memset((Destination),0,(Length))
#endif
//#ifndef ZeroMemoryEx
//	#define ZeroMemoryEx(Destination, Length) wmemset((Destination),0,(Length))
//#endif


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

#ifndef STRING_COMPARE_FUNCTION
#define STRING_COMPARE_FUNCTION
#ifdef _MSC_VER
#define StringEqual(a, b) (!wcscmp((a), (b)))
#define StringEqualA(a, b) (!strcmp((a), (b)))
#define StringEqualN(a, b, n) (!wcsncmp((a), (b), (n)))
#define StringEqualNA(a, b, n) (!strncmp((a), (b), (n)))
#define StringEqualI(a, b) (!_wcsicmp((a), (b)))
#define StringEqualIA(a, b) (!_stricmp((a), (b)))
#define StringEqualNI(a, b, n) (!_wcsnicmp((a), (b), (n)))
#define StringEqualNIA(a, b, n) (!_strnicmp((a), (b), (n)))
#define StrCmp(a, b) (wcscmp((a), (b)))
#define StrCmpA(a, b) (strcmp((a), (b)))
#define StrCmpN(a, b, n) (wcsncmp((a), (b), (n)))
#define StrCmpNA(a, b, n) (strncmp((a), (b), (n)))
#define StrCmpI(a, b) (_wcsicmp((a), (b)))
#define StrCmpIA(a, b) (_stricmp((a), (b)))
#define StrCmpNI(a, b, n) (_wcsnicmp((a), (b), (n)))
#define StrCmpNIA(a, b, n) (_strnicmp((a), (b), (n)))
#define StringStr(a, b) (wcsstr((a), (b)))
#define StringStrA(a, b) (strstr((a), (b)))
#define StringStrI(a, b) (strstrIW((a), (b)))
#define StringStrIA(a, b) (strstrI((a), (b)))
#else
#define StringEqual(a, b) (!wcscmp((a), (b)))
#define StringEqualA(a, b) (!strcmp((a), (b)))
#define StringEqualN(a, b, n) (!wcsncmp((a), (b), (n)))
#define StringEqualNA(a, b, n) (!strncmp((a), (b), (n)))
#define StringEqualI(a, b) (!wcscasecmp((a), (b)))
#define StringEqualIA(a, b) (!strcasecmp((a), (b)))
#define StringEqualNI(a, b, n) (!wcsncasecmp((a), (b), (n)))
#define StringEqualNIA(a, b, n) (!strncasecmp((a), (b), (n)))
#define StrCmp(a, b) (wcscmp((a), (b)))
#define StrCmpA(a, b) (strcmp((a), (b)))
#define StrCmpN(a, b, n) (wcsncmp((a), (b), (n)))
#define StrCmpNA(a, b, n) (strncmp((a), (b), (n)))
#define StrCmpI(a, b) (wcscasecmp((a), (b)))
#define StrCmpIA(a, b) (strcasecmp((a), (b)))
#define StrCmpNI(a, b, n) (wcsncasecmp((a), (b), (n)))
#define StrCmpNIA(a, b, n) (strncasecmp((a), (b), (n)))
#define StringStr(a, b) (wcsstr((a), (b)))
#define StringStrA(a, b) (strstr((a), (b)))
#define StringStrI(a, b) (wcscasestr((a), (b)))
#define StringStrIA(a, b) (strcasestr((a), (b)))
#endif
#define StringCompare	 StrCmp
#define StringCompareA   StrCmpA
#define StringCompareN   StrCmpN
#define StringCompareNA  StrCmpNA
#define StringCompareI   StrCmpI
#define StringCompareIA  StrCmpIA
#define StringCompareNI  StrCmpNI
#define StringCompareNIA StrCmpNIA
#endif //STRING_COMPARE_FUNCTION

#ifndef ERROR_SUCCESS
	#define ERROR_SUCCESS 0 // NOERROR 0
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
#ifndef ERROR_INVALID_FLAG_NUMBER
	#define ERROR_INVALID_FLAG_NUMBER 186 // EINVAL 22
#endif 
#ifndef ERROR_FILE_NOT_FOUND
	#define ERROR_FILE_NOT_FOUND 2 // ENOFILE 2
#endif 
#ifndef ERROR_PATH_NOT_FOUND
	#define ERROR_PATH_NOT_FOUND 3  // ENOENT 2
#endif 
#ifndef ERROR_INVALID_INDEX
	#define ERROR_INVALID_INDEX 1413  // ENOENT 2
#endif 
#ifndef ERROR_DELETE_PENDING
	#define ERROR_DELETE_PENDING 303  // EBUSY 16 - The file cannot be opened because it is in the process of being deleted.
#endif 
#ifndef ERROR_BUSY
	#define ERROR_BUSY 170  // EBUSY 16 - The requested resource is in use.
#endif 
#ifndef ERROR_SHARING_VIOLATION
	#define ERROR_SHARING_VIOLATION 32  // EBUSY 16
#endif 
#ifndef ERROR_LOCK_VIOLATION
	#define ERROR_LOCK_VIOLATION 33  // EBUSY 16
#endif 
#ifndef ERROR_NO_MORE_SEARCH_HANDLES
	#define ERROR_NO_MORE_SEARCH_HANDLES 113 // ENFILE 23
#endif
#ifndef ERROR_TOO_MANY_OPEN_FILES
	#define ERROR_TOO_MANY_OPEN_FILES 4 // EMFILE 24
#endif
#ifndef ERROR_NO_MORE_FILES
	#define ERROR_NO_MORE_FILES 18 // ENMFILE 89
#endif
#ifndef ERROR_NO_MORE_ITEMS
	#define ERROR_NO_MORE_ITEMS 259 // ENMFILE 89
#endif
#ifndef ERROR_FILE_EXISTS
	#define ERROR_FILE_EXISTS 80 // EEXIST 17
#endif
#ifndef ERROR_ALREADY_EXISTS
	#define ERROR_ALREADY_EXISTS 183 // EEXIST 17
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
				DWORD dwNestedLevel; //dwDelPending;
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
	}JSON_PARSE, * PJSON_PARSE;
#endif // _HANDLE_JSON_

#ifndef _JSON_PARENT_HANDLE_
#define _JSON_PARENT_HANDLE_
	typedef struct _JSONPH {
		union {
			PJSON pJSON;
			PCJSON pcJSON;
		};
		DWORD  dwIndex;
	} JSONPH, * PJSONPH;
#endif // _JSON_PARENT_HANDLE_

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
//// CreateKey && Merge Flags
#define JSON_CREATED_NEW_KEY		 0x00000001  // The key did not exist and was created.
#define JSON_OPENED_EXISTING_KEY	 0x00000002  // The key existed and was simply opened without being changed.
#define JSON_APPENDDATA			     0x00000008
#define JSON_REPLACEONLY			 0x00000010
#define JSON_NOOVERWRITE			 0x00000020
#define JSON_REPLACEONLY_NOOVERWRITE 0x00000030
#define JSON_NOOVERWRITE_VALUE	     0x00000040
#define JSON_OVERWRITEALL			 0x00000080
//#define JSON_NOCREATE				 0x00000100
//#define JSON_NOOCCURRENCE			 0x00000200

#define JSONPtr(pJSON) pJSON
#define JSONContainer(pJSON) (pJSON->IsContainer ? pJSON->lpMembers : NULL)
#define JSONContainerLength(pJSON) (pJSON->IsContainer ? pJSON->dwMemCount : 0)
#define JSONContainerSize(pJSON) (pJSON->IsContainer ? pJSON->dwMemBuffer : 0)
#define JSONMembers JSONContainer
#define JSONMembersLength JSONContainerLength
#define JSONMembersSize JSONContainerSize
#define JSONValueData(pJSON) (pJSON->IsStrValue ? pJSON->lpszValue : NULL)
#define JSONValueLength(pJSON) (pJSON->IsStrValue ? pJSON->cchValue : 0)
#define JSONCounHandle(pJSON) pJSON->cHandle

PJSON JSONInitializeHandle();
#define JSONCreate JSONInitializeHandle
VOID _JSONFree(PJSON pJSON);
VOID JSONFree(PJSON pJSON);
LPWSTR JSONEscapeStr(LPCWSTR lpcString, DWORD cchString, PJSON pJSON);
LPWSTR JSONUnEscapeStr(LPCWSTR lpcString, DWORD cchString, DWORD cchUnEscapeStr);
DWORD JSONToNumber(PCJSON pJSON, PJSON_NUMBER pJSONNum);
DWORD JSONToNumberEx(PJSON pJSON, PJSON_NUMBER pJSONNum);
PJSON JSONParse(LPCWSTR lpSrc, DWORD cSrc, LPCWSTR* lpcszEnd);
DWORD _JSONGetStringifySize(PCJSON pJSON, DWORD cTab);
DWORD JSONGetStringifySize(PCJSON pJSON);
LPWSTR _JSONStringify(PCJSON pJSON, LPWSTR lpStr, DWORD cTab);
LPWSTR JSONStringify(PCJSON pJSON);
DWORD _JSONClose(PPJSON ppJSON);
#define JSONClose(pJSON) _JSONClose(&pJSON)
#define JSONCloseKey JSONClose
#define JSONCloseHandle JSONClose
BOOLEAN _JSONIsHandleBusy(PCJSON pJSON);
BOOLEAN _JSONIsHandleBusyEx(const PJSON pJSON);
BOOLEAN JSONIsHandleBusy(const PJSON pJSON, const BOOLEAN bITH);
DWORD JSONOpenKey(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPJSON ppJSONResult);
#define JSONGetKey JSONOpenKey
DWORD _JSONOpenKeyEx(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPJSON ppJSONResult, PJSONPH pJSONPH);
#define JSONOpenKeyEx(pJSON, lpszKeyName, cchKeyName, ppJSONResult) _JSONOpenKeyEx(pJSON, lpszKeyName, cchKeyName, ppJSONResult, NULL)
#define JSONGetKeyEx JSONOpenKeyEx
DWORD _JSONCreateKey(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, WORD wType, PPJSON ppJSONResult, LPDWORD lpdwDisposition, LPDWORD lpdwIndex);
#define JSONCreateKey(pJSON, lpszKeyName, cchKeyName, wType, ppJSONResult, lpdwDisposition) _JSONCreateKey(pJSON, lpszKeyName, cchKeyName, wType, ppJSONResult, lpdwDisposition, NULL)
DWORD JSONInsertKey(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, WORD wType, PPJSON ppJSONResult, DWORD dwIndex);
DWORD JSONInsertElement(PJSON pJSON, WORD wType, PPJSON ppJSONResult, DWORD dwIndex);
#define JSONCreateElement(pJSON, wType, ppJSONResult) JSONInsertElement(pJSON, wType, ppJSONResult, -1)
DWORD JSONCommitChanges(PJSON pJSON);
DWORD JSONEnumKey(PCJSON pJSON, DWORD dwIndex, PPJSON ppJSONResult);
//DWORD JSONEnumKeyEx(PJSON pJSON, DWORD dwIndex, PPJSON ppJSONResult);
#define JSONOpenKeyByIndex JSONEnumKey
#define JSONOpenElement JSONEnumKey
DWORD _JSONCopyTree(PJSON pJSON, PCJSON pSrcJSON);
DWORD JSONCopyTree(PJSON pJSON, PCJSON pSrcJSON);
PJSON JSONCopyTreeEx(PCJSON pSrcJSON);
DWORD JSONDeleteOnClose(PJSON pJSON);
#define JSONDeleteElement JSONDeleteOnClose
DWORD JSONDeleteByIndex(PJSON pJSON, DWORD dwIndex);
DWORD JSONDeleteKey(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName);
DWORD JSONDeleteKeyEx(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName);
DWORD JSONRenameKey(PJSON pJSON, LPCWSTR lpszNewKeyName, DWORD cchNewKeyName);
#define JSONSetKeyName JSONRenameKey
DWORD JSONSetValue(PJSON pJSON, WORD wValueType, LPCVOID lpData, DWORD dwDataCount);
DWORD JSONAddKeyValue(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, WORD wValueType, LPCVOID lpData, DWORD dwDataCount);
DWORD JSONAppend(PJSON pJSON, PCJSON pJSONToAppend);
DWORD JSONMerge(PJSON pJSON, PCJSON pJSONToMerge, WORD wFlags);
DWORD JSONReserve(PJSON pJSON, DWORD dwReserve);
#ifndef PFN_QSORT_COMPARE
	#define PFN_QSORT_COMPARE
	typedef int(*PFN_QSORTCOMPARE)(const void* i, const void* j);
#endif // PFN_QSORT_COMPARE
VOID _JSONSort(PJSON pJSON, int iLeft, int iRight);
//VOID _JSONSortS(PJSON pMem, int iLeft, int iRight);
//VOID _JSONSortN(PJSON pMem, int iLeft, int iRight);
DWORD JSONSort(PJSON pJSON, DWORD dwStart, DWORD dwEnd);
INT _JSONCompare(PCJSON piMem, PCJSON pjMem);
DWORD JSONQSort(PJSON pJSON, PFN_QSORTCOMPARE JSONCmp);

#ifdef USING_JSONPARSE_EX
#ifdef __cplusplus
DWORD __JSONParse__(PJSON pJSON, LPCWSTR& lpSrc, DWORD& cSrc);
#else
DWORD __JSONParse__(PJSON pJSON, LPCWSTR* lpcSrc, LPDWORD lpdwSrc);
#endif // __JSONParse__
HJSON JSONParseEx(LPCWSTR lpcString, DWORD cchString, LPCWSTR* lpcszEnd);
#endif //USING_JSONPARSE_EX

/*
#define IsCharSpaceEx(wch) (iswspace(wch) || wch == L'\x200B' || wch == L'\xFEFF')
#define IsJSONSpace(wch) ((wch == L'\x0009' || wch == L'\x0020' || wch == L'\x000D' || wch == L'\x000A' || wch == L'\x2028' || wch == L'\x2029'))
#define IsJSONSpaceEx(wch) (wch == L'\0' || IsCharSpaceEx(wch))
#define IsJSONUnEscapeSpace(wch) (wch != L'\x0020' && (wch == L'\x0000' || wch < L'\x0020' || wch == L'\x00A0' || wch == L'\x0085' || wch == L'\x2028' || wch == L'\x2029' || wch == L'\x202F' \
																		|| wch == L'\x1680' || wch == L'\x180E' || (wch > L'\x1FFF' && wch < L'\x200C') || wch == L'\x3000' || wch == L'\xFEFF' || wch == L'\x205F'))
#define IsJSONUnEscapeSpaceEx(wch) (wch != L'\x0020' && (wch < L'\x0020' || IsCharSpaceEx(wch)))

//#ifndef _CHARISSPACEEX_
//	#define _CHARISSPACEEX_
//	#define CharIsSpace(wch) ((wch == L'\x0009' || wch == L'\x0020' || wch == L'\x000D' || wch == L'\x000A' || wch == L'\x00A0' || wch == L'\x000C' \
//							  || wch == L'\x000B' || wch == L'\x0085' || wch == L'\x2028' || wch == L'\x2029' || wch == L'\x202F' || wch == L'\x1680' \
//							  || wch == L'\x180E' || (wch > L'\x1FFF' && wch < L'\x200C') || wch == L'\x3000' || wch == L'\xFEFF' || wch == L'\x205F'))
//	#define CharIsSpaceEx(wch) (wch == L'\0' || CharIsSpace(wch))
//#endif

#ifndef RTL_INTEGER_CHARS
	#define RTL_INTEGER_CHARS
	//static const CHAR RtlIntegerChars[]   = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	static const WCHAR RtlIntegerWChars[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };
#endif // RTL_INTEGER_CHARS
*/

#ifdef __cplusplus
}
#endif

#endif  // _JSON_H_
