/*
 * Copyright 2013 Arvid Gerstmann
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */

#include "internetIndicator.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <syslog.h>
#include <stdlib.h>
#include <stdio.h>

struct internetIndicator *indicator;

void signalHandler(int sig);
void daemonShutdown();

int main(int argc, char** argv) {
	const char * kill[] = { "--kill", "-k" };
	const char * input[1];

	if (argv[1] == NULL) {
		input[0] = "";
	} else {
		input[0] = argv[1];
	}

	// Setup Signal Handler
	struct sigaction newSigAction;
    sigset_t newSigSet;

	// Set signal mask and block them
    sigemptyset(&newSigSet);
    sigaddset(&newSigSet, SIGCHLD);  /* ignore child - i.e. we don't need to wait for it */
    sigaddset(&newSigSet, SIGTSTP);  /* ignore Tty stop signals */
    sigaddset(&newSigSet, SIGTTOU);  /* ignore Tty background writes */
    sigaddset(&newSigSet, SIGTTIN);  /* ignore Tty background reads */
    sigprocmask(SIG_BLOCK, &newSigSet, NULL);   /* Block the above specified signals */

    // Set up the signal handler
    newSigAction.sa_handler = signalHandler;
    sigemptyset(&newSigAction.sa_mask);
    newSigAction.sa_flags = 0;

    // Signals to handle
    sigaction(SIGHUP, &newSigAction, NULL);     /* catch hangup signal */
    sigaction(SIGTERM, &newSigAction, NULL);    /* catch term signal */
    sigaction(SIGINT, &newSigAction, NULL);     /* catch interrupt signal */

    // Init Indicator
	indicator = malloc(indicator_size());
	indicator_setDaemonName(indicator, "internetIndicator");
	indicator_setRedLed(indicator, 0);
	indicator_setGreenLed(indicator, 7);

	if (strcmp(input[0], kill[0]) == 0 || strcmp(input[0], kill[1]) == 0) {
		// Stop the Indicator.
		indicator_stop(indicator);

	} else {
		// Start the indicator.
		indicator_start(indicator);
	}

	return 0;
}

void daemonShutdown() {
	indicator_stop(indicator);
}

void signalHandler(int sig) {
    switch(sig) {
        case SIGHUP:
            syslog(LOG_WARNING, "Received SIGHUP signal.");
            break;
        case SIGINT:
        case SIGTERM:
            syslog(LOG_INFO, "Daemon exiting");
            daemonShutdown();
            exit(EXIT_SUCCESS);
            break;
        default:
            syslog(LOG_WARNING, "Unhandled signal %s", strsignal(sig));
            break;
    }
}