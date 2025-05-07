#include "header/pf_database.h"
#include "header/device/nonbus_device.h"
#include <istream>
#include <iostream>

using namespace std;

NONBUS_DEVICE::NONBUS_DEVICE(iSTEPS& toolkit) : DEVICE(toolkit)
{
    ownership.clear();
}

NONBUS_DEVICE::~NONBUS_DEVICE()
{
    ;
}

void NONBUS_DEVICE::set_ownership(const OWNERSHIP& os)
{
    this->ownership = os;
}

OWNERSHIP NONBUS_DEVICE::get_ownership() const
{
    return this->ownership;
}

size_t NONBUS_DEVICE::get_owner_count() const
{
    return ownership.get_owner_count();
}

size_t NONBUS_DEVICE::get_owner_of_index(size_t index) const
{
    return ownership.get_owner_of_index(index);
}

double NONBUS_DEVICE::get_fraction_of_owner_of_index(size_t index) const
{
    return ownership.get_fraction_of_owner_of_index(index);
}
