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
  * Compile command: g++ toggleLED.cpp -o toggleLED -I/usr/local/include -L/usr/local/lib -lwiringPi
  *
  */

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>

using namespace std;

int pinOutput = 7;
int pinInput = 0;
int toggleLED = 0;
int toggleLED2 = 0;


void sighandler(int sig)
{
        cout << "Exiting..." << endl;
        digitalWrite(pinOutput, 0);
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

        pinMode(pinOutput, OUTPUT);
        pinMode(pinInput, INPUT);

        while (true)
        {
                if (digitalRead(pinInput) == 1)
                {
                        if (toggleLED == 0)
                        {
                                digitalWrite(pinOutput, 1);
                                cout << "Turn LED On" << endl;
                                delay(1000);
                                toggleLED = 1;
                        }
                        else
                        {
                                digitalWrite(pinOutput, 0);
                                cout << "Turn LED Off" << endl;
                                delay(1000);
                                toggleLED = 0;
                        }
                }
        }
}
