#include "stream.h"
#include <string.h>
#include <ctype.h>
#include <math.h>

extern unsigned long millis(void);


int stream_timedRead(Stream* s) {
	int c;
	s->startMillis = millis();
	do {
		c = s->read(s);
		if (c >= 0) return c;
	} while (millis() - s->startMillis < s->timeout);
	return -1;
}

int stream_timedPeek(Stream* s) {
	int c;
	s->startMillis = millis();
	do {
		c = s->peek(s);
		if (c >= 0) return c;
	} while (millis() - s->startMillis < s->timeout);
	return -1;
}

int stream_peekNextDigit(Stream* s, LookaheadMode lookahead, bool detectDecimal) {
	int c;
	while (1) {
		c = stream_timedPeek(s);
		if (c < 0 ||
			c == '-' ||
			(c >= '0' && c <= '9') ||
			(detectDecimal && c == '.')) {
			return c;
		}
		switch (lookahead) {
		case SKIP_NONE:
			return -1;
		case SKIP_WHITESPACE:
			if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
				break;
			else
				return -1;
		case SKIP_ALL:
			break;
		}
		s->read(s); 
	}
}


void stream_setTimeout(Stream* s, unsigned long timeout) {
	s->timeout = timeout;
}

unsigned long stream_getTimeout(Stream* s) {
	return s->timeout;
}


bool stream_find(Stream* s, const char* target) {
	return stream_findUntil_len(s, target, strlen(target), NULL, 0);
}

bool stream_find_len(Stream* s, const char* target, size_t length) {
	return stream_findUntil_len(s, target, length, NULL, 0);
}

bool stream_findUntil(Stream* s, const char* target, const char* terminator) {
	size_t tlen = target ? strlen(target) : 0;
	size_t termLen = terminator ? strlen(terminator) : 0;
	return stream_findUntil_len(s, target, tlen, terminator, termLen);
}

bool stream_findUntil_len(Stream* s, const char* target, size_t targetLen,
						  const char* terminator, size_t termLen) {
	if (targetLen == 0) return true;
	size_t idx = 0;
	size_t termIdx = 0;
	while (1) {
		int c = stream_timedRead(s);
		if (c < 0) return false;
		
		if (terminator && termLen > 0) {
			if (c == terminator[termIdx]) {
				termIdx++;
				if (termIdx == termLen) return false;
			} else {
				termIdx = 0;
			}
		}
		
		if (c == target[idx]) {
			idx++;
			if (idx == targetLen) return true;
		} else {
			size_t newIdx = 0;
			if (c == target[0]) newIdx = 1;
			idx = newIdx;
		}
	}
}


long stream_parseInt(Stream* s, LookaheadMode lookahead, char ignore) {
	bool isNegative = false;
	long value = 0;
	int c = stream_peekNextDigit(s, lookahead, false);
	if (c < 0) return 0;
	
	do {
		if (c == ignore) {
		} else if (c == '-') {
			isNegative = true;
		} else if (c >= '0' && c <= '9') {
			value = value * 10 + (c - '0');
		}
		s->read(s);
		c = stream_timedPeek(s);
	} while ((c >= '0' && c <= '9') || c == ignore);
	
	if (isNegative) value = -value;
	return value;
}


float stream_parseFloat(Stream* s, LookaheadMode lookahead, char ignore) {
	bool isNegative = false;
	long intPart = 0;
	long fracPart = 0;
	int fracDigits = 0;
	bool afterDecimal = false;
	
	int c = stream_peekNextDigit(s, lookahead, true);
	if (c < 0) return 0.0f;
	
	if (c == '-') {
		isNegative = true;
		s->read(s);
		c = stream_timedPeek(s);
	}
	
	while (c >= '0' && c <= '9') {
		intPart = intPart * 10 + (c - '0');
		s->read(s);
		c = stream_timedPeek(s);
	}
	
	if (c == '.') {
		afterDecimal = true;
		s->read(s);
		c = stream_timedPeek(s);
		while (c >= '0' && c <= '9') {
			fracPart = fracPart * 10 + (c - '0');
			fracDigits++;
			s->read(s);
			c = stream_timedPeek(s);
		}
	}
	
	while (c == ignore) {
		s->read(s);
		c = stream_timedPeek(s);
	}
	
	float result = (float)intPart;
	if (afterDecimal && fracDigits > 0) {
		float divisor = 1.0f;
		for (int i = 0; i < fracDigits; i++) divisor *= 10.0f;
		result += (float)fracPart / divisor;
	}
	if (isNegative) result = -result;
	return result;
}


size_t stream_readBytes(Stream* s, char* buffer, size_t length) {
	size_t count = 0;
	while (count < length) {
		int c = stream_timedRead(s);
		if (c < 0) break;
		buffer[count++] = (char)c;
	}
	return count;
}

size_t stream_readBytesUntil(Stream* s, char terminator, char* buffer, size_t length) {
	size_t count = 0;
	while (count < length) {
		int c = stream_timedRead(s);
		if (c < 0 || c == terminator) break;
		buffer[count++] = (char)c;
	}
	return count;
}


size_t stream_readString(Stream* s, char* buffer, size_t bufsize) {
	size_t count = 0;
	if (bufsize == 0) return 0;
	while (count < bufsize - 1) {
		int c = stream_timedRead(s);
		if (c < 0) break;
		buffer[count++] = (char)c;
	}
	buffer[count] = '\0';
	return count;
}

size_t stream_readStringUntil(Stream* s, char terminator, char* buffer, size_t bufsize) {
	size_t count = 0;
	if (bufsize == 0) return 0;
	while (count < bufsize - 1) {
		int c = stream_timedRead(s);
		if (c < 0 || c == terminator) break;
		buffer[count++] = (char)c;
	}
	buffer[count] = '\0';
	return count;
}   