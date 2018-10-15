/*

MessagePack - Unpacking example

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

char * snfloat(char * buffer, uint8_t len, uint8_t decimals, float value) {

    bool negative = value < 0;

    uint32_t mul = 1;
    for (uint8_t i=0; i<decimals; i++) mul *= 10;

    value = abs(value);
    uint32_t value_int = int(value);
    uint32_t value_dec = int((value - value_int) * mul);

    char format[20];
    snprintf(format, sizeof(format), "%s%%lu.%%0%ulu", negative ? "-" : "", decimals);
    snprintf(buffer, len, format, value_int, value_dec);

    return buffer;

}

void setup() {

    // Init serial communications
    Serial.begin(115200);
    while (!Serial);
    Serial.println();
    Serial.println("MessagePack Library - Unpacking example");
    Serial.println();

    // Create a message pack object, defaults to 100 bytes of memory
    MessagePack * message = new MessagePack();

    // Load the binary message
    //unsigned char data[] = { 0x03, 0x52, 0x50, 0x33, 0x33, 0xCB, 0x41, 0x43, 0xCD, 0x5C, 0x7D, 0x44 }; // 25.4,67,1013.45
    unsigned char data[] = { 0x03, 0x26, 0x40, 0x2E, 'H', 'E', 'L', 'L', 'O', 0x00, 0xFF, 0xA0, 0x45, 0x00 }; // 46,HELLO,4563199
    message->unpack(data, sizeof(data));

    // Number of elements
    unsigned char count = message->count();
    Serial.print("Number of values: ");
    Serial.println(count);

    // Walk elements
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
        } else if (MESSAGE_PACK_FLOAT == type) {
            char number[10];
            snfloat(number, sizeof(number), 2, message->getFloat(i));
            snprintf(buffer, sizeof(buffer), "Value #%d is a FLOAT: %s\n", i, number);
        } else if (MESSAGE_PACK_TEXT == type) {
            unsigned char size = message->getTextLength(i);
            char text[size+1];
            snprintf(buffer, sizeof(buffer), "Value #%d is a TEXT: %s\n", i, message->getText(i, text));
        } else {
            snprintf(buffer, sizeof(buffer), "Value #%d is UNKNOWN\n", i);
        }
        Serial.print(buffer);
    }


}

void loop() {}
