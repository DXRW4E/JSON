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

#include "JSON.h"

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

HJSON JSONInitializeHandle() {
	PJSON pJSON = (PJSON)MemAllocEx(sizeof(SJSON));
	if (!pJSON) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pJSON->pHJSON = (HJSON)MemAllocEx(sizeof(_HJSON) * (JSON_MAX_HANDLE + 1));
	if (!pJSON->pHJSON) { MemFree(pJSON); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pJSON->IsHeader = TRUE; pJSON->IsObject = TRUE; pJSON->pHJSON[JSON_MAX_HANDLE].pJSON = pJSON;
	return &pJSON->pHJSON[JSON_MAX_HANDLE];
}

VOID _JSONFree(PJSON pJSON) {
	if (!pJSON) return;
	if (pJSON->lpszKeyName) MemFree(pJSON->lpszKeyName);
	if (pJSON->IsContainer) { for (PJSON pMember = pJSON->lpMembers; pJSON->dwMemCount; --pJSON->dwMemCount, ++pMember) { _JSONFree(pMember); }; }
	if (pJSON->lpMembers) MemFree(pJSON->lpMembers); ///lpMembers == lpszValue
	return;
}

VOID JSONFree(HJSON hJSON) {
	if (!hJSON || !hJSON->pJSON) return;
	PJSON pJSON = hJSON->pJSON, pHeader = pJSON->Header; BOOLEAN bType = pJSON->IsContainer;
	_JSONFree(pJSON);
	if (pJSON->IsHeader /*&& !pJSON->IsStackMem*/) { MemFree(pJSON->pHJSON); MemFree(pJSON); }
	else { ZeroMemory(pJSON, sizeof(SJSON)); pJSON->Header = pHeader; pJSON->IsContainer = bType; }
	return;
}

LPWSTR JSONEscapeStr(LPCWSTR lpcString, DWORD cchString, HJSON hJSON) {
	LPCWSTR lpStr = NULL; LPWSTR lpszString = NULL, lpXChr = NULL; DWORD cchEscapeStr = 0, cStr = 0, cCh = 0; WCHAR wChar = 0;
	//if (hJSON && hJSON->pJSON && hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) hJSON NULL;
	if (!lpcString) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
	else if (!cchString) cchString = StrLen(lpcString); ////else if (!cchString && !(cchString = StrLen(lpcString))) { SetLastError(ERROR_INVALID_DATA); return NULL; }
	lpszString = (LPWSTR)MemAlloc(((cchString * 6) + 1) * sizeof(wchar_t));
	if (!lpszString) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	else if (hJSON && hJSON->pJSON && hJSON->pJSON->IsString) { hJSON->pJSON->cchValue = cchString; hJSON->pJSON->cchUnEscapeStr = 0; hJSON->pJSON->IsEscapeStr = FALSE; }
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
		if (hJSON && hJSON->pJSON && hJSON->pJSON->IsString) { hJSON->pJSON->cchValue = cchEscapeStr; hJSON->pJSON->cchUnEscapeStr = cchString; hJSON->pJSON->IsEscapeStr = TRUE; }
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

DWORD JSONToNumber(const HJSON hJSON, PJSON_NUMBER pJSONNum) {
	if (!hJSON || !hJSON->pJSON || !hJSON->pJSON->IsNumber || !pJSONNum) return ERROR_INVALID_PARAMETER;
	PJSON pMember = hJSON->pJSON; LPWSTR lpEnd = NULL; //JSON_NUMBER jsonNumber = { 0 };
	if (pMember->IsFloat) { pJSONNum->dValue = wcstod(pMember->lpszValue, &lpEnd); }
	else if (pMember->IsSigned) { pJSONNum->illValue = wcstoll(pMember->lpszValue, &lpEnd, 10); }
	else { pJSONNum->ullValue = wcstoull(pMember->lpszValue, &lpEnd, 10); }
	return NO_ERROR;
}

DWORD JSONToNumberEx(HJSON hJSON, PJSON_NUMBER pJSONNum) {
	if (!hJSON || !hJSON->pJSON || !hJSON->pJSON->IsNumber) return ERROR_INVALID_PARAMETER;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pMember = hJSON->pJSON; LPWSTR lpStr = pMember->lpszValue, lpEnd = NULL; DWORD cCh = pMember->cchValue;
	if (!pMember->IsNumInit) {
		if (pMember->IsFloat) { pMember->dValue = wcstod(lpStr, &lpEnd); } //if (&lpStr[pMember->cchValue] != lpEnd) ?????
		else {
			if (*lpStr == L'-' || *lpStr == L'+') { ++lpStr; --cCh; }
			if (pMember->IsHexDigit) {
				for (cCh -= 2, lpStr += 2; cCh; --cCh, ++lpStr) {
					if (*lpStr >= L'A') {
						if (*lpStr > L'F') {
							if (*lpStr < L'a' || *lpStr > L'f') return ERROR_INVALID_DATA;
							pMember->illValue = (pMember->illValue << 4) + (*lpStr - L'a') + 10;
						}
						else pMember->illValue = (pMember->illValue << 4) + (*lpStr - L'A') + 10;
					}
					else if (*lpStr < L'0' || *lpStr > L'9') return ERROR_INVALID_DATA;
					else pMember->illValue = (pMember->illValue << 4) + (*lpStr - L'0');
				}
			}
			else { while (!(*lpStr < L'0' || *lpStr > L'9')) { pMember->illValue *= 10; pMember->illValue += *lpStr - L'0'; ++lpStr; } }
			if (pMember->IsSigned) pMember->illValue = -pMember->illValue;
		}
		pMember->IsNumInit = TRUE;
	}
	if (pJSONNum) {
		if (pMember->IsFloat) pJSONNum->dValue = pMember->dValue; // or (pMember->wValueType & JSON_IS_FLOAT)
		else if (pMember->IsSigned) pJSONNum->illValue = pMember->illValue; // or (pMember->wValueType & JSON_IS_SIGNED)
		else pJSONNum->ullValue = pMember->ullValue;
	}
	return NO_ERROR;
}

HJSON JSONParse(LPCWSTR lpSrc, DWORD cSrc, LPCWSTR* lpcszEnd) {
	if (!lpSrc || (!cSrc && !(cSrc = StrLen(lpSrc)))) { SetLastError(ERROR_INVALID_DATA); return NULL; }
	LPCWSTR lpStr = NULL; LPWSTR lpEnd = NULL; HJSON phJSON = NULL; PJSON pHeader = NULL, pJSON = NULL, pMember = NULL;
	PJSON_PARSE pJSONParse = NULL, pParse = NULL; DWORD cCh = 0, dwIndex = 0, dwParseBuffer = 32, dwLastError = ERROR_INVALID_DATA, dwNestedLevel = 1, dwMaxNL = 1; WCHAR wCh = 0; BOOLEAN bContinue = FALSE;
	pJSONParse = (PJSON_PARSE)MemAllocEx(dwParseBuffer * sizeof(JSON_PARSE));
	if (!pJSONParse) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pParse = pJSONParse; ++dwIndex; pHeader = (PJSON)MemAllocEx(sizeof(SJSON));
	if (!pHeader) { MemFree(pJSONParse); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pParse->pJSON = pHeader; pJSON = pHeader; phJSON = (HJSON)MemAllocEx(sizeof(_HJSON) * (JSON_MAX_HANDLE + 1));
	if (!phJSON) { MemFree(pJSON); MemFree(pJSONParse); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pJSON->IsHeader = TRUE; pJSON->IsObject = TRUE; pJSON->Header = pJSON; phJSON[JSON_MAX_HANDLE].pJSON = pJSON;
	pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer += 8) * sizeof(SJSON));
	if (!pJSON->lpMembers) { MemFree(pJSON); MemFree(pJSONParse); MemFree(phJSON); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pMember = pJSON->lpMembers; while (cSrc && IsJSONSpaceEx(*lpSrc)) { --cSrc; ++lpSrc; }
	if (!cSrc || *lpSrc != L'{') { goto ReturnError; }
	do {
		bContinue = FALSE; do { ++lpSrc; --cSrc; } while (cSrc && IsJSONSpaceEx(*lpSrc));
		if (pJSON->dwMemCount == pJSON->dwMemBuffer) {
			pMember = (PJSON)MemReAlloc(pJSON->lpMembers, (pJSON->dwMemBuffer *= 2) * sizeof(SJSON));
			if (!pMember) { dwLastError = ERROR_NOT_ENOUGH_MEMORY; goto ReturnError; }
			pJSON->lpMembers = pMember; pMember += pJSON->dwMemCount; ////ZeroMemory(&pMember[pJSON->dwMemCount], pJSON->dwMemCount * sizeof(SJSON));
		}
		ZeroMemory(pMember, sizeof(SJSON)); pMember->Header = pJSON->Header;
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
	MemFree(pJSONParse); pJSON->dwNestedLevel = dwMaxNL; pJSON->pHJSON = phJSON; return &phJSON[JSON_MAX_HANDLE];
ReturnError:
	_JSONFree(pHeader); MemFree(pHeader); MemFree(pJSONParse); MemFree(phJSON);
	if (lpcszEnd) *lpcszEnd = lpSrc; SetLastError(dwLastError); return NULL;
}

DWORD _JSONGetStringifySize(PCJSON pJSON, DWORD cTab) { // cTab is for Internal Use Only
	PJSON pMember = NULL; DWORD cchBuffer = 0, dwMemCount = 0; //, cchOffset = 0;
	//// JSONGetStringifySize() check this
	//if (!pJSON || !pJSON->IsContainer || pJSON->IsDeletePending) { SetLastError(ERROR_INVALID_HANDLE); return 0; } //or ERROR_INVALID_PARAMETER or ERROR_DELETE_PENDING
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

DWORD JSONGetStringifySize(const HJSON hJSON) {
	if (!hJSON || !hJSON->pJSON || !hJSON->pJSON->IsContainer || hJSON->pJSON->IsDeletePending) { SetLastError(ERROR_INVALID_HANDLE); return 0; } //or ERROR_INVALID_PARAMETER or ERROR_DELETE_PENDING
	DWORD cchBuffer = 0, cTab = 0, cchRoot = 0;
	if (!hJSON->pJSON->IsHeader) { ++cTab; cchRoot = 7 + hJSON->pJSON->cchKeyName + 4; }
	cchBuffer = _JSONGetStringifySize(hJSON->pJSON, cTab); if (!cchBuffer) return 0; //use GetLastError()
	return (cchRoot + cchBuffer);
}

LPWSTR _JSONStringify(PCJSON pJSON, LPWSTR lpStr, DWORD cTab) {
	PJSON pMember = NULL; LPWSTR lpStart = lpStr; DWORD cStr = 0, dwMemCount = 0;
	//// JSONStringify() check this
	//if (!pJSON || !pJSON->IsContainer || pJSON->IsDeletePending || !lpStr) { SetLastError(ERROR_INVALID_HANDLE); return FALSE; } //or ERROR_INVALID_PARAMETER or ERROR_DELETE_PENDING
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
			else if (!(lpStr = _JSONStringify(pMember, lpStr, cTab))) { SetLastError(ERROR_INVALID_DATA); return NULL; }
		}
		else if (pMember->IsString) { *lpStr = (pMember->IsSingleQuote ? L'\'' : L'"'); CopyMemoryEx(++lpStr, pMember->lpszValue, pMember->cchValue); lpStr += pMember->cchValue; *lpStr = (pMember->IsSingleQuote ? L'\'' : L'"'); ++lpStr; } //// *lpStr = _DSQM[pMember->IsSingleQuote]; -- //pMember->IsEscapeStr; pMember->IsSingleQuote;
		else if (pMember->IsStrValue) { CopyMemoryEx(lpStr, pMember->lpszValue, pMember->cchValue); lpStr += pMember->cchValue; } //(pMember->IsBoolean || pMember->IsNull || pMember->IsNumber) ---> pMember->IsSigned; pMember->IsFloat; pMember->IsHexDigit;
		else { SetLastError(ERROR_INVALID_DATA); return NULL; } /// ERROR??? It should never happen.
		if (dwMemCount > 1) { CopyMemoryEx(lpStr, L",\r\n", 3); lpStr += 3; }
	}
	if ((--pMember)->IsDeletePending) { lpStr -= 3; if (lpStart == lpStr) { ++lpStr; goto EndObj; }; }
	CopyMemoryEx(lpStr, L"\r\n", 2); lpStr += 2;
	for (cStr = --cTab; cStr; --cStr, ++lpStr) { *lpStr = L'\t'; }
EndObj:
	*lpStr = (pJSON->IsArray ? L']' : L'}'); ++lpStr;
	return lpStr;
}

LPWSTR JSONStringify(const HJSON hJSON) {
	if (!hJSON || !hJSON->pJSON || !hJSON->pJSON->IsContainer || hJSON->pJSON->IsDeletePending) { SetLastError(ERROR_INVALID_HANDLE); return NULL; }
	PCJSON pJSON = hJSON->pJSON; LPWSTR lpszOffset = NULL, lpszBuffer = NULL, lpStr = NULL; DWORD cchBuffer = 0, cTab = 0; //, cchJSONStringify = 0; 
	cchBuffer = JSONGetStringifySize(hJSON);
	if (!cchBuffer) return NULL; if (cchBuffer > 2) cchBuffer += 8; // 1 for NULL, plus 7 char, just for safety, they are not needed
	lpszBuffer = (LPWSTR)MemAlloc((cchBuffer) * sizeof(wchar_t));
	if (!lpszBuffer) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	else if (cchBuffer == 2) { CopyMemoryEx(lpszBuffer, (pJSON->IsObject ? L"{}\0" : L"[]\0"), 3); return lpszBuffer; }
	lpszOffset = lpszBuffer; if (!pJSON->IsHeader) { ++cTab; lpszOffset = &lpszBuffer[(pJSON->cchKeyName + 8)]; }
	lpszOffset = _JSONStringify(pJSON, lpszOffset, cTab);
	if (!lpszOffset) { MemFree(lpszBuffer); return NULL; }
	else if (!pJSON->IsHeader) {
		lpStr = lpszBuffer; CopyMemoryEx(lpStr, L"{\r\n\t", 4); lpStr += 4;
		*lpStr = (pJSON->IsKeyNameSQ ? L'\'' : L'"'); CopyMemoryEx(++lpStr, pJSON->lpszKeyName, pJSON->cchKeyName); lpStr += pJSON->cchKeyName; *lpStr = (pJSON->IsKeyNameSQ ? L'\'' : L'"'); *++lpStr = L':'; *++lpStr = L' ';
		CopyMemoryEx(lpszOffset, L"\r\n}\0", 4); lpszOffset += 3;
	}
	*lpszOffset = L'\0'; //cchBuffer = (DWORD)(lpszOffset - lpszBuffer); lpszBuffer[cchBuffer] = L'\0';
	return lpszBuffer;
}

DWORD JSONClose(HJSON hJSON) {
	if (!hJSON || !hJSON->pJSON) return ERROR_INVALID_HANDLE;
	PJSON pJSON = hJSON->pJSON;
	if (pJSON->IsHeader) { JSONFree(hJSON); }
	else {
		if (pJSON->cHandle) { if (--pJSON->cHandle == 0 || hJSON->LockHandle) pJSON->IsHandleLock = FALSE; }
		if (pJSON->Header->cHandle) --pJSON->Header->cHandle;
		ZeroMemory(hJSON, sizeof(_HJSON));
	}
	return NO_ERROR;
}

DWORD JSONLock(HJSON hJSON) {
	if (!hJSON || !hJSON->pJSON) return ERROR_INVALID_HANDLE;
	else if (hJSON->LockHandle) return NO_ERROR;
	else if (hJSON->pJSON->IsHandleLock) return ERROR_LOCK_VIOLATION;
	hJSON->LockHandle = TRUE; hJSON->pJSON->IsHandleLock = TRUE;
	return NO_ERROR;
}

DWORD JSONUnLock(HJSON hJSON) {
	if (!hJSON || !hJSON->pJSON) return ERROR_INVALID_HANDLE;
	else if (hJSON->LockHandle) { hJSON->LockHandle = FALSE; hJSON->pJSON->IsHandleLock = FALSE; }
	return NO_ERROR;
}

BOOLEAN _JSONIsHandleBusy(PCJSON pJSON) {
	PJSON pMember = NULL; DWORD dwCount = 0;
	//if (!pJSON) return FALSE;
	////else if (pJSON->IsHeader) return (pJSON->cHandle ? TRUE : FALSE);
	if (pJSON->cHandle) return TRUE;
	else if (pJSON->IsContainer) { for (pMember = pJSON->lpMembers, dwCount = pJSON->dwMemCount; dwCount; --dwCount, ++pMember) { if (_JSONIsHandleBusy(pMember)) return TRUE; }; }
	return FALSE;
}

BOOLEAN _JSONIsHandleBusyEx(PCJSON pJSON, const BOOLEAN bITH) {
	PJSON pMember = NULL; DWORD dwCount = 0;
	if (!pJSON) return FALSE;
	else if (pJSON->IsHeader) { return (pJSON->cHandle ? TRUE : FALSE); }
	else if (pJSON->cHandle > bITH) return TRUE;
	else if (pJSON->IsContainer) { for (pMember = pJSON->lpMembers, dwCount = pJSON->dwMemCount; dwCount; --dwCount, ++pMember) { if (_JSONIsHandleBusy(pMember)) return TRUE; }; }
	return FALSE;
}

BOOLEAN JSONIsHandleBusy(const HJSON hJSON, const BOOLEAN bITH) {
	if (!hJSON || !hJSON->pJSON) return FALSE;
	PJSON pJSON = hJSON->pJSON, pMember = NULL; DWORD dwCount = 0;
	if (pJSON->IsHeader) { return (pJSON->cHandle ? TRUE : FALSE); }
	else if (pJSON->cHandle > bITH) return TRUE;
	else if (pJSON->IsContainer) { for (pMember = pJSON->lpMembers, dwCount = pJSON->dwMemCount; dwCount; --dwCount, ++pMember) { if (_JSONIsHandleBusy(pMember)) return TRUE; }; }
	return FALSE;
}

DWORD JSONOpenKey(HJSON hJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PHJSON phJSONResult) {
	if (!hJSON || !hJSON->pJSON || !phJSONResult) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pMember = NULL; HJSON phJSON = NULL; DWORD dwMemCount = 0, dwIndex = 0, i = 0;
	if (!pJSON->IsObject || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; // ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwMemCount; --dwMemCount, ++pMember, ++dwIndex) { ////if (pMember->cchKeyName != cchKeyName || CompareMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) continue;
		if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
			if (pMember->Header->cHandle >= JSON_MAX_HANDLE) return ERROR_TOO_MANY_OPEN_FILES;
			else if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
			//else if (pMember->IsHandleLock) return ERROR_LOCK_VIOLATION;
			for (phJSON = pMember->Header->pHJSON, i = 0; i < JSON_MAX_HANDLE && phJSON->pJSON; ++i, ++phJSON) {};
			++pMember->Header->cHandle; phJSON->pJSON = pMember; phJSON->dwIndex = dwIndex; *phJSONResult = phJSON; ////*phJSONResult = &pMember->Header->pHJSON[dwIndex]
			++pMember->cHandle; return NO_ERROR;
		}
	}
	return ERROR_FILE_NOT_FOUND; //ENOENT
}

DWORD JSONOpenKeyEx(HJSON hJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PHJSON phJSONResult) {
	if (!hJSON || !hJSON->pJSON || !phJSONResult) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pMember = pJSON; HJSON phJSON = NULL;
	LPCWSTR lpSubKey = lpszKeyName, lpIndex = NULL; DWORD dwMemCount = 1, cchSubKey = 0, dwIndex = 0, i = 0; BOOLEAN bFound = FALSE;
	if (!pJSON->IsContainer || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; //or ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	else if (*lpszKeyName == L'[') { cchSubKey = cchKeyName; goto GetKeyIndex; }
	do {
		lpSubKey = wmemchr(lpszKeyName, L'.', cchKeyName); //wcschr(lpszKeyName, L'.');
		lpIndex = wmemchr(lpszKeyName, L'[', (lpSubKey ? (lpSubKey - lpszKeyName) : cchKeyName));
		if (lpIndex) { lpSubKey = lpIndex; } cchSubKey = 0;
		if (lpSubKey) { cchSubKey = cchKeyName - (DWORD)(lpSubKey - lpszKeyName); cchKeyName -= cchSubKey; if (!lpIndex) { ++lpSubKey; --cchSubKey; }; } //if (*lpSubKey == L'.')
		for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount, dwIndex = 0; dwMemCount; --dwMemCount, ++pMember, ++dwIndex) {
			if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
				if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
				while (cchSubKey && *lpSubKey == L'[') {
				GetKeyIndex:
					dwIndex = 0; while (--cchSubKey && !(*++lpSubKey < L'0' || *lpSubKey > L'9')) { dwIndex *= 10; dwIndex += *lpSubKey - L'0'; }
					if (*lpSubKey != L']') return ERROR_INVALID_PARAMETER; if (--cchSubKey && *++lpSubKey == L'.') { --cchSubKey; ++lpSubKey; };
					if (!pMember->IsContainer || dwIndex >= pMember->dwMemCount) return ERROR_INVALID_INDEX;
					pMember = &pMember->lpMembers[dwIndex]; if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
				}
				if (cchSubKey) { pJSON = pMember; lpszKeyName = lpSubKey; cchKeyName = cchSubKey; }
				else bFound = TRUE;
				break;
			}
		}
	} while (dwMemCount && cchSubKey && pJSON->IsObject && pJSON->dwMemCount);
	if (!bFound) return ERROR_FILE_NOT_FOUND; //ENOENT
	else if (pMember->Header->cHandle >= JSON_MAX_HANDLE) return ERROR_TOO_MANY_OPEN_FILES;
	//else if (pMember->IsHandleLock) return ERROR_LOCK_VIOLATION;
	for (phJSON = pMember->Header->pHJSON, i = 0; i < JSON_MAX_HANDLE && phJSON->pJSON; ++i, ++phJSON) {};
	++pMember->Header->cHandle; phJSON->pJSON = pMember; phJSON->dwIndex = dwIndex; *phJSONResult = phJSON;
	++pMember->cHandle; return NO_ERROR;
}

DWORD JSONCreateKey(HJSON hJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, WORD wType, PHJSON phJSONResult, LPDWORD lpdwDisposition) { //wType - see JSON_IS_XXXXX
	if (!hJSON || !hJSON->pJSON || !phJSONResult) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pHeader = (pJSON->IsHeader ? pJSON : pJSON->Header), pMember = NULL, pMemB = NULL, pMemS = NULL, pMemE = NULL; HJSON phJSON = NULL; DWORD dwIndex = 0, dwCount = 0, i = 0;
	if (pHeader->cHandle >= JSON_MAX_HANDLE) return ERROR_TOO_MANY_OPEN_FILES;
	else if (!pJSON->IsObject || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; //ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	for (pMember = pJSON->lpMembers, dwCount = pJSON->dwMemCount; dwCount; --dwCount, ++pMember, ++dwIndex) { ////if (pMember->cchKeyName != cchKeyName || CompareMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) continue;
		if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
			if (lpdwDisposition) *lpdwDisposition = JSON_OPENED_EXISTING_KEY;
			if (pMember->IsDeletePending) break;//return ERROR_DELETE_PENDING;
			//else if (pMember->IsHandleLock) return ERROR_LOCK_VIOLATION;
			goto AddHandle;
		}
	}
	if (pJSON->dwMemBuffer) {
		if (pJSON->dwMemBuffer == pJSON->dwMemCount) {
			pMember = (PJSON)MemReAlloc((pMemB = pJSON->lpMembers), (pJSON->dwMemBuffer *= 2) * sizeof(SJSON));
			if (!pMember) { pJSON->dwMemBuffer /= 2; return ERROR_NOT_ENOUGH_MEMORY; }
			pJSON->lpMembers = pMember;
			for (phJSON = pHeader->pHJSON, pMemS = (pMemB - 1), pMemE = &pMemB[pJSON->dwMemCount], dwCount = pHeader->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
				if (!phJSON->pJSON) continue; --dwCount;
				if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) phJSON->pJSON = &pMember[phJSON->dwIndex];
			}
			//dwCount = pJSON->dwMemCount; while(dwCount && !pMember->cHandle) { --dwCount; ++pMember; }
			//if (dwCount) {
			//	for (phJSON = pHeader->pHJSON, pMemS = (pMemB - 1), pMemE = &pMemB[pJSON->dwMemCount], dwCount = pHeader->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
			//		if (!phJSON->pJSON) continue; --dwCount;
			//		if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) phJSON->pJSON = &pMember[phJSON->dwIndex]; ////phJSON->pJSON = &pMember[phJSON->pJSON - pMemB];
			//	}
			//}
		}
	}
	else {
		pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer += 8) * sizeof(SJSON));
		if (!pJSON->lpMembers) { pJSON->dwMemBuffer = 0; return ERROR_NOT_ENOUGH_MEMORY; }
	}
	pMember = &pJSON->lpMembers[pJSON->dwMemCount]; ZeroMemory(pMember, sizeof(SJSON));
	pMember->lpszKeyName = (LPWSTR)MemAlloc((cchKeyName + 1) * sizeof(wchar_t));
	if (!pMember->lpszKeyName) return ERROR_NOT_ENOUGH_MEMORY;
	pMember->Header = pHeader; pMember->wValueType = wType;
	CopyMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName); ////CopyMemory(pMember->lpszKeyName, lpszKeyName, cchKeyName * sizeof(wchar_t));
	pMember->cchKeyName = cchKeyName; pMember->lpszKeyName[pMember->cchKeyName] = L'\0';
	++pJSON->dwMemCount; if (lpdwDisposition) *lpdwDisposition = JSON_CREATED_NEW_KEY;
AddHandle:
	for (phJSON = pHeader->pHJSON, i = 0; i < JSON_MAX_HANDLE && phJSON->pJSON; ++i, ++phJSON) {};
	++pHeader->cHandle; phJSON->pJSON = pMember; phJSON->dwIndex = dwIndex; *phJSONResult = phJSON;
	++pMember->cHandle; return NO_ERROR;
}

DWORD JSONInsertKey(HJSON hJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, WORD wType, PHJSON phJSONResult, DWORD dwIndex) {
	if (!hJSON || !hJSON->pJSON || !phJSONResult) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pMember = NULL, pHeader = (pJSON->IsHeader ? pJSON : pJSON->Header), pMemS = NULL, pMemE = NULL;
	SJSON sJSON; HJSON hMember = NULL, phJSON = NULL; DWORD dwDisposition = 0, dwLastError = 0, dwMove = 0, i = 0, dwCount = 0; BOOLEAN bUp = FALSE;
	if (!pJSON->IsObject || pJSON->IsDeletePending || !(wType & JSON_VALUE_TYPE)) return ERROR_INVALID_PARAMETER; //ERROR_DELETE_PENDING
	else if (dwIndex == -1) dwIndex = pJSON->dwMemCount;
	else if (dwIndex > pJSON->dwMemCount) return ERROR_INVALID_INDEX;
	dwLastError = JSONCreateKey(hJSON, lpszKeyName, cchKeyName, wType, &hMember, &dwDisposition); *phJSONResult = hMember;
	if (dwLastError) return dwLastError;
	else if (dwDisposition == JSON_OPENED_EXISTING_KEY && dwIndex == pJSON->dwMemCount) { JSONClose(hMember); return ERROR_INVALID_INDEX; }
	else if (hMember->dwIndex == dwIndex) return NO_ERROR;
	else if (hMember->dwIndex > dwIndex) { bUp = TRUE; dwMove = (hMember->dwIndex - dwIndex); i = dwIndex; }
	else { dwMove = (dwIndex - hMember->dwIndex); i = hMember->dwIndex; }
	pMember = &pJSON->lpMembers[i]; CopyMemory(&sJSON, hMember->pJSON, sizeof(SJSON));
	if (bUp) { MoveMemory(&pMember[1], pMember, dwMove * sizeof(SJSON)); }
	else { CopyMemory(pMember, &pMember[1], dwMove * sizeof(SJSON)); }
	for (phJSON = pHeader->pHJSON, pMemS = (pMember - 1), pMemE = &pMember[dwMove + 2], dwCount = pHeader->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
		if (!phJSON->pJSON) continue; --dwCount;
		if (phJSON->pJSON == hMember->pJSON) { phJSON->pJSON = &pJSON->lpMembers[dwIndex]; phJSON->dwIndex = dwIndex; }
		else if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) {
			if (bUp) { phJSON->pJSON = &pJSON->lpMembers[++phJSON->dwIndex]; }
			else { phJSON->pJSON = &pJSON->lpMembers[--phJSON->dwIndex]; }
		}
	}
	CopyMemory(hMember->pJSON, &sJSON, sizeof(SJSON));
	return NO_ERROR;
}

DWORD JSONInsertElement(HJSON hJSON, WORD wType, PHJSON phJSONResult, DWORD dwIndex) { //dwIndex = -1, wType - see JSON_IS_XXXXX
	if (!hJSON || !hJSON->pJSON || !phJSONResult) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pMember = NULL, pHeader = (pJSON->IsHeader ? pJSON : pJSON->Header), pMemB = NULL, pMemS = NULL, pMemE = NULL; HJSON phJSON = NULL; DWORD dwMove = 0, dwCount = 0, i = 0;
	if (!pJSON->IsArray || pJSON->IsDeletePending || !(wType & JSON_VALUE_TYPE)) return ERROR_INVALID_PARAMETER; //ERROR_DELETE_PENDING
	else if (pHeader->cHandle > JSON_MAX_HANDLE) return ERROR_TOO_MANY_OPEN_FILES;
	else if (dwIndex == -1) dwIndex = pJSON->dwMemCount;
	else if (dwIndex > pJSON->dwMemCount) return ERROR_INVALID_INDEX;
	else dwMove = (pJSON->dwMemCount - dwIndex);
	if (pJSON->dwMemBuffer) {
		if (pJSON->dwMemBuffer == pJSON->dwMemCount) {
			pMember = (PJSON)MemReAlloc((pMemB = pJSON->lpMembers), (pJSON->dwMemBuffer *= 2) * sizeof(SJSON));
			if (!pMember) { pJSON->dwMemBuffer /= 2; return ERROR_NOT_ENOUGH_MEMORY; }
			pJSON->lpMembers = pMember;
			for (phJSON = pHeader->pHJSON, pMemS = (pMemB - 1), pMemE = &pMemB[pJSON->dwMemCount], dwCount = pHeader->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
				if (!phJSON->pJSON) continue; --dwCount;
				if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) phJSON->pJSON = &pMember[phJSON->dwIndex]; ////phJSON->pJSON = &pMember[phJSON->pJSON - pMemB];
			}
		}
	}
	else {
		pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer += 8) * sizeof(SJSON));
		if (!pJSON->lpMembers) { pJSON->dwMemBuffer = 0; return ERROR_NOT_ENOUGH_MEMORY; }
	}
	pMember = &pJSON->lpMembers[dwIndex];
	if (dwMove) {
		MoveMemory(&pMember[1], pMember, dwMove * sizeof(SJSON));
		for (phJSON = pHeader->pHJSON, pMemS = (pMember - 1), pMemE = &pMember[++dwMove], dwCount = pHeader->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
			if (!phJSON->pJSON) continue; --dwCount;
			if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) { phJSON->pJSON = &pJSON->lpMembers[++phJSON->dwIndex]; }
		}
	}
	ZeroMemory(pMember, sizeof(SJSON)); pMember->Header = pHeader; pMember->wValueType = wType;
	for (phJSON = pHeader->pHJSON, i = 0; i < JSON_MAX_HANDLE && phJSON->pJSON; ++i, ++phJSON) {};
	++pHeader->cHandle; phJSON->pJSON = pMember; phJSON->dwIndex = dwIndex; *phJSONResult = phJSON;
	++pJSON->dwMemCount; ++pMember->cHandle; return NO_ERROR;
}

DWORD JSONCommitChanges(HJSON hJSON) {
	if (!hJSON || !hJSON->pJSON) { SetLastError(ERROR_INVALID_HANDLE); return -1; }
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) { SetLastError(ERROR_LOCK_VIOLATION); return -1; }
	PJSON pJSON = hJSON->pJSON, pMember = NULL, pMemS = NULL, pMemE = NULL; HJSON phJSON = NULL;
	SJSON sJSON; DWORD dwMemCount = 0, dwIndex = 0, dwCount = 0, i = 0, dwBusy = 0; BOOLEAN bBusy = FALSE;
	if (!pJSON->IsContainer || !pJSON->dwMemCount) { SetLastError(ERROR_INVALID_PARAMETER); return -1; }
	for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwIndex < dwMemCount; ++dwIndex, ++pMember) {
		if (pMember->IsDeletePending) {
			dwCount = (--dwMemCount - dwIndex) + dwBusy;
			if (_JSONIsHandleBusy(pMember)) { CopyMemory(&sJSON, pMember, sizeof(SJSON)); ++dwBusy; bBusy = TRUE; }
			else { _JSONFree(pMember); --pJSON->dwMemCount; bBusy = FALSE; }
			if (!dwCount) break;
			CopyMemory(pMember, &pMember[1], dwCount * sizeof(SJSON)); //since lpDest < lpSource, CopyMemory() should by always Ok, or if you want to be more safe use MoveMemory()
			if (bBusy) { CopyMemory(&pMember[dwCount], &sJSON, sizeof(SJSON)); bBusy = FALSE; }
			for (phJSON = pMember->Header->pHJSON, pMemS = pMember, pMemE = &pMember[++dwCount], dwCount = pMember->Header->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
				if (!phJSON->pJSON) continue; --dwCount;
				if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) {
					if (phJSON->dwIndex == dwIndex) phJSON->dwIndex = (pJSON->dwMemCount - 1); //in case of bBusy = TRUE;
					else phJSON->pJSON = &pJSON->lpMembers[--phJSON->dwIndex];
				}
			}
			--dwIndex;
		}
	}
	return (pJSON->dwMemCount - dwBusy);
}

DWORD JSONEnumKey(HJSON hJSON, DWORD dwIndex, PHJSON phJSONResult) {
	if (!hJSON || !hJSON->pJSON || !phJSONResult) return ERROR_INVALID_HANDLE;
	PJSON pMember = &hJSON->pJSON->lpMembers[dwIndex]; HJSON phJSON = NULL; DWORD i = 0;
	if (!hJSON->pJSON->IsContainer || dwIndex >= hJSON->pJSON->dwMemCount) return ERROR_NO_MORE_ITEMS;
	else if (pMember->Header->cHandle >= JSON_MAX_HANDLE) return ERROR_TOO_MANY_OPEN_FILES;
	//else if (pMember->IsHandleLock) return ERROR_LOCK_VIOLATION;
	for (phJSON = pMember->Header->pHJSON, i = 0; i < JSON_MAX_HANDLE && phJSON->pJSON; ++i, ++phJSON) {};
	++pMember->Header->cHandle; phJSON->pJSON = pMember; phJSON->dwIndex = dwIndex; *phJSONResult = phJSON;
	++pMember->cHandle; return NO_ERROR;
}

DWORD JSONEnumKeyEx(HJSON hJSON, DWORD dwIndex, PHJSON phJSONResult) {
	if (!hJSON || !hJSON->pJSON || !phJSONResult) return ERROR_INVALID_HANDLE;
	PJSON pMember = &hJSON->pJSON->lpMembers[dwIndex]; HJSON phJSON = NULL; DWORD i = 0;
	dwIndex += hJSON->dwDelPending; pMember += hJSON->dwDelPending;
	if (!hJSON->pJSON->IsContainer || dwIndex >= hJSON->pJSON->dwMemCount) { hJSON->dwDelPending = 0; return ERROR_NO_MORE_ITEMS; }
	while (pMember->IsDeletePending) { ++pMember; ++hJSON->dwDelPending; if (++dwIndex == hJSON->pJSON->dwMemCount) { hJSON->dwDelPending = 0; return ERROR_NO_MORE_ITEMS; }; }
	if (pMember->Header->cHandle >= JSON_MAX_HANDLE) return ERROR_TOO_MANY_OPEN_FILES;
	//else if (pMember->IsHandleLock) return ERROR_LOCK_VIOLATION;
	for (phJSON = pMember->Header->pHJSON, i = 0; i < JSON_MAX_HANDLE && phJSON->pJSON; ++i, ++phJSON) {};
	++pMember->Header->cHandle; phJSON->pJSON = pMember; phJSON->dwIndex = dwIndex; *phJSONResult = phJSON;
	++pMember->cHandle; return NO_ERROR;
}

DWORD _JSONCopyTree(PJSON pJSON, PCJSON pSrcJSON, PJSON pHeader) {
	//// JSONCopyTree() check this
	//if (!pJSON || !pSrcJSON || pSrcJSON->IsDeletePending) return ERROR_INVALID_HANDLE; //or ERROR_INVALID_PARAMETER or ERROR_DELETE_PENDING
	PJSON pSrcMember = pSrcJSON->lpMembers, pMember = NULL; DWORD dwMemCount = pSrcJSON->dwMemCount;
	CopyMemory(pJSON, pSrcJSON, sizeof(SJSON)); pJSON->lpMembers = NULL; pJSON->dwMemCount = 0; pJSON->Header = pHeader; pJSON->wHandleType = pSrcJSON->IsHeader;
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
			else if (_JSONCopyTree(pMember, pSrcMember, pHeader)) goto ReturnError;
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

DWORD JSONCopyTree(HJSON hJSON, const HJSON hSrcJSON) {
	if (!hJSON || !hJSON->pJSON || !hSrcJSON || !hSrcJSON->pJSON || hSrcJSON->pJSON->IsDeletePending) return ERROR_INVALID_HANDLE; //or ERROR_DELETE_PENDING or ERROR_INVALID_TARGET_HANDLE
	else if ((hJSON->pJSON->IsHeader || hSrcJSON->pJSON->IsHeader) && hJSON->pJSON->IsHeader != hSrcJSON->pJSON->IsHeader) return ERROR_INVALID_PARAMETER;
	PJSON pJSON = hJSON->pJSON; HJSON phJSON = pJSON->pHJSON; DWORD dwLastError = 0;
	if (_JSONIsHandleBusyEx(pJSON, TRUE)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
	else { _JSONFree(pJSON); }
	dwLastError = _JSONCopyTree(pJSON, hSrcJSON->pJSON, pJSON);
	if (pJSON->IsHeader) { pJSON->pHJSON = phJSON; ZeroMemory(phJSON, JSON_MAX_HANDLE * sizeof(_HJSON)); }
	return dwLastError;
}

HJSON JSONCopyTreeEx(const HJSON hSrcJSON) {
	if (!hSrcJSON || !hSrcJSON->pJSON || hSrcJSON->pJSON->IsDeletePending) { SetLastError(ERROR_INVALID_HANDLE); return NULL; }; //or ERROR_INVALID_PARAMETER or ERROR_DELETE_PENDING
	PJSON pHeader = NULL, pJSON = (PJSON)MemAllocEx(sizeof(SJSON)); DWORD dwLastError = 0;
	if (!pJSON) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	dwLastError = _JSONCopyTree(pJSON, hSrcJSON->pJSON, pJSON);
	if (dwLastError) { MemFree(pJSON); SetLastError(dwLastError); return NULL; }
	else if (!pJSON->IsHeader) {
		if (pJSON->IsObject) pJSON->IsHeader = TRUE;
		else {
			pHeader = (PJSON)MemAllocEx(sizeof(SJSON));
			if (!pHeader) { _JSONFree(pJSON); MemFree(pJSON); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
			pJSON->Header = pHeader; pHeader->IsHeader = TRUE; pHeader->IsObject = TRUE; pHeader->lpMembers = pJSON; pHeader->dwMemBuffer = 1; pHeader->dwMemCount = 1; pJSON = pHeader;
		}
	}
	pJSON->pHJSON = (HJSON)MemAllocEx(sizeof(_HJSON) * (JSON_MAX_HANDLE + 1));
	if (!pJSON->pHJSON) { _JSONFree(pJSON); MemFree(pJSON); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
	pJSON->pHJSON[JSON_MAX_HANDLE].pJSON = pJSON;
	return &pJSON->pHJSON[JSON_MAX_HANDLE];
}

DWORD JSONDeleteOnClose(HJSON hJSON) { //JSONDeleteElement
	if (!hJSON || !hJSON->pJSON || hJSON->pJSON->IsHeader) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsDeletePending) return NO_ERROR; //ERROR_DELETE_PENDING
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	hJSON->pJSON->IsDeletePending = TRUE;
	return NO_ERROR;
}

DWORD JSONDeleteByIndex(HJSON hJSON, DWORD dwIndex) {
	if (!hJSON || !hJSON->pJSON) return ERROR_INVALID_HANDLE;
	PJSON pJSON = hJSON->pJSON, pMember = &pJSON->lpMembers[dwIndex], pMemS = NULL, pMemE = NULL; HJSON phJSON = NULL; DWORD dwCount = 0, i = 0;
	if (!pJSON->IsContainer || dwIndex >= pJSON->dwMemCount) return ERROR_NO_MORE_ITEMS;
	else if (pJSON->IsDeletePending || pMember->IsDeletePending) return NO_ERROR; //ERROR_DELETE_PENDING
	else if ((pJSON->IsHandleLock && !hJSON->LockHandle) || pMember->IsHandleLock) return ERROR_LOCK_VIOLATION;
#ifdef _JSON_DELETE_ON_CLOSE
	pMember->IsDeletePending = TRUE; return NO_ERROR;
#endif
	if (_JSONIsHandleBusy(pMember)) { pMember->IsDeletePending = TRUE; return NO_ERROR; }
	_JSONFree(pMember); dwCount = --pJSON->dwMemCount - dwIndex;
	if (dwCount) {
		CopyMemory(pMember, &pMember[1], dwCount * sizeof(SJSON)); //since lpDest < lpSource, CopyMemory() should by always Ok, or if you want to be more safe use MoveMemory()
		for (phJSON = pMember->Header->pHJSON, pMemS = pMember, pMemE = &pMember[++dwCount], dwCount = pMember->Header->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
			if (!phJSON->pJSON) continue; --dwCount;
			if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) { phJSON->pJSON = &pJSON->lpMembers[--phJSON->dwIndex]; }
		}
	}
	return NO_ERROR;
}

DWORD JSONDeleteKey(HJSON hJSON, LPCWSTR lpszKeyName, DWORD cchKeyName) {
	if (!hJSON || !hJSON->pJSON) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pMember = NULL, pMemS = NULL, pMemE = NULL; HJSON phJSON = NULL; DWORD dwCount = 0, dwIndex = 0, i = 0;
	if (!pJSON->IsObject || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; //or ERROR_INVALID_HANDLE or ERROR_DELETE_PENDING or ERROR_NO_MORE_FILES
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	for (pMember = pJSON->lpMembers; dwIndex < pJSON->dwMemCount; ++dwIndex, ++pMember) { ////if (pMember->cchKeyName != cchKeyName || CompareMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) continue;
		if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) { break; }
	}
	if (dwIndex == pJSON->dwMemCount) return ERROR_FILE_NOT_FOUND; //ENOENT
	else if (pMember->IsDeletePending) return NO_ERROR; //ERROR_DELETE_PENDING
	else if (pMember->IsHandleLock) return ERROR_LOCK_VIOLATION;
#ifdef _JSON_DELETE_ON_CLOSE
	pMember->IsDeletePending = TRUE; return NO_ERROR;
#endif
	if (_JSONIsHandleBusy(pMember)) { pMember->IsDeletePending = TRUE; return NO_ERROR; }
	_JSONFree(pMember); dwCount = --pJSON->dwMemCount - dwIndex;
	if (dwCount) {
		CopyMemory(pMember, &pMember[1], dwCount * sizeof(SJSON)); //since lpDest < lpSource, CopyMemory() should by always Ok, or if you want to be more safe use MoveMemory()
		for (phJSON = pMember->Header->pHJSON, pMemS = pMember, pMemE = &pMember[++dwCount], dwCount = pMember->Header->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
			if (!phJSON->pJSON) continue; --dwCount;
			if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) { phJSON->pJSON = &pJSON->lpMembers[--phJSON->dwIndex]; }
		}
	}
	return NO_ERROR;
}

DWORD JSONDeleteKeyEx(HJSON hJSON, LPCWSTR lpszKeyName, DWORD cchKeyName) {
	if (!hJSON || !hJSON->pJSON) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pMember = pJSON, pMemS = NULL, pMemE = NULL; HJSON phJSON = NULL;
	LPCWSTR lpSubKey = lpszKeyName, lpIndex = NULL; DWORD dwMemCount = 1, cchSubKey = 0, dwIndex = 0, dwCount = 0, i = 0; BOOLEAN bFound = FALSE;
	if (!pJSON->IsContainer || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; //or ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	else if (*lpszKeyName == L'[') { cchSubKey = cchKeyName; goto GetKeyIndex; }
	do {
		lpSubKey = wmemchr(lpszKeyName, L'.', cchKeyName); //wcschr(lpszKeyName, L'.');
		lpIndex = wmemchr(lpszKeyName, L'[', (lpSubKey ? (lpSubKey - lpszKeyName) : cchKeyName));
		if (lpIndex) lpSubKey = lpIndex; cchSubKey = 0;
		if (lpSubKey) { cchSubKey = cchKeyName - (DWORD)(lpSubKey - lpszKeyName); cchKeyName -= cchSubKey; if (!lpIndex) { ++lpSubKey; --cchSubKey; }; } //if (*lpSubKey == L'.')
		for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount, dwIndex = 0; dwMemCount; --dwMemCount, ++pMember, ++dwIndex) {
			if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
				if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
				while (cchSubKey && *lpSubKey == L'[') {
				GetKeyIndex:
					dwIndex = 0; while (--cchSubKey && !(*++lpSubKey < L'0' || *lpSubKey > L'9')) { dwIndex *= 10; dwIndex += *lpSubKey - L'0'; }
					if (*lpSubKey != L']') return ERROR_INVALID_PARAMETER; if (--cchSubKey && *++lpSubKey == L'.') { --cchSubKey; ++lpSubKey; };
					if (!pMember->IsContainer || dwIndex >= pMember->dwMemCount) return ERROR_INVALID_INDEX;
					pMember = &pMember->lpMembers[dwIndex]; i = dwIndex; if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
				}
				if (cchSubKey) { pJSON = pMember; lpszKeyName = lpSubKey; cchKeyName = cchSubKey; }
				else bFound = TRUE;
				break;
			}
		}
	} while (dwMemCount && cchSubKey && pJSON->IsObject && pJSON->dwMemCount);
	if (!bFound) return ERROR_FILE_NOT_FOUND; //ENOENT
	else if (pMember->IsHandleLock) return ERROR_LOCK_VIOLATION;
#ifdef _JSON_DELETE_ON_CLOSE
	pMember->IsDeletePending = TRUE; return NO_ERROR;
#endif
	if (_JSONIsHandleBusy(pMember)) { pMember->IsDeletePending = TRUE; return NO_ERROR; }
	_JSONFree(pMember); dwCount = --pJSON->dwMemCount - dwIndex;
	if (dwCount) {
		CopyMemory(pMember, &pMember[1], dwCount * sizeof(SJSON)); //since lpDest < lpSource, CopyMemory() should by always Ok, or if you want to be more safe use MoveMemory()
		for (phJSON = pMember->Header->pHJSON, pMemS = pMember, pMemE = &pMember[++dwCount], dwCount = pMember->Header->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
			if (!phJSON->pJSON) continue; --dwCount;
			if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) { phJSON->pJSON = &pJSON->lpMembers[--phJSON->dwIndex]; }
		}
	}
	return NO_ERROR;
}

DWORD JSONRenameKey(HJSON hJSON, LPCWSTR lpszNewKeyName, DWORD cchNewKeyName) {
	if (!hJSON || !hJSON->pJSON || hJSON->pJSON->IsHeader || !hJSON->pJSON->lpszKeyName) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	else if (hJSON->pJSON->IsDeletePending || !lpszNewKeyName) return ERROR_INVALID_PARAMETER; //or ERROR_DELETE_PENDING
	else if (!cchNewKeyName && !(cchNewKeyName = StrLen(lpszNewKeyName))) return ERROR_INVALID_DATA;
	PJSON pJSON = hJSON->pJSON; LPWSTR lpszKeyName = (LPWSTR)MemAlloc((cchNewKeyName + 1) * sizeof(wchar_t));
	if (!lpszKeyName) return ERROR_NOT_ENOUGH_MEMORY;
	if (pJSON->lpszKeyName) MemFree(pJSON->lpszKeyName);
	pJSON->lpszKeyName = lpszKeyName; pJSON->cchKeyName = cchNewKeyName;
	CopyMemoryEx(pJSON->lpszKeyName, lpszNewKeyName, pJSON->cchKeyName); pJSON->lpszKeyName[pJSON->cchKeyName] = L'\0';
	return NO_ERROR;
}

DWORD JSONSetValue(HJSON hJSON, WORD wValueType, LPCVOID lpData, DWORD dwDataCount) {
	if (!hJSON || !hJSON->pJSON || hJSON->pJSON->IsHeader) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pMember = NULL; SJSON sJSON, sNewKey;
	////SJSON sNewKey = { 0 }, sJSON = { NULL, NULL, (PJSON)lpData, 0, 0, dwDataCount, dwDataCount, wValueType, JSON_IS_HEADER_STACKMEM, 0 };
	if (!lpData || !(wValueType & JSON_VALUE_TYPE)) return ERROR_INVALID_PARAMETER;
	else if (/*pJSON->IsDeletePending ||*/ wValueType & JSON_IS_DELETEPENDING) return ERROR_DELETE_PENDING;
	else if (_JSONIsHandleBusyEx(pJSON, TRUE)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
	else if (pJSON->IsContainer) { for (pMember = pJSON->lpMembers; pJSON->dwMemCount; --pJSON->dwMemCount, ++pMember) { _JSONFree(pMember); } pJSON->dwMemBuffer = 0; }
	if (pJSON->lpszValue) { MemFree(pJSON->lpszValue); pJSON->lpszValue = NULL; pJSON->cchValue = 0; } //(lpMembers == lpszValue && dwMemCount == cchValue)
	pJSON->wValueType = wValueType;
	if (pJSON->IsContainer) { //(wValueType & JSON_IS_CONTAINER)
		ZeroMemory(&sJSON, sizeof(SJSON)); //ZeroMemory(&sNewKey, sizeof(SJSON));
		sJSON.wValueType = wValueType; sJSON.lpMembers = (PJSON)lpData; sJSON.dwMemBuffer = dwDataCount; sJSON.dwMemCount = dwDataCount;
		if (_JSONCopyTree(&sNewKey, &sJSON, pJSON->Header)) return ERROR_NOT_ENOUGH_MEMORY;
		else { pJSON->lpMembers = sNewKey.lpMembers; pJSON->dwMemBuffer = sNewKey.dwMemBuffer; pJSON->dwMemCount = sNewKey.dwMemCount; }
	}
	else { //pJSON->IsStrValue - (wValueType & JSON_IS_STRVALUE)
		if (!dwDataCount) { dwDataCount = StrLen((LPCWSTR)lpData); }
		if (dwDataCount && pJSON->IsString) { //(pJSON->wValueType & JSON_IS_STRING)
			pJSON->lpszValue = JSONEscapeStr((LPCWSTR)lpData, dwDataCount, hJSON);
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

DWORD JSONAddKeyValue(HJSON hJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, WORD wValueType, LPCVOID lpData, DWORD dwDataCount) { // JSONCreate()
	if (!hJSON || !hJSON->pJSON || hJSON->pJSON->IsHeader || !hJSON->pJSON->IsObject) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	else if (hJSON->pJSON->IsDeletePending || !lpData || !(wValueType & JSON_VALUE_TYPE)) return ERROR_INVALID_PARAMETER; //or ERROR_DELETE_PENDING
	HJSON hMember = NULL; PJSON pJSON = hJSON->pJSON, pMember = NULL; DWORD dwLastError = NO_ERROR, dwDisposition = 0;
	dwLastError = JSONCreateKey(hJSON, lpszKeyName, cchKeyName, wValueType, &hMember, &dwDisposition);
	if (dwLastError) { return dwLastError; }
	else if (dwDisposition == JSON_OPENED_EXISTING_KEY) { JSONClose(hMember); return ERROR_ALREADY_EXISTS; }
	dwLastError = JSONSetValue(hMember, wValueType, lpData, dwDataCount);
	pMember = hMember->pJSON; --pMember->cHandle; --pMember->Header->cHandle; ZeroMemory(hMember, sizeof(_HJSON));
	if (dwLastError) { MemFree(pMember->lpszKeyName); ZeroMemory(pMember, sizeof(SJSON)); --pJSON->dwMemCount; }
	return dwLastError;
}

DWORD JSONAppend(HJSON hJSON, const HJSON hJSONToAppend) {
	if (!hJSON || !hJSON->pJSON || !hJSONToAppend || !hJSONToAppend->pJSON) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pJSONToAppend = hJSONToAppend->pJSON, pTmpJSON = NULL, pMember = NULL, pHeader = NULL, pMemB = NULL, pMemS = NULL, pMemE = NULL; HJSON phJSON = NULL; DWORD dwCount = 0, i = 0;
	if (!pJSON->IsContainer || pJSON->IsDeletePending || !pJSONToAppend->IsContainer || !pJSONToAppend->dwMemCount || pJSONToAppend->IsDeletePending) return ERROR_INVALID_PARAMETER; //or ERROR_INVALID_HANDLE or ERROR_DELETE_PENDING
	dwCount = pJSON->dwMemCount + pJSONToAppend->dwMemCount; pHeader = (pJSON->IsHeader ? pJSON : pJSON->Header);
	if (pJSON->dwMemBuffer) {
		if (dwCount >= pJSON->dwMemBuffer) {
			pMember = (PJSON)MemReAlloc((pMemB = pJSON->lpMembers), (dwCount + 8) * sizeof(SJSON));
			if (!pMember) { return ERROR_NOT_ENOUGH_MEMORY; }
			pJSON->lpMembers = pMember; pJSON->dwMemBuffer = (dwCount + 8);
			for (phJSON = pHeader->pHJSON, pMemS = (pMemB - 1), pMemE = &pMemB[pJSON->dwMemCount], dwCount = pHeader->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
				if (!phJSON->pJSON) continue; --dwCount;
				if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) phJSON->pJSON = &pMember[phJSON->dwIndex]; ////phJSON->pJSON = &pMember[phJSON->pJSON - pMemB];
			}
		}
	}
	else {
		pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer = dwCount + 8) * sizeof(SJSON));
		if (!pJSON->lpMembers) { pJSON->dwMemBuffer = 0; return ERROR_NOT_ENOUGH_MEMORY; }
	}
	pTmpJSON = (PJSON)MemAlloc(sizeof(SJSON));
	if (!pTmpJSON) return ERROR_NOT_ENOUGH_MEMORY;
	if (_JSONCopyTree(pTmpJSON, pJSONToAppend, pHeader)) { MemFree(pTmpJSON); return ERROR_NOT_ENOUGH_MEMORY; }
	CopyMemory(&pJSON->lpMembers[pJSON->dwMemCount], pTmpJSON->lpMembers, pTmpJSON->dwMemCount * sizeof(SJSON));
	pJSON->dwMemCount += pTmpJSON->dwMemCount; MemFree(pTmpJSON);
	return NO_ERROR;
}

DWORD _JSONMerge(PJSON pJSON, PCJSON pJSONToMerge, WORD wFlags, PJSON pHeader) {
	//// JSONMerge() check this
	//if (!pJSON || !pJSON->IsContainer || !pJSONToMerge || !pJSONToMerge->IsContainer || pJSONToMerge->IsDeletePending) return ERROR_INVALID_PARAMETER; //or ERROR_INVALID_HANDLE or ERROR_DELETE_PENDING
	if (!pJSONToMerge->dwMemCount) return NO_ERROR;
	PJSON pMember = NULL, pMem = NULL, pMemTM = NULL, pMemB = NULL, pMemS = NULL, pMemE = NULL; HJSON phJSON = NULL; DWORD dwCount = 0, dwMemCount = 0, dwMemTMCount = 0, dwError = 0, dwIndex = 0, i = 0; BOOLEAN bFound = FALSE;
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
			if (pMember->IsContainer && (pMember->IsContainer == pMemTM->IsContainer)) { dwError = _JSONMerge(pMember, pMemTM, wFlags, pMember->Header); if (dwError) { return dwError; }; continue; }
			else if (wFlags & JSON_NOOVERWRITE) continue;
			if (pMember->IsHandleLock) return ERROR_LOCK_VIOLATION;
			else if (_JSONIsHandleBusy(pMember)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
			_JSONFree(pMember);
		}
		else if (wFlags & JSON_REPLACEONLY) continue;
		else if (pJSON->dwMemBuffer) {
			if (pJSON->dwMemCount == pJSON->dwMemBuffer) {
				pMember = (PJSON)MemReAlloc((pMemB = pJSON->lpMembers), (pJSON->dwMemBuffer *= 2) * sizeof(SJSON));
				if (!pMember) { pJSON->dwMemBuffer /= 2; return ERROR_NOT_ENOUGH_MEMORY; }
				pJSON->lpMembers = pMember; pMember += pJSON->dwMemCount;
				for (phJSON = pHeader->pHJSON, pMemS = (pMemB - 1), pMemE = &pMemB[pJSON->dwMemCount], dwCount = pHeader->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
					if (!phJSON->pJSON) continue; --dwCount;
					if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) phJSON->pJSON = &pJSON->lpMembers[phJSON->dwIndex]; ////phJSON->pJSON = &pJSON->lpMembers[phJSON->pJSON - pMemB];
				}
			}
			else { pMember = &pJSON->lpMembers[pJSON->dwMemCount]; }
		}
		else {
			pJSON->lpMembers = (PJSON)MemAlloc((pJSON->dwMemBuffer = 8) * sizeof(SJSON));
			if (!pJSON->lpMembers) { pJSON->dwMemBuffer = 0; return ERROR_NOT_ENOUGH_MEMORY; }
			pMember = pJSON->lpMembers;
		}
		if (_JSONCopyTree(pMember, pMemTM, pHeader)) return ERROR_NOT_ENOUGH_MEMORY;
		if (!bFound) ++pJSON->dwMemCount;
	}
	//}
	return NO_ERROR;
}

DWORD JSONMerge(HJSON hJSON, const HJSON hJSONToMerge, WORD wFlags) {
	if (!hJSON || !hJSON->pJSON || !hJSONToMerge || !hJSONToMerge->pJSON) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pJSONToMerge = hJSONToMerge->pJSON, pHeader = (pJSON->IsHeader ? pJSON : pJSON->Header);
	if (!pJSON->IsContainer || !pJSONToMerge->IsContainer || pJSONToMerge->IsDeletePending) return ERROR_INVALID_PARAMETER; //or ERROR_INVALID_HANDLE or ERROR_DELETE_PENDING
	else if (!pJSONToMerge->dwMemCount) return NO_ERROR;
	if (wFlags & JSON_APPENDDATA) return JSONAppend(hJSON, hJSONToMerge);
	else if (wFlags & JSON_OVERWRITEALL) {
		if (_JSONIsHandleBusyEx(pJSON, TRUE)) return ERROR_BUSY; // EBUSY - Sub Handle is Open, Edit Pending
		_JSONFree(pJSON);
		if (_JSONCopyTree(pJSON, pJSONToMerge, pHeader)) return ERROR_NOT_ENOUGH_MEMORY;
		return NO_ERROR;
	}
	return _JSONMerge(pJSON, pJSONToMerge, wFlags, pHeader);
}

DWORD JSONReserve(HJSON hJSON, DWORD dwReserve) {
	if (!hJSON || !hJSON->pJSON) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pMember = NULL, pHeader = NULL, pMemB = NULL, pMemS = NULL, pMemE = NULL; HJSON phJSON = NULL; DWORD dwCount = 0, i = 0;
	if (!pJSON->IsContainer || pJSON->dwMemBuffer >= dwReserve) return ERROR_INVALID_PARAMETER;
	if (pJSON->dwMemBuffer) {
		pMember = (PJSON)MemReAlloc((pMemB = pJSON->lpMembers), dwReserve * sizeof(SJSON));
		if (!pMember) return ERROR_NOT_ENOUGH_MEMORY;
		pJSON->lpMembers = pMember; pHeader = (pJSON->IsHeader ? pJSON : pJSON->Header);
		for (phJSON = pHeader->pHJSON, pMemS = (pMemB - 1), pMemE = &pMemB[pJSON->dwMemCount], dwCount = pHeader->cHandle, i = 0; dwCount && i < JSON_MAX_HANDLE; ++i, ++phJSON) {
			if (!phJSON->pJSON) continue; --dwCount;
			if (phJSON->pJSON > pMemS && phJSON->pJSON < pMemE) phJSON->pJSON = &pMember[phJSON->dwIndex]; ////phJSON->pJSON = &pMember[phJSON->pJSON - pMemB];
		}
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

DWORD JSONSort(HJSON hJSON, DWORD dwStart, DWORD dwEnd) {
	if (!hJSON || !hJSON->pJSON) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	PJSON pJSON = hJSON->pJSON, pMember = NULL; DWORD dwCount = 0;
	if (!dwEnd) dwEnd = (pJSON->dwMemCount - 1); //dwEnd = (pJSON->dwMemCount ? (pJSON->dwMemCount - 1) : 0);
	if (!pJSON->IsContainer || pJSON->dwMemCount < 2 || dwStart >= dwEnd || dwEnd >= pJSON->dwMemCount) return ERROR_INVALID_PARAMETER;
	for (pMember = pJSON->lpMembers, dwCount = pJSON->dwMemCount; dwCount; --dwCount, ++pMember) { if (pMember->cHandle) ERROR_BUSY; } // EBUSY - Sub Handle is Open, Edit Pending
	_JSONSort(pJSON, dwStart, dwEnd);
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

DWORD JSONQSort(HJSON hJSON, PFN_QSORTCOMPARE JSONCmp) {
	if (!hJSON || !hJSON->pJSON || !hJSON->pJSON->IsContainer) return ERROR_INVALID_HANDLE;
	else if (hJSON->pJSON->IsHandleLock && !hJSON->LockHandle) return ERROR_LOCK_VIOLATION;
	else if (hJSON->pJSON->dwMemCount < 2) return NO_ERROR;
	PJSON pJSON = hJSON->pJSON, pMember = NULL; DWORD dwCount = 0;
	for (pMember = pJSON->lpMembers, dwCount = pJSON->dwMemCount; dwCount; --dwCount, ++pMember) { if (pMember->cHandle) ERROR_BUSY; } // EBUSY - Sub Handle is Open, Edit Pending
	qsort(pJSON->lpMembers, pJSON->dwMemCount, sizeof(SJSON), JSONCmp);
	return NO_ERROR;
}


