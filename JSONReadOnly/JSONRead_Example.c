#include <stdio.h>
#ifdef _MSC_VER
//#pragma warning(disable: 4996) //project properties->Configuration Properties->C / C++->General->SDL checks->No.
#define _CRT_SECURE_NO_WARNINGS
//#include "JSONRead.h"
#include "JSONRead.c"
//#include "FileReadWrite.h"
#else
#include "/storage/emulated/0/JSON/JSONRead.c"
//#include "/storage/emulated/0/JSON/FileReadWrite.h"
#endif

WCHAR wsJSONStr[] = L"{"     \
L"	\"context\": {"     \
L"		\"arrayList\": [ { \"fff\": \"xx6\" }, { \"eee\": \"xx5\" }, { \"aaa\": \"xx1\" }, { \"ddd\": \"xx4\" }, { \"ccc\": \"xx3\" }, { \"bbb\": \"xx2\" } ],"     \
L"		\"LibraryIDs\": [,,6542, [\"3333\",[\"11\",[],\"888\"], \"777777\"], 3854, 8521, 06438, 7777, \"aaaa\", false, null, true, ,,],"     \
L"		\"user\": {"     \
L"			\'lockedSafetyMode\': false,"     \
L"			\'DeleteKey\': false"     \
L"		},"     \
L"		\"racyCheckOk\": \"abcdefghijklmno\","     \
L"		\"contentCheckOk\": [ 0x222, \"22222\", [ true, false ], {\"111\":\"444\"}, \"\",,  ],"     \
L"		\"arrayStr\": [ \"eee\", \"aaa\", \"fff\", \"ggg\", \"ddd\", \"hhh\", \"iii\", \"ccc\", \"bbb\" ],"     \
L"		\"arrayNum\": [ 777, 222, 666, 111, 555, 999, 333, 888, 444 ]"     \
L"	},"     \
L"	\"TestKey\": \"test\\nstring\\n1234\","     \
L"	\"TestKey2\": -1"     \
L"}";

DWORD JSONOpenKeyI(PCJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPCJSON ppJSONResult) {
	if (!pJSON || !ppJSONResult || !pJSON->IsObject || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; // ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	PCJSON pMember = NULL; DWORD dwMemCount = 0;
	for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwMemCount; --dwMemCount, ++pMember) { ////if (pMember->cchKeyName != cchKeyName || StrCmpNI(pMember->lpszKeyName, lpszKeyName, cchKeyName)) continue;
		if (pMember->cchKeyName == cchKeyName && StringEqualNI(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
			if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
			*ppJSONResult = pMember; return NO_ERROR; ////++pMember->cHandle; 
		}
	}
	return ERROR_FILE_NOT_FOUND; //ENOENT
}

DWORD JSONOpenKeyExI(PCJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPCJSON ppJSONResult) {
	PJSON pMember = NULL; LPCWSTR lpSubKey = NULL; DWORD dwMemCount = 0, cchSubKey = 0;
	if (!pJSON || !pJSON->IsContainer || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName || !ppJSONResult) return ERROR_INVALID_PARAMETER; //or ERROR_INVALID_HANDLE or ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	lpSubKey = wmemchr(lpszKeyName, L'.', cchKeyName); //wcschr(lpszKeyName, L'.');
	if (lpSubKey) { cchSubKey = cchKeyName - (DWORD)(lpSubKey - lpszKeyName); cchKeyName -= cchSubKey; ++lpSubKey; --cchSubKey; }
	for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwMemCount; --dwMemCount, ++pMember) { ////if (pMember->cchKeyName != cchKeyName || StrCmpNI(pMember->lpszKeyName, lpszKeyName, cchKeyName)) continue;
		if (pMember->cchKeyName == cchKeyName && StringEqualNI(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
			if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
			if (cchSubKey) return JSONOpenKeyExI(pMember, lpSubKey, cchSubKey, ppJSONResult);
			*ppJSONResult = pMember; return NO_ERROR; ////++pMember->cHandle; 
		}
	}
	return ERROR_FILE_NOT_FOUND; //ENOENT
}

//JSONParse() by Default initialize all Numbers, so in `JSONRead.c` (ReadOnly mod) you don't need to use the `JSONToNumber()`, but in any case if you want you can use\create your function for to convert the string to number, the `JSONToNumber()` is already there just to give an example, or for more see the `JSONToNumberEx()` in `JSON.c` && `JSONEx.c`
int main() {
	LPWSTR lpszJSON = NULL, lpszJSONEx = NULL; LPCWSTR lpEnd = NULL; DWORD cchStr = 0, cchStrEx = 0, dwLastError = 0, dwDisposition = 0, dwIndex = 0;
	HJSON hJSON = NULL, hJSONEx = NULL; HCJSON hJSONMrg = NULL, hJSKeyEx = NULL, hJSArray = NULL; LPWSTR lpStrfy = NULL; BOOL bReturn = FALSE;
	HCJSON hEnumKey = NULL, hJSEnumKey = NULL, hJSKey = NULL, hJSNewKey = NULL;

	lpszJSON = &wsJSONStr[0];
	cchStr = StrLen(lpszJSON); wprintf(L"1. StrLen %d \n", cchStr);

	hJSON = JSONParse(lpszJSON, cchStr, &lpEnd);
	////MemFree(lpszJSON); lpszJSON = NULL;
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	HCJSON hcJSON = NULL; hcJSON = hJSON;
	wprintf(L"1. isHeader %d \n", hcJSON->IsHeader);
	wprintf(L"2. isObject %d \n", hcJSON->IsObject);
	wprintf(L"3. dwNestedLevel %d \n", hcJSON->dwNestedLevel);
	wprintf(L"4. dwMemCount %d \n", JSONContainerLength(hcJSON));
	wprintf(L"5. JSONGetStringifySize %d\n", JSONGetStringifySize(hcJSON));
	//dwLastError = JSONOpenKey1(hcJSON, L"context.user.lockedSafetymode", 29, &hJSKey);
	//if (dwLastError) { wprintf(L"0. JSONOpenKey1 ERROR %d \n", dwLastError); JSONFree(hJSON); return 0; }
	//else {
	//	//(hJSKey->wValueType & JSON_IS_STRING)  (hJSKey->wValueType & JSON_IS_ESCAPESTR) (hJSKey->wValueType & JSON_IS_SINGLEQUOTE) 
	//	wprintf(L"JSONKey1 - isString %d - isEscapeStr %d - isSingleQuote %d\n", JSONIsString(hJSKey), JSONIsEscapeStr(hJSKey), JSONIsSingleQuote(hJSKey));
	//	wprintf(L"JSONKey - cchValue %d - Value %ls\n", hJSKey->cchValue, hJSKey->lpszValue); //cchUnEscapeStr is for Internal Use Only ---> wprintf(L"JSONKey - cchUnEscapeStr %d\n" hJSKey->cchUnEscapeStr);
	//	if (JSONIsEscapeStr(hJSKey)) { wprintf(L"JSONKey - Value %ls\n", JSONUnEscapeStr(hJSKey->lpszValue, hJSKey->cchValue, 0)); }
	//}
	//dwLastError = JSONOpenKey2(hcJSON, L"context.user.lockedSafetymode", 29, &hJSKey);
	//if (dwLastError) { wprintf(L"0. JSONOpenKey2 ERROR %d \n", dwLastError); JSONFree(hJSON); return 0; }
	//else {
	//	//(hJSKey->wValueType & JSON_IS_STRING)  (hJSKey->wValueType & JSON_IS_ESCAPESTR) (hJSKey->wValueType & JSON_IS_SINGLEQUOTE) 
	//	wprintf(L"JSONKey2 - isString %d - isEscapeStr %d - isSingleQuote %d\n", JSONIsString(hJSKey), JSONIsEscapeStr(hJSKey), JSONIsSingleQuote(hJSKey));
	//	wprintf(L"JSONKey - cchValue %d - Value %ls\n", hJSKey->cchValue, hJSKey->lpszValue); //cchUnEscapeStr is for Internal Use Only ---> wprintf(L"JSONKey - cchUnEscapeStr %d\n" hJSKey->cchUnEscapeStr);
	//	if (JSONIsEscapeStr(hJSKey)) { wprintf(L"JSONKey - Value %ls\n", JSONUnEscapeStr(hJSKey->lpszValue, hJSKey->cchValue, 0)); }
	//}
	//JSONFree(hJSON); return 0;
	lpStrfy = JSONStringify(hcJSON);
	if (!lpStrfy) { wprintf(L"6. JSONStringify ERROR %d \n", GetLastError()); return 0; }
	else { wprintf(L"6. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
	wprintf(L"\n\nTest JSONOpenKey\n");
	//// JSONOpenKey() does not support subkey and array index, example "items.description" or "items[1].description" return ERROR_FILE_NOT_FOUND
	//dwLastError = JSONOpenKeyI(hcJSON, L"Testkey", 7, &hJSKey);
	//dwLastError = JSONOpenKeyExI(hcJSON, L"ContexT.contentCheckok", 22, &hJSKey);
	//dwLastError = JSONOpenKey(hcJSON, L"TestKey", 7, &hJSKey);
	//dwLastError = JSONOpenKeyEx(hcJSON, L"[0][1][0]", 9, &hJSKey);
	//dwLastError = JSONOpenKeyEx(hcJSON, L"context.LibraryIDs[3][0]", 24, &hJSKey);
	dwLastError = JSONOpenKeyEx(hcJSON, L"context.arrayNum[2]", 19, &hJSKey);
	//dwLastError = JSONOpenKeyEx(hcJSON, L"context.contentCheckOk[3].111", 29, &hJSKey);
	if (dwLastError) { wprintf(L"0. JSONOpenKey ERROR %d \n", dwLastError); JSONFree(hJSON); return 0; }
	else {
		wprintf(L"1. JSONGetKey - isContainer %d\n", hJSKey->IsContainer);
		wprintf(L"2. JSONGetKey - isStrValue %d\n", hJSKey->IsStrValue);
		wprintf(L"JSONKey - wValueType 0x%04X\n", hJSKey->wValueType); // - L"%02X%02X"
		if (hJSKey->IsObject || hJSKey->IsArray) { //if (hJSKey->wValueType & JSON_IS_OBJECT_ARRAY)
			//(hJSKey->wValueType & JSON_IS_OBJECT) (hJSKey->wValueType & JSON_IS_ARRAY)
			if (hJSKey->IsObject) wprintf(L"JSONKey - isObject %d\n", hJSKey->IsObject);
			else wprintf(L"JSONKey - isArray %d\n", hJSKey->IsArray);
			wprintf(L"JSONKey - Member Count %d \n", JSONContainerLength(hJSKey));
		}
		else if (hJSKey->IsString) { //if (hJSKey->wValueType & JSON_IS_STRING)
			//(hJSKey->wValueType & JSON_IS_STRING)  (hJSKey->wValueType & JSON_IS_ESCAPESTR) (hJSKey->wValueType & JSON_IS_SINGLEQUOTE) 
			wprintf(L"JSONKey - isString %d - isEscapeStr %d - isSingleQuote %d\n", hJSKey->IsString, hJSKey->IsEscapeStr, hJSKey->IsSingleQuote);
			wprintf(L"JSONKey - cchValue %d - Value %ls\n", hJSKey->cchValue, hJSKey->lpszValue); //cchUnEscapeStr is for Internal Use Only ---> wprintf(L"JSONKey - cchUnEscapeStr %d\n" hJSKey->cchUnEscapeStr);
			if (hJSKey->IsEscapeStr) { wprintf(L"JSONKey - Value %ls\n", JSONUnEscapeStr(hJSKey->lpszValue, hJSKey->cchValue, 0)); }
		}
		else if (hJSKey->IsBoolean) { //if (hJSKey->wValueType & JSON_IS_BOOLEAN)
			//(hJSKey->wValueType & JSON_IS_BOOLTRUE)
			wprintf(L"JSONKey - isBoolean %d - isBoolean %d\n", hJSKey->IsBoolean, hJSKey->IsBoolTrue);
			wprintf(L"JSONKey - cchValue %d - Value %ls\n", hJSKey->cchValue, hJSKey->lpszValue);
			wprintf(L"JSONKey - bValue "); wprintf(hJSKey->bValue ? L"True\n" : L"False\n"); //get BOOL
		}
		else if (hJSKey->IsNull) { //if (hJSKey->wValueType & JSON_IS_NULL)
			wprintf(L"JSONKey - isNull %d\n", hJSKey->IsNull);
			wprintf(L"JSONKey - cchValue %d - Value %ls\n", hJSKey->cchValue, hJSKey->lpszValue);
		}
		else if (hJSKey->IsNumber) { //if (hJSKey->wValueType & JSON_IS_NUMBER)
			//(hJSKey->wValueType & JSON_IS_SIGNED)  (hJSKey->wValueType & JSON_IS_FLOAT) (hJSKey->wValueType & JSON_IS_HEXDIGIT) 
			wprintf(L"JSONKey - isNumber %d - isSigned %d - isFloat %d - isHexDigit %d\n", hJSKey->IsNumber, hJSKey->IsSigned, hJSKey->IsFloat, hJSKey->IsHexDigit);
			wprintf(L"JSONKey - cchValue %d - Value %ls\n", hJSKey->cchValue, hJSKey->lpszValue);
			//see struct _JSON_NUMBER_ 
            if (hJSKey->IsFloat) { //or if (hJSKey->wValueType & JSON_IS_FLOAT)
                wprintf(L"JSONKey - dValue %.8lf\n", hJSKey->dValue); //get DOUBLE
                wprintf(L"JSONKey - fValue %f\n", hJSKey->fValue); //get FLOAT
            }
            else if (hJSKey->IsSigned) { //or if (hJSKey->wValueType & JSON_IS_SIGNED)
                wprintf(L"JSONKey - iValue %d\n", hJSKey->iValue); //get INT
                wprintf(L"JSONKey - illValue %lld\n", hJSKey->illValue); //get INT64
            }
            else {
                wprintf(L"JSONKey - dwValue %u\n", hJSKey->dwValue); //get DWORD
                wprintf(L"JSONKey - ullValue %llu\n", hJSKey->ullValue); //get UINT64
            }
		}
	}
	wprintf(L"41. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSKey));
	lpStrfy = JSONStringify(hJSKey); //should be JSONStringify Error//JSONClose(hJSKey);	
	if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); } //JSONFree(hJSON); return 0;
	else { wprintf(L"51. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
		//bReturn = FileWriteEx("/storage/emulated/0/Download/JSON_TEST/05_JSONOpenKey.json", lpStrfy, StrLen(lpStrfy), iFlagEx, iMode, UTF_8);
		//if (!bReturn) { wprintf(L"6. FileWriteEx ERROR %d \n", GetLastError()); }
		//MemFree(lpStrfy); lpStrfy = NULL;
	//}
	dwLastError = JSONOpenKey(hcJSON, L"context", 7, &hJSKey); //dwLastError =  JSONCreateKey(hJSNewKey, L"ObjectKey", 9, JSON_IS_OBJECT, &hJSKeyEx, &dwDisposition);
	if (dwLastError) { wprintf(L"0. JSONOpenKey ERROR %d \n", dwLastError); JSONFree(hJSONEx); return 0; }
	dwIndex = 0; //dwIndex 0 is "ObjectKey", JSONEnumKey(hJSNewKey, 0, &hJSEnumKey) return ERROR_SHARING_VIOLATION - handle is busy, handle is already open, you can (avoid this) open\enum all (in ReadOnly), using JSONFindFirstKey() and JSONFindNextKey() that return const handle, that not need to use JSONCloseKey()
	while (JSONEnumKey(hJSKey, dwIndex, &hJSEnumKey) == NO_ERROR) {
		wprintf(L"KeyNameLength %d - KeyName %ls \n", hJSEnumKey->cchKeyName, hJSEnumKey->lpszKeyName);
		if (hJSEnumKey->IsContainer) { //if (hJSEnumKey->IsObject || hJSEnumKey->IsArray) or //if (hJSEnumKey->wValueType & JSON_IS_OBJECT_ARRAY)
			wprintf(L"MemberCount %d - MemberBuffer %d \n", JSONContainerLength(hJSEnumKey), JSONContainerSize(hJSEnumKey));
		}
		else if (hJSEnumKey->IsString) { //if (hJSEnumKey->wValueType & JSON_IS_STRING)
			wprintf(L"cchValue %d - Value %ls\n", hJSEnumKey->cchValue, hJSEnumKey->lpszValue);
		}
		++dwIndex;
	}

	JSONFree(hJSON); hJSON = NULL; // or JSONClose(hJSON); //JSONClose() && JSONCloseHandle() && JSONCloseKey() - if Handle is Header run JSONFree();
	system("pause");
	return 0;
}

