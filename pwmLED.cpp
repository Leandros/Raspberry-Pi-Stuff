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
  * Depandancies: WiringPi, WiringPi SoftPWM
  * Compile command: g++ pwmLED.cpp -o pwmLED -I/usr/local/include -L/usr/local/lib -lwiringPi -lpthread
  *
  */

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>

using namespace std;

int PIN_OUTPUT = 7;
int DELAY_MS = 10;

void sighandler(int sig)
{
        cout << "Exiting..." << endl;
        pinMode(PIN_OUTPUT, OUTPUT);
        digitalWrite(PIN_OUTPUT, 0);
        exit(0);
}

int main ()
{
        signal(SIGABRT, &sighandler);
        signal(SIGTERM, &sighandler);
        signal(SIGINT, &sighandler);

        if (wiringPiSetup() == -1)
        {
                return 1;
        }
        
        if (softPwmCreate(PIN_OUTPUT, 0, 100) != 0)
        {
                cout << "Error";
                return 1;
        }
        
        while(1)
        {
                for(int up = 0; up <= 100; up++)
                {
                        softPwmWrite(PIN_OUTPUT, up);
                        delay(DELAY_MS);
                }
                for(int down = 100; down >= 0; down--)
                {
                        softPwmWrite(PIN_OUTPUT, down);
                        delay(DELAY_MS);
                }
        }
        
}