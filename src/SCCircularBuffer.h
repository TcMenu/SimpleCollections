/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Dave Cherry)
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

#ifndef SIMPLECOLLECTIONS_CIRCULARBUFFER_H
#define SIMPLECOLLECTIONS_CIRCULARBUFFER_H

#include <string.h>
#include <inttypes.h>

#if defined(__MBED__) || defined(ARDUINO_ARDUINO_NANO33BLE)
#include <mbed_atomic.h>
typedef volatile uint32_t* position_ptr_t;
typedef volatile uint32_t position_t;
inline bool casAtomic(position_ptr_t ptr, position_t expected, position_t newVal) {
    uint32_t exp = expected;
    return core_util_atomic_cas_u32(ptr, &exp, newVal);
}
inline uint16_t readAtomic(position_ptr_t ptr) { return *(ptr); }
#elif defined(ESP8266)
#include <Arduino.h>
typedef volatile uint32_t* position_ptr_t;
typedef volatile uint32_t position_t;
#define NEEDS_CAS_EMULATION
#elif defined(ESP32)
#include <Arduino.h>
typedef volatile uint32_t* position_ptr_t;
typedef volatile uint32_t position_t;
inline bool casAtomic(position_ptr_t ptr, position_t expected, position_t newVal) {
    uint32_t exp32 = expected;
    uint32_t new32 = newVal;
    uxPortCompareSet(ptr, exp32, &new32);
    return new32 == expected;
}
inline uint16_t readAtomic(position_ptr_t ptr) { return *(ptr); }
#else
#include <Arduino.h>
typedef volatile uint16_t* position_ptr_t;
typedef volatile uint16_t position_t;
#define NEEDS_CAS_EMULATION
#endif

// In this case we are only partially thread safe, we block against interrupts firing but that is about all we can do
// For AVR and smaller ARM devices that don't support pre-emptive threads this may be enough.
#ifdef NEEDS_CAS_EMULATION
inline bool casAtomic(position_ptr_t ptr, position_t expected, position_t newVal) {
        auto ret = false;
        noInterrupts();
        if(*ptr == expected) {
            *ptr = newVal;
            ret = true;
        }
        interrupts();
        return ret;
}
inline uint16_t readAtomic(position_ptr_t ptr) { return *ptr; }
#endif

namespace tccollection {

/**
 * Create a circular buffer that has a fixed size and can be filled and read at the same time one character at a time.
 * It is very thread safe on ESP32 and mbed based boards, it is thread safe on other boards, unless you are using that
 * board with an unexpected RTOS, in which case you would need to determine suitability yourself.
 *
 * Imagine the buffer like a circle with a given number of segments, both the reader and writer pointers start at 0.
 * As the data structure fills up the writer will move around, and the reader can try and "keep up". Once the reader
 * or writer gets to the end it will go back to 0 (start). This is by design and therefore, IT'S POSSIBLE TO LOSE DATA.
 */
    class SCCircularBuffer {
    private:
        position_t readerPosition;
        position_t writerPosition;
        const uint16_t bufferSize;
        uint8_t *const buffer;

    public:
        explicit SCCircularBuffer(uint16_t size);

        ~SCCircularBuffer();

        bool available() const { return readerPosition != writerPosition; }

        void put(uint8_t by) { buffer[nextPosition(&writerPosition)] = by; }

        uint8_t get() { return buffer[nextPosition(&readerPosition)]; }

    private:
        uint16_t nextPosition(position_ptr_t by);
    };

}



#endif //SIMPLECOLLECTIONS_CIRCULARBUFFER_H
