/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/



#include<config.h>

#include<libmutil/Thread.h>


#ifdef WIN32 
//#include"stdafx.h"
#include<windows.h>
#endif

#ifndef _MSC_VER
#include <unistd.h>
#endif

#ifdef HAVE_PTHREAD_H
#include<signal.h>
#endif

#ifdef HAVE_EXECINFO_H
#include<execinfo.h>
#endif

#include<libmutil/dbg.h>
#include<libmutil/merror.h>
#include<libmutil/Exception.h>
#include<libmutil/massert.h>

#if defined _MSC_VER || __MINGW32__
# define USE_WIN32_THREADS
#else
# ifdef HAVE_PTHREAD_H
#  define USE_POSIX_THREADS
# endif
#endif

using namespace std;

ThreadException::ThreadException(char *desc):Exception(desc){

}

void startFunction( void* (*f)() ){
	try{
		(*f)();
	}catch(Exception &e){
		cerr << "Thread: caught exception "<< flush << e.what()<<endl;
		cerr << "Thread: Stack trace:\n"+e.stackTrace()<<flush;
	}catch(Exception *e){
		cerr << "Thread: caught exception "<< flush << e->what()<<endl;
		cerr << "Thread: Stack trace:\n"+e->stackTrace()<<flush;
	}catch(exception &e){
		cerr << "Thread: caught exception:"<< flush << e.what()<<endl;
	}catch(exception *e){
		cerr << "Thread: caught exception:"<< flush << e->what()<<endl;
	}

}

void *startFunctionArg( void* (*f)(void*), void* arg){
	try{
		(*f)(arg);
	}catch(Exception &e){
		cerr << "Thread: caught exception "<< flush << e.what()<<endl;
		cerr << "Thread: Stack trace:\n"+e.stackTrace()<<flush;
	}catch(Exception *e){
		cerr << "Thread: caught exception "<< flush << e->what()<<endl;
		cerr << "Thread: Stack trace:\n"+e->stackTrace()<<flush;
	}catch(exception &e){
		cerr << "Thread: caught exception:"<< flush << e.what()<<endl;
	}catch(exception *e){
		cerr << "Thread: caught exception:"<< flush << e->what()<<endl;
	}

	return NULL;
}


void startRunnable(MRef<Runnable*> r){
	try{
		r->run();
	}catch(Exception &e){
		cerr << "Thread: caught exception "<< flush << e.what()<<endl;
		cerr << "Thread: Stack trace:\n"+e.stackTrace()<<flush;
	}catch(Exception *e){
		cerr << "Thread: caught exception "<< flush << e->what()<<endl;
		cerr << "Thread: Stack trace:\n"+e->stackTrace()<<flush;
	}catch(exception &e){
		cerr << "Thread: caught exception:"<< flush << e.what()<<endl;
	}catch(exception *e){
		cerr << "Thread: caught exception:"<< flush << e->what()<<endl;
	}
}

typedef struct tmpstruct{
    void *fun;
    void *arg;
} tmpstruct;

#ifdef WIN32
#define MINISIP_THREAD_IMPLEMENTED
static DWORD WINAPI ThreadStarter( LPVOID lpParam ) { 
	MRef<Runnable *> self = *(static_cast<MRef <Runnable *> *>(lpParam));
	delete (static_cast<MRef <Runnable *> *>(lpParam));
	startRunnable(self);
	//self->run();
	return 0; 
} 

static DWORD WINAPI StaticThreadStarter(LPVOID lpParam) {
	void* (*f)();
	f=(void* (*)())lpParam;
	startFunction(f);
	//(*f)();
	return 0;
}

static DWORD WINAPI StaticThreadStarterArg(LPVOID lpParam)
{
	//printf("StaticThreadStarter: ALIVE - thread created\n");
        tmpstruct *tmp = (tmpstruct*)lpParam;
	void* (*f)(void*);
	f=(void* (*)(void*)) tmp->fun;
	void *arg = tmp->arg;
	delete tmp;
	//(*f)(tmp->arg);
	startFunctionArg(f, arg);
	
	return 0;
}




#endif //WIN32

#ifdef USE_POSIX_THREADS
#define MINISIP_THREAD_IMPLEMENTED

#ifdef USE_POSIX_THREADS
#ifdef HAVE_EXECINFO_H
#include <ucontext.h>
#include <dlfcn.h>

#if defined(REG_RIP)
#define REGFORMAT "%016lx"
#elif defined(REG_EIP)
#define REGFORMAT "%08x"
#else
#define REGFORMAT "%x"
#endif

//Thanks to Jaco Kroon for this function
//http://tlug.up.ac.za/wiki/index.php/Obtaining_a_stack_trace_in_C_upon_SIGSEGV
static void signalHandler(int signum, siginfo_t* info, void*ptr) {
    static const char *si_codes[3] = {"", "SEGV_MAPERR", "SEGV_ACCERR"};

    int f = 0;
    ucontext_t *ucontext = (ucontext_t*)ptr;
    Dl_info dlinfo;
    void **bp = 0;
    void *ip = 0;

    fprintf(stderr, "EXCEPTION CAUGHT:\n");
    fprintf(stderr, "info.si_signo = %d\n", signum);
    fprintf(stderr, "info.si_errno = %d\n", info->si_errno);
    fprintf(stderr, "info.si_code  = %d (%s)\n", info->si_code, si_codes[info->si_code]);
    fprintf(stderr, "info.si_addr  = %p\n", info->si_addr);
    
/* For register content, enable the following two lines: 
    for(i = 0; i < NGREG; i++)
        fprintf(stderr, "reg[%02d]       = 0x" REGFORMAT "\n", i, ucontext->uc_mcontext.gregs[i]);
*/
#if defined(REG_RIP)
    ip = (void*)ucontext->uc_mcontext.gregs[REG_RIP];
    bp = (void**)ucontext->uc_mcontext.gregs[REG_RBP];
#elif defined(REG_EIP)
    ip = (void*)ucontext->uc_mcontext.gregs[REG_EIP];
    bp = (void**)ucontext->uc_mcontext.gregs[REG_EBP];
#else
    fprintf(stderr, "Unable to retrieve Instruction Pointer (not printing stack trace).\n");
#define SIGSEGV_NOSTACK
#endif

#ifndef SIGSEGV_NOSTACK
    fprintf(stderr, "Stack trace:\n");
    while(bp && ip) {
        if(!dladdr(ip, &dlinfo))
            break;

        fprintf(stderr, "% 2d: %p <%s+%u> (%s)\n",
                ++f,
                ip,
                dlinfo.dli_sname,
                (unsigned)((unsigned)ip - (unsigned)dlinfo.dli_saddr),
                dlinfo.dli_fname);

        if(dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))
            break;

        ip = bp[1];
        bp = (void**)bp[0];
    }
    fprintf(stderr, "End of stack trace\n");
#endif
}


static bool handleSignal(int sig){
	struct sigaction sa;
	sa.sa_handler = NULL;
	sa.sa_sigaction =  signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_ONESHOT | SA_SIGINFO;
	return sigaction(sig, &sa, NULL) == 0;
}

#endif
#endif



#ifdef HAVE_EXECINFO_H
/**
 *  
 *
 */
#define SIGNAL_HANDLER_DECLARED
void setupDefaultSignalHandling(){	

	if (!handleSignal(SIGSEGV)){
		cerr << "Thread: Could not install stack trace output for the SIGSEGV signal"<<endl;
	}
	if (!handleSignal(SIGBUS)){
		cerr << "Thread: Could not install stack trace output for the SIGSEGV signal"<<endl;
	}
	if (!handleSignal(SIGFPE)){
		cerr << "Thread: Could not install stack trace output for the SIGSEGV signal"<<endl;
	}
	if (!handleSignal(SIGILL)){
		cerr << "Thread: Could not install stack trace output for the SIGSEGV signal"<<endl;
	}
	if (!handleSignal(SIGSYS)){
		cerr << "Thread: Could not install stack trace output for the SIGSEGV signal"<<endl;
	}
}
#endif

static void *LinuxThreadStarter(void *arg){
#ifdef HAVE_EXECINFO_H
	setupDefaultSignalHandling();
#endif
	/* Keep a reference to yourself as long as you run */
	MRef<Runnable *> self = *(static_cast<MRef <Runnable *> *>(arg));
	delete (static_cast<MRef <Runnable *> *>(arg));
#ifdef DEBUG_OUTPUT
	mdbg << "LinuxThreadStarter: thread created"<< end;
#endif

	startRunnable(self);
	//self->run();

#ifdef DEBUG_OUTPUT
	mdbg <<"LinuxThreadStarter: thread terminated"<< end;
#endif
	return NULL;
	//pthread_exit( (void *) 0 ); //cesc
}

static void *LinuxStaticThreadStarter(void *arg){
#ifdef HAVE_EXECINFO_H
	setupDefaultSignalHandling();
#endif
	#ifdef DEBUG_OUTPUT
		mdbg << "LinuxStaticThreadStarter: thread created"<< end;
	#endif
	void* (*f)();
	f=(void* (*)())arg;
	//(*f)();
	startFunction(f);
	#ifdef DEBUG_OUTPUT
		mdbg <<"LinuxStaticThreadStarter: thread terminated"<< end;
	#endif
	return NULL;
}

static void *LinuxStaticThreadStarterArg(void *arg){
#ifdef HAVE_EXECINFO_H
	setupDefaultSignalHandling();
#endif
	#ifdef DEBUG_OUTPUT
		mdbg << "LinuxStaticThreadStarter: thread created"<< end;
	#endif
        tmpstruct *tmp = (tmpstruct*)arg;
        void* (*f)(void*);
        f=(void* (*)(void*)) tmp->fun;
        void *argptr = tmp->arg;
        delete tmp;
        startFunctionArg(f, argptr);

	#ifdef DEBUG_OUTPUT
		mdbg <<"LinuxStaticThreadStarter: thread terminated"<< end;
	#endif
	return NULL;
}



#endif

#ifndef SIGNAL_HANDLER_DECLARED
void setupDefaultSignalHandling(){
#ifdef DEBUG_OUTPUT
	cerr << "libmutil: setupDefaultSignalHandling: No stack trace signal handler available"<<endl;
#endif
}
#endif

#ifndef MINISIP_THREAD_IMPLEMENTED
#error Thread not fully implemented
#endif


Thread::Thread(MRef<Runnable *> runnable){
	massert(runnable);
        MRef<Runnable *> *self = new MRef<Runnable *>(runnable);
#ifdef WIN32
	DWORD threadId;

	handle_ptr = new HANDLE;

	*((HANDLE*)handle_ptr) = CreateThread(
			NULL,                        // default security attributes
			0,                           // use default stack size
			ThreadStarter,                  // thread function
			(LPVOID) self,                // argument to thread function
			0,                           // use default creation flags
			&threadId);
	
	if (*((HANDLE*)handle_ptr)==NULL){
		merror("Thread::Thread: CreateThread");
                delete self;
		throw ThreadException("Could not create thread.");
        }
//	printf("In Thread, windows part - thread created\n");

#endif //WIN32

#ifdef USE_POSIX_THREADS
	//handle_ptr = malloc(sizeof(pthread_t));
	handle_ptr = new pthread_t;
	int ret;
	
	//set attribs for the threads ...
/*	
	pthread_attr_t attr;
	pthread_attr_init( &attr);
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL );
	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
*/
	
	ret = pthread_create(
			(pthread_t*)handle_ptr, 
			NULL, // either NULL or &attr,
			LinuxThreadStarter, 
			self);
	
// 	pthread_attr_destroy( &attr );
	
	if ( ret != 0 ){
		merror("Thread::Thread: pthread_create");
                delete self;
		#ifdef DEBUG_OUTPUT
			merr << "In Thread, linux part - thread NOT created" << end;
		#endif
		throw ThreadException("Could not create thread.");
	}
	#ifdef DEBUG_OUTPUT
		mdbg << "In Thread, linux part - thread created" << end;
	#endif
	
#endif

}

Thread::~Thread(){
	if( handle_ptr ){
#ifdef WIN32
		delete (HANDLE *)handle_ptr;
#endif
#ifdef USE_POSIX_THREADS
		delete (pthread_t *)handle_ptr;
#endif
	}
	handle_ptr = NULL;
}

int Thread::createThread(void f()){
#ifdef WIN32
	HANDLE threadHandle;
	massert(sizeof(threadHandle)==4);
	DWORD id;
	LPVOID fptr;
	fptr = (void*)f;
	threadHandle = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size 
			StaticThreadStarter,                  // thread function 
			fptr,                // argument to thread function
			0,                           // use default creation flags
			&id);

	if (threadHandle==NULL){
		merror("Thread::Thread: CreateThread");
		throw ThreadException("Could not create thread.");
	}
	return (int)threadHandle;
#endif
	
#ifdef USE_POSIX_THREADS
	pthread_t threadHandle;
	#ifdef DEBUG_OUTPUT
 		mdbg << "Running createThread"<< end;
	#endif
	pthread_create(&threadHandle, NULL, LinuxStaticThreadStarter, (void*)f);
	return (int)threadHandle;
#endif

}

int Thread::createThread(void *f(void*), void *arg){
#ifdef WIN32
//        massert(1==0 /*UNIMPLEMENTED - ARGUMENT TO THREAD*/);

	tmpstruct *argptr = new struct tmpstruct;
        argptr->fun = (void*)f;
        argptr->arg = arg;
        
	HANDLE threadHandle;
	DWORD id;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "createThread: Creating thread" << end;
	#endif
	threadHandle = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size  
			StaticThreadStarterArg,                  // thread function 
			argptr,                // argument to thread function 
			0,                           // use default creation flags 
			&id);
	#ifdef DEBUG_OUTPUT
		mdbg << "createThread: done Creating thread" << end;
	#endif

	if (threadHandle==NULL){
		merror("Thread::createThread: CreateThread");
		throw ThreadException("Could not create thread.");
	}
	return (int)threadHandle;
#endif
	
#ifdef USE_POSIX_THREADS
	tmpstruct *argptr = new struct tmpstruct;
        argptr->fun = (void*)f;
        argptr->arg = arg;
 
	pthread_t threadHandle;
	#ifdef DEBUG_OUTPUT
		mdbg << "Running createThread" << end;
	#endif
	pthread_create(&threadHandle, NULL, LinuxStaticThreadStarterArg, argptr);
	return (int)threadHandle;
#endif

}

void * Thread::join(){
#ifdef USE_POSIX_THREADS
	void * returnValue;
	int ret;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "Thread::join(): before join" << end;
	#endif
	ret = pthread_join( 
			*( (pthread_t *)handle_ptr ), 
			&returnValue );
	
	if( ret != 0 ){
		
		#ifdef DEBUG_OUTPUT
			merror("Thread::join: pthread_join");
		#endif
		return NULL;
	} 
	
	return returnValue;
        
#elif defined WIN32
	HANDLE handle = *((HANDLE*)handle_ptr);
	if (WaitForSingleObject( handle, INFINITE )==WAIT_FAILED){
		merror("Thread::join: WaitForSingleObject");
	}
        return NULL;
#endif
}

void Thread::join(int handle){
#ifdef USE_WIN32_THREADS
	HANDLE h = (HANDLE)handle;
	if (WaitForSingleObject( h, INFINITE )==WAIT_FAILED){
		merror("Thread::join:WaitForSingleObject");
	}
#elif defined USE_POSIX_THREADS
	if( pthread_join( handle, NULL) ){
		#ifdef DEBUG_OUTPUT
			merror("Thread::join: pthread_join");
		#endif
	}
#endif
}

int Thread::msleep(int32_t ms){
#ifdef USE_WIN32_THREADS
	Sleep(ms); //function returns void
	return 0;
#elif defined USE_POSIX_THREADS
	struct timespec request;
	request.tv_sec = ms/1000;
	request.tv_nsec = (long) (ms%1000) * 1000 * 1000;
	return nanosleep( &request, NULL );
#endif
}


bool Thread::kill( ) {
#ifdef USE_POSIX_THREADS
	int ret;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "Thread::kill(): before cancel" << end;
	#endif
	ret = pthread_cancel( *( (pthread_t *)handle_ptr ) );
	
	if( ret != 0 ){
		#ifdef DEBUG_OUTPUT
			merr << "Thread::kill(): ERROR" << end;
		#endif
		return false;
	} 
	
	return true;
        
#elif defined WIN32
	HANDLE handle = *((HANDLE*)handle_ptr);
	BOOL ret;
	DWORD lpExitCode;
	
	GetExitCodeThread( handle, &lpExitCode );
	ret = TerminateThread( handle, lpExitCode );
        if( ret == 0 ) {
		
		#ifdef DEBUG_OUTPUT
			merror("Thread::kill: TerminateThread");
		#endif
		return false;
	}
	return true;
#endif
}

bool Thread::kill( int handle) {
#ifdef USE_POSIX_THREADS
	int ret;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "Thread::kill(): before cancel" << end;
	#endif
	ret = pthread_cancel( handle );
	
	if( ret != 0 ){
		#ifdef DEBUG_OUTPUT
			merr << "Thread::kill(): ERROR" << end;
		#endif
		return false;
	} 
	
	return true;
        
#elif defined WIN32
	HANDLE h = (HANDLE)handle;
	BOOL ret;
	DWORD lpExitCode;
	
	GetExitCodeThread( h, &lpExitCode );
	ret = TerminateThread( h, lpExitCode );
        if( ret == 0 ) {
		#ifdef DEBUG_OUTPUT
			merror("Thread::kill: TerminateThread");
		#endif
		return false;
	}
	return true;
#endif
}


