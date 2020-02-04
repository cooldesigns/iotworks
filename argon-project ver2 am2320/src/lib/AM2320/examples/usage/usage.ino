// Example usage for chrismayu_am2320 library by someone.

#include "chrismayu_am2320.h"

// Initialize objects from the lib
Chrismayu_am2320 chrismayu_am2320;

void setup() {
    // Call functions on initialized library objects that require hardware
    chrismayu_am2320.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    chrismayu_am2320.process();
}
