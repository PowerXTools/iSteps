#include "header/apis/steps_api.h"
#include "header/apis/steps_api_common.h"
#include "header/basic/utility.h"
#include "header/steps_namespace.h"

const char* api_get_wt_generator_related_model_name(size_t bus, char* identifier, char* model_type, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
	snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", "");
    DEVICE_ID did = get_wt_generator_device_id(bus, identifier);
    PF_DATA& psdb = toolkit.get_database();
    WT_GENERATOR* generator = psdb.get_wt_generator(did);
    if(generator==NULL)
    {
        show_device_not_exist_with_api(did, __FUNCTION__);
        return toolkit.steps_char_buffer;
    }

    string MODEL_TYPE = string2upper(model_type);
    if(MODEL_TYPE=="WTGEN" or MODEL_TYPE=="WT GENERATOR")
    {
        WT_GENERATOR_MODEL* model = generator->get_wt_generator_model();
        if(model!=NULL)
			snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", (model->get_model_name()).c_str());
		return toolkit.steps_char_buffer;
    }
    if(MODEL_TYPE=="WTAERD" or MODEL_TYPE=="WT AERODYNAMIC")
    {
        WT_AERODYNAMIC_MODEL* model = generator->get_wt_aerodynamic_model();
        if(model!=NULL)
			snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", (model->get_model_name()).c_str());
		return toolkit.steps_char_buffer;
    }
    if(MODEL_TYPE=="WTELEC" or MODEL_TYPE=="WT ELECTRICAL")
    {
        WT_ELECTRICAL_MODEL* model = generator->get_wt_electrical_model();
        if(model!=NULL)
			snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", (model->get_model_name()).c_str());
		return toolkit.steps_char_buffer;
    }
    if(MODEL_TYPE=="WTTURB" or MODEL_TYPE=="WT TURBINE")
    {
        WT_TURBINE_MODEL* model = generator->get_wt_turbine_model();
        if(model!=NULL)
			snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", (model->get_model_name()).c_str());
		return toolkit.steps_char_buffer;
    }
    if(MODEL_TYPE=="WTPITCH" or MODEL_TYPE=="WT PITCH")
    {
        WT_PITCH_MODEL* model = generator->get_wt_pitch_model();
        if(model!=NULL)
			snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", (model->get_model_name()).c_str());
		return toolkit.steps_char_buffer;
    }
    if(MODEL_TYPE=="WINDSPD" or MODEL_TYPE=="WIND SPEED")
    {
        WIND_SPEED_MODEL* model = generator->get_wind_speed_model();
        if(model!=NULL)
			snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", (model->get_model_name()).c_str());
		return toolkit.steps_char_buffer;
    }
    show_model_type_not_supported_for_device_with_api(MODEL_TYPE, did, __FUNCTION__);
	return toolkit.steps_char_buffer;
}

double api_get_wt_generator_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DEVICE_ID did = get_wt_generator_device_id(bus, identifier);
    PF_DATA& psdb = toolkit.get_database();
    WT_GENERATOR* generator = psdb.get_wt_generator(did);
    if(generator==NULL)
    {
        show_device_not_exist_with_api(did, __FUNCTION__);
        return 0.0;
    }
    string MODEL_TYPE = string2upper(model_type);
    string PARAMETER_NAME = string2upper(parameter_name);
    if(MODEL_TYPE=="WTGEN" or MODEL_TYPE=="WT GENERATOR")
    {
        WT_GENERATOR_MODEL* model = generator->get_wt_generator_model();
        if(model!=NULL)
            return model->get_model_data_with_name(PARAMETER_NAME);
        else
            return 0.0;
    }

    if(MODEL_TYPE=="WTAERD" or MODEL_TYPE=="WT AERODYNAMIC")
    {
        WT_AERODYNAMIC_MODEL* model = generator->get_wt_aerodynamic_model();
        if(model!=NULL)
            return model->get_model_data_with_name(PARAMETER_NAME);
        else
            return 0.0;
    }

    if(MODEL_TYPE=="WTELEC" or MODEL_TYPE=="WT ELECTRICAL")
    {
        WT_ELECTRICAL_MODEL* model = generator->get_wt_electrical_model();
        if(model!=NULL)
            return model->get_model_data_with_name(PARAMETER_NAME);
        else
            return 0.0;
    }
    if(MODEL_TYPE=="WTTURB" or MODEL_TYPE=="WT TURBINE")
    {
        WT_TURBINE_MODEL* model = generator->get_wt_turbine_model();
        if(model!=NULL)
            return model->get_model_data_with_name(PARAMETER_NAME);
        else
            return 0.0;
    }
    if(MODEL_TYPE=="WTPITCH" or MODEL_TYPE=="WT PITCH")
    {
        WT_PITCH_MODEL* model = generator->get_wt_pitch_model();
        if(model!=NULL)
            return model->get_model_data_with_name(PARAMETER_NAME);
        else
            return 0.0;
    }
    if(MODEL_TYPE=="WINDSPD" or MODEL_TYPE=="WIND SPEED")
    {
        WIND_SPEED_MODEL* model = generator->get_wind_speed_model();
        if(model!=NULL)
            return model->get_model_data_with_name(PARAMETER_NAME);
        else
            return 0.0;
    }
    show_model_type_not_supported_for_device_with_api(MODEL_TYPE, did, __FUNCTION__);
    return 0.0;
}

void api_set_wt_generator_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DEVICE_ID did = get_wt_generator_device_id(bus, identifier);
    PF_DATA& psdb = toolkit.get_database();
    WT_GENERATOR* generator = psdb.get_wt_generator(did);
    if(generator==NULL)
    {
        show_device_not_exist_with_api(did, __FUNCTION__);
        return;
    }
    string MODEL_TYPE = string2upper(model_type);
    string PARAMETER_NAME = string2upper(parameter_name);
    if(MODEL_TYPE=="WTGEN" or MODEL_TYPE=="WT GENERATOR")
    {
        WT_GENERATOR_MODEL* model = generator->get_wt_generator_model();
        if(model!=NULL)
            return model->set_model_data_with_name(PARAMETER_NAME, value);
        else
            return;
    }

    if(MODEL_TYPE=="WTAERD" or MODEL_TYPE=="WT AERODYNAMIC")
    {
        WT_AERODYNAMIC_MODEL* model = generator->get_wt_aerodynamic_model();
        if(model!=NULL)
            return model->set_model_data_with_name(PARAMETER_NAME, value);
        else
            return;
    }
    if(MODEL_TYPE=="WTELEC" or MODEL_TYPE=="WT ELECTRICAL")
    {
        WT_ELECTRICAL_MODEL* model = generator->get_wt_electrical_model();
        if(model!=NULL)
            return model->set_model_data_with_name(PARAMETER_NAME, value);
        else
            return;
    }
    if(MODEL_TYPE=="WTTURB" or MODEL_TYPE=="WT TURBINE")
    {
        WT_TURBINE_MODEL* model = generator->get_wt_turbine_model();
        if(model!=NULL)
            return model->set_model_data_with_name(PARAMETER_NAME, value);
        else
            return;
    }
    if(MODEL_TYPE=="WTPITCH" or MODEL_TYPE=="WT PITCH")
    {
        WT_PITCH_MODEL* model = generator->get_wt_pitch_model();
        if(model!=NULL)
            return model->set_model_data_with_name(PARAMETER_NAME, value);
        else
            return;
    }
    if(MODEL_TYPE=="WINDSPD" or MODEL_TYPE=="WIND SPEED")
    {
        WIND_SPEED_MODEL* model = generator->get_wind_speed_model();
        if(model!=NULL)
            return model->set_model_data_with_name(PARAMETER_NAME, value);
        else
            return;
    }
    show_model_type_not_supported_for_device_with_api(MODEL_TYPE, did, __FUNCTION__);
}

size_t api_get_wt_generator_related_model_float_parameter_count(size_t bus, char* identifier, char* model_type, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DEVICE_ID did = get_wt_generator_device_id(bus, identifier);
    PF_DATA& psdb = toolkit.get_database();
    WT_GENERATOR* generator = psdb.get_wt_generator(did);
    if(generator==NULL)
    {
        show_device_not_exist_with_api(did, __FUNCTION__);
        return 0;
    }
    string MODEL_TYPE = string2upper(model_type);
    if(MODEL_TYPE=="WTGEN" or MODEL_TYPE=="WT GENERATOR")
    {
        WT_GENERATOR_MODEL* model = generator->get_wt_generator_model();
        if(model!=NULL)
            return model->get_model_float_parameter_count();
        else
            return 0;
    }

    if(MODEL_TYPE=="WTAERD" or MODEL_TYPE=="WT AERODYNAMIC")
    {
        WT_AERODYNAMIC_MODEL* model = generator->get_wt_aerodynamic_model();
        if(model!=NULL)
            return model->get_model_float_parameter_count();
        else
            return 0;
    }
    if(MODEL_TYPE=="WTELEC" or MODEL_TYPE=="WT ELECTRICAL")
    {
        WT_ELECTRICAL_MODEL* model = generator->get_wt_electrical_model();
        if(model!=NULL)
            return model->get_model_float_parameter_count();
        else
            return 0;
    }
    if(MODEL_TYPE=="WTTURB" or MODEL_TYPE=="WT TURBINE")
    {
        WT_TURBINE_MODEL* model = generator->get_wt_turbine_model();
        if(model!=NULL)
            return model->get_model_float_parameter_count();
        else
            return 0;
    }
    if(MODEL_TYPE=="WTPITCH" or MODEL_TYPE=="WT PITCH")
    {
        WT_PITCH_MODEL* model = generator->get_wt_pitch_model();
        if(model!=NULL)
            return model->get_model_float_parameter_count();
        else
            return 0;
    }
    if(MODEL_TYPE=="WINDSPD" or MODEL_TYPE=="WIND SPEED")
    {
        WIND_SPEED_MODEL* model = generator->get_wind_speed_model();
        if(model!=NULL)
            return model->get_model_float_parameter_count();
        else
            return 0;
    }
    show_model_type_not_supported_for_device_with_api(MODEL_TYPE, did, __FUNCTION__);
    return 0;
}
const char* api_get_wt_generator_related_model_float_parameter_name(size_t bus, char* identifier, char* model_type, size_t parameter_index, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DEVICE_ID did = get_wt_generator_device_id(bus, identifier);
    PF_DATA& psdb = toolkit.get_database();
    WT_GENERATOR* generator = psdb.get_wt_generator(did);
    string name = "";
    if(generator==NULL)
    {
        show_device_not_exist_with_api(did, __FUNCTION__);
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", name.c_str());
        return toolkit.steps_char_buffer;
    }
    string MODEL_TYPE = string2upper(model_type);
    if(MODEL_TYPE=="WTGEN" or MODEL_TYPE=="WT GENERATOR")
    {
        WT_GENERATOR_MODEL* model = generator->get_wt_generator_model();
        if(model!=NULL)
            name = model->get_model_data_name(parameter_index);
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", name.c_str());
        return toolkit.steps_char_buffer;
    }

    if(MODEL_TYPE=="WTAERD" or MODEL_TYPE=="WT AERODYNAMIC")
    {
        WT_AERODYNAMIC_MODEL* model = generator->get_wt_aerodynamic_model();
        if(model!=NULL)
            name = model->get_model_data_name(parameter_index);
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", name.c_str());
        return toolkit.steps_char_buffer;
    }
    if(MODEL_TYPE=="WTELEC" or MODEL_TYPE=="WT ELECTRICAL")
    {
        WT_ELECTRICAL_MODEL* model = generator->get_wt_electrical_model();
        if(model!=NULL)
            name = model->get_model_data_name(parameter_index);
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", name.c_str());
        return toolkit.steps_char_buffer;
    }
    if(MODEL_TYPE=="WTTURB" or MODEL_TYPE=="WT TURBINE")
    {
        WT_TURBINE_MODEL* model = generator->get_wt_turbine_model();
        if(model!=NULL)
            name = model->get_model_data_name(parameter_index);
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", name.c_str());
        return toolkit.steps_char_buffer;
    }
    if(MODEL_TYPE=="WTPITCH" or MODEL_TYPE=="WT PITCH")
    {
        WT_PITCH_MODEL* model = generator->get_wt_pitch_model();
        if(model!=NULL)
            name = model->get_model_data_name(parameter_index);
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", name.c_str());
        return toolkit.steps_char_buffer;
    }
    if(MODEL_TYPE=="WINDSPD" or MODEL_TYPE=="WIND SPEED")
    {
        WIND_SPEED_MODEL* model = generator->get_wind_speed_model();
        if(model!=NULL)
            name = model->get_model_data_name(parameter_index);
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", name.c_str());
        return toolkit.steps_char_buffer;
    }
    show_model_type_not_supported_for_device_with_api(MODEL_TYPE, did, __FUNCTION__);
    snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", name.c_str());
    return toolkit.steps_char_buffer;
}
