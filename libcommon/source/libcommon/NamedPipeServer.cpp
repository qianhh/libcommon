#include "StdAfx.h"
#include "NamedPipeServer.h"
#include "common.h"
#include <process.h>
#include <string>
#include "log.h"
#include "Buffer.h"
using namespace std;

#define PIPE_TIMEOUT 5000
#define BUFSIZE 8192
 
namespace eric { namespace common {
//enum PIPE_STATUS
//{
//	PIPE_READ_SIZE = 0,
//	PIPE_READ_CONTENT,
//	PIPE_WRITE_SIZE,
//	PIPE_WRITE_CONTENT,
//};

typedef struct 
{ 
   OVERLAPPED oOverlap; 
   HANDLE hPipeInst; 
   NamedPipeServer *pServer;
   char chRequest[BUFSIZE]; 
   DWORD cbRead;
   char chReply[BUFSIZE]; 
   DWORD cbToWrite; 
 //  PIPE_STATUS status;
}PIPEINST, *LPPIPEINST;

static void DisconnectAndClose(LPPIPEINST lpPipeInst); 
static BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo); 
static void WINAPI CompletedWriteRoutine(DWORD dwErr, DWORD cbWritten, LPOVERLAPPED lpOverLap); 
static void WINAPI CompletedReadRoutine(DWORD dwErr, DWORD cbBytesRead, LPOVERLAPPED lpOverLap); 

NamedPipeServer::NamedPipeServer(void) : m_hThread(NULL), m_bExitEvent(false)
	, m_hPipe(NULL)
{
	memset(m_szPipeName, 0, sizeof(m_szPipeName));
}

NamedPipeServer::~NamedPipeServer(void)
{
	CLOSE_HANDLE(m_hThread);
	CLOSE_HANDLE(m_hPipe);
}

bool NamedPipeServer::Initialize(wchar_t *pszPipeName/* = L"\\\\.\\pipe\\AppService"*/)
{
	wcscpy_s(m_szPipeName, pszPipeName);
	unsigned int uThreadId = 0;
	m_hThread = _beginthreadex(NULL, 0, NamedPipeServer::ThreadFunction, reinterpret_cast<void*>(this), 0, &uThreadId);

	return m_hThread!=NULL;
}

unsigned int WINAPI NamedPipeServer::ThreadFunction(void* pContext)
{
	NamedPipeServer *pThread = static_cast<NamedPipeServer*>(pContext);
	
	DWORD dwResult = 0;
	__try
	{
		dwResult = pThread->Execute();
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
	return dwResult;
}

unsigned int NamedPipeServer::Execute(void)
{
	HANDLE hConnectEvent; 
	OVERLAPPED oConnect; 
	LPPIPEINST lpPipeInst; 
	DWORD dwWait, cbRet; 
	BOOL fSuccess, fPendingIO; 

	// Create one event object for the connect operation. 

	hConnectEvent = CreateEvent( 
		NULL,    // default security attribute
		TRUE,    // manual reset event 
		TRUE,    // initial state = signaled 
		NULL);   // unnamed event object 

	if (hConnectEvent == NULL) 
	{
		LOG_TRACE("CreateEvent failed, error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
		return 0;
	}

	oConnect.hEvent = hConnectEvent; 

	// Call a subroutine to create one instance, and wait for 
	// the client to connect. 

	fPendingIO = CreateAndConnectInstance(&oConnect); 

	while (!m_bExitEvent) 
	{ 
		// Wait for a client to connect, or for a read or write 
		// operation to be completed, which causes a completion 
		// routine to be queued for execution. 

		dwWait = WaitForSingleObjectEx( 
			hConnectEvent,  // event object to wait for 
			500,			// waits indefinitely 
			TRUE);          // alertable wait enabled 

		switch (dwWait) 
		{ 
			// The wait conditions are satisfied by a completed connect 
			// operation. 
		case 0: 
			// If an operation is pending, get the result of the 
			// connect operation. 

			if (fPendingIO) 
			{ 
				fSuccess = GetOverlappedResult( 
					(HANDLE)m_hPipe,     // pipe handle 
					&oConnect, // OVERLAPPED structure 
					&cbRet,    // bytes transferred 
					FALSE);    // does not wait 
				if (!fSuccess) 
				{
					LOG_TRACE("ConnectNamedPipe error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
					return 0;
				}
			} 

			// Allocate storage for this instance. 

			lpPipeInst = (LPPIPEINST) GlobalAlloc( 
				GPTR, sizeof(PIPEINST)); 
			if (lpPipeInst == NULL) 
			{
				LOG_TRACE("GlobalAlloc failed error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
				return 0;
			}

			lpPipeInst->hPipeInst = (HANDLE)m_hPipe; 
			lpPipeInst->pServer = this;

			// Start the read operation for this client. 
			// Note that this same routine is later used as a 
			// completion routine after a write operation. 

			lpPipeInst->cbToWrite = 0; 
			CompletedWriteRoutine(0, 0, (LPOVERLAPPED) lpPipeInst); 

			// Create new pipe instance for the next client. 

			fPendingIO = CreateAndConnectInstance( 
				&oConnect); 
			break; 

			// The wait is satisfied by a completed read or write 
			// operation. This allows the system to execute the 
			// completion routine. 

		case WAIT_IO_COMPLETION: 
			break; 
		case WAIT_TIMEOUT:
			break;

			// An error occurred in the wait function. 

		default: 
			{
				LOG_TRACE("WaitForSingleObjectEx error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
				return 0;
			}
		} 
	} 
	return 0;
}

 
// CompletedWriteRoutine(DWORD, DWORD, LPOVERLAPPED) 
// This routine is called as a completion routine after writing to 
// the pipe, or when a new client has connected to a pipe instance.
// It starts another read operation. 
 
void WINAPI CompletedWriteRoutine(DWORD dwErr, DWORD cbWritten, 
   LPOVERLAPPED lpOverLap) 
{ 
   LPPIPEINST lpPipeInst; 
   BOOL fRead = FALSE; 
 
// lpOverlap points to storage for this instance. 
 
   lpPipeInst = (LPPIPEINST) lpOverLap; 
 
// The write operation has finished, so read the next request (if 
// there is no error). 
 
   if ((dwErr == 0) && (cbWritten == lpPipeInst->cbToWrite)) 
      fRead = ReadFileEx( 
         lpPipeInst->hPipeInst, 
         lpPipeInst->chRequest, 
         BUFSIZE*sizeof(TCHAR), 
         (LPOVERLAPPED) lpPipeInst, 
         (LPOVERLAPPED_COMPLETION_ROUTINE) CompletedReadRoutine); 
 
// Disconnect if an error occurred. 
 
   if (! fRead) 
	   DisconnectAndClose(lpPipeInst); 
} 
 
// CompletedReadRoutine(DWORD, DWORD, LPOVERLAPPED) 
// This routine is called as an I/O completion routine after reading 
// a request from the client. It gets data and writes it to the pipe. 
 
void WINAPI CompletedReadRoutine(DWORD dwErr, DWORD cbBytesRead, 
    LPOVERLAPPED lpOverLap) 
{ 
   LPPIPEINST lpPipeInst; 
   BOOL fWrite = FALSE; 
 
// lpOverlap points to storage for this instance. 
 
   lpPipeInst = (LPPIPEINST) lpOverLap; 
   lpPipeInst->cbRead = cbBytesRead;
 
// The read operation has finished, so write a response (if no 
// error occurred). 
 
   if ((dwErr == 0) && (cbBytesRead != 0)) 
   { 
	   lpPipeInst->pServer->GetAnswerToRequest(lpPipeInst); 
 
      fWrite = WriteFileEx( 
         lpPipeInst->hPipeInst, 
         lpPipeInst->chReply, 
         lpPipeInst->cbToWrite, 
         (LPOVERLAPPED) lpPipeInst, 
         (LPOVERLAPPED_COMPLETION_ROUTINE) CompletedWriteRoutine); 
   } 
 
// Disconnect if an error occurred. 
 
   if (! fWrite) 
      DisconnectAndClose(lpPipeInst); 
} 
 
// DisconnectAndClose(LPPIPEINST) 
// This routine is called when an error occurs or the client closes 
// its handle to the pipe. 
 
void DisconnectAndClose(LPPIPEINST lpPipeInst) 
{ 
// Disconnect the pipe instance. 
 
   if (! DisconnectNamedPipe(lpPipeInst->hPipeInst) ) 
   {
      LOG_TRACE("DisconnectNamedPipe failed, error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
   }
 
// Close the handle to the pipe instance. 
 
   CloseHandle(lpPipeInst->hPipeInst); 
 
// Release the storage for the pipe instance. 
 
   if (lpPipeInst != NULL) 
      GlobalFree(lpPipeInst); 
} 
 
// CreateAndConnectInstance(LPOVERLAPPED) 
// This function creates a pipe instance and connects to the client. 
// It returns TRUE if the connect operation is pending, and FALSE if 
// the connection has been completed. 
 
BOOL NamedPipeServer::CreateAndConnectInstance(void *lpoOverlap) 
{
   m_hPipe = (uintptr_t)CreateNamedPipeW( 
      m_szPipeName,             // pipe name 
      PIPE_ACCESS_DUPLEX |      // read/write access 
      FILE_FLAG_OVERLAPPED,     // overlapped mode 
      PIPE_TYPE_MESSAGE |       // message-type pipe 
      PIPE_READMODE_MESSAGE |   // message read mode 
      PIPE_WAIT,                // blocking mode 
      PIPE_UNLIMITED_INSTANCES, // unlimited instances 
      BUFSIZE*sizeof(char),    // output buffer size 
      BUFSIZE*sizeof(char),    // input buffer size 
      PIPE_TIMEOUT,             // client time-out 
      NULL);                    // default security attributes
   if ((HANDLE)m_hPipe == INVALID_HANDLE_VALUE) 
   {
      LOG_TRACE("CreateNamedPipe failed, error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
      return 0;
   }
 
// Call a subroutine to connect to the new client. 
 
   return ConnectToNewClient((HANDLE)m_hPipe, (LPOVERLAPPED)lpoOverlap); 
}

BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo) 
{ 
   BOOL fConnected, fPendingIO = FALSE; 
 
// Start an overlapped connection for this pipe instance. 
   fConnected = ConnectNamedPipe(hPipe, lpo); 
 
// Overlapped ConnectNamedPipe should return zero. 
   if (fConnected) 
   {
      LOG_TRACE("ConnectNamedPipe failed, error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
      return 0;
   }
 
   switch (GetLastError()) 
   { 
   // The overlapped connection in progress. 
      case ERROR_IO_PENDING: 
         fPendingIO = TRUE; 
         break; 
 
   // Client is already connected, so signal an event. 
 
      case ERROR_PIPE_CONNECTED: 
         if (SetEvent(lpo->hEvent)) 
            break; 
 
   // If an error occurs during the connect operation... 
      default: 
      {
         LOG_TRACE("ConnectNamedPipe failed, error[%d]:%s", GetLastError(), GetLastErrorMessage(GetLastError()).c_str());
         return 0;
      }
   } 
   return fPendingIO; 
}

void NamedPipeServer::GetAnswerToRequest(void *pipe)
{
	LPPIPEINST lpPipeInst = (LPPIPEINST)pipe;
	if (!lpPipeInst) return;
	if (lpPipeInst->cbRead > BUFSIZE) return;
	Buffer bufRecv;
	bufRecv.Append(lpPipeInst->chRequest, lpPipeInst->cbRead);
	LOG_TRACE("Recv data of %d size", lpPipeInst->cbRead);
	OnReadEvent(lpPipeInst, bufRecv);
}

bool NamedPipeServer::Send(void *pipe, const Buffer &bufSend)
{
	LPPIPEINST lpPipeInst = (LPPIPEINST)pipe;
	if (!lpPipeInst) return false;
	if (lpPipeInst->cbRead > BUFSIZE) return false;
	long lSendSize = bufSend.GetSize();
	if (lSendSize <= 0 || lSendSize > BUFSIZE) return false;
	memcpy(lpPipeInst->chReply, bufSend.GetBuffer(), lSendSize);
	lpPipeInst->cbToWrite = lSendSize;
	LOG_TRACE("Send data of %d size", lSendSize);
	return true;
}

void NamedPipeServer::Exit(void)
{
	m_bExitEvent = TRUE;
	if (WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hThread, 500))
		TerminateProcess((HANDLE)m_hThread, 1);
	LOG_TRACE("Named pipe thread exit");
}

void NamedPipeServer::OnReadEvent(void *pipe, const Buffer &bufRecv)
{
}

}}
