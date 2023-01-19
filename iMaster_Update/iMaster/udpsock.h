#ifndef _UDPSOCK_H_
#define _UDPSOCK_H_

#include <winsock2.h>


#define WSA_ASYNC_MASTER		(WM_USER+520)
#define WM_RECEIVE_UDP_MASTER	(WM_USER+521)
#define WSA_ASYNC_SLAVE			(WM_USER+522)
#define WM_RECEIVE_UDP_SLAVE	(WM_USER+523)


#define WSA_ASYNC		(WM_USER+20)
#define WM_RECEIVE_UDP	(WM_USER+21)
#define BUFSIZE 2048

#define UDP_LIVE		1
#define UDP_DEAD		0

class CUDPSock
{
public:
   typedef struct tagDataPaket
   {
      tagDataPaket()
      {
         iDataLen = BUFSIZE + 8; // 4 is long, 1 is checksum, 3 is dumy
         nCheckSum = 0;
         memset(szData, 0, BUFSIZE);
      }
      void SetCheck()
      {
         nCheckSum = 0;
         for(int i=0; i<BUFSIZE; i++)
         {
            nCheckSum ^= szData[i];
         }
      }
      BOOL GetCheck()
      {
         BYTE nTmp = 0;
         for(int i=0; i<BUFSIZE; i++)
         {
            nTmp ^= szData[i];
         }

         return (nCheckSum == nTmp);
      }
      long iDataLen;	// reserved

      BYTE szData[BUFSIZE];
      BYTE nCheckSum;
      BYTE nDumy[3];	// dumy
   } DATAPACKET;

public:
   SOCKET m_Sock;
   unsigned short m_iPort;
   SOCKADDR_IN m_Addr;

   CUDPSock();
   ~CUDPSock();

   BOOL Create(HWND hWnd,
               UINT nSocketPort = 0,
				u_int iMessage = WSA_ASYNC,
               LPCTSTR lpszSocketAddress = NULL, 
               int nSocketType = SOCK_DGRAM,
               long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE
               );
   void Close();
   int SendTo(const void* lpBuf, int nBufLen, char* lpszHostAddress = NULL, int nFlags = 0);
   int SendToWithPort(const void* lpBuf, int nBufLen, char* lpszHostAddress = NULL, int nFlags = 0 , unsigned short nPortNum = 8000);

   void OnAccept(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   void OnRead(char* szBuf);
   void OnWrite(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   void OnClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

   void ErrorMessage(HWND hwnd, char *error, int err_num = 0);
};

#endif