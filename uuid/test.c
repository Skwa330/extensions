#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <pthread.h>

#include "uuid.h"

int main() {
    struct uuid uuid;

    char buf[37];

    for (;;) {
        uuid_random(&uuid);
        uuid_to_str(&uuid, buf);
        puts(buf);
    }
}