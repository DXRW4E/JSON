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

#include "JSONEx.h"
 //#include "/storage/emulated/0/Android/Include/JSONEx.h"

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
//

//#define _JSON_DELETE_ON_CLOSE

PJSON JSONInitializeHandle() {
	PJSON pJSON = (PJSON)MemAllocEx(sizeof(SJSON));
	if (!pJSON) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pJSON->IsHeader = TRUE; pJSON->IsObject = TRUE; return pJSON;
}

VOID _JSONFree(PJSON pJSON) {
	if (!pJSON) return;
	if (pJSON->lpszKeyName) MemFree(pJSON->lpszKeyName);
	if (pJSON->IsContainer) { for (PJSON pMember = pJSON->lpMembers; pJSON->dwMemCount; --pJSON->dwMemCount, ++pMember) { _JSONFree(pMember); }; }
	if (pJSON->lpMembers) MemFree(pJSON->lpMembers); ///lpMembers == lpszValue
	return;
}

VOID JSONFree(PJSON pJSON) {
	if (!pJSON) return;
	BOOLEAN bType = pJSON->IsContainer;
	_JSONFree(pJSON);
	if (pJSON->IsHeader /*&& !pJSON->IsStackMem*/) { MemFree(pJSON); }
	else { ZeroMemory(pJSON, sizeof(SJSON)); pJSON->IsContainer = bType; }
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

DWORD JSONToNumberEx(PJSON pJSON, PJSON_NUMBER pJSONNum) {
	if (!pJSON || !pJSON->IsNumber) return ERROR_INVALID_PARAMETER;
	LPWSTR lpStr = pJSON->lpszValue, lpEnd = NULL; DWORD cCh = pJSON->cchValue;
	if (!pJSON->IsNumInit) {
		if (pJSON->IsFloat) { pJSON->dValue = wcstod(lpStr, &lpEnd); } //if (&lpStr[pJSON->cchValue] != lpEnd) ?????
		else {
			if (*lpStr == L'-' || *lpStr == L'+') { ++lpStr; --cCh; }
			if (pJSON->IsHexDigit) {
				for (cCh -= 2, lpStr += 2; cCh; --cCh, ++lpStr) {
					if (*lpStr >= L'A') {
						if (*lpStr > L'F') {
							if (*lpStr < L'a' || *lpStr > L'f') return ERROR_INVALID_DATA;
							pJSON->illValue = (pJSON->illValue << 4) + (*lpStr - L'a') + 10;
						}
						else pJSON->illValue = (pJSON->illValue << 4) + (*lpStr - L'A') + 10;
					}
					else if (*lpStr < L'0' || *lpStr > L'9') return ERROR_INVALID_DATA;
					else pJSON->illValue = (pJSON->illValue << 4) + (*lpStr - L'0');
				}
			}
			else { while (!(*lpStr < L'0' || *lpStr > L'9')) { pJSON->illValue *= 10; pJSON->illValue += *lpStr - L'0'; ++lpStr; } }
			if (pJSON->IsSigned) pJSON->illValue = -pJSON->illValue;
		}
		pJSON->IsNumInit = TRUE;
	}
	if (pJSONNum) {
		if (pJSON->IsFloat) pJSONNum->dValue = pJSON->dValue; // or (pJSON->wValueType & JSON_IS_FLOAT)
		else if (pJSON->IsSigned) pJSONNum->illValue = pJSON->illValue; // or (pJSON->wValueType & JSON_IS_SIGNED)
		else pJSONNum->ullValue = pJSON->ullValue;
	}
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
	_JSONFree(pHeader); MemFree(pHeader); MemFree(pJSONParse);
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

DWORD _JSONClose(PPJSON ppJSON) {
	PJSON pJSON = (ppJSON ? *ppJSON : NULL);
	if (!pJSON) return ERROR_INVALID_PARAMETER;
	else if (pJSON->IsHeader) { _JSONFree(pJSON); MemFree(pJSON); }
	else if (pJSON->cHandle) --pJSON->cHandle;
	*ppJSON = NULL; return NO_ERROR;
}

BOOLEAN _JSONIsHandleBusy(PCJSON pJSON) {
	PJSON pMember = NULL; DWORD dwCount = 0;
	//if (!pJSON) return FALSE;
	if (pJSON->cHandle) return TRUE;
	else if (pJSON->IsContainer) { for (pMember = pJSON->lpMembers, dwCount = pJSON->dwMemCount; dwCount; --dwCount, ++pMember) { if (_JSONIsHandleBusy(pMember)) return TRUE; }; }
	return FALSE;
}

BOOLEAN _JSONIsHandleBusyEx(const PJSON pJSON) {
	if (!pJSON || !pJSON->IsContainer) return FALSE;
	PJSON pMember = pJSON->lpMembers; DWORD dwCount = pJSON->dwMemCount; while (dwCount) { if (pMember->cHandle) return TRUE; --dwCount; ++pMember; }
	return FALSE;
}

BOOLEAN JSONIsHandleBusy(const PJSON pJSON, const BOOLEAN bITH) {
	PJSON pMember = NULL; DWORD dwCount = 0;
	if (!pJSON) return FALSE;
	else if (pJSON->cHandle > bITH) return TRUE;
	else if (pJSON->IsContainer) { for (pMember = pJSON->lpMembers, dwCount = pJSON->dwMemCount; dwCount; --dwCount, ++pMember) { if (_JSONIsHandleBusy(pMember)) return TRUE; }; }
	return FALSE;
}

DWORD JSONOpenKey(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPJSON ppJSONResult) {
	if (!pJSON || !ppJSONResult || !pJSON->IsObject || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; // ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	PJSON pMember = NULL; DWORD dwMemCount = 0;
	for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwMemCount; --dwMemCount, ++pMember) { ////if (pMember->cchKeyName != cchKeyName || CompareMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) continue;
		if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
			if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
			++pMember->cHandle; *ppJSONResult = pMember; return NO_ERROR;
		}
	}
	return ERROR_FILE_NOT_FOUND; //ENOENT
}

DWORD _JSONOpenKeyEx(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPJSON ppJSONResult, PJSONPH pJSONPH) { //pJSONPH is for Internal Use Only
	if (!pJSON || !ppJSONResult || !pJSON->IsContainer || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; //or ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	PJSON pMember = pJSON; LPCWSTR lpSubKey = lpszKeyName, lpIndex = NULL; DWORD dwMemCount = 1, cchSubKey = 0, i = 0;
	if (*lpszKeyName == L'[') { cchSubKey = cchKeyName; goto GetKeyIndex; }
	do {
		lpSubKey = wmemchr(lpszKeyName, L'.', cchKeyName); //wcschr(lpszKeyName, L'.');
		lpIndex = wmemchr(lpszKeyName, L'[', (lpSubKey ? (lpSubKey - lpszKeyName) : cchKeyName));
		if (lpIndex) { lpSubKey = lpIndex; } cchSubKey = 0;
		if (lpSubKey) { cchSubKey = cchKeyName - (DWORD)(lpSubKey - lpszKeyName); cchKeyName -= cchSubKey; if (!lpIndex) { ++lpSubKey; --cchSubKey; }; } //if (*lpSubKey == L'.')
		for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount, i = 0; dwMemCount; --dwMemCount, ++pMember, ++i) {
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
				else { ++pMember->cHandle; *ppJSONResult = pMember; if (pJSONPH) { pJSONPH->pJSON = pJSON; pJSONPH->dwIndex = i; } return NO_ERROR; }
			}
		}
	} while (dwMemCount && cchSubKey && pJSON->IsObject && pJSON->dwMemCount);
	return ERROR_FILE_NOT_FOUND; //ENOENT
}

DWORD _JSONCreateKey(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, WORD wType, PPJSON ppJSONResult, LPDWORD lpdwDisposition, LPDWORD lpdwIndex) { //wType - see JSON_IS_XXXXX //lpdwIndex is for Internal Use Only
	if (!pJSON || !ppJSONResult || !pJSON->IsObject || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; //ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	PJSON pMember = NULL; DWORD dwCount = 0, i = 0;
	for (pMember = pJSON->lpMembers, dwCount = pJSON->dwMemCount; dwCount; --dwCount, ++pMember, ++i) { ////if (pMember->cchKeyName != cchKeyName || CompareMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) continue;
		if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
			if (lpdwDisposition) *lpdwDisposition = JSON_OPENED_EXISTING_KEY;
			if (lpdwIndex) *lpdwIndex = i;
			if (pMember->IsDeletePending) break;//return ERROR_DELETE_PENDING;
			++pMember->cHandle; *ppJSONResult = pMember; return NO_ERROR;
		}
	}
	if (pJSON->dwMemBuffer) {
		if (pJSON->dwMemBuffer == pJSON->dwMemCount) {
			if (_JSONIsHandleBusyEx(pJSON)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
			pMember = (PJSON)MemReAlloc(pJSON->lpMembers, (pJSON->dwMemBuffer *= 2) * sizeof(SJSON));
			if (!pMember) { pJSON->dwMemBuffer /= 2; return ERROR_NOT_ENOUGH_MEMORY; }
			pJSON->lpMembers = pMember;
		}
	}
	else {
		pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer += 8) * sizeof(SJSON));
		if (!pJSON->lpMembers) { pJSON->dwMemBuffer = 0; return ERROR_NOT_ENOUGH_MEMORY; }
	}
	pMember = &pJSON->lpMembers[pJSON->dwMemCount]; ZeroMemory(pMember, sizeof(SJSON));
	pMember->lpszKeyName = (LPWSTR)MemAlloc((cchKeyName + 1) * sizeof(wchar_t));
	if (!pMember->lpszKeyName) return ERROR_NOT_ENOUGH_MEMORY;
	CopyMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName); ////CopyMemory(pMember->lpszKeyName, lpszKeyName, cchKeyName * sizeof(wchar_t));
	pMember->cchKeyName = cchKeyName; pMember->lpszKeyName[pMember->cchKeyName] = L'\0';
	++pJSON->dwMemCount; if (lpdwDisposition) *lpdwDisposition = JSON_CREATED_NEW_KEY;
	pMember->wValueType = wType; if (lpdwIndex) *lpdwIndex = pJSON->dwMemCount;
	++pMember->cHandle; *ppJSONResult = pMember; return NO_ERROR;
}

DWORD JSONInsertKey(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, WORD wType, PPJSON ppJSONResult, DWORD dwIndex) {
	if (!pJSON || !ppJSONResult || !pJSON->IsObject || pJSON->IsDeletePending || !(wType & JSON_VALUE_TYPE)) return ERROR_INVALID_PARAMETER; //ERROR_DELETE_PENDING
	else if (dwIndex == -1) dwIndex = pJSON->dwMemCount;
	else if (dwIndex > pJSON->dwMemCount) return ERROR_INVALID_INDEX;
	PJSON pMember = NULL; SJSON sJSON; DWORD dwDisposition = 0, dwiOpen = 0, dwLastError = 0, i = 0, dwCount = 0; BOOLEAN bUp = FALSE;
	dwLastError = _JSONCreateKey(pJSON, lpszKeyName, cchKeyName, wType, &pMember, &dwDisposition, &dwiOpen);
	if (dwLastError) return dwLastError; --pMember->cHandle;
	if (dwDisposition == JSON_OPENED_EXISTING_KEY && dwIndex == pJSON->dwMemCount) { return ERROR_INVALID_INDEX; }
	else if (dwiOpen == dwIndex) return NO_ERROR;
	else if (_JSONIsHandleBusyEx(pJSON)) { return ERROR_BUSY; } // EBUSY - Sub Handle is Open, Edit Pending
	else if (dwiOpen > dwIndex) { bUp = TRUE; dwCount = (dwiOpen - dwIndex); i = dwIndex; }
	else { dwCount = (dwIndex - dwiOpen); i = dwiOpen; }
	++pMember->cHandle; CopyMemory(&sJSON, pMember, sizeof(SJSON)); pMember = &pJSON->lpMembers[i];
	if (bUp) { MoveMemory(&pMember[1], pMember, dwCount * sizeof(SJSON)); }
	else { CopyMemory(pMember, &pMember[1], dwCount * sizeof(SJSON)); }
	*ppJSONResult = &pJSON->lpMembers[dwIndex]; CopyMemory(*ppJSONResult, &sJSON, sizeof(SJSON));
	return NO_ERROR;
}

DWORD JSONInsertElement(PJSON pJSON, WORD wType, PPJSON ppJSONResult, DWORD dwIndex) { //dwIndex = -1, wType - see JSON_IS_XXXXX
	if (!pJSON || !ppJSONResult || !pJSON->IsArray || pJSON->IsDeletePending || !(wType & JSON_VALUE_TYPE)) return ERROR_INVALID_PARAMETER; //ERROR_DELETE_PENDING
	PJSON pMember = NULL; DWORD dwCount = 0;
	if (dwIndex == -1) dwIndex = pJSON->dwMemCount;
	else if (dwIndex > pJSON->dwMemCount) return ERROR_INVALID_INDEX;
	else dwCount = (pJSON->dwMemCount - dwIndex);
	if (pJSON->dwMemBuffer) {
		if (pJSON->dwMemBuffer == pJSON->dwMemCount) {
			if (_JSONIsHandleBusyEx(pJSON)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
			pMember = (PJSON)MemReAlloc(pJSON->lpMembers, (pJSON->dwMemBuffer *= 2) * sizeof(SJSON));
			if (!pMember) { pJSON->dwMemBuffer /= 2; return ERROR_NOT_ENOUGH_MEMORY; }
			pJSON->lpMembers = pMember;
		}
	}
	else {
		pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer += 8) * sizeof(SJSON));
		if (!pJSON->lpMembers) { pJSON->dwMemBuffer = 0; return ERROR_NOT_ENOUGH_MEMORY; }
	}
	pMember = &pJSON->lpMembers[dwIndex];
	if (dwCount) { MoveMemory(&pMember[1], pMember, dwCount * sizeof(SJSON)); }
	ZeroMemory(pMember, sizeof(SJSON)); pMember->wValueType = wType;
	++pJSON->dwMemCount; ++pMember->cHandle; *ppJSONResult = pMember; return NO_ERROR;
}

DWORD JSONCommitChanges(PJSON pJSON) {
	if (!pJSON || !pJSON->IsContainer || !pJSON->dwMemCount) { SetLastError(ERROR_INVALID_PARAMETER); return -1; }
	else if (_JSONIsHandleBusyEx(pJSON)) { SetLastError(ERROR_BUSY); return -1; } //EBUSY - Sub Handle is Open, Edit Pending
	PJSON pMember = NULL; DWORD dwMemCount = 0, dwIndex = 0, dwCount = 0;
	for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwIndex < dwMemCount; ++dwIndex, ++pMember) {
		if (pMember->IsDeletePending) {
			if (_JSONIsHandleBusy(pJSON)) { SetLastError(ERROR_BUSY); return -1; } //EBUSY - Sub Handle is Open, Edit Pending
			dwCount = (--dwMemCount - dwIndex); _JSONFree(pMember); --dwIndex; --pJSON->dwMemCount;
			if (!dwCount) break;
			CopyMemory(pMember, &pMember[1], dwCount * sizeof(SJSON)); //since lpDest < lpSource, CopyMemory() should by always Ok, or if you want to be more safe use MoveMemory()
		}
	}
	return pJSON->dwMemCount;
}

DWORD JSONEnumKey(PCJSON pJSON, DWORD dwIndex, PPJSON ppJSONResult) {
	if (!pJSON || !ppJSONResult) return ERROR_INVALID_PARAMETER;
	else if (!pJSON->IsContainer || dwIndex >= pJSON->dwMemCount) return ERROR_NO_MORE_ITEMS;
	*ppJSONResult = &pJSON->lpMembers[dwIndex]; (*ppJSONResult)->cHandle += 1; 
	return NO_ERROR;
}

//DWORD JSONEnumKeyEx(PJSON pJSON, DWORD dwIndex, PPJSON ppJSONResult) {
//	if (!pJSON || !ppJSONResult) return ERROR_INVALID_PARAMETER;
//	else if (!pJSON->IsContainer || dwIndex >= pJSON->dwMemCount) { pJSON->dwDelPending = 0; return ERROR_NO_MORE_ITEMS; }
//	PJSON pMember = &pJSON->lpMembers[dwIndex];
//	if (pMember->cHandle == 1) {
//		dwIndex += pJSON->dwDelPending; pMember += pJSON->dwDelPending;
//		while (pMember->IsDeletePending) { ++pMember; ++pJSON->dwDelPending; if (++dwIndex == pJSON->dwMemCount) { pJSON->dwDelPending = 0; return ERROR_NO_MORE_ITEMS; }; }
//	}
//	++pMember->cHandle; *ppJSONResult = pMember;
//	return NO_ERROR;
//}

DWORD _JSONCopyTree(PJSON pJSON, PCJSON pSrcJSON) {
	//// JSONCopyTree() check this
	//if (!pJSON || !pSrcJSON || pSrcJSON->IsDeletePending) return ERROR_INVALID_PARAMETER; //or ERROR_INVALID_PARAMETER or ERROR_DELETE_PENDING
	PJSON pSrcMember = pSrcJSON->lpMembers, pMember = NULL; DWORD dwMemCount = pSrcJSON->dwMemCount;
	CopyMemory(pJSON, pSrcJSON, sizeof(SJSON)); pJSON->lpMembers = NULL; pJSON->dwMemCount = 0; pJSON->wHandleType = pSrcJSON->IsHeader;
	if (pSrcJSON->lpszKeyName) {
		pJSON->lpszKeyName = (LPWSTR)MemAlloc((pJSON->cchKeyName + 1) * sizeof(wchar_t));
		if (!pJSON->lpszKeyName) goto ReturnError;
		CopyMemoryEx(pJSON->lpszKeyName, pSrcJSON->lpszKeyName, pJSON->cchKeyName); ////CopyMemory(pJSON->lpszKeyName, pSrcJSON->lpszKeyName, pJSON->cchKeyName * sizeof(wchar_t));
		pJSON->lpszKeyName[pJSON->cchKeyName] = L'\0';
	}
	if (pSrcJSON->IsContainer) {
		if (!pSrcJSON->dwMemCount) { pJSON->dwMemBuffer = 0; return NO_ERROR; }
		pJSON->lpMembers = (PJSON)MemAlloc(pSrcJSON->dwMemCount * sizeof(SJSON));
		if (!pJSON->lpMembers) goto ReturnError;
		pJSON->dwMemBuffer = pSrcJSON->dwMemCount;
		for (pMember = pJSON->lpMembers; dwMemCount; --dwMemCount, ++pSrcMember) {
			if (pSrcMember->IsDeletePending) continue;
			else if (_JSONCopyTree(pMember, pSrcMember)) goto ReturnError;
			++pMember; ++pJSON->dwMemCount;
		}
	}
	else if (pSrcJSON->lpszValue) {
		pJSON->lpszValue = (LPWSTR)MemAlloc((pSrcJSON->cchValue + 1) * sizeof(wchar_t));
		if (!pJSON->lpszValue) goto ReturnError;
		CopyMemoryEx(pJSON->lpszValue, pSrcJSON->lpszValue, pSrcJSON->cchValue);
		pJSON->cchValue = pSrcJSON->cchValue; pJSON->lpszValue[pJSON->cchValue] = L'\0';
	}
	return NO_ERROR;
ReturnError:
	_JSONFree(pJSON);
	return ERROR_NOT_ENOUGH_MEMORY;
}

DWORD JSONCopyTree(PJSON pJSON, PCJSON pSrcJSON) {
	if (!pJSON || !pSrcJSON || pSrcJSON->IsDeletePending) return ERROR_INVALID_PARAMETER; //or ERROR_INVALID_PARAMETER or ERROR_DELETE_PENDING
	else if ((pJSON->IsHeader || pSrcJSON->IsHeader) && pJSON->IsHeader != pSrcJSON->IsHeader) return ERROR_INVALID_PARAMETER;
	else if (JSONIsHandleBusy(pJSON, TRUE)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
	else { _JSONFree(pJSON); }
	return _JSONCopyTree(pJSON, pSrcJSON);
}

PJSON JSONCopyTreeEx(PCJSON pSrcJSON) {
	if (!pSrcJSON || pSrcJSON->IsDeletePending) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }; //or ERROR_DELETE_PENDING
	PJSON pHeader = NULL, pJSON = (PJSON)MemAllocEx(sizeof(SJSON)); DWORD dwLastError = 0;
	if (!pJSON) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	dwLastError = _JSONCopyTree(pJSON, pSrcJSON);
	if (dwLastError) { MemFree(pJSON); SetLastError(dwLastError); return NULL; }
	else if (!pJSON->IsHeader) {
		if (pJSON->IsObject) pJSON->IsHeader = TRUE;
		else {
			pHeader = (PJSON)MemAllocEx(sizeof(SJSON));
			if (!pHeader) { _JSONFree(pJSON); MemFree(pJSON); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
			pHeader->IsHeader = TRUE; pHeader->IsObject = TRUE; pHeader->lpMembers = pJSON; pHeader->dwMemBuffer = 1; pHeader->dwMemCount = 1; pJSON = pHeader;
		}
	}
	return pJSON;
}

DWORD JSONDeleteOnClose(PJSON pJSON) {
	if (!pJSON || pJSON->IsHeader) return ERROR_INVALID_PARAMETER; ////if (pJSON->IsDeletePending) return ERROR_DELETE_PENDING;
	pJSON->IsDeletePending = TRUE; return NO_ERROR;
}

DWORD JSONDeleteByIndex(PJSON pJSON, DWORD dwIndex) {
	if (!pJSON) return ERROR_INVALID_PARAMETER;
	PJSON pMember = &pJSON->lpMembers[dwIndex]; DWORD dwCount = 0;
	if (!pJSON->IsContainer || dwIndex >= pJSON->dwMemCount) return ERROR_NO_MORE_ITEMS;
	else if (pJSON->IsDeletePending || pMember->IsDeletePending) return NO_ERROR; //ERROR_DELETE_PENDING
#ifdef _JSON_DELETE_ON_CLOSE
	pMember->IsDeletePending = TRUE; return NO_ERROR;
#endif
	if (_JSONIsHandleBusy(pMember)) { pMember->IsDeletePending = TRUE; return NO_ERROR; }
	_JSONFree(pMember); dwCount = --pJSON->dwMemCount - dwIndex;
	if (dwCount) { CopyMemory(pMember, &pMember[1], dwCount * sizeof(SJSON)); }
	return NO_ERROR;
}

DWORD JSONDeleteKey(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName) {
	if (!pJSON || !pJSON->IsObject || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; //or ERROR_DELETE_PENDING or ERROR_NO_MORE_FILES
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	PJSON pMember = NULL; DWORD dwCount = 0, dwIndex = 0;
	for (pMember = pJSON->lpMembers; dwIndex < pJSON->dwMemCount; ++dwIndex, ++pMember) { ////if (pMember->cchKeyName != cchKeyName || CompareMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) continue;
		if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) { break; }
	}
	if (dwIndex == pJSON->dwMemCount) return ERROR_FILE_NOT_FOUND; //ENOENT
	else if (pMember->IsDeletePending) return NO_ERROR; //ERROR_DELETE_PENDING
#ifdef _JSON_DELETE_ON_CLOSE
	pMember->IsDeletePending = TRUE; return NO_ERROR;
#endif
	if (_JSONIsHandleBusy(pMember)) { pMember->IsDeletePending = TRUE; return NO_ERROR; }
	_JSONFree(pMember); dwCount = --pJSON->dwMemCount - dwIndex;
	if (dwCount) { CopyMemory(pMember, &pMember[1], dwCount * sizeof(SJSON)); }
	return NO_ERROR;
}

DWORD JSONDeleteKeyEx(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName) {
	if (!pJSON) return ERROR_INVALID_PARAMETER;
	PJSON pMember = NULL; JSONPH hP = { 0 }; DWORD dwLastError = 0, dwCount = 0;
	dwLastError = _JSONOpenKeyEx(pJSON, lpszKeyName, cchKeyName, &pMember, &hP);
	if (dwLastError) return ((dwLastError == ERROR_DELETE_PENDING) ? NO_ERROR : dwLastError);
	--pMember->cHandle;
#ifdef _JSON_DELETE_ON_CLOSE
	pMember->IsDeletePending = TRUE; return NO_ERROR;
#endif
	if (_JSONIsHandleBusy(pMember)) { pMember->IsDeletePending = TRUE; return NO_ERROR; }
	_JSONFree(pMember); dwCount = --hP.pJSON->dwMemCount - hP.dwIndex;
	if (dwCount) { CopyMemory(pMember, &pMember[1], dwCount * sizeof(SJSON)); }
	return NO_ERROR;
}

DWORD JSONRenameKey(PJSON pJSON, LPCWSTR lpszNewKeyName, DWORD cchNewKeyName) {
	if (!pJSON || pJSON->IsHeader || pJSON->IsDeletePending || !pJSON->lpszKeyName || !lpszNewKeyName) return ERROR_INVALID_PARAMETER; //or ERROR_DELETE_PENDING
	else if (!cchNewKeyName && !(cchNewKeyName = StrLen(lpszNewKeyName))) return ERROR_INVALID_DATA;
	LPWSTR lpszKeyName = (LPWSTR)MemAlloc((cchNewKeyName + 1) * sizeof(wchar_t));
	if (!lpszKeyName) return ERROR_NOT_ENOUGH_MEMORY;
	if (pJSON->lpszKeyName) MemFree(pJSON->lpszKeyName);
	pJSON->lpszKeyName = lpszKeyName; pJSON->cchKeyName = cchNewKeyName;
	CopyMemoryEx(pJSON->lpszKeyName, lpszNewKeyName, pJSON->cchKeyName); pJSON->lpszKeyName[pJSON->cchKeyName] = L'\0';
	return NO_ERROR;
}

DWORD JSONSetValue(PJSON pJSON, WORD wValueType, LPCVOID lpData, DWORD dwDataCount) {
	if (!pJSON || pJSON->IsHeader) return ERROR_INVALID_PARAMETER;
	PJSON pMember = NULL; SJSON sJSON, sNewKey;
	////SJSON sNewKey = { 0 }, sJSON = { NULL, NULL, (PJSON)lpData, 0, 0, dwDataCount, dwDataCount, wValueType, JSON_IS_HEADER_STACKMEM, 0 };
	if (!lpData || !(wValueType & JSON_VALUE_TYPE)) return ERROR_INVALID_PARAMETER;
	else if (/*pJSON->IsDeletePending ||*/ wValueType & JSON_IS_DELETEPENDING) return ERROR_DELETE_PENDING;
	else if (JSONIsHandleBusy(pJSON, TRUE)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
	else if (pJSON->IsContainer) { for (pMember = pJSON->lpMembers; pJSON->dwMemCount; --pJSON->dwMemCount, ++pMember) { _JSONFree(pMember); } pJSON->dwMemBuffer = 0; }
	if (pJSON->lpszValue) { MemFree(pJSON->lpszValue); pJSON->lpszValue = NULL; pJSON->cchValue = 0; } //(lpMembers == lpszValue && dwMemCount == cchValue)
	pJSON->wValueType = wValueType;
	if (pJSON->IsContainer) { //(wValueType & JSON_IS_CONTAINER)
		ZeroMemory(&sJSON, sizeof(SJSON)); //ZeroMemory(&sNewKey, sizeof(SJSON));
		sJSON.wValueType = wValueType; sJSON.lpMembers = (PJSON)lpData; sJSON.dwMemBuffer = dwDataCount; sJSON.dwMemCount = dwDataCount;
		if (_JSONCopyTree(&sNewKey, &sJSON)) return ERROR_NOT_ENOUGH_MEMORY;
		else { pJSON->lpMembers = sNewKey.lpMembers; pJSON->dwMemBuffer = sNewKey.dwMemBuffer; pJSON->dwMemCount = sNewKey.dwMemCount; }
	}
	else { //pJSON->IsStrValue - (wValueType & JSON_IS_STRVALUE)
		if (!dwDataCount) { dwDataCount = StrLen((LPCWSTR)lpData); }
		if (dwDataCount && pJSON->IsString) { //(pJSON->wValueType & JSON_IS_STRING)
			pJSON->lpszValue = JSONEscapeStr((LPCWSTR)lpData, dwDataCount, pJSON);
			return (pJSON->lpszValue ? NO_ERROR : ERROR_NOT_ENOUGH_MEMORY);
		}
		pJSON->lpszValue = (LPWSTR)MemAlloc((dwDataCount + 1) * sizeof(wchar_t));
		if (!pJSON->lpszValue) { pJSON->cchValue = 0; return ERROR_NOT_ENOUGH_MEMORY; }
		pJSON->cchValue = dwDataCount;
		CopyMemoryEx(pJSON->lpszValue, (LPCWSTR)lpData, dwDataCount); ////CopyMemory(pJSON->lpszValue, lpszValue, cchValue * sizeof(wchar_t));
		pJSON->lpszValue[pJSON->cchValue] = L'\0';
	}
	return NO_ERROR;
}

DWORD JSONAddKeyValue(PJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, WORD wValueType, LPCVOID lpData, DWORD dwDataCount) { // JSONCreate()
	if (!pJSON || pJSON->IsHeader || !pJSON->IsObject || pJSON->IsDeletePending || !lpData || !(wValueType & JSON_VALUE_TYPE)) return ERROR_INVALID_PARAMETER; //or ERROR_DELETE_PENDING
	PJSON pMember = NULL; DWORD dwLastError = NO_ERROR, dwDisposition = 0;
	dwLastError = _JSONCreateKey(pJSON, lpszKeyName, cchKeyName, wValueType, &pMember, &dwDisposition, NULL);
	if (dwLastError) return dwLastError;
	else if (dwDisposition == JSON_OPENED_EXISTING_KEY) { --pMember->cHandle; return ERROR_ALREADY_EXISTS; }
	dwLastError = JSONSetValue(pMember, wValueType, lpData, dwDataCount); --pMember->cHandle;
	if (dwLastError) { MemFree(pMember->lpszKeyName); ZeroMemory(pMember, sizeof(SJSON)); --pJSON->dwMemCount; }
	return dwLastError;
}

DWORD JSONAppend(PJSON pJSON, PCJSON pJSONToAppend) {
	if (!pJSON || !pJSONToAppend || !pJSON->IsContainer || pJSON->IsDeletePending || !pJSONToAppend->IsContainer || !pJSONToAppend->dwMemCount || pJSONToAppend->IsDeletePending) return ERROR_INVALID_PARAMETER; //or ERROR_DELETE_PENDING
	PJSON pTmpJSON = NULL, pMember = NULL; DWORD dwCount = pJSON->dwMemCount + pJSONToAppend->dwMemCount;
	if (pJSON->dwMemBuffer) {
		if (dwCount >= pJSON->dwMemBuffer) {
			if (_JSONIsHandleBusyEx(pJSON)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
			pMember = (PJSON)MemReAlloc(pJSON->lpMembers, (dwCount + 8) * sizeof(SJSON));
			if (!pMember) { return ERROR_NOT_ENOUGH_MEMORY; }
			pJSON->lpMembers = pMember; pJSON->dwMemBuffer = (dwCount + 8);
		}
	}
	else {
		pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer = dwCount + 8) * sizeof(SJSON));
		if (!pJSON->lpMembers) { pJSON->dwMemBuffer = 0; return ERROR_NOT_ENOUGH_MEMORY; }
	}
	pTmpJSON = (PJSON)MemAlloc(sizeof(SJSON));
	if (!pTmpJSON) return ERROR_NOT_ENOUGH_MEMORY;
	if (_JSONCopyTree(pTmpJSON, pJSONToAppend)) { MemFree(pTmpJSON); return ERROR_NOT_ENOUGH_MEMORY; }
	CopyMemory(&pJSON->lpMembers[pJSON->dwMemCount], pTmpJSON->lpMembers, pTmpJSON->dwMemCount * sizeof(SJSON));
	pJSON->dwMemCount += pTmpJSON->dwMemCount; MemFree(pTmpJSON);
	return NO_ERROR;
}

DWORD JSONMerge(PJSON pJSON, PCJSON pJSONToMerge, WORD wFlags) {
	if (!pJSON || !pJSON->IsContainer || !pJSONToMerge || !pJSONToMerge->IsContainer || pJSONToMerge->IsDeletePending) return ERROR_INVALID_PARAMETER; //or ERROR_INVALID_PARAMETER or ERROR_DELETE_PENDING
	else if (!pJSONToMerge->dwMemCount) return NO_ERROR;
	else if (wFlags & JSON_APPENDDATA) return JSONAppend(pJSON, pJSONToMerge);
	PJSON pMember = NULL, pMem = NULL, pMemTM = NULL; DWORD dwMemCount = 0, dwMemTMCount = 0, dwError = 0, dwIndex = 0; BOOLEAN bFound = FALSE;
	//if (wFlags & JSON_REPLACEONLY_NOOVERWRITE) {
	if (pJSON->IsContainer != pJSONToMerge->IsContainer) { pMember = pJSON->lpMembers; pMemTM = pJSONToMerge->lpMembers; goto CheckKey; }
	for (pMemTM = pJSONToMerge->lpMembers, dwMemTMCount = pJSONToMerge->dwMemCount; dwMemTMCount; --dwMemTMCount, ++pMemTM, ++dwIndex) {
		pMember = NULL; bFound = FALSE; if (pMemTM->IsDeletePending) continue;
		if (pJSON->IsObject) { //if (pJSON->IsObject && pJSONToMerge->IsObject) {
			for (pMem = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwMemCount; --dwMemCount, ++pMem) { ////if (pMem->cchKeyName != cchKeyName || CompareMemoryEx(pMem->lpszKeyName, lpszKeyName, cchKeyName)) continue;
				//if (pMem->IsDeletePending) continue;
				if (pMem->cchKeyName == pMemTM->cchKeyName && EqualMemoryEx(pMem->lpszKeyName, pMemTM->lpszKeyName, pMemTM->cchKeyName)) { pMember = pMem; bFound = TRUE; break; }
			}
		}
		else if (dwIndex < pJSON->dwMemCount) { pMember = &pJSON->lpMembers[dwIndex]; bFound = TRUE; /*if (pMember->IsDeletePending) continue;*/ } ////if (pMember->ValueType == pMemTM->ValueType)
		if (pMember) {
		CheckKey:
			if (pMember->IsContainer && (pMember->IsContainer == pMemTM->IsContainer)) { dwError = JSONMerge(pMember, pMemTM, wFlags); if (dwError) { return dwError; }; continue; }
			else if (wFlags & JSON_NOOVERWRITE) continue;
			_JSONFree(pMember);
		}
		else if (wFlags & JSON_REPLACEONLY) continue;
		else if (pJSON->dwMemBuffer) {
			if (pJSON->dwMemCount == pJSON->dwMemBuffer) {
				if (_JSONIsHandleBusyEx(pJSON)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
				pMember = (PJSON)MemReAlloc(pJSON->lpMembers, (pJSON->dwMemBuffer *= 2) * sizeof(SJSON));
				if (!pMember) { pJSON->dwMemBuffer /= 2; return ERROR_NOT_ENOUGH_MEMORY; }
				pJSON->lpMembers = pMember; pMember += pJSON->dwMemCount;
			}
			else { pMember = &pJSON->lpMembers[pJSON->dwMemCount]; }
		}
		else {
			pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer = 8) * sizeof(SJSON));
			if (!pJSON->lpMembers) { pJSON->dwMemBuffer = 0; return ERROR_NOT_ENOUGH_MEMORY; }
			pMember = pJSON->lpMembers;
		}
		if (_JSONCopyTree(pMember, pMemTM)) return ERROR_NOT_ENOUGH_MEMORY;
		if (!bFound) ++pJSON->dwMemCount;
	}
	//}
	return NO_ERROR;
}

DWORD JSONReserve(PJSON pJSON, DWORD dwReserve) {
	if (!pJSON || !pJSON->IsContainer || pJSON->dwMemBuffer >= dwReserve) return ERROR_INVALID_PARAMETER;
	else if (pJSON->dwMemBuffer) {
		if (_JSONIsHandleBusyEx(pJSON)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
		PJSON pMember = (PJSON)MemReAlloc(pJSON->lpMembers, dwReserve * sizeof(SJSON));
		if (!pMember) return ERROR_NOT_ENOUGH_MEMORY;
		pJSON->lpMembers = pMember;
	}
	else {
		pJSON->lpMembers = (PJSON)MemAlloc(dwReserve * sizeof(SJSON));
		if (!pJSON->lpMembers) return ERROR_NOT_ENOUGH_MEMORY;
	}
	pJSON->dwMemBuffer = dwReserve; return NO_ERROR;
}

VOID _JSONSort(PJSON pJSON, int iLeft, int iRight) {
	int i = iLeft, j = iRight; SJSON sJSON, * pMem = pJSON->lpMembers; LPCWSTR lpString; JSON_NUMBER JSONNum; JSONNum.illValue = pMem[(iLeft + iRight) / 2].illValue; //JSONNum = { .illValue = pMem[(iLeft + iRight) / 2].illValue };
	do {
		if (pJSON->IsObject) { //StrCmp/StringCompare - StrCmpN/StringCompareN - StrCmpI/StringCompareI - StrCmpNI/StringCompareNI
			lpString = pMem[(iLeft + iRight) / 2].lpszKeyName;
			while ((StringCompareI(pMem[i].lpszKeyName, lpString) < 0) && (i < iRight)) { ++i; }
			while ((StringCompareI(pMem[j].lpszKeyName, lpString) > 0) && (j > iLeft)) { --j; }
		} //else if (pJSON->IsArray) {
		else if (pMem[i].IsString) { //pJSON->IsArray, compare lpszValue
			lpString = pMem[(iLeft + iRight) / 2].lpszValue;
			while ((StringCompareI(pMem[i].lpszValue, lpString) < 0) && (i < iRight)) { ++i; }
			while ((StringCompareI(pMem[j].lpszValue, lpString) > 0) && (j > iLeft)) { --j; }
		}
		else if (pMem[i].IsNumber) {
			if (pMem[i].IsFloat) { // use dValue or fValue to compare DOUBLE or float
				while (pMem[i].dValue < JSONNum.dValue && (i < iRight)) { ++i; }
				while (pMem[j].dValue > JSONNum.dValue && (j > iLeft)) { --j; }
			}
			else if (pMem[i].IsSigned) { // use illValue to compare INT64 or INT iValue
				while (pMem[i].illValue < JSONNum.illValue && (i < iRight)) { ++i; }
				while (pMem[j].illValue > JSONNum.illValue && (j > iLeft)) { --j; }
			}
			else { //use ullValue to compare UINT64 or UINT uValue
				while (pMem[i].ullValue < JSONNum.ullValue && (i < iRight)) { ++i; }
				while (pMem[j].ullValue > JSONNum.ullValue && (j > iLeft)) { --j; }
			}
		}
		else { ++i; --j; continue; }
		if (i <= j) { sJSON = pMem[i]; pMem[i] = pMem[j]; pMem[j] = sJSON; ++i; --j; }
	} while (i <= j);
	if (iLeft < j) { _JSONSort(pJSON, iLeft, j); }
	if (i < iRight) { _JSONSort(pJSON, i, iRight); }
}

DWORD JSONSort(PJSON pJSON, DWORD dwStart, DWORD dwEnd) {
	if (!pJSON || !pJSON->IsContainer || pJSON->dwMemCount < 2 || dwEnd >= pJSON->dwMemCount) return ERROR_INVALID_PARAMETER;
	else if (!dwEnd) dwEnd = (pJSON->dwMemCount - 1); //dwEnd = (pJSON->dwMemCount ? (pJSON->dwMemCount - 1) : 0);
	else if (dwStart >= dwEnd) return ERROR_INVALID_PARAMETER;
	else if (/*pJSON->cHandle > 1 ||*/ _JSONIsHandleBusyEx(pJSON)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
	_JSONSort(pJSON, dwStart, dwEnd); //_JSONSortN(pJSON, dwStart, dwEnd);
	return NO_ERROR;
}

INT _JSONCompare(PCJSON piMem, PCJSON pjMem) {
	if (piMem->lpszKeyName) return StringCompareI(piMem->lpszKeyName, pjMem->lpszKeyName); //StrCmp/StringCompare - StrCmpN/StringCompareN - StrCmpI/StringCompareI - StrCmpNI/StringCompareNI
	else if (piMem->IsString) return StringCompareI(piMem->lpszValue, pjMem->lpszValue); //pJSON->IsArray, compare lpszValue
	else if (piMem->IsNumber) {
		if (piMem->IsFloat) return (INT)(piMem->dValue - pjMem->dValue); // use dValue or fValue to compare DOUBLE or float
		else if (piMem->IsSigned) return (INT)(piMem->illValue - pjMem->illValue); // use illValue to compare INT64 or INT iValue
		else return (INT)(piMem->ullValue - pjMem->ullValue); //use ullValue to compare UINT64 or UINT uValue
	}
	else return 0;
}

DWORD JSONQSort(PJSON pJSON, PFN_QSORTCOMPARE JSONCmp) {
	if (!pJSON || !pJSON->IsContainer || pJSON->dwMemCount < 2) return ERROR_INVALID_PARAMETER;
	else if (/*pJSON->cHandle > 1 ||*/ _JSONIsHandleBusyEx(pJSON)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
	qsort(pJSON->lpMembers, pJSON->dwMemCount, sizeof(SJSON), (PFN_QSORTCOMPARE)JSONCmp);
	return NO_ERROR;
}

#ifdef USING_JSONPARSE_EX
#ifdef __cplusplus
DWORD __JSONParse__(PJSON pJSON, LPCWSTR& lpSrc, DWORD& cSrc) {
#else
DWORD __JSONParse__(PJSON pJSON, LPCWSTR * lpcSrc, LPDWORD lpdwSrc) {
	LPCWSTR lpSrc = *lpcSrc; DWORD cSrc = *lpdwSrc;
#endif
	LPCWSTR lpStr = NULL; LPWSTR lpEnd = NULL; PJSON pMember = NULL;
	DWORD cCh = 0, dwLastError = ERROR_INVALID_DATA; WCHAR wCh = 0;
	if (!pJSON || !lpSrc) { return ERROR_INVALID_PARAMETER; }
	pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer += 8) * sizeof(SJSON));
	if (!pJSON->lpMembers) { pJSON->dwMemBuffer = 0; return ERROR_NOT_ENOUGH_MEMORY; }
	pMember = pJSON->lpMembers;
	while (cSrc && IsJSONSpaceEx(*lpSrc)) { --cSrc; ++lpSrc; }
	if (!pJSON->IsArray) { if (!cSrc || *lpSrc != L'{') { goto ReturnError; } pJSON->IsObject = TRUE; }
	do {
		do { ++lpSrc; --cSrc; } while (cSrc && IsJSONSpaceEx(*lpSrc));
		if (pJSON->dwMemCount == pJSON->dwMemBuffer) {
			pMember = (PJSON)MemReAlloc(pJSON->lpMembers, (pJSON->dwMemBuffer *= 2) * sizeof(SJSON));
			if (!pMember) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
			pJSON->lpMembers = pMember; pMember += pJSON->dwMemCount; ////ZeroMemory(&pMember[pJSON->dwMemCount], pJSON->dwMemCount * sizeof(SJSON));
		}
		ZeroMemory(pMember, sizeof(SJSON)); //pMember->Header = pJSON->Header;
		if (!pJSON->IsArray) {
			if (!cSrc || (*lpSrc != L'"' && *lpSrc != L'\'')) break; //{ if (pJSON->dwMemCount) goto ReturnError; break; }
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
			for (; cSrc && (*lpSrc == L'.' || *lpSrc == L'-' || *lpSrc == L'+' || IsCharXDigit(*lpSrc)); --cSrc, ++lpSrc) { if (*lpSrc == L'.') pMember->IsFloat = TRUE; }
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
		else if (*lpSrc == L'[') {
			pMember->IsArray = TRUE;
			for (lpStr = (lpSrc + 1), cCh = (cSrc - 1); cCh && IsJSONSpaceEx(*lpStr); --cCh, ++lpStr);
			if (cCh && *lpStr == L']') { lpSrc = ++lpStr; cSrc = --cCh; }
		#ifdef __cplusplus
			else if ((dwLastError = __JSONParse__(pMember, lpSrc, cSrc))) goto ReturnError;
		#else
			else if ((dwLastError = __JSONParse__(pMember, &lpSrc, &cSrc))) goto ReturnError;
		#endif
		}
		else if (*lpSrc == L'{') {
			pMember->IsObject = TRUE;
			for (lpStr = (lpSrc + 1), cCh = (cSrc - 1); cCh && IsJSONSpaceEx(*lpStr); --cCh, ++lpStr);
			if (cCh && *lpStr == L'}') { lpSrc = ++lpStr; cSrc = --cCh; }
		#ifdef __cplusplus
			else if ((dwLastError = __JSONParse__(pMember, lpSrc, cSrc))) goto ReturnError;
		#else
			else if ((dwLastError = __JSONParse__(pMember, &lpSrc, &cSrc))) goto ReturnError;
		#endif
		}
		else if (pJSON->IsArray) { break; }
		else { goto ReturnError; }
		/*
				else if (*lpSrc == L'u') { //if (cSrc < 9 || StrCmpN(lpSrc, L"undefined", 9)) goto ReturnError;
					if (cSrc > 8  && lpSrc[1] == L'n' && lpSrc[2] == L'd' && lpSrc[3] == L'e' && lpSrc[4] == L'f' && lpSrc[5] == L'i' && lpSrc[6] == L'n' && lpSrc[7] == L'e' && lpSrc[8] == L'd') pMember->cchValue = 9;
					else { goto ReturnError; }
					pMember->IsUndefined = TRUE; lpSrc += 9; cSrc -= 9;
					pMember->lpszValue = (LPWSTR)MemAlloc((pMember->cchValue + 1) * sizeof(wchar_t));
					if (!pMember->lpszValue) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
					CopyMemoryEx(pMember->lpszValue, L"undefined", 9); ////MemCopy(pMember->lpszValue, L"undefined", 9 * sizeof(wchar_t));
					pMember->lpszValue[pMember->cchValue] = L'\0'; //while (cSrc && (IsJSONSpaceEx(*lpSrc))) { --cSrc; ++lpSrc; }
				}
				// or Name/Name() return undefined/value, like Global Properties/Functions, Example - Number == undefined, Number() == 0 or isNaN == null, isNaN() == true  isNaN(0) == false
				// escape, unescape, decodeURI, decodeURIComponent, encodeURI, encodeURIComponent, eval, EvalError,
				//  Float32Array, Float64Array, Int8Array, Int16Array, Int16Array, Uint8ClampedArray, Uint8Array, Uint16Array, Uint32Array, Number,
				//  isFinite, isNaN, Boolean, parseFloat, parseInt, Promise, Proxy, RangeError, ReferenceError , RegExp, Set,
				//  String, Symbol, SyntaxError, TypeError, URIError, WeakMap, WeakSet, Intl, Math, Infinity, NaN, Map, Object
		*/
		++pMember; ++pJSON->dwMemCount;
		while (cSrc && IsJSONSpaceEx(*lpSrc)) { --cSrc; ++lpSrc; }
	} while (cSrc && *lpSrc == L',');
	if (!cSrc) goto ReturnError;
	else if (pJSON->IsArray) { if (*lpSrc != L']') goto ReturnError; }
	else if (*lpSrc != L'}') { goto ReturnError; }
	--cSrc; ++lpSrc;
	if (!pJSON->dwMemCount) { if (pJSON->lpMembers) MemFree(pJSON->lpMembers); pJSON->lpMembers = NULL; pJSON->dwMemBuffer = 0; }
	else if (pJSON->dwMemBuffer > pJSON->dwMemCount) {
		pMember = (PJSON)MemReAlloc(pJSON->lpMembers, pJSON->dwMemCount * sizeof(SJSON));
		if (pMember) { pJSON->lpMembers = pMember; pJSON->dwMemBuffer = pJSON->dwMemCount; }
	}
#ifndef __cplusplus
	* lpcSrc = lpSrc; *lpdwSrc = cSrc;
#endif
	return NO_ERROR;
ReturnError:
#ifndef __cplusplus
	* lpcSrc = lpSrc; *lpdwSrc = cSrc;
#endif
	_JSONFree(pJSON); return dwLastError;
}

PJSON JSONParseEx(LPCWSTR lpcString, DWORD cchString, LPCWSTR * lpcszEnd) {
	if (!lpcString) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
	else if (!cchString && !(cchString = StrLen(lpcString))) { SetLastError(ERROR_INVALID_DATA); return NULL; }
	DWORD dwLastError = 0; PJSON pJSON = (PJSON)MemAllocEx(sizeof(SJSON));
	if (!pJSON) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pJSON->IsHeader = TRUE; pJSON->IsObject = TRUE; //pJSON->Header = pJSON;
#ifdef __cplusplus
	dwLastError = __JSONParse__(pJSON, lpcString, cchString);
#else
	dwLastError = __JSONParse__(pJSON, &lpcString, &cchString);
#endif
	if (lpcszEnd) *lpcszEnd = lpcString;
	if (!dwLastError) return pJSON;
	MemFree(pJSON); SetLastError(dwLastError); return NULL;
}
#endif //USING_JSONPARSE_EX

