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

#include <stdio.h>
#include <wiringPi.h>
#include <sr595.h>

#define PIN_BASE 100
#define PINS 24
#define DATA_PIN 3
#define CLOCK_PIN 0
#define LATCH_PIN 2

int main(void)
{
	int i;

	wiringPiSetup();

	sr595Setup(PIN_BASE, PINS, DATA_PIN, CLOCK_PIN, LATCH_PIN);

	for (;;) {
		for (i = 0; i <= 8; i++) {
			digitalWrite(107 - i, 1);
			digitalWrite(108 + i, 1);
			delay(100);
			if (i != 8) {
				digitalWrite(107 - i, 0);
				digitalWrite(108 + i, 0);
			}
		}
		for (i = 0; i <= 8; i++) {
			digitalWrite(100 + i, 1);
			digitalWrite(116 - i, 1);
			delay(100);
			digitalWrite(100 + i, 0);
			digitalWrite(116 - i, 0);
		}
	}

	return 0;
}