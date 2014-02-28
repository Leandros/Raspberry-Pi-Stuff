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
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>

// Private functions.
void indicator_daemonize(struct internetIndicator *i, char *rundir, char *pidfile);
void indicator_runMainLoop(struct internetIndicator *i);

struct internetIndicator {
    const char *daemonName;

    int pidFileHandle;
    int redLed;
    int greenLed;
};

//===========================================================================================================================
// Public Methods.
//===========================================================================================================================
size_t indicator_size() {
    return sizeof(struct internetIndicator);
}

int indicator_start(struct internetIndicator *i) {
    /* Debug logging
    setlogmask(LOG_UPTO(LOG_DEBUG));
    openlog(DAEMON_NAME, LOG_CONS, LOG_USER);
    */

    /* Logging */
    setlogmask(LOG_UPTO(LOG_INFO));
    openlog(i->daemonName, LOG_CONS | LOG_PERROR, LOG_USER);

    syslog(LOG_INFO, "Daemon starting up");

    if (access("/var/run/internetIndicator.pid", F_OK) != -1) {
        syslog(LOG_INFO, "Already running.");
        return 0;
    }

    /* Deamonize */
    indicator_daemonize(i, "/var/run/", "/var/run/internetIndicator.pid");

    /* WiringPi Setup */
    if (wiringPiSetup() == -1) {
        return 0;
    }

    pinMode(i->redLed, OUTPUT);
    pinMode(i->greenLed, OUTPUT);

    syslog(LOG_INFO, "Daemon running");

    // Start to run the main loop.
    indicator_runMainLoop(i);

    return 1;
}

int indicator_stop(struct internetIndicator *i) {
    if (access("/var/run/internetIndicator.pid", F_OK) != -1) {
        FILE *file = fopen("/var/run/internetIndicator.pid", "r");
        if (file == NULL) {
            return 0;
        }

        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *buffer = malloc(length);
        if (buffer) {
            fread (buffer, 1, length, file);
        }
        fclose(file);

        if (buffer) {
            printf("Buffer %s\n", buffer);
            // Kill Pid here.
        }

        remove("/var/run/internetIndicator.pid");
    } else {
        return 0;
    }

    digitalWrite(i->redLed, 0);
    digitalWrite(i->greenLed, 0);
    close(i->pidFileHandle);
    exit(EXIT_SUCCESS);
    return 1;
}


//===========================================================================================================================
// Getter.
//===========================================================================================================================
const char *indicator_daemonName(struct internetIndicator *i) {
    return i->daemonName;
}

int indicator_redLed(struct internetIndicator *i) {
    return i->redLed;
}

int indicator_greenLed(struct internetIndicator *i) {
    return i->greenLed;
}


//===========================================================================================================================
// Setter.
//===========================================================================================================================
void indicator_setDaemonName(struct internetIndicator *i, const char *name) {
    i->daemonName = name;
}

void indicator_setRedLed(struct internetIndicator *i, int led) {
    i->redLed = led;
}

void indicator_setGreenLed(struct internetIndicator *i, int led) {
    i->greenLed = led;
}


//===========================================================================================================================
// Private Methods.
//===========================================================================================================================
void indicator_daemonize(struct internetIndicator *i, char *rundir, char *pidfile) {
    int pid, sid, iterrator;
    char str[10];

    /* Check if deamon is already running */
    if (getppid() == 1) {
        return;
    }

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
    for (iterrator = getdtablesize(); iterrator >= 0; --iterrator) {
        close(iterrator);
    }

    /* Route I/O connections */

    /* Open STDIN */
    iterrator = open("/dev/null", O_RDWR);

    /* STDOUT */
    dup(iterrator);

    /* STDERR */
    dup(iterrator);

    chdir(rundir); /* change running directory */

    /* Ensure only one copy */
    i->pidFileHandle = open(pidfile, O_RDWR|O_CREAT, 0600);

    if (i->pidFileHandle == -1) {
        /* Couldn't open lock file */
        syslog(LOG_INFO, "Could not open PID lock file %s, exiting", pidfile);
        exit(EXIT_FAILURE);
    }

    /* Try to lock file */
    if (lockf(i->pidFileHandle,F_TLOCK,0) == -1) {
        /* Couldn't get lock on lock file */
        syslog(LOG_INFO, "Could not lock PID lock file %s, exiting", pidfile);
        exit(EXIT_FAILURE);
    }

    /* Get and format PID */
    sprintf(str,"%d\n",getpid());

    /* write pid to lockfile */
    write(i->pidFileHandle, str, strlen(str));
}

void indicator_runMainLoop(struct internetIndicator *i) {
    while (1) {
       FILE *output;

        if(!(output = popen("/sbin/route -n | grep -c '^0\\.0\\.0\\.0'","r"))) {
            printf("Error!");
            //fflush(1);
            while (1) {
                digitalWrite(i->redLed, 1);
                delay(1000);
                digitalWrite(i->redLed, 0);
                delay(1000);
            }
            return;
        }

        unsigned int iterrator;
        fscanf(output,"%u",&iterrator);

        if (iterrator == 0) {
            // Internet not connected.
            digitalWrite(i->greenLed, 0);
            digitalWrite(i->redLed, 1);
        }
        else if (iterrator == 1) {
            // Internet connected.
            digitalWrite(i->redLed, 0);
            digitalWrite(i->greenLed, 1);
        }

        pclose(output);
        delay(5000);
    }
}



