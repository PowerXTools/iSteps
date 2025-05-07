#include "header/model/converter_common_models/converter_model/converter_model.h"
#include "header/basic/utility.h"
#include "header/pf_database.h"
#include "header/iSTEPS.h"
#include <iostream>
using namespace std;
CONVERTER_MODEL::CONVERTER_MODEL(iSTEPS& toolkit)
{
    set_toolkit(toolkit);
}

CONVERTER_MODEL::~CONVERTER_MODEL()
{
    ;
}

void CONVERTER_MODEL::set_toolkit(iSTEPS& toolkit)
{
    this->toolkit = &toolkit;
}

iSTEPS& CONVERTER_MODEL::get_toolkit() const
{
    return *toolkit;
}

void CONVERTER_MODEL::set_bus_pointer(BUS* busptr)
{
    this->busptr = busptr;
}

void CONVERTER_MODEL::set_device_pointer(NONBUS_DEVICE* device)
{
    this->device = device;
}

BUS* CONVERTER_MODEL::get_bus_pointer() const
{
    return busptr;
}

NONBUS_DEVICE* CONVERTER_MODEL::get_device_pointer() const
{
    return device;
}

DEVICE_ID CONVERTER_MODEL::get_device_id() const
{
    return device->get_device_id();
}

string CONVERTER_MODEL::get_compound_device_name() const
{
    return get_device_id().get_compound_device_name();
}
