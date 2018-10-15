/*

Message pack

Unit test suite
This test suite uses Aunit unit testing framework (https://github.com/bxparks/AUnit)

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
#include "AUnit.h"
using namespace aunit;

//#define DEBUG

// -----------------------------------------------------------------------------
// Test class
// -----------------------------------------------------------------------------

class CustomTest: public TestOnce {

    protected:

        virtual void setup() override {
            message = new MessagePack();
        }

        virtual void teardown() override {
            delete message;
        }

        virtual void compare(size_t len, unsigned char * expected) {

            size_t buffer_len = message->length();
            unsigned char buffer[buffer_len];
            message->pack(buffer);

            #ifdef DEBUG
                char b[4];
                for (uint8_t i=0; i<buffer_len; i++) {
                    snprintf(b, sizeof(b), "%02X ", buffer[i]);
                    Serial.print(b);
                }
                Serial.println();
            #endif

            assertEqual(len, buffer_len);

            for (uint8_t i=0; i<len; i++) {
                assertEqual(buffer[i], expected[i]);
            }

        }

        MessagePack * message;

};

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

testF(CustomTest, test_empty) {
    assertEqual(message->count(), 0);
    assertEqual(message->length(), 1);
}

// -----------------------------------------------------------------------------

testF(CustomTest, test_pack_null) {
    message->addNull();
    assertEqual(message->count(), 1);
    uint8_t expected[] = {1, MESSAGE_PACK_NULL << 4};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_NULL);
}

testF(CustomTest, test_pack_boolean) {
    message->addBoolean(true);
    assertEqual(message->count(), 1);
    uint8_t expected[] = {1, MESSAGE_PACK_BOOLEAN << 4, 0x01};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_BOOLEAN);
    assertEqual(message->getBoolean(0), (bool) true);
}

testF(CustomTest, test_pack_short) {
    message->addShort(0x3F);
    assertEqual(message->count(), 1);
    uint8_t expected[] = {1, MESSAGE_PACK_SHORT << 4, 0x3F};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_SHORT);
    assertEqual(message->getShort(0), (unsigned char) 0x03F);
}

testF(CustomTest, test_pack_integer) {
    message->addInteger(0x45A0);
    assertEqual(message->count(), 1);
    uint8_t expected[] = {1, MESSAGE_PACK_INTEGER << 4, 0xA0, 0x45};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_INTEGER);
    assertEqual(message->getInteger(0), (unsigned int) 0x45A0);
}

testF(CustomTest, test_pack_long) {
    message->addLong(0x45A0FF);
    assertEqual(message->count(), 1);
    uint8_t expected[] = {1, MESSAGE_PACK_LONG << 4, 0xFF, 0xA0, 0x45, 0x00};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_LONG);
    assertEqual(message->getLong(0), (unsigned long) 0x45A0FF);
}

testF(CustomTest, test_pack_float) {
    message->addFloat(3.1415926);
    assertEqual(message->count(), 1);
    uint8_t expected[] = {1, MESSAGE_PACK_FLOAT << 4, 0xDA, 0x0F, 0x49, 0x40};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_FLOAT);
    assertEqual(message->getFloat(0), (float) 3.1415926);
}

testF(CustomTest, test_pack_text) {
    message->addText("HOLA");
    assertEqual(message->count(), 1);
    uint8_t expected[] = {1, MESSAGE_PACK_TEXT << 4, 'H', 'O', 'L', 'A', 0x00};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_TEXT);
    assertEqual(message->getTextLength(0), 4);
    char buffer[5];
    assertEqual(message->getText(0, buffer), "HOLA");
}

// -----------------------------------------------------------------------------

testF(CustomTest, test_pack_short_long) {
    message->addShort(0x2E);
    message->addLong(0x45A0FF);
    assertEqual(message->count(), 2);
    uint8_t expected[] = {2, MESSAGE_PACK_SHORT << 4 | MESSAGE_PACK_LONG, 0x2E, 0xFF, 0xA0, 0x45, 0x00};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_SHORT);
    assertEqual(message->type(1), MESSAGE_PACK_LONG);
}

testF(CustomTest, test_pack_long_boolean_long) {
    message->addLong(1);
    message->addBoolean(true);
    message->addLong(4);
    assertEqual(message->count(), 3);
    uint8_t expected[] = {3, MESSAGE_PACK_LONG << 4 | MESSAGE_PACK_BOOLEAN, MESSAGE_PACK_LONG << 4, 0x01, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_LONG);
    assertEqual(message->type(1), MESSAGE_PACK_BOOLEAN);
    assertEqual(message->type(2), MESSAGE_PACK_LONG);
}

testF(CustomTest, test_pack_short_integer_long) {
    message->addShort(0x2E);
    message->addInteger(0x34AB);
    message->addLong(0x45A0FF);
    assertEqual(message->count(), 3);
    uint8_t expected[] = {3, MESSAGE_PACK_SHORT << 4 | MESSAGE_PACK_INTEGER, MESSAGE_PACK_LONG << 4, 0x2E, 0xAB, 0x34, 0xFF, 0xA0, 0x45, 0x00};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_SHORT);
    assertEqual(message->type(1), MESSAGE_PACK_INTEGER);
    assertEqual(message->type(2), MESSAGE_PACK_LONG);
}

testF(CustomTest, test_pack_short_text_long) {
    message->addShort(0x2E);
    message->addText("HOLA");
    message->addLong(0x45A0FF);
    assertEqual(message->count(), 3);
    uint8_t expected[] = {3, MESSAGE_PACK_SHORT << 4 | MESSAGE_PACK_TEXT, MESSAGE_PACK_LONG << 4, 0x2E, 'H', 'O', 'L', 'A', 0x00, 0xFF, 0xA0, 0x45, 0x00};
    compare(sizeof(expected), expected);
    assertEqual(message->type(0), MESSAGE_PACK_SHORT);
    assertEqual(message->type(1), MESSAGE_PACK_TEXT);
    assertEqual(message->type(2), MESSAGE_PACK_LONG);
}

// -----------------------------------------------------------------------------

testF(CustomTest, test_unpack_text) {
    unsigned char data[] = {1, MESSAGE_PACK_TEXT << 4, 'H', 'O', 'L', 'A', 0x00};
    assertTrue(message->unpack(data, sizeof(data)));
    assertEqual(message->count(), 1);
    assertEqual(message->type(0), MESSAGE_PACK_TEXT);
    assertEqual(message->getTextLength(0), 4);
    char buffer[5];
    assertEqual(message->getText(0, buffer), "HOLA");
}

testF(CustomTest, test_unpack_short_float_long) {
    unsigned char data[] = {3, MESSAGE_PACK_SHORT << 4 | MESSAGE_PACK_FLOAT, MESSAGE_PACK_LONG << 4, 0x2E, 0xDA, 0x0F, 0x49, 0x40, 0xFF, 0xA0, 0x45, 0x00};
    assertTrue(message->unpack(data, sizeof(data)));
    assertEqual(message->count(), 3);
    assertEqual(message->type(0), MESSAGE_PACK_SHORT);
    assertEqual(message->type(1), MESSAGE_PACK_FLOAT);
    assertEqual(message->type(2), MESSAGE_PACK_LONG);
    assertEqual(message->getShort(0), (unsigned char) 0x2E);
    assertEqual(message->getFloat(1), (float) 3.1415926);
    assertEqual(message->getLong(2), (unsigned long) 0x45A0FF);
}

testF(CustomTest, test_unpack_long_boolean_long) {
    unsigned char data[] = {3, MESSAGE_PACK_LONG << 4 | MESSAGE_PACK_BOOLEAN, MESSAGE_PACK_LONG << 4, 0x01, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00};
    assertTrue(message->unpack(data, sizeof(data)));
    assertEqual(message->count(), 3);
    assertEqual(message->type(0), MESSAGE_PACK_LONG);
    assertEqual(message->type(1), MESSAGE_PACK_BOOLEAN);
    assertEqual(message->type(2), MESSAGE_PACK_LONG);
    assertEqual(message->getLong(0), (unsigned long) 1);
    assertEqual(message->getBoolean(1), (bool) true);
    assertEqual(message->getLong(2), (unsigned long) 4);
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

void setup() {

    Serial.begin(115200);
    while (!Serial);

    Printer::setPrinter(&Serial);
    #ifdef DEBUG
        //TestRunner::setVerbosity(Verbosity::kAll);
    #endif

}

void loop() {
    TestRunner::run();
}
