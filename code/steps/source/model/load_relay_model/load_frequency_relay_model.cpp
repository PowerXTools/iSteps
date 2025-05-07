#include "header/model/load_relay_model/load_frequency_relay_model.h"
#include "header/device/load.h"
#include "header/basic/utility.h"
#include "header/pf_database.h"
#include "header/iSTEPS.h"

LOAD_FREQUENCY_RELAY_MODEL::LOAD_FREQUENCY_RELAY_MODEL(iSTEPS& toolkit) : LOAD_RELAY_MODEL(toolkit)
{
    ;
}

LOAD_FREQUENCY_RELAY_MODEL::~LOAD_FREQUENCY_RELAY_MODEL()
{
    ;
}

string LOAD_FREQUENCY_RELAY_MODEL::get_model_type() const
{
    return "LOAD FREQUENCY RELAY";
}
