#ifndef EXT_UUID_UUID_H
#define EXT_UUID_UUID_H

#include <stdint.h>

struct uuid {
    union {
        char data[16];

        struct {
            uint64_t lower, upper;
        };
    };
};

void uuid_random(struct uuid *uuid);
int uuid_from_str(struct uuid *uuid, char *str);
void uuid_to_str(struct uuid *uuid, char *buffer);
int uuid_compare(struct uuid *a, struct uuid *b);

#endif /* EXT_UUID_UUID_H */