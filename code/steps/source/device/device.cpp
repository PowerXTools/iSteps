#include "header/pf_database.h"
#include "header/device/device.h"
#include <istream>
#include <iostream>

using namespace std;

DEVICE::DEVICE(iSTEPS& toolkit)
{
    set_toolkit(toolkit);
}

DEVICE::~DEVICE()
{
    ;
}

void DEVICE::set_toolkit(iSTEPS& toolkit)
{
    this->toolkit = (&toolkit);
}

iSTEPS& DEVICE::get_toolkit() const
{
    return (*toolkit);
}

string DEVICE::get_compound_device_name() const
{
    return get_device_id().get_compound_device_name();
}
