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

 /* ===============================================
  *                     README
  * ===============================================
  *
  * Depandancies: WiringPi
  * Compile command: g++ internetIndicator.cpp -o internetIndicator -I/usr/local/include -L/usr/local/lib -lwiringPi
  *
  */

#include <wiringPi.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sstream>
#include <signal.h>

#define DAEMON_NAME "internetIndicator"
using namespace std;

int pinOutputRed = 7;
int pinOutputGreen = 1;

void daemonShutdown();
void signal_handler(int sig);
void daemonize(char *rundir, char *pidfile);

int pidFilehandle;


void signal_handler(int sig) {
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

void daemonShutdown() {
	digitalWrite(pinOutputRed, 0);
	digitalWrite(pinOutputGreen, 0);
	close(pidFilehandle);
}

void daemonize(char *rundir, char *pidfile) {
    int pid, sid, i;
    char str[10];
    struct sigaction newSigAction;
    sigset_t newSigSet;

    /* Check if deamon is already running */
    if (getppid() == 1) {
        return;
    }

    /* Set signal mask and block them */
    sigemptyset(&newSigSet);
    sigaddset(&newSigSet, SIGCHLD);  /* ignore child - i.e. we don't need to wait for it */
    sigaddset(&newSigSet, SIGTSTP);  /* ignore Tty stop signals */
    sigaddset(&newSigSet, SIGTTOU);  /* ignore Tty background writes */
    sigaddset(&newSigSet, SIGTTIN);  /* ignore Tty background reads */
    sigprocmask(SIG_BLOCK, &newSigSet, NULL);   /* Block the above specified signals */

    /* Set up the signal handler */
    newSigAction.sa_handler = signal_handler;
    sigemptyset(&newSigAction.sa_mask);
    newSigAction.sa_flags = 0;

    /* Signals to handle */
    sigaction(SIGHUP, &newSigAction, NULL);     /* catch hangup signal */
    sigaction(SIGTERM, &newSigAction, NULL);    /* catch term signal */
    sigaction(SIGINT, &newSigAction, NULL);     /* catch interrupt signal */

    /* Fork */
    pid = fork();

    if (pid < 0) {
    	/* Error. Could not fork */
        exit(EXIT_FAILURE);
    }

    if (pid > 0){
    	/* Successfully created */
    	printf("Child process created: %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    umask(027); /* Set file permissions 750 */

    /* Get a new process group */
    sid = setsid();

    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    /* close all descriptors */
    for (i = getdtablesize(); i >= 0; --i) {
        close(i);
    }

    /* Route I/O connections */

    /* Open STDIN */
    i = open("/dev/null", O_RDWR);

    /* STDOUT */
    dup(i);

    /* STDERR */
    dup(i);

    chdir(rundir); /* change running directory */

    /* Ensure only one copy */
    pidFilehandle = open(pidfile, O_RDWR|O_CREAT, 0600);

    if (pidFilehandle == -1 ){
        /* Couldn't open lock file */
        syslog(LOG_INFO, "Could not open PID lock file %s, exiting", pidfile);
        exit(EXIT_FAILURE);
    }

    /* Try to lock file */
    if (lockf(pidFilehandle,F_TLOCK,0) == -1) {
        /* Couldn't get lock on lock file */
        syslog(LOG_INFO, "Could not lock PID lock file %s, exiting", pidfile);
        exit(EXIT_FAILURE);
    }

    /* Get and format PID */
    sprintf(str,"%d\n",getpid());

    /* write pid to lockfile */
    write(pidFilehandle, str, strlen(str));
}


int main (int argc, char** argv) {

	const char * kill[] = { "--kill", "-k" };
	const char * input[1];

	if (argv[1] == NULL) {
		input[0] = "";
	} else {
		input[0] = argv[1];
	}

	if (strcmp(input[0], kill[0]) == 0 || strcmp(input[0], kill[1]) == 0) {
		digitalWrite(pinOutputRed, 0);
        digitalWrite(pinOutputGreen, 0);
		exit(EXIT_SUCCESS);
		return 1;
	} else {

		/* Debug logging
        setlogmask(LOG_UPTO(LOG_DEBUG));
        openlog(DAEMON_NAME, LOG_CONS, LOG_USER);
        */

        /* Logging */
        setlogmask(LOG_UPTO(LOG_INFO));
        openlog(DAEMON_NAME, LOG_CONS | LOG_PERROR, LOG_USER);

        syslog(LOG_INFO, "Daemon starting up");

        /* Deamonize */
        daemonize(const_cast<char *>("/var/run/"), const_cast<char *>("/var/run/internetIndicator.pid"));

        /* WiringPi Setup */
        if (wiringPiSetup() == -1) {
	        return 1;
	    }

	    pinMode(pinOutputRed, OUTPUT);
	    pinMode(pinOutputGreen, OUTPUT);

        syslog(LOG_INFO, "Daemon running");


        while (1) {
	       FILE *output;

		    if(!(output = popen("/sbin/route -n | grep -c '^0\\.0\\.0\\.0'","r"))) {
		    	cout << "Error!";
		    	while (true) {
		    		digitalWrite(pinOutputRed, 1);
		    		delay(1000);
		    		digitalWrite(pinOutputRed, 0);
		    		delay(1000);
		    	}
		        return 1;
		    }

	    	unsigned int i;
		    fscanf(output,"%u",&i);

		    if (i == 0) {
		    	// Internet not connected.
		    	digitalWrite(pinOutputGreen, 0);
		    	digitalWrite(pinOutputRed, 1);
		    }
		    else if (i == 1) {
		    	// Internet connected.
		    	digitalWrite(pinOutputRed, 0);
		    	digitalWrite(pinOutputGreen, 1);
		    }

		    pclose(output);
		    delay(5000);
		}
	}
}

