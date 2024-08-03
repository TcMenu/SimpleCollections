#include <Arduino.h>
#include <unity.h>
#include <SimpleCollections.h>
#include <IoLogging.h>

void setUp() {}

void tearDown() {}

class TestStorage {
private:
    int testItem;
    int testKey;
public:
    TestStorage() {
        testItem = testKey = 0;
    }

    TestStorage(int key, int item) {
        this->testKey = key;
        this->testItem = item;
    }

    TestStorage(const TestStorage& other) {
        this->testItem = other.testItem;
        this->testKey = other.testKey;
    }

    TestStorage& operator=(const TestStorage& other) {
        if(this == &other) return *this;
        this->testItem = other.testItem;
        this->testKey = other.testKey;
        return *this;
    }

    int getKey() const {
        return testKey;
    }

    int getItem() const {
        return testItem;
    }
};

void printArray(const TestStorage* list, int size) {
    char sz[120];
    strcpy(sz, "array: ");
    for(int i=0;i<size;i++) {
        char intBuf[10];
        itoa(list[i].getKey(), intBuf, 10);
        strcat(sz, intBuf);
    }
    serdebug(sz);
}

TestStorage storage1(1, 100);
TestStorage storage2(2, 101);
TestStorage storage3(3, 102);
TestStorage storage4(4, 103);
TestStorage storage5(5, 104);
TestStorage storage6(6, 105);
TestStorage storage7(7, 106);
TestStorage storage8(8, 107);
TestStorage storage9(9, 108);
TestStorage storage10(10, 109);

void testNearestLocationEdgeCases() {
    BtreeList<int, TestStorage> btreeList(5, GROW_NEVER);
    TEST_ASSERT_EQUAL_UINT8(0, btreeList.nearestLocation(1));

    TEST_ASSERT_TRUE(btreeList.add(storage4));
    printArray(btreeList.items(), btreeList.count());

    TEST_ASSERT_EQUAL_UINT8(0, btreeList.nearestLocation(1));
    TEST_ASSERT_EQUAL_UINT8(1, btreeList.nearestLocation(5));
}

void testAddingWithoutSortOrResize() {
    BtreeList<int, TestStorage> btreeList(5, GROW_NEVER);

    TEST_ASSERT_TRUE(btreeList.add(storage1));
    TEST_ASSERT_TRUE(btreeList.add(storage2));
    TEST_ASSERT_TRUE(btreeList.add(storage3));
    TEST_ASSERT_TRUE(btreeList.add(storage4));
    TEST_ASSERT_TRUE(btreeList.add(storage5));
    TEST_ASSERT_FALSE(btreeList.add(storage6));
    printArray(btreeList.items(), btreeList.count());

    TEST_ASSERT_NOT_NULL(btreeList.getByKey(1));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(2));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(3));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(4));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(5));
    TEST_ASSERT_NULL(btreeList.getByKey(6));

    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(1)->getItem(), 100);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(2)->getItem(), 101);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(3)->getItem(), 102);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(4)->getItem(), 103);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(5)->getItem(), 104);
}

void testAddingWithSortNoResize() {
    BtreeList<int, TestStorage> btreeList(5, GROW_NEVER);

    TEST_ASSERT_TRUE(btreeList.add(storage2));
    TEST_ASSERT_TRUE(btreeList.add(storage1));
    TEST_ASSERT_TRUE(btreeList.add(storage5));
    TEST_ASSERT_TRUE(btreeList.add(storage4));
    TEST_ASSERT_TRUE(btreeList.add(storage3));
    TEST_ASSERT_FALSE(btreeList.add(storage6));
    printArray(btreeList.items(), btreeList.count());

    TEST_ASSERT_NOT_NULL(btreeList.getByKey(1));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(2));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(3));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(4));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(5));
    TEST_ASSERT_NULL(btreeList.getByKey(6));

    const TestStorage* allItems = btreeList.items();
    TEST_ASSERT_EQUAL_INT(allItems[0].getKey(), 1);
    TEST_ASSERT_EQUAL_INT(allItems[1].getKey(), 2);
    TEST_ASSERT_EQUAL_INT(allItems[2].getKey(), 3);
    TEST_ASSERT_EQUAL_INT(allItems[3].getKey(), 4);
    TEST_ASSERT_EQUAL_INT(allItems[4].getKey(), 5);

    TEST_ASSERT_EQUAL_INT(allItems[0].getKey(), btreeList.itemAtIndex(0)->getKey());
    TEST_ASSERT_EQUAL_INT(allItems[1].getKey(), btreeList.itemAtIndex(1)->getKey());

    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(1)->getItem(), 100);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(2)->getItem(), 101);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(3)->getItem(), 102);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(4)->getItem(), 103);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(5)->getItem(), 104);
}

void testAddingWithSortAndResizeBy5() {
    BtreeList<int, TestStorage> btreeList(5, GROW_BY_5);

    TEST_ASSERT_TRUE(btreeList.add(storage9));
    TEST_ASSERT_TRUE(btreeList.add(storage8));
    TEST_ASSERT_TRUE(btreeList.add(storage5));
    TEST_ASSERT_TRUE(btreeList.add(storage4));
    TEST_ASSERT_TRUE(btreeList.add(storage3));

    TEST_ASSERT_EQUAL_UINT8(5, btreeList.capacity());
    TEST_ASSERT_TRUE(btreeList.add(storage6));
    TEST_ASSERT_TRUE(btreeList.add(storage7));
    TEST_ASSERT_TRUE(btreeList.add(storage2));
    TEST_ASSERT_TRUE(btreeList.add(storage1));
    TEST_ASSERT_TRUE(btreeList.add(storage10));
    TEST_ASSERT_EQUAL_UINT8(10, btreeList.capacity());
    printArray(btreeList.items(), btreeList.count());

    TEST_ASSERT_NOT_NULL(btreeList.getByKey(1));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(2));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(3));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(4));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(5));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(6));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(7));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(8));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(9));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(10));

    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(1)->getItem(), 100);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(2)->getItem(), 101);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(3)->getItem(), 102);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(4)->getItem(), 103);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(5)->getItem(), 104);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(6)->getItem(), 105);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(7)->getItem(), 106);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(8)->getItem(), 107);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(9)->getItem(), 108);
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(10)->getItem(), 109);

    // clear the tree and ensure it clears
    btreeList.clear();
    TEST_ASSERT_EQUAL_UINT8(10, btreeList.capacity());
    TEST_ASSERT_EQUAL_UINT8(0, btreeList.count());

    // now add an item back and make sure we find it.
    TEST_ASSERT_TRUE(btreeList.add(storage9));
    TEST_ASSERT_NOT_NULL(btreeList.getByKey(9));
    TEST_ASSERT_EQUAL_INT(btreeList.getByKey(9)->getItem(), 108);
}

class NumericStorageItem {
private:
    uint32_t item;

public:
    NumericStorageItem() : item(0xffffffff) {}
    explicit NumericStorageItem(uint32_t value) : item(value) {}
    NumericStorageItem(const NumericStorageItem& other) = default;
    NumericStorageItem& operator= (const NumericStorageItem& other) = default;
    uint32_t getKey() const { return item; }
};

void printArray(const NumericStorageItem* list, int size) {
    char sz[160];
    strcpy(sz, "array: ");
    for(int i=0;i<size;i++) {
        char intBuf[10];
        itoa((int)(list[i].getKey()), intBuf, 10);
        strcat(sz, intBuf);
        strcat(sz, " ");
    }
    serdebug(sz);
}

void testAddingThenRemovingThenAddingItems() {
    BtreeList<uint32_t, NumericStorageItem> myList;

    for(int i=0;i<20;i++) {
        TEST_ASSERT_TRUE(myList.add(NumericStorageItem(i + 100)));
    }

    // should have 20 items now
    TEST_ASSERT_EQUAL_UINT8(myList.count(), bsize_t(20));

    // remove key 102
    TEST_ASSERT_TRUE(myList.removeByKey(102));
    TEST_ASSERT_FALSE(myList.removeByKey(10002));

    printArray(myList.items(), myList.count());

    // should have 19 items
    TEST_ASSERT_EQUAL_UINT8(myList.count(), bsize_t(19));

    // ensure that only 102 was removed
    TEST_ASSERT_NULL(myList.getByKey(102));
    TEST_ASSERT_NOT_NULL(myList.getByKey(103));
    TEST_ASSERT_NOT_NULL(myList.getByKey(101));

    // put 102 back
    myList.add(NumericStorageItem(102));

    printArray(myList.items(), myList.count());

    for(int i=0;i<20;i++) {
        auto entry = myList.itemAtIndex(i);
        TEST_ASSERT_EQUAL_UINT32(entry->getKey(), uint32_t(i + 100));
    }

    // now test removing the edge cases, IE head and tail of list
    myList.removeIndex(0);
    myList.removeIndex(19);
    printArray(myList.items(), myList.count());

    TEST_ASSERT_EQUAL_UINT8(myList.count(), bsize_t(18));
    TEST_ASSERT_NULL(myList.getByKey(100));
    TEST_ASSERT_NULL(myList.getByKey(119));
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(testNearestLocationEdgeCases);
    RUN_TEST(testAddingWithoutSortOrResize);
    RUN_TEST(testAddingWithSortNoResize);
    RUN_TEST(testAddingWithSortAndResizeBy5);
    RUN_TEST(testAddingThenRemovingThenAddingItems);
    UNITY_END();
}

void loop() {}