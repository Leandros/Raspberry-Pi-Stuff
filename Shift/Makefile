#
# Copyright 2013 Arvid Gerstmann
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
# in compliance with the License. You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under the License
# is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
# or implied. See the License for the specific language governing permissions and limitations under
# the License.
#

DEBUG		= -O3
CC			= gcc
INCLUDE		= -I/usr/local/include
CFLAGS		= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS		= -L/usr/local/lib
LDLIBS		= -lwiringPi

SRC			= Shift.c
OBJ			= $(SRC:.c=.o)
BINS		= $(SRC:.c=) shift

.PHONE:		clean all

Shift.o:		Shift.c
	$(CC) $(CFLAGS) -c Shift.c

all:		Shift.o
	$(CC) $(CFLAGS) Shift.o $(LDFLAGS) $(LDLIBS) -o shift

clean:
	rm -f $(OBJ) *~ core tags $(BINS)