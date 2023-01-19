#include "pch.h"
#include "UDPSock.h"
#include <ws2tcpip.h>


#define DEFAULT_PORT 6000

CUDPSock::CUDPSock()
{
}

CUDPSock::~CUDPSock()
{
}

BOOL CUDPSock::Create(HWND hWnd, UINT nSocketPort, u_int iMessage, LPCTSTR lpszSocketAddress, int nSocketType, long lEvent)
{
	WSADATA wsaData;
	
	if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		ErrorMessage(hWnd, "WSAStartup() Failure!!!", WSAGetLastError());
		return FALSE;
	}
	
	m_Sock = socket(AF_INET, nSocketType, 0);
	if(m_Sock == SOCKET_ERROR)
	{
		ErrorMessage(hWnd, "socket() Failure!!!", WSAGetLastError());
		return FALSE;
	}
	
	m_iPort = nSocketPort;
	m_Addr.sin_family = AF_INET;
	m_Addr.sin_port = htons(nSocketPort);

	if(lpszSocketAddress)
	{
//		m_Addr.sin_addr.s_addr = inet_addr(CT2A(lpszSocketAddress));
		int result = inet_pton(m_Addr.sin_family, (CT2A(lpszSocketAddress)), &(m_Addr.sin_addr.s_addr));
		if (result == -1)
			return FALSE;
	}
	else
	{
		m_Addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	{
		if(bind(m_Sock, (struct sockaddr *)&m_Addr, sizeof(m_Addr)) == SOCKET_ERROR)
		{
			ErrorMessage(hWnd, "bind() Failure!!!", WSAGetLastError());
			Close();
			return FALSE;
		}
		
		if(WSAAsyncSelect(m_Sock, hWnd, iMessage, FD_ACCEPT | FD_READ | FD_CLOSE | FD_OOB) == SOCKET_ERROR) // | FD_READ | FD_CLOSE
//		if(WSAAsyncSelect(m_Sock, hWnd, WSA_ASYNC, FD_ACCEPT | FD_READ | FD_CLOSE | FD_OOB) == SOCKET_ERROR) // | FD_READ | FD_CLOSE
//		if(WSAAsyncSelect(m_Sock, hWnd, WSA_ASYNC, FD_READ ) == SOCKET_ERROR) // | FD_READ | FD_CLOSE
		{
			ErrorMessage(hWnd, "WSAAsyncSelect(FD_ACCEPT) Failure!!!", WSAGetLastError());
			return FALSE;
		}
	}

	return TRUE;
}

int CUDPSock::SendTo(const void* lpBuf, int nBufLen, char* lpszHostAddress, int nFlags)
{
	sockaddr_in Addr;
	memset((char *)&Addr, 0x00, sizeof(Addr));
	Addr.sin_family = AF_INET;
//	Addr.sin_addr.s_addr = inet_addr((LPSTR)lpszHostAddress); 

	int result = inet_pton(Addr.sin_family, ((LPSTR)lpszHostAddress), &(Addr.sin_addr.s_addr));
	if (result == -1)
		return -1;

	Addr.sin_port = htons(m_iPort);

	char *buf = NULL;
	int iSize = lstrlen((LPCWSTR)lpBuf);

	buf = (char*)malloc(iSize);

	memcpy(buf,lpBuf,iSize);
	
	int iResult = sendto(m_Sock, buf, iSize, nFlags, (const struct sockaddr *)&Addr, sizeof(Addr));

	free(buf);
	//delete buf;

	return 0;
}

int CUDPSock::SendToWithPort(const void* lpBuf, int nBufLen, char* lpszHostAddress, int nFlags, unsigned short nPortNum)
{
	sockaddr_in Addr;
	memset((char *)&Addr, 0x00, sizeof(Addr));
	Addr.sin_family = AF_INET;
//	Addr.sin_addr.s_addr = inet_addr(lpszHostAddress); 
	
	int result = inet_pton(Addr.sin_family, lpszHostAddress, &(Addr.sin_addr.s_addr));
	if (result == -1)
		return -1;

	Addr.sin_port = htons(nPortNum);

	char *buf = NULL;
	int iSize = nBufLen+1;

	buf = (char*)malloc(iSize);

	memcpy(buf,lpBuf,iSize);
	
	int iResult = sendto(m_Sock, buf, iSize, nFlags, (const struct sockaddr *)&Addr, sizeof(Addr));

	free(buf);
	//delete buf;

	return 0;
}
void CUDPSock::Close()
{
	closesocket(m_Sock);
	WSACleanup();
}

void CUDPSock::ErrorMessage(HWND hwnd, char *error, int err_num)
{
	CString	szError;
	szError.Format(_T("%s : %d"), error, err_num);
	MessageBox(hwnd, szError, _T("Chatting Client"), MB_OK | MB_ICONERROR);
}

void CUDPSock::OnAccept(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MessageBox(hWnd, _T("Check"), _T("Accept"), 0);
}

void CUDPSock::OnRead(char* szBuf)
{
	char szTmp[BUFSIZE];
	
	int iResult = recv(m_Sock, szTmp, BUFSIZE, 0);
		
	if(iResult>0)
	{		
//		strncat(szBuf, szTmp, iResult);
		strncat_s(szBuf, sizeof(szBuf), szTmp, iResult);
	}
	else
	{
	}
	

}

void CUDPSock::OnWrite(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MessageBox(hWnd, "Check", "Write", 0);
}

void CUDPSock::OnClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MessageBox(hWnd, "Check", "Close", 0);
}
