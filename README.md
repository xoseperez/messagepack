# MessagePack

MessagePack is a library for AVR, SAMD, ESP8266 and ESP32 platforms to pack/unpack values into a compact, self-explanatory binary message.

[![version](https://img.shields.io/badge/version-1.0.02-brightgreen.svg)](CHANGELOG.md)
[![travis](https://travis-ci.org/xoseperez/MessagePack.svg?branch=master)](https://travis-ci.org/xoseperez/MessagePack)
[![codacy](https://img.shields.io/codacy/grade/4ccbea0317c4415eb2d1c562feced407/master.svg)](https://www.codacy.com/app/xoseperez/MessagePack/dashboard)
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
