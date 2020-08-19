#include "uuid.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32

#include <Windows.h>

struct timespec {
    long tv_sec, tv_nsec;
};

int clock_gettime(int clockid, struct timespec *spec)
{
    int64_t wintime;
    GetSystemTimeAsFileTime((FILETIME *)&wintime);
    wintime -= 116444736000000000ll;
    spec->tv_sec = wintime / 10000000ll;
    spec->tv_nsec = wintime % 10000000ll * 100;

    return 0;
}

#endif

int64_t nanotime() {
    struct timespec ts;
    clock_gettime(0, &ts);

    int64_t large_seconds = (int64_t)ts.tv_sec;
    int64_t large_nanoseconds = (int64_t)ts.tv_nsec;
    int64_t nanotime = large_seconds * 1000000000 + large_nanoseconds;
    return nanotime;
}

#define IMAX_BITS(m) ((m)/((m)%255+1) / 255%255*8 + 7-86/((m)%255+12))
#define RAND_MAX_WIDTH IMAX_BITS(RAND_MAX)
_Static_assert((RAND_MAX & (RAND_MAX + 1u)) == 0, "RAND_MAX not a Mersenne number");

uint64_t rand64() {
    uint64_t r = 0;

    for (int i = 0; i < 64; i += RAND_MAX_WIDTH) {
      r <<= RAND_MAX_WIDTH;
      r ^= (unsigned) rand();
    }

    return r;
}

void uuid_random(struct uuid *uuid) {
    srand(nanotime());

    uuid->lower = rand64();
    uuid->upper = rand64();
}

int unhex(int ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    } else if (tolower(ch) >= 'a' && tolower(ch) <= 'f') {
        return 10 + tolower(ch) - 'a';
    } else {
        return -1;
    }
}

int uuid_from_str(struct uuid *uuid, char *str) {
    int n, p, o;

    for (p = n = o = 0; n < p + 8; n += 2) {
        int v = 0;

        for (int i = 0; i < 2; i++) {
            int u = unhex(str[n + i]);
            if (u == -1) return -1;
            v = v * 16 + u;
        }

        uuid->data[o++] = (char)v;
    }

    if (str[n++] != '-') return -1;

    for (int x = 0; x < 3; x++) {
        for (p = n; n < p + 4; n += 2) {
            int v = 0;

            for (int i = 0; i < 2; i++) {
                int u = unhex(str[n + i]);
                if (u == -1) return -1;
                v = v * 16 + u;
            }

            uuid->data[o++] = (char)v;
        }

        if (str[n++] != '-') return -1;
    }

    for (int x = 0; x < 3; x++) {
        for (p = n; n < p + 4; n += 2) {
            int v = 0;

            for (int i = 0; i < 2; i++) {
                int u = unhex(str[n + i]);
                if (u == -1) return -1;
                v = v * 16 + u;
            }

            uuid->data[o++] = (char)v;
        }
    }

    return 0;
}

void uuid_to_str(struct uuid *uuid, char *buffer) {
    sprintf(buffer, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", (unsigned char)uuid->data[0], (unsigned char)uuid->data[1], (unsigned char)uuid->data[2], (unsigned char)uuid->data[3], (unsigned char)uuid->data[4], (unsigned char)uuid->data[5], (unsigned char)uuid->data[6], (unsigned char)uuid->data[7], (unsigned char)uuid->data[8], (unsigned char)uuid->data[9], (unsigned char)uuid->data[10], (unsigned char)uuid->data[11], (unsigned char)uuid->data[12], (unsigned char)uuid->data[13], (unsigned char)uuid->data[14], (unsigned char)uuid->data[15]);
}

int uuid_less(struct uuid *a, struct uuid *b) {
    if (a->upper == b->upper) {
        return a->lower < b->lower;
    }

    return a->upper < b->upper;
}

int uuid_greater(struct uuid *a, struct uuid *b) {
    if (a->upper == b->upper) {
        return a->lower > b->lower;
    }

    return a->upper > b->upper;
}

int uuid_compare(struct uuid *a, struct uuid *b) {
    if (uuid_less(a, b)) {
        return -1;
    } else if (uuid_greater(a, b)) {
        return 1;
    }

    return 0;
}