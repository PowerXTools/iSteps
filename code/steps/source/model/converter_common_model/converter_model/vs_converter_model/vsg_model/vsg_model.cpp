#include "header/model/converter_common_models/converter_model/vs_converter_model/vsg_model/vsg_model.h"
#include "header/basic/utility.h"
#include "header/pf_database.h"
#include "header/iSTEPS.h"
#include <iostream>
using namespace std;
VSG_MODEL::VSG_MODEL(iSTEPS& toolkit) : VS_CONVERTER_MODEL(toolkit)
{
    set_toolkit(toolkit);
}

VSG_MODEL::~VSG_MODEL()
{
    ;
}


