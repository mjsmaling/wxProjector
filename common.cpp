#include <assert.h>
#include "../include/common.h"

int random_number(int min, int max)
{
    if (min == 0 && max == RAND_MAX)
        return rand();

    int difference = max - min + 1;

    assert(difference != 0);

    float average = (float) RAND_MAX / (float) difference;

    int number = (average * rand()) + min;

    return number;
}
