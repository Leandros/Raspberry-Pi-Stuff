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

#include <stdlib.h>

struct internetIndicator;

size_t indicator_size();

// Public Methods.
int indicator_start(struct internetIndicator *i);
int indicator_stop(struct internetIndicator *i);

// Getter.
const char *indicator_daemonName(struct internetIndicator *i);
int indicator_redLed(struct internetIndicator *i);
int indicator_greenLed(struct internetIndicator *i);

// Setter.
void indicator_setDaemonName(struct internetIndicator *i, const char *name);
void indicator_setRedLed(struct internetIndicator *i, int led);
void indicator_setGreenLed(struct internetIndicator *i, int led);