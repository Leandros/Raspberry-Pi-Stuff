#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>

using namespace std;

int pinOutputRed = 7;
int pinOutputGreen = 1;

void sighandler(int sig)
{
        cout << "Exiting..." << endl;
        digitalWrite(pinOutputRed, 0);
        digitalWrite(pinOutputGreen, 0);
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

    pinMode(pinOutputRed, OUTPUT);
    pinMode(pinOutputGreen, OUTPUT);
    
    while (true)
    {
    	FILE *output;

	    if(!(output = popen("/sbin/route -n | grep -c '^0\\.0\\.0\\.0'","r")))
	    {
	    	cout << "Error!";
	    	while (true)
	    	{
	    		digitalWrite(pinOutputRed, 1);
	    		delay(1000);
	    		digitalWrite(pinOutputRed, 0);
	    		delay(1000);
	    	}
	        return 1;
	    }

    	unsigned int i;
	    fscanf(output,"%u",&i);

	    if (i == 0)
	    {
	    	digitalWrite(pinOutputRed, 1);
	    }
	    else if (i == 1)
	    {
	    	digitalWrite(pinOutputGreen, 1);
	    }

	    pclose(output);
	    delay(5000);
	}

}
