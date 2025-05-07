#include "header/apis/steps_api.h"
#include "header/apis/steps_api_common.h"
#include "header/basic/utility.h"
#include "header/steps_namespace.h"
#include "header/data_imexporter/psse_imexporter.h"
#include "header/data_imexporter/bpa_imexporter.h"

int api_get_zone_integer_data(size_t zone, char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();

    ZONE* zoneptr = psdb.get_zone(zone);
    if(zoneptr!=NULL)
    {
        string PARAMETER_NAME = string2upper(parameter_name);
        if(PARAMETER_NAME=="ZONE" or PARAMETER_NAME=="ZONE NUMBER")
            return zoneptr->get_zone_number();

        show_parameter_not_supported_for_area_zone_owner_with_api(PARAMETER_NAME, zone, __FUNCTION__);
        return 0;
    }
    else
    {
        show_area_zone_owner_not_exist_with_api(zone, __FUNCTION__);
        return 0;
    }
}

void api_set_zone_integer_data(size_t zone, char* parameter_name, int value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();

    ZONE* zoneptr = psdb.get_zone(zone);
    if(zoneptr!=NULL)
    {
        string PARAMETER_NAME = string2upper(parameter_name);
        if(PARAMETER_NAME=="ZONE" or PARAMETER_NAME=="ZONE NUMBER")
            return zoneptr->set_zone_number(value);

        show_parameter_not_supported_for_area_zone_owner_with_api(PARAMETER_NAME, zone, __FUNCTION__);
    }
    else
        show_area_zone_owner_not_exist_with_api(zone, __FUNCTION__);
}

double api_get_zone_float_data(size_t zone, char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();

    ZONE* zoneptr = psdb.get_zone(zone);
    if(zoneptr!=NULL)
    {
        string PARAMETER_NAME = string2upper(parameter_name);

        show_parameter_not_supported_for_area_zone_owner_with_api(PARAMETER_NAME, zone, __FUNCTION__);
        return 0.0;
    }
    else
    {
        show_area_zone_owner_not_exist_with_api(zone, __FUNCTION__);
        return 0.0;
    }
}

void api_set_zone_float_data(size_t zone, char* parameter_name, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s() has not been implemented. Input parameters are provided: %zu, %s, %f.",
             __FUNCTION__, zone, parameter_name, value);
    toolkit.show_information(buffer);
    return;
}

const char* api_get_zone_string_data(size_t zone, char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();

	snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", "");

    ZONE* zoneptr = psdb.get_zone(zone);
    if(zoneptr!=NULL)
    {
        string PARAMETER_NAME = string2upper(parameter_name);
		if (PARAMETER_NAME == "NAME" or PARAMETER_NAME == "ZONE NAME")
		{
			snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", (zoneptr->get_zone_name()).c_str());
			return toolkit.steps_char_buffer;
		}

        show_parameter_not_supported_for_area_zone_owner_with_api(PARAMETER_NAME, zone, __FUNCTION__);
        return toolkit.steps_char_buffer;
    }
    else
    {
        show_area_zone_owner_not_exist_with_api(zone, __FUNCTION__);
        return toolkit.steps_char_buffer;
    }
}

void api_set_zone_string_data(size_t zone, char* parameter_name, char* value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();

    string BLANK = "";
    ZONE* zoneptr = psdb.get_zone(zone);
    if(zoneptr!=NULL)
    {
        string PARAMETER_NAME = string2upper(parameter_name);
        if(PARAMETER_NAME=="NAME" or PARAMETER_NAME=="ZONE NAME")
            return zoneptr->set_zone_name(value);

        show_parameter_not_supported_for_area_zone_owner_with_api(PARAMETER_NAME, zone, __FUNCTION__);
    }
    else
        show_area_zone_owner_not_exist_with_api(zone, __FUNCTION__);
}

bool api_get_zone_boolean_data(size_t zone, char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();

    ZONE* zoneptr = psdb.get_zone(zone);
    if(zoneptr!=NULL)
    {
        string PARAMETER_NAME = string2upper(parameter_name);

        show_parameter_not_supported_for_area_zone_owner_with_api(PARAMETER_NAME, zone, __FUNCTION__);
        return false;
    }
    else
    {
        show_area_zone_owner_not_exist_with_api(zone, __FUNCTION__);
        return false;
    }
}

void api_set_zone_boolean_data(size_t zone, char* parameter_name, bool value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s() has not been implemented. Input parameters are provided: %zu, %s, %s.",
             __FUNCTION__, zone, parameter_name, (value==true?"True":"False"));
    toolkit.show_information(buffer);
    return;
}
