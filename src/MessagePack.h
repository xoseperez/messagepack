/*

Message pack

Message binary packing/unpacking library

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

#ifndef MESSAGE_PACK_H
#define MESSAGE_PACK_H

#ifdef LUA_COMPATIBILITY_MODE

enum {

    MESSAGE_PACK_FLOAT,
    MESSAGE_PACK_LONG,
    MESSAGE_PACK_NULL,
    MESSAGE_PACK_BOOLEAN,
    MESSAGE_PACK_TEXT,
    MESSAGE_PACK_SHORT,
    MESSAGE_PACK_INTEGER,

    MESSAGE_PACK_MAX,
    MESSAGE_PACK_ERROR = 0x0F

};

unsigned char MESSAGE_PACK_LEN[] = {4, 4, 0, 1, 0, 1, 2};

#else

enum {

    MESSAGE_PACK_NULL,
    MESSAGE_PACK_BOOLEAN,
    MESSAGE_PACK_SHORT,
    MESSAGE_PACK_INTEGER,
    MESSAGE_PACK_LONG,
    MESSAGE_PACK_FLOAT,
    MESSAGE_PACK_TEXT,

    MESSAGE_PACK_MAX,
    MESSAGE_PACK_ERROR = 0x0F

};

unsigned char MESSAGE_PACK_LEN[] = {0, 1, 1, 2, 4, 4, 0};

#endif


class MessagePack {

    public:

        MessagePack(unsigned char buffer_size = 100) : _buffer_size(buffer_size) {
            _buffer = (unsigned char *) malloc(_buffer_size);
            if (_buffer) _buffer[0] = 0;
        }

        ~MessagePack() {
            if (_buffer) free(_buffer);
        }

        // ---------------------------------------------------------------------

        bool addNull() {
            return _addVoid(MESSAGE_PACK_NULL, NULL, 0);
        }

        bool addBoolean(bool value) {
            unsigned char tmp = value ? 1 : 0;
            return _addVoid(MESSAGE_PACK_BOOLEAN, (void *) &tmp, 1);
        }

        bool addShort(unsigned char value) {
            return _addVoid(MESSAGE_PACK_SHORT, (void *) &value, 1);
        }

        bool addInteger(unsigned int value) {
            return _addVoid(MESSAGE_PACK_INTEGER, (void *) &value, 2);
        }

        bool addLong(unsigned long value) {
            return _addVoid(MESSAGE_PACK_LONG, (void *) &value, 4);
        }

        bool addFloat(float value) {
            return _addVoid(MESSAGE_PACK_FLOAT, (void *) &value, 4);
        }

        bool addText(const char * value) {
            return _addVoid(MESSAGE_PACK_TEXT, (void *) value, strlen(value) + 1);
        }

        // ---------------------------------------------------------------------

        bool getBoolean(unsigned char index) {
            unsigned char value = 0;
            _getVoid(index, MESSAGE_PACK_BOOLEAN, &value);
            return value == 1;
        }

        unsigned char getShort(unsigned char index) {
            unsigned char value = 0;
            _getVoid(index, MESSAGE_PACK_SHORT, &value);
            return value;
        }

        unsigned int getInteger(unsigned char index) {
            unsigned int value = 0;
            _getVoid(index, MESSAGE_PACK_INTEGER, &value);
            return value;
        }

        unsigned long getLong(unsigned char index) {
            unsigned long value = 0;
            _getVoid(index, MESSAGE_PACK_LONG, &value);
            return value;
        }

        float getFloat(unsigned char index) {
            float value = 0;
            _getVoid(index, MESSAGE_PACK_FLOAT, &value);
            return value;
        }

        unsigned char getTextLength(unsigned char index) {
            if (MESSAGE_PACK_TEXT != _type(index)) return 0;
            unsigned char start = _getPosition(index);
            unsigned char position = start;
            while (0x00 != _buffer[position]) ++position;
            return position - start;
        }

        char * getText(unsigned char index, char * text) {
            if (MESSAGE_PACK_TEXT != _type(index)) return 0;
            unsigned char position = _getPosition(index);
            unsigned char len = getTextLength(index);
            memcpy(text, &_buffer[position], len+1);
            return text;
        }

        // ---------------------------------------------------------------------

        unsigned char count() {
            return _buffer[0];
        }

        unsigned char type(unsigned char index) {
            if (!_buffer) return MESSAGE_PACK_ERROR;
            if (index >= count()) return MESSAGE_PACK_ERROR;
            return _type(index);
        }

        unsigned char length() {
            return _length;
        }

        void pack(unsigned char * buffer) {
            memcpy(buffer, _buffer, _length);
        }

        unsigned char * pack() {
            return _buffer;
        }

        bool unpack(unsigned char * buffer, unsigned char len) {
            if (len > _buffer_size) return false;
            memcpy(_buffer, buffer, len);
            _length = len;
            return true;
        }

    private:

        bool _addVoid(unsigned char type, void * data, unsigned char len) {

            if (!_buffer) return false;
            if (_buffer_size - _length < len) return false;

            memcpy(&_buffer[_length], data, len);
            _length += len;

            // Check if we have to move everything forward 1 byte
            if (0 == count() % 2) {
                unsigned char start = count() / 2 + 1;
                for (unsigned char i=_length; i > start; i--) {
                    _buffer[i] = _buffer[i-1];
                }
                _buffer[start] = 0;
                _length++;
            }

            // Save the type in the header
            _setType(count(), type);

            // Increment counter
            _buffer[0] = count() + 1;

            return true;

        }

        bool _getVoid(unsigned char index, unsigned char type, void * data) {
            if (type != _type(index)) return false;
            unsigned char position = _getPosition(index);
            memcpy(data, &_buffer[position], MESSAGE_PACK_LEN[type]);
            return true;
        }

        unsigned char _getHeaderLength() {
            return ( count() + 1 ) / 2 + 1;
        }

        unsigned char _getPosition(unsigned char index) {
            unsigned char position = _getHeaderLength();
            for (unsigned char i=0; i<index; i++) {
                unsigned char type = _type(i);
                if (MESSAGE_PACK_TEXT == type) {
                    while (0x00 != _buffer[position]) ++position;
                    ++position;
                } else {
                    position += MESSAGE_PACK_LEN[type];
                }
            }
            return position;
        }

        unsigned char _type(unsigned char index) {
            unsigned char byte = _buffer[index / 2 + 1];
            if (0 == index % 2) {
                byte >>= 4;
            }
            return byte & 0x0F;
        }

        void _setType(unsigned char index, unsigned char type) {
            unsigned char byte = _buffer[index / 2 + 1];
            type = type & 0x0F;
            if (0 == index % 2) {
                type <<= 4;
            }
            _buffer[index / 2 + 1] = byte | type;
        }

        unsigned char * _buffer;
        unsigned char _buffer_size = 0;
        unsigned char _length = 1;

};

#endif // MESSAGE_PACK_H
