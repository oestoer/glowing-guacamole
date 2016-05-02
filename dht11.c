#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DHTPIN	     7
#define MIN_INTERVAL 2000
#define MAX_TIMINGS  85

int data[5] = {0, 0, 0, 0, 0 };
uint32_t lastReadTime = -MIN_INTERVAL;

bool readDht11Data(void)
{
    uint32_t currTime = millis();
    uint8_t i, j = 0, counter = 0, lastState = HIGH;

    // prevent reads under 2s delays.
    if ((currTime - lastReadTime) < 2000) {
        printf("itt, %d, %d, %d", currTime, lastReadTime, (currTime-lastReadTime));
        return false;
    }
    lastReadTime = currTime;

    // reset 40 bits of data to zero.
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    // start request
    pinMode(DHTPIN, OUTPUT);
    digitalWrite(DHTPIN, LOW);  // pull-down bus
    delay(18);                  // for 18ms
    digitalWrite(DHTPIN, HIGH); // pull-up bus
    delayMicroseconds(40);      // for 40 (mikro) us

    // prepare pin to read data
    pinMode(DHTPIN, INPUT);

    for (i = 0; i < MAX_TIMINGS; i++) {
        counter = 0;
        while (digitalRead(DHTPIN) == lastState) {
            counter++;
            delayMicroseconds(1);
            if (counter == 255) {
                break;
            }
        }
        lastState = digitalRead(DHTPIN);
        if (counter == 255) {
            break;
        }

        // skip the first 3 transitions.
        if ((i >= 4) && (i % 2 == 0)) {
            
            // bits into storage bytes
            data[j / 8] <<= 1;
            if (counter > 16) {
                data[j / 8] |= 1;
            }
            j++;
        }
    }

    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        return true;
    }

    printf("data error\n");
    return false;
}

void printDht11Data(void)
{
    float temp, relH;

    if (readDht11Data()) {
        temp = data[2];
        relH = data[0];
    }

    printf("T  = %.1f °C\nRH = %.1f\%\n\n", temp, relH);
}

int main(void)
{
    int8_t retVal = 0;
    printf("\nDHT11 Humidity and Temperature Sensor\n\n");

    if (0 != (retVal = wiringPiSetup())) {
        printf("init error: %d", retVal);
        return 1;
    }

    while (1) {
        printDht11Data();
        delay(2000);
    }

    return 0 ;
}
