#include "sighandlers.h"
#include "sync.h"
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


/**
 * \brief Signal handler for SIGINT. Requests immediate program termination.
 * \param signum Ignored.
*/
static void sigintHandler(int signum)
{
	(void) signum;
	Thread_activateKillSwitch();
}


/**
 * \brief Signal handler for SIGTERM. Requests immediate program termination.
 * \param signum Ignored.
*/
static void sigtermHandler(int signum)
{
	(void) signum;
	Thread_activateKillSwitch();
}


/**
 * \brief Registers handler function for various signals.
 * \param signum Signal to associate the handler with, eg. SIGINT.
 * \param handler Handler funtion to be invoked when given signal is received.
*/
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