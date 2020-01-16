#pragma once

typedef struct cond_label {
    long long start;
    long long go_to;

    cond_label(long long start, long long go_to) {
        this->start = start;
        this->go_to = go_to;
    }
} cond_label;

typedef struct for_label {
    symbol* iterator;
    symbol* start;
    symbol* end;
    cond_label* jump_label;

    for_label(symbol* iterator, symbol* start, symbol* end, cond_label* label) {
        this->iterator = iterator;
        this->start = start;
        this->end = end;
        this->jump_label = label;
    }
} for_label;