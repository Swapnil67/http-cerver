#ifndef SV_H_
#define SV_H_

#include <ctype.h>

typedef struct { 
    size_t len;    
    const char *data;
} String_View;

static inline
String_View sv_chop_by_delim(String_View *sv, char delim) {
    size_t i = 0;
    while (i < sv->len && sv->data[i] != delim) {
        i += 1;
    }
    // printf("%ld - %ld\n", sv->count, i);

    String_View result = {
        .len = i,
        .data = sv->data,
    };

    if (i < sv->len) {
        sv->len -= i + 1;
        sv->data += i + 1;
    }
    else {
        sv->len -= i;
        sv->data += i;
    }

    return result;
}

static inline
String_View sv_trim_left(String_View sv) {
    size_t i = 0;
    while(i < sv.len && isspace(sv.data[i])) {
	i += 1;
    }

    return (String_View) {
	.len = sv.len - i,
	.data = sv.data + i
    };
}

static inline
String_View sv_trim_right(String_View sv) {
    size_t i = 0;
    while(i < sv.len && isspace(sv.data[sv.len - i - 1])) {
	i += 1;
    }

    return (String_View) {
	.len = sv.len - i,
	.data = sv.data
    };
}

static inline
String_View sv_trim(String_View sv) {
    return sv_trim_right(sv_trim_left(sv));
}

#endif // * SV_H_
