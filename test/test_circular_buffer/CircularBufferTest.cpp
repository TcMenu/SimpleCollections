
#include <Arduino.h>
#include <unity.h>
#include <SCCircularBuffer.h>
#include <IoLogging.h>
#include <pthread.h>

void setUp() {}

void tearDown() {}

void putIntoBuffer(tccollection::SCCircularBuffer& buffer, const char* data) {
    while(*data) {
        buffer.put(*data);
        data++;
    }
}

bool verifyBuffer(tccollection::SCCircularBuffer& buffer, const char* data) {
    bool ret = true;
    while(*data) {
        if(!buffer.available()) {
            serdebugF2("Not available at ", *data);
            return false;
        }
        else {
            auto act = buffer.get();
            serdebugF3("Read expected, actual: ", *data, (char)act);
            if(act != *data) ret = false;
        }
        data++;
    }
    return ret;
}

void testWritingAndThenReadingWithoutLoss() {
    tccollection::SCCircularBuffer buffer(20);

    putIntoBuffer(buffer, "hello");
    TEST_ASSERT_TRUE(verifyBuffer(buffer, "hello"));
    TEST_ASSERT_FALSE(buffer.available());

    putIntoBuffer(buffer, "world");
    TEST_ASSERT_TRUE(verifyBuffer(buffer, "world"));
    TEST_ASSERT_FALSE(buffer.available());

    putIntoBuffer(buffer, "0123456789");
    TEST_ASSERT_TRUE(verifyBuffer(buffer, "0123456789"));
    TEST_ASSERT_FALSE(buffer.available());
}

void testWritingAndThenReadingMoreThanAvailable() {
    tccollection::SCCircularBuffer buffer(20);

    putIntoBuffer(buffer, "this is longer than 20 chars");
    // The buffer has wrapped, so we have lost everything before the wrapping point basically
    TEST_ASSERT_TRUE(verifyBuffer(buffer, "20 chars"));
    TEST_ASSERT_FALSE(buffer.available());
}

volatile bool testRunning = true;
tccollection::SCCircularBuffer glBuffer(200);
uint8_t counter = 0;

void* threadProc(void*) {
    while(testRunning && counter < 200) {
        glBuffer.put(counter);
        counter++;
        vTaskDelay(1);
    }
    return nullptr;
}

void testThreadedWriterAndReader() {
    pthread_t myThreadPtr;
    pthread_create(&myThreadPtr, nullptr, threadProc, (void*) nullptr);

    auto millisThen = millis();
    uint8_t myCount = 0;
    int itemsReceived = 0;

    while (myCount < 199U && (millis() - millisThen) < 1000) {
        if (glBuffer.available()) {
            myCount = glBuffer.get();
            serdebugF2("Thread read ", myCount);
            itemsReceived++;
        } else {
            vPortYield();
        }
    }

    TEST_ASSERT_GREATER_THAN_UINT8(198, myCount);
    TEST_ASSERT_GREATER_THAN_UINT8(190, itemsReceived);

    // Cleanup
    pthread_join(myThreadPtr, nullptr);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(testWritingAndThenReadingWithoutLoss);
    RUN_TEST(testWritingAndThenReadingMoreThanAvailable);
    RUN_TEST(testThreadedWriterAndReader);
    UNITY_END();
}

void loop() {}