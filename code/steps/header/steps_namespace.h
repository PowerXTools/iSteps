#ifndef STEPS_NAMESPACE_H
#define STEPS_NAMESPACE_H

#include "header/iSTEPS.h"
#include "header/basic/steps_define.h"
#include "header/basic/constants.h"
#include "header/basic/string_int_map.h"

#include <mutex>

extern StringIntMap toolkit_str_int_map;
extern iSTEPS default_toolkit;
extern iSTEPS* toolkits[MAX_TOOLKIT_SIZE];
extern std::mutex mtx;
extern bool use_fast_math;

#endif // STEPS_NAMESPACE_H
