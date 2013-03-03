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

// Left to Right.
int PIN_OUTPUT[10] = { 7, 0, 2, 3, 12, 13, 14, 11, 10, 6 };

int DELAY_MS_SHORT = 5;
int DELAY_MS = 10;
int DELAY_MS_LONG = 20;


void sighandler(int sig)
{
        cout << "Exiting..." << endl;
        
        for(int downPin = 0; downPin < 10; downPin++)
        {
                softPwmWrite(PIN_OUTPUT[downPin], 0);
                delay(DELAY_MS);
        }
        
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
        
        // Prepare LEDs.
        for(int i = 0; i < 10; i++)
        {
                softPwmCreate(PIN_OUTPUT[i], 0, 100);
        }
        
        // Turn LEDs one by one on.
        for(int upPin = 0; upPin < 10; upPin++)
        {
                for(int upCount = 0; upCount <= 100; upCount++)
                {       
                      softPwmWrite(PIN_OUTPUT[upPin], upCount);
                      delay(DELAY_MS_SHORT);
                }
                
                delay(DELAY_MS);
        }

        while(1)
        {
                for(int downPin = 0; downPin < 10; downPin++)
                {
                        for(int downCount = 100; downCount > 0; downCount -= 5)
                        {
                                softPwmWrite(PIN_OUTPUT[downPin], downCount);
                                delay(DELAY_MS_SHORT);
                        }
                        for(int upCountAgain = 0; upCountAgain <= 100; upCountAgain += 5)
                        {
                                softPwmWrite(PIN_OUTPUT[downPin], upCountAgain);
                                delay(DELAY_MS_SHORT);
                        }

                        delay(DELAY_MS);
                }
        }
        
        
}