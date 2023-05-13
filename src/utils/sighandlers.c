#include "sighandlers.h"
#include "thread_utils.h"
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


static void sigintHandler(int signum)
{
	(void) signum;
	Thread_activateKillSwitch();
}


static void sigtermHandler(int signum)
{
	(void) signum;
	Thread_activateKillSwitch();
}


static void registerHandler(int signum, void (*handler)(int))
{
	struct sigaction action;
	memset(&action, 0, sizeof action);
	action.sa_handler = handler;
	// Assign new action to signal, pass NULL for old action pointer as we are not interested in it
	sigaction(signum, &action, NULL);
}


void RegisterSigintHandler()
{
	registerHandler(SIGINT, sigintHandler);
}


void RegisterSigtermHandler()
{
	registerHandler(SIGTERM, sigtermHandler);
}