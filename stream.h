#ifndef STREAM_H
#define STREAM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "print.h"   

typedef enum {
    SKIP_ALL,          
    SKIP_NONE,        
    SKIP_WHITESPACE   
} LookaheadMode;

#define NO_IGNORE_CHAR 0x01   

typedef struct Stream Stream;
struct Stream {
    Print base;   

    int (*available)(Stream* self);
    int (*read)(Stream* self);
    int (*peek)(Stream* self);

    unsigned long timeout;
    unsigned long startMillis;
};

int stream_timedRead(Stream* s);
int stream_timedPeek(Stream* s);
int stream_peekNextDigit(Stream* s, LookaheadMode lookahead, bool detectDecimal);

void stream_setTimeout(Stream* s, unsigned long timeout);
unsigned long stream_getTimeout(Stream* s);

bool stream_find(Stream* s, const char* target);
bool stream_find_len(Stream* s, const char* target, size_t length);
bool stream_findUntil(Stream* s, const char* target, const char* terminator);
bool stream_findUntil_len(Stream* s, const char* target, size_t targetLen,
                          const char* terminator, size_t termLen);

long stream_parseInt(Stream* s, LookaheadMode lookahead, char ignore);
float stream_parseFloat(Stream* s, LookaheadMode lookahead, char ignore);

size_t stream_readBytes(Stream* s, char* buffer, size_t length);
size_t stream_readBytesUntil(Stream* s, char terminator, char* buffer, size_t length);

size_t stream_readString(Stream* s, char* buffer, size_t bufsize);
size_t stream_readStringUntil(Stream* s, char terminator, char* buffer, size_t bufsize);


#endif