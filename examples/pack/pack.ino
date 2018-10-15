/*

MessagePack - Packing example

Copyright (C) 2018 by Xose Pérez <xose.perez@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "MessagePack.h"

void setup() {

    // Init serial communications
    Serial.begin(115200);
    while (!Serial);
    Serial.println();
    Serial.println("MessagePack Library - Packing example");
    Serial.println();

    // Create a message pack object, defaults to 100 bytes of memory
    MessagePack * message = new MessagePack();

    // Add some values, for instance temperature, humidity and pressure
    message->addFloat(25.4);
    message->addShort(67);
    message->addFloat(1013.45);

    // Now dump message to serial
    char buffer[6];
    Serial.print("Packed message: { ");
    for (unsigned char i=0; i<message->length(); i++) {
        if (i>0) Serial.print(", ");
        snprintf(buffer, sizeof(buffer), "0x%02X", message->pack()[i]);
        Serial.print(buffer);
    }
    Serial.println(" }");

}

void loop() {}
