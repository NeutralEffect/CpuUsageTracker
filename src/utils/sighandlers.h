/**
 * \file sighandlers.h
 * Signal handling and handler management.
*/
#ifndef SIGHANDLERS_H_INCLUDED
#define SIGHANDLERS_H_INCLUDED


/**
 * \brief Registers handler for SIGINT signal.
*/
void RegisterSigintHandler(void);


/**
 * \brief Registers handler for SIGTERM signal.
*/
void RegisterSigtermHandler(void);


#endif // !SIGHANDLERS_H_INCLUDED
