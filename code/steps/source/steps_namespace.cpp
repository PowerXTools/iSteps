#include "header/steps_namespace.h"

StringIntMap toolkit_str_int_map;

iSTEPS default_toolkit;
iSTEPS* toolkits[MAX_TOOLKIT_SIZE];
std::mutex mtx;

bool use_fast_math = false;

