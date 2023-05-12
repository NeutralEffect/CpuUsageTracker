#ifndef SIGHANDLERS_H_INCLUDED
#define SIGHANDLERS_H_INCLUDED


/**
 * \brief Registers handler for SIGINT signal.
*/
void RegisterSigintHandler();


/**
 * \brief Registers handler for SIGTERM signal.
*/
void RegisterSigtermHandler();


#endif // !SIGHANDLERS_H_INCLUDED