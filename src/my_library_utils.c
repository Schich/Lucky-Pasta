#include "my_library_utils.h"

PPEB GetPEB() {
#ifdef _M_X64
  return (PPEB)__readgsqword(0x60);
#else
  return (PPEB)__readfsdword(0x30);
#endif
}

void ToLowerW(WCHAR *str) {
  while (*str) {
    if (*str >= L'A' && *str <= L'Z') {
      *str += 0x20;
    }
    str++;
  }
}

void DecodeString(char *out, const unsigned char *in) {
  size_t i = 0;
  while (out[i] = DEOBF_BYTE(in[i])) {
    i++;
  }
}

BOOL CompareUnicodeString(UNICODE_STRING str1, LPCWSTR str2) {
  WCHAR temp[MAX_PATH];
  int len = str1.Length / sizeof(WCHAR);
  if (len >= MAX_PATH)
    return FALSE;
  wcsncpy_s(temp, MAX_PATH, str1.Buffer, len);
  temp[len] = L'\0';
  ToLowerW(temp);

  WCHAR target[MAX_PATH];
  wcsncpy_s(target, MAX_PATH, str2, MAX_PATH - 1);
  target[MAX_PATH - 1] = L'\0';
  ToLowerW(target);

  return wcscmp(temp, target) == 0;
}

HMODULE PebLoadLibraryA(LPCSTR moduleName) {
  WCHAR wideName[MAX_PATH];
  MultiByteToWideChar(CP_ACP, 0, moduleName, -1, wideName, MAX_PATH);

  PPEB peb = GetPEB();
  PLIST_ENTRY head = &peb->Ldr->InLoadOrderModuleList;
  PLIST_ENTRY curr = head->Flink;

  while (curr != head) {
    PLDR_DATA_TABLE_ENTRY entry = (PLDR_DATA_TABLE_ENTRY)curr;
    if (CompareUnicodeString(entry->BaseDllName, wideName)) {
      return (HMODULE)entry->DllBase;
    }
    curr = curr->Flink;
  }

  return NULL;
}


// Function to resolve the kernel32 dll 
FARPROC ResolveKernel32Obf(const unsigned char *enc, HMODULE *dllModule, const unsigned char *dllNameEnc) {

  if (!*dllModule) {
    char dllName[64];
    DecodeString(dllName, dllNameEnc);

    *dllModule = PebLoadLibraryA(dllName);
    if (!*dllModule)
      return NULL;
  }

  char name[64];
  DecodeString(name, enc);

  return GetProcAddress(*dllModule, name);
}

// Function to resolve dlls (needs kernel32 resolved)
FARPROC ResolveObf(const unsigned char *enc, HMODULE *dllModule, const unsigned char *dllNameEnc) {

  if (!*dllModule) {
    char dllName[64];
    DecodeString(dllName, dllNameEnc);

    *dllModule = MyGetModuleHandleA(dllName);
    if (!*dllModule)
      *dllModule = MyLoadLibraryA(dllName);

    if (!*dllModule)
      return NULL;
  }

  char name[64];
  DecodeString(name, enc);

  return GetProcAddress(*dllModule, name);
}
