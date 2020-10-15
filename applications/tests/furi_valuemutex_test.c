#include <stdio.h>
#include <string.h>
#include "flipper_v2.h"
#include "log.h"

#include "minunit.h"

void test_furi_valuemutex() {
    const int init_value = 0xdeadbeef;
    const int changed_value = 0x12345678;

    int value = init_value;
    bool result;
    ValueMutex valuemutex;

    // init mutex case
    result = init_mutex(&valuemutex, &value, sizeof(value));
    mu_assert(result, "init mutex failed");

    // acquire mutex case
    int* value_pointer = acquire_mutex(&valuemutex, 100);
    mu_assert_pointers_eq(value_pointer, &value);

    // second acquire mutex case
    int* value_pointer_second = acquire_mutex(&valuemutex, 100);
    mu_assert_pointers_eq(value_pointer_second, NULL);

    // change value case
    *value_pointer = changed_value;
    mu_assert_int_eq(value, changed_value);

    // release mutex case
    result = release_mutex(&valuemutex, &value);
    mu_assert(result, "release mutex failed");

    // TODO
    //acquire mutex blocking case
    //write mutex blocking case
    //read mutex blocking case
}

/*
TEST: concurrent access

1. Create holding record
2. Open it twice
3. Change value simultaneously in two app and check integrity
*/

// TODO this test broke because mutex in furi is not implemented

typedef struct {
    // a and b must be equal
    uint8_t a;
    uint8_t b;
} ConcurrentValue;

void furi_concurent_app(void* p) {
    ValueMutex* mutex = (ValueMutex*)p;
    if(mutex == NULL) {
        printf("cannot open mutex\n");
        furiac_exit(NULL);
    }

    for(size_t i = 0; i < 10; i++) {
        ConcurrentValue* value = (ConcurrentValue*)acquire_mutex_block(mutex);

        if(value == NULL) {
            printf("cannot take record\n");
            release_mutex(mutex, value);
            furiac_exit(NULL);
        }

        // emulate read-modify-write broken by context switching
        uint8_t a = value->a;
        uint8_t b = value->b;
        a++;
        b++;
        delay(2);
        value->a = a;
        value->b = b;
        release_mutex(mutex, value);
    }

    furiac_exit(NULL);
}

void test_furi_concurrent_access() {
    // 1. Create holding record
    ConcurrentValue value = {.a = 0, .b = 0};
    ValueMutex mutex;
    mu_check(init_mutex(&mutex, &value, sizeof(value)));

    // 3. Create second app for interact with it
    FuriApp* second_app = furiac_start(furi_concurent_app, "furi concurent app", (void*)&mutex);

    // 4. multiply ConcurrentValue::a
    for(size_t i = 0; i < 4; i++) {
        ConcurrentValue* value = (ConcurrentValue*)acquire_mutex_block(&mutex);

        if(value == NULL) {
            release_mutex(&mutex, value);
            mu_fail("cannot take record\n");
        }

        // emulate read-modify-write broken by context switching
        uint8_t a = value->a;
        uint8_t b = value->b;
        a++;
        b++;
        value->a = a;
        delay(10); // this is only for test, do not add delay between take/give in prod!
        value->b = b;
        release_mutex(&mutex, value);
    }

    delay(50);

    mu_assert_pointers_eq(second_app->handler, NULL);

    mu_assert_int_eq(value.a, value.b);
}