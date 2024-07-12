# JSON for C/C++


JSONParse() by Default initialize all Numbers, so in `JSONRead.c` (ReadOnly mod) you don't need to use the `JSONToNumber()`, but in any case if you want you can use\create your function for to convert the string to number, the `JSONToNumber()` is already there just to give an example, or for more see the `JSONToNumberEx()` in `JSON.c` && `JSONEx.c`

Guys, you only need to add `JSONRead.h` and `JSONRead.c` to your projects source and that's it

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
```

# JSONParse() && JSONStringify()
```c
#include <stdio.h>
#include "JSONRead.c"

int main() {
	HJSON hJSON = NULL; LPCWSTR lpEnd = NULL; LPWSTR lpStrfy = NULL;

	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	wprintf(L"1. isHeader %d \n", hcJSON->IsHeader);
	wprintf(L"2. isObject %d \n", hcJSON->IsObject);
	wprintf(L"3. dwNestedLevel %d \n", hcJSON->dwNestedLevel);
	wprintf(L"4. dwMemCount %d \n", JSONContainerLength(hJSON));
	wprintf(L"5. JSONGetStringifySize %d\n", JSONGetStringifySize(hJSON));
	lpStrfy = JSONStringify(hJSON);
	if (!lpStrfy) { wprintf(L"6. JSONStringify ERROR %d \n", GetLastError()); }
	else { wprintf(L"6. lpStrfy %ls\n", lpStrfy); MemFree(lpStrfy); lpStrfy = NULL; }
	JSONFree(hJSON); hJSON = NULL;
	return 0;
}
```

# JSONOpenKey() && JSONOpenKeyEx() && JSONOpenElement()
```c
#include <stdio.h>
#include "JSONRead.c"

int main() {
	HJSON hJSON = NULL; HCJSON hcJSON = NULL, hJSKey = NULL, hJSElement = NULL; LPWSTR lpszJSON = NULL; LPCWSTR lpEnd = NULL; DWORD dwLastError = 0, cchStr = 0;

	//lpszJSON = FileReadEx(lpszJSONPath);
	lpszJSON = &wsJSONStr[0]; cchStr = StrLen(lpszJSON); //wprintf(L"1. StrLen %d \n", cchStr);
	hJSON = JSONParse(lpszJSON, cchStr, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	hcJSON = hJSON; //No Needed, but for safety ect ect
	dwLastError = JSONOpenKey(hcJSON, L"TestKey", 7, &hJSKey);
	if (dwLastError) { wprintf(L"1. JSONOpenKey ERROR %d \n", dwLastError); }
	else { //if (hJSKey->IsObject || hJSKey->IsArray) { //if (hJSKey->wValueType & JSON_IS_OBJECT_ARRAY)
		if (hJSKey->IsContainer) { wprintf(L"hJSKey - MemberCount %d  - MemberBuffer %d \n", JSONContainerLength(hJSKey), JSONContainerSize(hJSKey)); }
		else {
			wprintf(L"hJSKey - ValueLength %d  - ValueData %ls \n", JSONValueLength(hJSKey), JSONValueData(hJSKey));
			if (hJSKey->IsEscapeStr) { wprintf(L"JSONKey - Value %ls\n", JSONUnEscapeStr(JSONValueData(hJSKey), JSONValueLength(hJSKey), 0)); }
		}
		hJSKey = NULL;
	}
	dwLastError = JSONOpenKeyEx(hcJSON, L"context.arrayList[2].aaa", 24, &hJSKey);
	if (dwLastError) { wprintf(L"2. JSONOpenKeyEx ERROR %d \n", dwLastError); }
	else { //if (hJSKey->IsObject || hJSKey->IsArray) { //if (hJSKey->wValueType & JSON_IS_OBJECT_ARRAY)
		if (hJSKey->IsContainer) { wprintf(L"hJSKey - MemberCount %d  - MemberBuffer %d \n", JSONContainerLength(hJSKey), JSONContainerSize(hJSKey)); }
		else { wprintf(L"hJSKey - ValueLength %d  - ValueData %ls \n", JSONValueLength(hJSKey), JSONValueData(hJSKey)); }
		hJSKey = NULL;
	}
	dwLastError = JSONOpenKeyEx(hcJSON, L"context.LibraryIDs[3][1]", 24, &hJSKey);
	if (dwLastError) { wprintf(L"3. JSONOpenKeyEx ERROR %d \n", dwLastError); }
	else {
		dwLastError = JSONOpenElement(hJSKey, 2, &hJSElement); //Open Index 2
		if (dwLastError) { wprintf(L"4. JSONOpenKey ERROR %d \n", dwLastError); }
		else { //if (hJSElement->IsObject || hJSElement->IsArray) { //if (hJSElement->wValueType & JSON_IS_OBJECT_ARRAY)
			if (hJSElement->IsContainer) { wprintf(L"hJSElement - MemberCount %d  - MemberBuffer %d \n", JSONContainerLength(hJSElement), JSONContainerSize(hJSElement)); }
			else { wprintf(L"hJSElement - ValueLength %d  - ValueData %ls \n", JSONValueLength(hJSElement), JSONValueData(hJSElement)); }
			hJSElement = NULL;
		}
		hJSKey = NULL;
	}
	JSONFree(hJSON); hJSON = NULL; hcJSON = NULL;
	return 0;
}
```

# JSONEnumKey()
```c
#include <stdio.h>
#include "JSONRead.c"

int main() {
	HJSON hJSON = NULL; HCJSON hcJSON = NULL, hJSKey = NULL, hJSEnumKey = NULL; LPCWSTR lpEnd = NULL; DWORD dwLastError = 0, dwIndex = 0;

	hJSON = JSONParse(wsJSONStr, 0, &lpEnd);
	if (!hJSON) { wprintf(L"0. JSONParse ERROR %d \n, %ls", GetLastError(), lpEnd); return 0; }
	hcJSON = hJSON; //No Needed, but for safety ect ect
	dwLastError = JSONOpenKey(hcJSON, L"context", 7, &hJSKey);
	if (dwLastError) { wprintf(L"1. JSONOpenKey ERROR %d \n", dwLastError); }
	else {
		while (JSONEnumKey(hJSKey, dwIndex, &hJSEnumKey) == NO_ERROR) {
			if (hJSEnumKey->cchKeyName) { wprintf(L"0. cchKeyName %d - KeyName %ls\n", hJSEnumKey->cchKeyName, hJSEnumKey->lpszKeyName); }
			wprintf(L"1. isContainer %d\n", hJSEnumKey->IsContainer);
			wprintf(L"2. isStrValue %d\n", hJSEnumKey->IsStrValue);
			wprintf(L"3. wValueType 0x%04X\n", hJSEnumKey->wValueType); // - L"%02X%02X"
			if (hJSEnumKey->IsObject || hJSEnumKey->IsArray) { //if (hJSEnumKey->wValueType & JSON_IS_OBJECT_ARRAY)
				//(hJSEnumKey->wValueType & JSON_IS_OBJECT) (hJSEnumKey->wValueType & JSON_IS_ARRAY)
				if (hJSEnumKey->IsObject) wprintf(L"4. isObject %d\n", hJSEnumKey->IsObject);
				else wprintf(L"4. isArray %d\n", hJSEnumKey->IsArray);
				wprintf(L"5. Member Count %d \n", JSONContainerLength(hJSEnumKey));
			}
			else if (hJSEnumKey->IsString) { //if (hJSEnumKey->wValueType & JSON_IS_STRING)
				//(hJSEnumKey->wValueType & JSON_IS_STRING)  (hJSEnumKey->wValueType & JSON_IS_ESCAPESTR) (hJSEnumKey->wValueType & JSON_IS_SINGLEQUOTE) 
				wprintf(L"4. isString %d - isEscapeStr %d - isSingleQuote %d\n", hJSEnumKey->IsString, hJSEnumKey->IsEscapeStr, hJSEnumKey->IsSingleQuote);
				wprintf(L"5. cchValue %d - Value %ls\n", hJSEnumKey->cchValue, hJSEnumKey->lpszValue); //cchUnEscapeStr is for Internal Use Only ---> wprintf(L"JSONKey - cchUnEscapeStr %d\n" hJSEnumKey->cchUnEscapeStr);
				if (hJSEnumKey->IsEscapeStr) { wprintf(L"JSONKey - Value %ls\n", JSONUnEscapeStr(hJSEnumKey->lpszValue, hJSEnumKey->cchValue, 0)); }
			}
			else if (hJSEnumKey->IsBoolean) { //if (hJSEnumKey->wValueType & JSON_IS_BOOLEAN)
				//(hJSEnumKey->wValueType & JSON_IS_BOOLTRUE)
				wprintf(L"4. isBoolean %d - isBoolean %d\n", hJSEnumKey->IsBoolean, hJSEnumKey->IsBoolTrue);
				wprintf(L"5. cchValue %d - Value %ls\n", JSONValueLength(hJSEnumKey), JSONValueData(hJSEnumKey));
				wprintf(L"6. bValue "); wprintf(hJSEnumKey->bValue ? L"True\n" : L"False\n"); //get BOOL
			}
			else if (hJSEnumKey->IsNull) { //if (hJSEnumKey->wValueType & JSON_IS_NULL)
				wprintf(L"4. isNull %d\n", hJSEnumKey->IsNull);
				wprintf(L"5. cchValue %d - Value %ls\n", JSONValueLength(hJSEnumKey), JSONValueData(hJSEnumKey));
			}
			else if (hJSEnumKey->IsNumber) { //if (hJSEnumKey->wValueType & JSON_IS_NUMBER)
				//(hJSEnumKey->wValueType & JSON_IS_SIGNED)  (hJSEnumKey->wValueType & JSON_IS_FLOAT) (hJSEnumKey->wValueType & JSON_IS_HEXDIGIT) 
				wprintf(L"4. isNumber %d - isSigned %d - isFloat %d - isHexDigit %d\n", hJSEnumKey->IsNumber, hJSEnumKey->IsSigned, hJSEnumKey->IsFloat, hJSEnumKey->IsHexDigit);
				wprintf(L"5. cchValue %d - Value %ls\n", JSONValueLength(hJSEnumKey), JSONValueData(hJSEnumKey));
				//see struct _JSON_NUMBER_ 
				if (hJSEnumKey->IsFloat) { // or if (hJSEnumKey->wValueType & JSON_IS_FLOAT)
					wprintf(L"4.dValue %.8lf\n", hJSEnumKey->dValue); //get DOUBLE
					wprintf(L"5. fValue %f\n", hJSEnumKey->fValue); //get FLOAT
				}
				else if (hJSEnumKey->IsSigned) { // or if (hJSEnumKey->wValueType & JSON_IS_SIGNED)
					wprintf(L"4. iValue %d\n", hJSEnumKey->iValue); //get INT
					wprintf(L"5. illValue %lld\n", hJSEnumKey->illValue); //get INT64
				}
				else {
					wprintf(L"4. dwValue %u\n", hJSEnumKey->dwValue); //get DWORD
					wprintf(L"5. ullValue %llu\n", hJSEnumKey->ullValue); //get UINT64
				}
			}
			++dwIndex; hJSEnumKey = NULL;
		}
		hJSKey = NULL;
	}
	JSONFree(hJSON); hJSON = NULL; hcJSON = NULL;
	return 0;
}
```



