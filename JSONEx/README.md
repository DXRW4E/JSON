# JSON for C/C++

***To be always 100% safe is recommended to use the [JSON for C/C++](https://github.com/DXRW4E/JSON)***

Guys this `JSONEx.c` (I just called JSONEx.c) is the original `JSON.c` code, in fact it is more cleaner and more readable and editable (more easier to modify\update\improve), the problem is that it doesn't support Handle Management like `JSON.c`
in simple words it does not support resize, If a capacity change takes place, all open Handle\HINSTANCE of that container are invalidated, example
```c
////hMemberList Container Count 5 - Size 5
// MemberList[5]
//  Member0[0]
//  Member1[1]
//  Member2[2]
//  Member3[3]
//  Member4[4]

HJSON hJSKey2 = NULL, hJSKey5 = NULL; DWORD dwLastError = 0;
dwLastError = JSONOpenKey(hMemberList, L"Member2", 7, &hJSKey2);
//return ERROR_SUCCESS 0

dwLastError = JSONCreateKey(hMemberList, L"Member5", 7, JSON_IS_OBJECT, &hJSKey5, NULL);
//return ERROR_BUSY 170 - EBUSY 16

// Delete the Key is not a problem, since if Container is busy JSON.c and JSONEx.c make the Key DeletePending, 
//  no need to resize Container
dwLastError = JSONDeleteKey(hMemberList, L"Member3", 7);
//return ERROR_SUCCESS 0
```

***To avoid all this you need to use JSONReserve()***
```c
HJSON hJSKey2 = NULL, hJSKey5 = NULL; DWORD dwLastError = 0;
dwLastError = JSONReserve(hMemberList, 10);
//return ERROR_SUCCESS 0

dwLastError = JSONOpenKey(hMemberList, L"Member2", 7, &hJSKey2);
//return ERROR_SUCCESS 0

dwLastError = JSONCreateKey(hMemberList, L"Member5", 7, JSON_IS_OBJECT, &hJSKey5, NULL);
//return ERROR_SUCCESS 0

////hMemberList Container Count 6 - Size 10
// MemberList[6]
//  Member0[0]
//  Member1[1]
//  Member2[2]
//  Member3[3]
//  Member4[4]
//  Member5[5]
```
Guys all this in case you use a lot of handles ehhh, but normally for personal use, the `JSONEx.c` should be always be OKKKKKK.

Regard string comparison, if you want to search\get\open KeyName in Case-insensitive string comparison, in `JSONOpenKey()` or `JSONOpenKeyEx()` you just need to replace the 
```c
if (pMember->cchKeyName == cchKeyName && EqualMemoryEx(pMember->lpszKeyName, lpszKeyName, cchKeyName))
```
with the
```c
if (pMember->cchKeyName == cchKeyName && StringEqualNI(pMember->lpszKeyName, lpszKeyName, cchKeyName))
```
Or as I've always said, everything is really simple there, you can write your funcs in a few lines, example
```c
DWORD JSONOpenKeyI(PCJSON pJSON, LPCWSTR lpszKeyName, DWORD cchKeyName, PPCJSON ppJSONResult) {
	if (!pJSON || !ppJSONResult || !pJSON->IsObject || !pJSON->dwMemCount || pJSON->IsDeletePending || !lpszKeyName) return ERROR_INVALID_PARAMETER; // ERROR_DELETE_PENDING
	else if (!cchKeyName && !(cchKeyName = StrLen(lpszKeyName))) return ERROR_INVALID_DATA;
	PCJSON pMember = NULL; DWORD dwMemCount = 0;
	for (pMember = pJSON->lpMembers, dwMemCount = pJSON->dwMemCount; dwMemCount; --dwMemCount, ++pMember) { ////if (pMember->cchKeyName != cchKeyName || StrCmpNI(pMember->lpszKeyName, lpszKeyName, cchKeyName)) continue;
		if (pMember->cchKeyName == cchKeyName && StringEqualNI(pMember->lpszKeyName, lpszKeyName, cchKeyName)) {
			if (pMember->IsDeletePending) return ERROR_DELETE_PENDING;
			++pMember->cHandle; *ppJSONResult = pMember; return NO_ERROR;
		}
	}
	return ERROR_FILE_NOT_FOUND; //ENOENT
}
```
```c
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
			++pMember->cHandle; *ppJSONResult = pMember; return NO_ERROR;
		}
	}
	return ERROR_FILE_NOT_FOUND; //ENOENT
}
```
or use Function like `PathMatchSpecW()` && `fnmatch()`


Guys, you only need to add `JSONEx.h` and `JSONEx.c` to your projects source and that's it

here are some examples
# JSON used for testing
```c
WCHAR wsJSONStr[] = L"{"     \
	L"	\"context\": {"     \
	L"		\"arrayList\": [ { \"fff\": \"xx6\" }, { \"eee\": \"xx5\" }, { \"aaa\": \"xx1\" }, { \"ddd\": \"xx4\" }, { \"ccc\": \"xx3\" }, { \"bbb\": \"xx2\" } ],"     \
	L"		\"LibraryIDs\": [,,6542, [\"3333\",[\"11\",[],\"888\"], \"777777\"], 3854, 8521, 06438, 7777, \"aaaa\", false, null, true, ,,],"     \
	L"		\"user\": {"     \
	L"			\'lockedSafetyMode\': false,"     \
	L"			\'DeleteKey1\': false"     \
	L"		},"     \
	L"		\"DeleteKey2\": \"abcdefghijklmno\","     \
	L"		\"DeleteKeyOnClose\": [ 0x222, \"22222\", [ true, false ], {\"111\":\"444\"}, \"\",,  ],"     \
	L"		\"NoOverwrite\": [ \"\",,  ],"     \
	L"		\"ReplaceOnly\": [ \"\",,  ],"     \
	L"		\"arrayStr\": [ \"eee\", \"aaa\", \"fff\", \"ggg\", \"ddd\", \"hhh\", \"iii\", \"ccc\", \"bbb\" ],"     \
	L"		\"arrayNum\": [ 777, 222, 666, 111, 555, 999, 333, 888, 444 ]"     \
	L"	},"     \
	L"	\"TestKey\": \"test\\nstring\\n1234\","     \
	L"	\"TestKey2\": -1"     \
	L"}";

WCHAR wsJSONStrEx[] = L"{"     \
	L"	\"NewKeyName3\": [],"     \
	L"	\"context\": {"     \
	L"		\"arrayList\": [ { \"iii\": \"yy9\" }, { \"hhh\": \"yy8\" }, { \"aaa\": \"yy1\" }, { \"ddd\": \"yy4\" }, { \"ccc\": \"yy3\" }, { \"ggg\": \"yy7\" } ],"     \
	L"		\"LibraryIDs\": [,,54321, [\"222\",[\"99\",[],\"5555\"], \"88888\"], 3854, 1233, 06438, 7777, \"bbbb\", true, null, true, ,,],"     \
	L"		\"user\": {"     \
	L"			\'lockedSafetyMode\': \"StringData\""     \
	L"		},"     \
	L"		\"NoOverwrite\": 12345,"     \
	L"		\"ReplaceOnly2\": {}"     \
	L"	},"     \
	L"	\"TestKey\": 123.456"     \
	L"}";
```

# JSONParse() && JSONStringify()
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL; LPCWSTR lpEnd = NULL; LPWSTR lpStrfy = NULL;

	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	wprintf(L"1. isHeader %d \n", JSONPtr(hJSON)->IsHeader);
	wprintf(L"2. isObject %d \n", JSONPtr(hJSON)->IsObject);
	wprintf(L"3. dwNestedLevel %d \n", JSONPtr(hJSON)->dwNestedLevel);
	wprintf(L"4. dwMemCount %d \n", JSONContainerLength(hJSON));
	wprintf(L"5. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSON));
	lpStrfy = JSONStringify(hJSON);
	if (!lpStrfy) { wprintf(L"6. JSONStringify ERROR %d \n", GetLastError()); }
	else { wprintf(L"6. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
	JSONFree(hJSON); hJSON = NULL; // or JSONClose(hJSON); //JSONClose() && JSONCloseHandle() && JSONCloseKey() - if Handle is Header run JSONFree();
	return 0;
}
```

# JSONOpenKey() && JSONOpenKeyEx() && JSONOpenElement()
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL, hJSKey = NULL, hJSElement = NULL; LPWSTR lpszJSON = NULL; LPCWSTR lpEnd = NULL; DWORD dwLastError = 0, cchStr = 0;

	//lpszJSON = FileReadEx(lpszJSONPath);
	lpszJSON = &wsJSONStr[0]; cchStr = StrLen(lpszJSON); //wprintf(L"1. StrLen %d \n", cchStr);
	hJSON = JSONParse(lpszJSON, cchStr, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	dwLastError = JSONOpenKey(hJSON, L"TestKey", 7, &hJSKey);
	if (dwLastError) { wprintf(L"1. JSONOpenKey ERROR %d \n", dwLastError); }
	else { //if (JSONPtr(hJSKey)->IsObject || JSONPtr(hJSKey)->IsArray) { //if (JSONPtr(hJSKey)->wValueType & JSON_IS_OBJECT_ARRAY)
		if (JSONPtr(hJSKey)->IsContainer) { wprintf(L"hJSKey - MemberCount %d  - MemberBuffer %d \n", JSONContainerLength(hJSKey), JSONContainerSize(hJSKey)); }
		else { 
			wprintf(L"hJSKey - ValueLength %d  - ValueData %ls \n", JSONValueLength(hJSKey), JSONValueData(hJSKey));
			if (JSONPtr(hJSKey)->IsEscapeStr) { wprintf(L"JSONKey - Value %ls\n", JSONUnEscapeStr(JSONValueData(hJSKey), JSONValueLength(hJSKey), 0)); }
		}
		JSONCloseHandle(hJSKey);  //or JSONCloseKey(hJSKey)  or JSONClose(hJSKey); 
	}
	dwLastError = JSONOpenKeyEx(hJSON, L"context.arrayList[2].aaa", 24, &hJSKey);
	if (dwLastError) { wprintf(L"2. JSONOpenKeyEx ERROR %d \n", dwLastError); }
	else { //if (JSONPtr(hJSKey)->IsObject || JSONPtr(hJSKey)->IsArray) { //if (JSONPtr(hJSKey)->wValueType & JSON_IS_OBJECT_ARRAY)
		if (JSONPtr(hJSKey)->IsContainer) { wprintf(L"hJSKey - MemberCount %d  - MemberBuffer %d \n", JSONContainerLength(hJSKey), JSONContainerSize(hJSKey)); }
		else { wprintf(L"hJSKey - ValueLength %d  - ValueData %ls \n", JSONValueLength(hJSKey), JSONValueData(hJSKey)); }
		JSONCloseHandle(hJSKey);  //or JSONCloseKey(hJSKey)  or JSONClose(hJSKey); 
	}
	dwLastError = JSONOpenKeyEx(hJSON, L"context.LibraryIDs[3][1]", 24, &hJSKey);
	if (dwLastError) { wprintf(L"3. JSONOpenKeyEx ERROR %d \n", dwLastError); }
	else {
        dwLastError = JSONOpenElement(hJSKey, 2, &hJSElement); //Open Index 2
        if (dwLastError) { wprintf(L"4. JSONOpenKey ERROR %d \n", dwLastError); }
        else { //if (JSONPtr(hJSElement)->IsObject || JSONPtr(hJSElement)->IsArray) { //if (JSONPtr(hJSElement)->wValueType & JSON_IS_OBJECT_ARRAY)
            if (JSONPtr(hJSElement)->IsContainer) { wprintf(L"hJSElement - MemberCount %d  - MemberBuffer %d \n", JSONContainerLength(hJSElement), JSONContainerSize(hJSElement)); }
            else { wprintf(L"hJSElement - ValueLength %d  - ValueData %ls \n", JSONValueLength(hJSElement), JSONValueData(hJSElement)); }
            JSONCloseHandle(hJSElement);  //or JSONCloseKey(hJSElement)  or JSONClose(hJSElement); 
        }
		JSONCloseHandle(hJSKey);  //or JSONCloseKey(hJSKey)  or JSONClose(hJSKey); 
	}
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONCreateKey() && JSONInsertKey() && JSONCreateElement() && JSONInsertElement()
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL, hJSKey = NULL, hJSNewKey = NULL, hJSElement = NULL; 
	LPCWSTR lpEnd = NULL, lpStrfy = NULL; DWORD dwLastError = 0, dwDisposition = 0;

	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	dwLastError = JSONCreateKey(hJSON, L"NewKeyName", 10, JSON_IS_OBJECT, &hJSNewKey, &dwDisposition);
	if (dwLastError) { wprintf(L"1. JSONCreateKey ERROR %d \n", dwLastError); }
	else {
		wprintf(L"1. dwDisposition %d \n", dwDisposition);
		wprintf(L"1. IsArray %d \n", JSONPtr(hJSNewKey)->IsArray);
		wprintf(L"2. isObject %d \n", JSONPtr(hJSNewKey)->IsObject);
		wprintf(L"3.0. hJSON dwMemCount %d \n", JSONContainerLength(hJSON));
		wprintf(L"3.1. hJSNewKey dwMemBuffer %d - dwMemCount %d \n", JSONContainerSize(hJSNewKey), JSONContainerLength(hJSNewKey));
		wprintf(L"4. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSNewKey));
		dwLastError = JSONCreateKey(hJSNewKey, L"NewKeyName2", 11, JSON_IS_ARRAY, &hJSKey, &dwDisposition);
		if (dwLastError) { wprintf(L"0. JSONCreateKey ERROR %d \n", dwLastError); JSONFree(hJSON); return 0; }
		else {
            wprintf(L"1. dwDisposition %d \n", dwDisposition);
            wprintf(L"1. IsArray %d \n", JSONPtr(hJSKey)->IsArray);
            wprintf(L"2. isObject %d \n", JSONPtr(hJSNewKey)->IsObject);
            wprintf(L"3.0. hJSON dwMemCount %d \n", JSONContainerLength(hJSON));
            wprintf(L"3.1. hJSKey dwMemBuffer %d - dwMemCount %d \n", JSONContainerSize(hJSKey), JSONContainerLength(hJSKey));
            wprintf(L"4. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSNewKey));
            JSONClose(hJSKey);
		}
		JSONClose(hJSNewKey);
	}
	dwLastError = JSONInsertKey(hJSON, L"NewKeyName3", 11, JSON_IS_OBJECT, &hJSNewKey, 0);
	if (dwLastError) { wprintf(L"2. JSONInsertKey ERROR %d \n", dwLastError); JSONFree(hJSON); return 0; }
	else {
		dwLastError = JSONCreateKey(hJSNewKey, L"NewKeyName4", 11, JSON_IS_ARRAY, &hJSKey, &dwDisposition);
		if (dwLastError) { wprintf(L"3. JSONCreateKey ERROR %d \n", dwLastError); JSONFree(hJSON); return 0; }
		else {
			dwLastError = JSONCreateElement(hJSKey, JSON_IS_STRING, &hJSElement);
			if (dwLastError) { wprintf(L"4. JSONCreateElement ERROR %d \n", dwLastError); }
			else {
				dwLastError = JSONSetValue(hJSElement, JSON_IS_STRING, L"1673458487", 10);
				if (dwLastError) { wprintf(L"5. JSONSetValue ERROR %d \n", dwLastError); }
				JSONClose(hJSElement);
			}
			dwLastError = JSONInsertElement(hJSKey, (JSON_IS_NUMBER | JSON_IS_FLOAT), &hJSElement, 0);
			if (dwLastError) { wprintf(L"6. JSONInsertElement ERROR %d \n", dwLastError); }
			else {
				dwLastError = JSONSetValue(hJSElement, (JSON_IS_NUMBER | JSON_IS_FLOAT), L"123.4578", 8);
				if (dwLastError) { wprintf(L"7. JSONSetValue ERROR %d \n", dwLastError); }
				JSONClose(hJSElement);
			}
			JSONClose(hJSKey);
		}
		JSONClose(hJSNewKey);
	}
	lpStrfy = JSONStringify(hJSON);
	if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); }
	else { wprintf(L"5. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONRenameKey()
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL, hJSKey = NULL; LPCWSTR lpEnd = NULL, lpStrfy = NULL; DWORD dwLastError = 0;

	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	dwLastError = JSONOpenKey(hJSON, L"TestKey2", 8, &hJSKey);
	if (dwLastError) { wprintf(L"1. JSONOpenKey ERROR %d \n", dwLastError); }
	else {
		dwLastError = JSONRenameKey(hJSKey, L"NewKeyName0", 11);
		if (dwLastError) { wprintf(L"2. JSONRenameKey ERROR %d \n", dwLastError); JSONFree(hJSON); return 0; }
		else {
			wprintf(L"1. IsArray %d \n", JSONPtr(hJSKey)->IsArray);
			wprintf(L"2. isObject %d \n", JSONPtr(hJSKey)->IsObject);
			wprintf(L"3. dwMemBuffer %d - dwMemCount %d \n", JSONContainerSize(hJSKey), JSONContainerLength(hJSKey));
			wprintf(L"4. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSON));	
		}
		JSONClose(hJSKey);
	}
	lpStrfy = JSONStringify(hJSON);
	if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); }
	else { wprintf(L"5. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONDeleteKey() && JSONDeleteKeyEx() && JSONDeleteOnClose()
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL, hJSKey = NULL; LPCWSTR lpEnd = NULL, lpStrfy = NULL; DWORD dwLastError = 0;

	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	dwLastError = JSONOpenKey(hJSON, L"context", 7, &hJSKey);
	if (dwLastError) { wprintf(L"1. JSONOpenKey ERROR %d \n", dwLastError); }
	else {
		wprintf(L"2. dwMemBuffer %d - dwMemCount %d \n", JSONContainerSize(hJSKey), JSONContainerLength(hJSKey));
		dwLastError = JSONDeleteKey(hJSKey, L"DeleteKey2", 10);
		if (dwLastError) { wprintf(L"2. JSONDeleteKey ERROR %d \n", dwLastError); }
		wprintf(L"3. dwMemBuffer %d - dwMemCount %d \n", JSONContainerSize(hJSKey), JSONContainerLength(hJSKey));
		JSONClose(hJSKey);
	}
	dwLastError = JSONDeleteKeyEx(hJSON, L"context.user.DeleteKey1", 23);
	if (dwLastError) { wprintf(L"3. JSONOpenKeyEx ERROR %d \n", dwLastError); }
	dwLastError = JSONOpenKeyEx(hJSON, L"context.DeleteKeyOnClose", 24, &hJSKey);
	if (dwLastError) { wprintf(L"6. JSONOpenKey ERROR %d \n", dwLastError); }
	else {
		dwLastError = JSONDeleteOnClose(hJSKey);
		if (dwLastError) { wprintf(L"7. JSONDeleteOnClose ERROR %d \n", dwLastError); }
		JSONClose(hJSKey);
	}
	lpStrfy = JSONStringify(hJSON);
	if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); }
	else { wprintf(L"5. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONEnumKey()
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL, hJSKey = NULL, hJSEnumKey = NULL; LPCWSTR lpEnd = NULL; DWORD dwLastError = 0, dwIndex = 0;

	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	dwLastError = JSONOpenKey(hJSON, L"context", 7, &hJSKey);
	if (dwLastError) { wprintf(L"1. JSONOpenKey ERROR %d \n", dwLastError); }
	else {
		while (JSONEnumKey(hJSKey, dwIndex, &hJSEnumKey) == NO_ERROR) {
			if (JSONPtr(hJSEnumKey)->cchKeyName) { wprintf(L"0. cchKeyName %d - KeyName %ls\n", JSONPtr(hJSEnumKey)->cchKeyName, JSONPtr(hJSEnumKey)->lpszKeyName); }
			wprintf(L"1. isContainer %d\n", JSONPtr(hJSEnumKey)->IsContainer);
			wprintf(L"2. isStrValue %d\n", JSONPtr(hJSEnumKey)->IsStrValue);
			wprintf(L"3. wValueType 0x%04X\n", JSONPtr(hJSEnumKey)->wValueType); // - L"%02X%02X"
			if (JSONPtr(hJSEnumKey)->IsObject || JSONPtr(hJSEnumKey)->IsArray) { //if (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_OBJECT_ARRAY)
				//(JSONPtr(hJSEnumKey)->wValueType & JSON_IS_OBJECT) (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_ARRAY)
				if (JSONPtr(hJSEnumKey)->IsObject) wprintf(L"4. isObject %d\n", JSONPtr(hJSEnumKey)->IsObject);
				else wprintf(L"4. isArray %d\n", JSONPtr(hJSEnumKey)->IsArray);
				wprintf(L"5. Member Count %d \n", JSONContainerLength(hJSEnumKey));
			}
			else if (JSONPtr(hJSEnumKey)->IsString) { //if (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_STRING)
				//(JSONPtr(hJSEnumKey)->wValueType & JSON_IS_STRING)  (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_ESCAPESTR) (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_SINGLEQUOTE) 
				wprintf(L"4. isString %d - isEscapeStr %d - isSingleQuote %d\n", JSONPtr(hJSEnumKey)->IsString, JSONPtr(hJSEnumKey)->IsEscapeStr, JSONPtr(hJSEnumKey)->IsSingleQuote);
				wprintf(L"5. cchValue %d - Value %ls\n", JSONPtr(hJSEnumKey)->cchValue, JSONPtr(hJSEnumKey)->lpszValue); //cchUnEscapeStr is for Internal Use Only ---> wprintf(L"JSONKey - cchUnEscapeStr %d\n" JSONPtr(hJSEnumKey)->cchUnEscapeStr);
				if (JSONPtr(hJSEnumKey)->IsEscapeStr) { wprintf(L"JSONKey - Value %ls\n", JSONUnEscapeStr(JSONPtr(hJSEnumKey)->lpszValue, JSONPtr(hJSEnumKey)->cchValue, 0)); }
			}
			else if (JSONPtr(hJSEnumKey)->IsBoolean) { //if (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_BOOLEAN)
				//(JSONPtr(hJSEnumKey)->wValueType & JSON_IS_BOOLTRUE)
				wprintf(L"4. isBoolean %d - isBoolean %d\n", JSONPtr(hJSEnumKey)->IsBoolean, JSONPtr(hJSEnumKey)->IsBoolTrue);
				wprintf(L"5. cchValue %d - Value %ls\n", JSONValueLength(hJSEnumKey), JSONValueData(hJSEnumKey));
				wprintf(L"6. bValue "); wprintf(JSONPtr(hJSEnumKey)->bValue ? L"True\n" : L"False\n"); //get BOOL
			}
			else if (JSONPtr(hJSEnumKey)->IsNull) { //if (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_NULL)
				wprintf(L"4. isNull %d\n", JSONPtr(hJSEnumKey)->IsNull);
				wprintf(L"5. cchValue %d - Value %ls\n", JSONValueLength(hJSEnumKey), JSONValueData(hJSEnumKey));
			}
			else if (JSONPtr(hJSEnumKey)->IsNumber) { //if (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_NUMBER)
				//(JSONPtr(hJSEnumKey)->wValueType & JSON_IS_SIGNED)  (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_FLOAT) (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_HEXDIGIT) 
				wprintf(L"4. isNumber %d - isSigned %d - isFloat %d - isHexDigit %d\n", JSONPtr(hJSEnumKey)->IsNumber, JSONPtr(hJSEnumKey)->IsSigned, JSONPtr(hJSEnumKey)->IsFloat, JSONPtr(hJSEnumKey)->IsHexDigit);
				wprintf(L"5. cchValue %d - Value %ls\n", JSONValueLength(hJSEnumKey), JSONValueData(hJSEnumKey));
				//see struct _JSON_NUMBER_ 
				if (JSONPtr(hJSEnumKey)->IsFloat) { // or if (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_FLOAT)
					wprintf(L"4.dValue %.8lf\n", JSONPtr(hJSEnumKey)->dValue); //get DOUBLE
					wprintf(L"5. fValue %f\n", JSONPtr(hJSEnumKey)->fValue); //get FLOAT
				}
				else if (JSONPtr(hJSEnumKey)->IsSigned) { // or if (JSONPtr(hJSEnumKey)->wValueType & JSON_IS_SIGNED)
					wprintf(L"4. iValue %d\n", JSONPtr(hJSEnumKey)->iValue); //get INT
					wprintf(L"5. illValue %lld\n", JSONPtr(hJSEnumKey)->illValue); //get INT64
				}
				else {
					wprintf(L"4. dwValue %u\n", JSONPtr(hJSEnumKey)->dwValue); //get DWORD
					wprintf(L"5. ullValue %llu\n", JSONPtr(hJSEnumKey)->ullValue); //get UINT64
				}
			}
			++dwIndex; JSONClose(hJSEnumKey); 
		}
		JSONCloseHandle(hJSKey);  //or JSONCloseKey(hJSKey)  or JSONClose(hJSKey); 
	}
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONCopyTreeEx()
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL, hJSCopy = NULL; LPCWSTR lpEnd = NULL, lpStrfy = NULL;
	
	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	else {
	   hJSCopy = JSONCopyTreeEx(hJSON);
		if (!hJSCopy) { wprintf(L"1. JSONCopyTree ERROR %d \n", GetLastError()); }
		else {
			wprintf(L"1. isHeader %d \n", JSONPtr(hJSCopy)->IsHeader);
			wprintf(L"2. isObject %d \n", JSONPtr(hJSCopy)->IsObject);
			wprintf(L"3. dwMemCount %d \n", JSONContainerLength(hJSCopy));
			wprintf(L"4. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSCopy));
			lpStrfy = JSONStringify(hJSCopy);
			if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); }
			else { wprintf(L"5. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
			JSONFree(hJSCopy); hJSCopy = NULL;
		}
	}
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONAppend()
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL, hJSNew = NULL; LPWSTR lpszJSON = NULL, lpStrfy = NULL; LPCWSTR lpEnd = NULL; DWORD dwLastError = 0, cchStr = 0;

	lpszJSON = &wsJSONStr[0]; cchStr = StrLen(lpszJSON); //wprintf(L"1. StrLen %d \n", cchStr);
	hJSON = JSONParse(lpszJSON, cchStr, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	hJSNew = JSONCopyTreeEx(hJSON);
	if (!hJSNew) { wprintf(L"1. JSONCopyTree ERROR %d \n", GetLastError()); }
	else {
		dwLastError = JSONAppend(hJSNew, hJSON);
		if (dwLastError) { wprintf(L"0. JSONAppend ERROR %d \n", dwLastError); }
		else {
			wprintf(L"1. isHeader %d \n", JSONPtr(hJSNew)->IsHeader);
			wprintf(L"2. isObject %d \n", JSONPtr(hJSNew)->IsObject);
			wprintf(L"3. dwMemCount %d \n", JSONContainerLength(hJSNew));
			wprintf(L"4. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSNew));
			lpStrfy = JSONStringify(hJSNew);
			if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); }
			else { wprintf(L"5. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
		}
		JSONFree(hJSNew); hJSNew = NULL;
	}
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONMerge()
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL, hJSNew = NULL, hJSONToMrg = NULL; LPCWSTR lpEnd = NULL, lpStrfy = NULL; DWORD dwLastError = 0;

	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
 	hJSONToMrg = JSONParse(wsJSONStrEx, 0, &lpEnd);
	if (!hJSONToMrg) { wprintf(L"1. JSONParse ERROR %d %ls\n", GetLastError(), lpEnd); JSONFree(hJSON); return 0; }
	hJSNew = JSONCopyTreeEx(hJSON);
	if (!hJSNew) { wprintf(L"2. JSONCopyTree ERROR %d \n", GetLastError()); }
	else {
		dwLastError = JSONMerge(hJSNew, hJSONToMrg, 0);
		if (dwLastError) { wprintf(L"3. JSONMerge ERROR %d \n", dwLastError); }
		else {
			wprintf(L"1. isHeader %d \n", JSONPtr(hJSNew)->IsHeader);
			wprintf(L"2. isObject %d \n", JSONPtr(hJSNew)->IsObject);
			wprintf(L"3. dwMemCount %d \n", JSONContainerLength(hJSNew));
			wprintf(L"4. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSNew));
			lpStrfy = JSONStringify(hJSNew);
			if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); }
			else { wprintf(L"5. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
		}
		JSONClose(hJSNew); hJSNew = NULL;
	}
	hJSNew = JSONCopyTreeEx(hJSON);
	if (!hJSNew) { wprintf(L"4. JSONCopyTree ERROR %d \n", GetLastError()); }
	else {
		dwLastError = JSONMerge(hJSNew, hJSONToMrg, JSON_REPLACEONLY);
		if (dwLastError) { wprintf(L"5. JSONMerge ERROR %d \n", dwLastError); }
		else {
			lpStrfy = JSONStringify(hJSNew);
			if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); }
			else { wprintf(L"5. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
		}
		JSONClose(hJSNew); hJSNew = NULL;
	}
	hJSNew = JSONCopyTreeEx(hJSON);
	if (!hJSNew) { wprintf(L"6. JSONCopyTree ERROR %d \n", GetLastError()); }
	else {
		dwLastError = JSONMerge(hJSNew, hJSONToMrg, JSON_NOOVERWRITE);
		if (dwLastError) { wprintf(L"7. JSONMerge ERROR %d \n", dwLastError); }
		else {
			lpStrfy = JSONStringify(hJSNew);
			if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); }
			else { wprintf(L"5. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
		}
		JSONClose(hJSNew); hJSNew = NULL;
	}
	JSONClose(hJSONToMrg); hJSONToMrg = NULL;
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONReserve()
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL; LPCWSTR lpEnd = NULL; DWORD dwLastError = 0;

	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
  	dwLastError = JSONReserve(hJSON, 111);
	if (dwLastError) { wprintf(L"1. JSONReserve ERROR %d \n", dwLastError); }
	else {
		wprintf(L"1. isHeader %d \n", JSONPtr(hJSON)->IsHeader);
		wprintf(L"2. isObject %d \n", JSONPtr(hJSON)->IsObject);
		wprintf(L"3. dwMemBuffer %d - dwMemCount %d \n", JSONContainerSize(hJSON), JSONContainerLength(hJSON));
		//wprintf(L"4. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSON));
	}
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONSort() && JSONQSort()
```c
#include <stdio.h>
#include "JSONEx.c"

INT MyJSONObjCompare(PCJSON piMem, PCJSON pjMem) {
	return StringCompareI(piMem->lpMembers[0].lpszKeyName, pjMem->lpMembers[0].lpszKeyName); //StrCmp/StringCompare - StrCmpN/StringCompareN - StrCmpI/StringCompareI - StrCmpNI/StringCompareNI
	//if (piMem->lpszKeyName) return StringCompareI(piMem->lpszKeyName, pjMem->lpszKeyName); //StrCmp/StringCompare - StrCmpN/StringCompareN - StrCmpI/StringCompareI - StrCmpNI/StringCompareNI
	//else if (piMem->IsString) return StringCompareI(piMem->lpszValue, pjMem->lpszValue); //pJSON->IsArray, compare lpszValue
	//else if (piMem->IsNumber) {
	//	if (piMem->IsFloat) return (INT)(piMem->dValue - pjMem->dValue); // use dValue or fValue to compare DOUBLE or float
	//	else if (piMem->IsSigned) return (INT)(piMem->illValue - pjMem->illValue); // use illValue to compare INT64 or INT iValue
	//	else return (INT)(piMem->ullValue - pjMem->ullValue); //use ullValue to compare UINT64 or UINT uValue
	//}
	//else return 0;
}

int main() {
	HJSON hJSON = NULL, hJSKey = NULL; LPCWSTR lpEnd = NULL, lpStrfy = NULL; DWORD dwLastError = 0;

	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
 	dwLastError = JSONOpenKeyEx(hJSON, L"context.arrayStr", 16, &hJSKey);
	if (dwLastError) { wprintf(L"1. JSONOpenKeyEx ERROR %d \n", dwLastError); }
	else {
		dwLastError = JSONSort(hJSKey, 0, 0); //dwLastError = JSONQSort(hJSKey, (PFN_QSORTCOMPARE)_JSONCompare);
		if (dwLastError) { wprintf(L"2. JSONSort ERROR %d \n", dwLastError); }
		JSONCloseKey(hJSKey);
	}
	dwLastError = JSONOpenKeyEx(hJSON, L"context.arrayNum", 16, &hJSKey);
	if (dwLastError) { wprintf(L"3. JSONOpenKeyEx ERROR %d \n", dwLastError); }
	else {
		dwLastError = JSONSort(hJSKey, 0, 0); //dwLastError = JSONQSort(hJSKey, (PFN_QSORTCOMPARE)_JSONCompare);
		if (dwLastError) { wprintf(L"4. JSONSort ERROR %d \n", dwLastError); }
		JSONCloseKey(hJSKey);
	}
	dwLastError = JSONOpenKeyEx(hJSON, L"context.arrayList", 17, &hJSKey);
	if (dwLastError) { wprintf(L"5. JSONOpenKeyEx ERROR %d \n", dwLastError); }
	else {
		dwLastError = JSONQSort(hJSKey, (PFN_QSORTCOMPARE)MyJSONObjCompare);
		if (dwLastError) { wprintf(L"6. JSONSort ERROR %d \n", dwLastError); }
		JSONCloseKey(hJSKey);
	}
	lpStrfy = JSONStringify(hJSON);
	if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); }
	else { wprintf(L"5. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONSetValue() && Create JSON Object, Array, String, Number
```c
#include <stdio.h>
#include "JSONEx.c"

int main() {
	HJSON hJSON = NULL, hJSNewKey = NULL, hJSKey = NULL, hJSKeyEx = NULL, hJSArray = NULL, hJSEnumKey = NULL;
	LPWSTR lpszJSON = NULL, lpStrfy = NULL; LPCWSTR lpEnd = NULL; DWORD dwLastError = 0, dwDisposition = 0, dwIndex = 0;

	hJSON = JSONParse(L"{}", 2, &lpEnd); // or InitializeJSONHandle(hJSON);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d\n", GetLastError()); return 0; }
	dwLastError = JSONCreateKey(hJSON, L"NewKeyTest", 10, JSON_IS_OBJECT, &hJSNewKey, &dwDisposition);
	if (dwLastError) { wprintf(L"0. JSONCreateKey ERROR %d \n", dwLastError); JSONFree(hJSON); return 0; }
	dwLastError = JSONCreateKey(hJSNewKey, L"ObjectKey", 9, JSON_IS_OBJECT, &hJSKey, &dwDisposition);
	if (dwLastError) { wprintf(L"1. JSONCreateKey ERROR %d \n", dwLastError); }
	JSONClose(hJSKey);
	WCHAR wsJSONArr[] = L"{ \"ArrayKey\": [,2319, [\"444\",[\"000\",[],\"999\"], \"555\"], 3854, false, 987, null, true, ,,] }"; //
	hJSArray = JSONParse(wsJSONArr, StrLen(wsJSONArr), &lpEnd);
	if (hJSArray) {
		dwLastError = JSONOpenKey(hJSArray, L"ArrayKey", 8, &hJSKeyEx);
		if (!dwLastError) {
			dwLastError = JSONCreateKey(hJSNewKey, L"ArrayKey", 8, JSON_IS_ARRAY, &hJSKey, &dwDisposition);
			if (dwLastError) { wprintf(L"2. JSONCreateKey ERROR %d \n", dwLastError); }
			dwLastError = JSONSetValue(hJSKey, JSON_IS_ARRAY, JSONContainer(hJSKeyEx), JSONContainerLength(hJSKeyEx));
			if (dwLastError) { wprintf(L"2. JSONSetValue ERROR %d \n", dwLastError); }
			JSONClose(hJSKeyEx); JSONClose(hJSKey);
		}
		else { wprintf(L"0. JSONOpenKey ERROR %d \n", dwLastError); }
		JSONFree(hJSArray);
	}
	else { wprintf(L"0. JSONParse ERROR %d - %ls\n", GetLastError(), lpEnd); }
	dwLastError = JSONCreateKey(hJSNewKey, L"StringKey", 9, JSON_IS_STRING, &hJSKey, &dwDisposition);
	if (dwLastError) { wprintf(L"3. JSONCreateKey ERROR %d \n", dwLastError); }
	dwLastError = JSONSetValue(hJSKey, JSON_IS_STRING, L"1673458487", 10);
	if (dwLastError) { wprintf(L"3. JSONSetValue ERROR %d \n", dwLastError); }
	JSONClose(hJSKey);
	dwLastError = JSONCreateKey(hJSNewKey, L"BoolKey", 7, JSON_IS_BOOLEAN, &hJSKey, &dwDisposition);
	if (dwLastError) { wprintf(L"4. JSONCreateKey ERROR %d \n", dwLastError); }
	dwLastError = JSONSetValue(hJSKey, JSON_IS_BOOLEAN, L"false", 5);
	if (dwLastError) { wprintf(L"4. JSONSetValue ERROR %d \n", dwLastError); }
	JSONClose(hJSKey);
	dwLastError = JSONCreateKey(hJSNewKey, L"BoolKey1", 8, (JSON_IS_BOOLEAN | JSON_IS_BOOLTRUE), &hJSKey, &dwDisposition);
	if (dwLastError) { wprintf(L"5. JSONCreateKey ERROR %d \n", dwLastError); }
	dwLastError = JSONSetValue(hJSKey, (JSON_IS_BOOLEAN | JSON_IS_BOOLTRUE), L"true", 4);
	if (dwLastError) { wprintf(L"5. JSONSetValue ERROR %d \n", dwLastError); }
	JSONClose(hJSKey);
	dwLastError = JSONCreateKey(hJSNewKey, L"NullKey", 7, JSON_IS_NULL, &hJSKey, &dwDisposition);
	if (dwLastError) { wprintf(L"6. JSONCreateKey ERROR %d \n", dwLastError); }
	dwLastError = JSONSetValue(hJSKey, JSON_IS_NULL, L"null", 4);
	if (dwLastError) { wprintf(L"6. JSONSetValue ERROR %d \n", dwLastError); }
	JSONClose(hJSKey);
	dwLastError = JSONCreateKey(hJSNewKey, L"NumberKey", 9, JSON_IS_NUMBER, &hJSKey, &dwDisposition);
	if (dwLastError) { wprintf(L"7. JSONCreateKey ERROR %d \n", dwLastError); }
	dwLastError = JSONSetValue(hJSKey, JSON_IS_NUMBER, L"12345", 5);
	if (dwLastError) { wprintf(L"7. JSONSetValue ERROR %d \n", dwLastError); }
	JSONClose(hJSKey);
	dwLastError = JSONCreateKey(hJSNewKey, L"NumSigKey", 9, (JSON_IS_NUMBER | JSON_IS_SIGNED), &hJSKey, &dwDisposition);
	if (dwLastError) { wprintf(L"8. JSONCreateKey ERROR %d \n", dwLastError); }
	dwLastError = JSONSetValue(hJSKey, (JSON_IS_NUMBER | JSON_IS_SIGNED), L"-6789", 5);
	if (dwLastError) { wprintf(L"8. JSONSetValue ERROR %d \n", dwLastError); }
	JSONClose(hJSKey);
	dwLastError = JSONCreateKey(hJSNewKey, L"NumFloatKey", 11, (JSON_IS_NUMBER | JSON_IS_FLOAT), &hJSKey, &dwDisposition);
	if (dwLastError) { wprintf(L"9. JSONCreateKey ERROR %d \n", dwLastError); }
	dwLastError = JSONSetValue(hJSKey, (JSON_IS_NUMBER | JSON_IS_FLOAT), L"123.4578", 8);
	if (dwLastError) { wprintf(L"9. JSONSetValue ERROR %d \n", dwLastError); }
	JSONClose(hJSKey);
	dwLastError = JSONOpenKey(hJSNewKey, L"ObjectKey", 9, &hJSKeyEx); 
	if (dwLastError) { wprintf(L"0. JSONOpenKey ERROR %d \n", dwLastError); JSONFree(hJSON); return 0; }
	dwIndex = 1; //dwIndex 0 is "ObjectKey",
	while (JSONEnumKey(hJSNewKey, dwIndex, &hJSEnumKey) == NO_ERROR) {
		dwLastError = JSONCreateKey(hJSKeyEx, JSONPtr(hJSEnumKey)->lpszKeyName, JSONPtr(hJSEnumKey)->cchKeyName, JSONPtr(hJSEnumKey)->wValueType, &hJSKey, &dwDisposition);
		if (dwLastError) { wprintf(L"10. JSONCreateKey ERROR %d \n", dwLastError); }
		if (JSONPtr(hJSEnumKey)->IsContainer) { dwLastError = JSONSetValue(hJSKey, JSONPtr(hJSEnumKey)->wValueType, JSONPtr(hJSEnumKey)->lpMembers, JSONPtr(hJSEnumKey)->dwMemCount); }
		else { dwLastError = JSONSetValue(hJSKey, JSONPtr(hJSEnumKey)->wValueType, JSONPtr(hJSEnumKey)->lpszValue, JSONPtr(hJSEnumKey)->cchValue); }
		//// or
		//if (JSONPtr(hJSEnumKey)->IsContainer) { dwLastError = JSONSetValue(hJSKey, JSONPtr(hJSEnumKey)->wValueType, JSONContainer(hJSEnumKey), JSONContainerLength(hJSEnumKey)); }
		//else { dwLastError = JSONSetValue(hJSKey, JSONPtr(hJSEnumKey)->wValueType, JSONValueData(hJSEnumKey), JSONValueLength(hJSEnumKey)); }
		if (dwLastError) { wprintf(L"10. JSONSetValue ERROR %d \n", dwLastError); }
		++dwIndex; JSONClose(hJSEnumKey); JSONClose(hJSKey);
	}
	JSONClose(hJSKeyEx); JSONClose(hJSNewKey);
	wprintf(L"5. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSON));
	lpStrfy = JSONStringify(hJSON);
	if (!lpStrfy) { wprintf(L"5. JSONStringify ERROR %d \n", GetLastError()); JSONFree(hJSON); return 0; }
	else { wprintf(L"5. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }

	JSONFree(hJSON); hJSON = NULL; //or JSONClose(hJSON); //JSONClose() && JSONCloseHandle() && JSONCloseKey() - if Handle is Header run JSONFree();
	return 0;
}

/*
// JSONStringify() should return
{
	"NewKeyTest": {
		"ObjectKey": {
			"ArrayKey": [
				null,
				2319,
				[
					"444",
					[
						"000",
						[],
						"999"
					],
					"555"
				],
				3854,
				false,
				987,
				null,
				true,
				null,
				null
			],
			"StringKey": "1673458487",
			"BoolKey": false,
			"BoolKey1": true,
			"NullKey": null,
			"NumberKey": 12345,
			"NumSigKey": -6789,
			"NumFloatKey": 123.4578
		},
		"ArrayKey": [
			null,
			2319,
			[
				"444",
				[
					"000",
					[],
					"999"
				],
				"555"
			],
			3854,
			false,
			987,
			null,
			true,
			null,
			null
		],
		"StringKey": "1673458487",
		"BoolKey": false,
		"BoolKey1": true,
		"NullKey": null,
		"NumberKey": 12345,
		"NumSigKey": -6789,
		"NumFloatKey": 123.4578
	}
}
*/
```



