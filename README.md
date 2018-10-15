# MessagePack

MessagePack is a library for AVR, SAMD, ESP8266 and ESP32 platforms to pack/unpack values into a compact, self-explanatory binary message.

[![version](https://img.shields.io/badge/version-1.0.0-brightgreen.svg)](CHANGELOG.md)
[![travis](https://travis-ci.org/xoseperez/messagepack.svg?branch=master)](https://travis-ci.org/xoseperez/messagepack)
[![codacy](https://img.shields.io/codacy/grade/54c6bf3a367745509677e28dfbe1e0d1/master.svg)](https://www.codacy.com/app/xoseperez/messagepack/dashboard)
[![license](https://img.shields.io/github/license/xoseperez/MessagePack.svg)](LICENSE)
<br />
[![donate](https://img.shields.io/badge/donate-PayPal-blue.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=xose%2eperez%40gmail%2ecom&lc=US&no_note=0&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHostedGuest)
[![twitter](https://img.shields.io/twitter/follow/xoseperez.svg?style=social)](https://twitter.com/intent/follow?screen_name=xoseperez)

## Features

The main features of the MessagePack library are:

* Compact yet self-explanatory message format
* Targeted to wireless communication
* Support for boolean, short, integer, long, and float numbers and c-strings.

## Usage

Check the examples folder for working examples of the library.

### Packing

```

#include "MessagePack.h"

void send(unsigned char * data, unsigned char length) {
    // ...
}

void setup() {

    MessagePack * message = new MessagePack();

    // Add some values, for instance temperature, humidity and pressure
    message->addFloat(25.4);
    message->addShort(67);
    message->addFloat(1013.45);

    // Send message
    send(message->pack(), message->length());

}

loop() {}

```

### Unpacking

```

#include "MessagePack.h"

void parse(unsigned char * data, unsigned char length) {

    MessagePack * message = new MessagePack();

    // Load the data
    message->unpack(data, length);

    // Number of elements
    unsigned char count = message->count();
    Serial.print("Number of values: ");
    Serial.println(count);

    // Walk elements (only for integer numbers, see example for full functionality)
    char buffer[32];
    for (unsigned char i=0; i<count; i++) {
        unsigned char type = message->type(i);
        if (MESSAGE_PACK_BOOLEAN == type) {
            snprintf(buffer, sizeof(buffer), "Value #%d is a BOOLEAN: %d\n", i, message->getBoolean(i) ? 1 : 0);
        } else if (MESSAGE_PACK_SHORT == type) {
            snprintf(buffer, sizeof(buffer), "Value #%d is a SHORT: %u\n", i, message->getShort(i));
        } else if (MESSAGE_PACK_INTEGER == type) {
            snprintf(buffer, sizeof(buffer), "Value #%d is an INTEGER: %u\n", i, message->getInteger(i));
        } else if (MESSAGE_PACK_LONG == type) {
            snprintf(buffer, sizeof(buffer), "Value #%d is a LONG: %lu\n", i, message->getLong(i));
        }
        Serial.print(buffer);
    }

}

setup() {}

loop() {

    if (Serial.available()) {

        // ...

        // send the message to the parser routine
        parse(data, length);

    }

}

```

## Format

This library works under the following assumptions:

* Data is in little-endian format (least significant byte, is at the lowest address)
* Integers have a length of 8, 16 or 32 bits (for SHORT, INTEGER and LONG types)
* Floats are represented in IEEE 754 single-precision format, and have a length of 32 bits
* Booleans are represented in a byte (1 = true, 0 = false)
* Strings are ended by the NULL char (0x00)

Data is binary packed with this format:

* First byte: number of values packed in the string
* N bytes with the information about the data type of each packed value. Each byte have information about 2 packed values, one for each nibble. Data types are coded in this way:
    * 0b0000: Null
    * 0b0001: Boolean
    * 0b0010: Short
    * 0b0011: Integer
    * 0b0100: Long
    * 0b0101: Float
    * 0b0110: String
* N bytes of data

```
The pack string (without spaces) for 17 (short), true (boolean), nil and 452 (integer) are:

03 21 30 11 01 C401

03 is the number of packed values, in this case 3
21 is the type of the first and second packed values: short and boolean
30 is the type of the third and fourth packed values: integer and null
11 is integer 17 (0x11) in hexadecimal 1-byte integer (a.k.a. short)
01 is boolean true
C401 is integer 452 (0x01C4) in 2-bytes little-endian

Total length is 7 bytes. Take note that the nil value is only present in the header and has not representation in the data.

```

## License

Copyright (C) 2018 by Xose Pérez <xose dot perez at gmail dot com>

The MessagePack library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The MessagePack library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MessagePack library.  If not, see <http://www.gnu.org/licenses/>.
