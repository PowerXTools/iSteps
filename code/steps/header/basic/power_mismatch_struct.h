#ifndef GREATEST_POWER_CURRENT_MISMATCH_STRUCT_H
#define GREATEST_POWER_CURRENT_MISMATCH_STRUCT_H

#include <cstddef>

struct GREATEST_POWER_CURRENT_MISMATCH_STRUCT
{
    double greatest_power_mismatch_in_MVA;
    size_t bus_with_greatest_power_mismatch;
    double greatest_current_mismatch_in_pu;
    size_t bus_with_greatest_current_mismatch;
};

#endif // GREATEST_POWER_CURRENT_MISMATCH_STRUCT_H
