#include "header/apis/steps_api.h"
#include "header/apis/steps_api_common.h"
#include "header/basic/utility.h"
#include "header/steps_namespace.h"
#include "header/data_imexporter/psse_imexporter.h"
#include "header/data_imexporter/bpa_imexporter.h"

const char* api_get_vsc_hvdc_related_model_name(char* name, char* model_type, size_t converter_index, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
	snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", "");

    PF_DATA& psdb = toolkit.get_database();
    VSC_HVDC* vsc_hvdc = psdb.get_vsc_hvdc(name);
    if(vsc_hvdc==NULL)
    {
        show_vsc_hvdc_not_exist_with_api(name, __FUNCTION__);
        return toolkit.steps_char_buffer;
    }
    DEVICE_ID did = vsc_hvdc->get_device_id();

    string MODEL_TYPE = string2upper(model_type);
    if(MODEL_TYPE=="PROJECT" or MODEL_TYPE=="VSC LCC_HVDC2T PROJECT")
    {
        VSC_HVDC_NETWORK_MODEL* model = vsc_hvdc->get_vsc_hvdc_network_model();
        if(model!=NULL)
			snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", (model->get_model_name()).c_str());
		return toolkit.steps_char_buffer;;
    }
    if(MODEL_TYPE=="CONVERTER" or MODEL_TYPE=="VSC LCC_HVDC2T CONVERTER")
    {
        VSC_HVDC_CONVERTER_MODEL* model = vsc_hvdc->get_vsc_hvdc_converter_model(converter_index);
        if(model!=NULL)
			snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", (model->get_model_name()).c_str());
		return toolkit.steps_char_buffer;;
    }
    show_model_type_not_supported_for_device_with_api(MODEL_TYPE, did, __FUNCTION__);
    return toolkit.steps_char_buffer;
}

double api_get_vsc_hvdc_related_model_float_parameter(char* name, char* model_type, char* parameter_name, size_t converter_index, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    VSC_HVDC* vsc_hvdc = psdb.get_vsc_hvdc(name);
    if(vsc_hvdc==NULL)
    {
        show_vsc_hvdc_not_exist_with_api(name, __FUNCTION__);
        return 0.0;
    }
    DEVICE_ID did = vsc_hvdc->get_device_id();

    string MODEL_TYPE = string2upper(model_type);
    string PARAMETER_NAME = string2upper(parameter_name);
    if(MODEL_TYPE=="PROJECT" or MODEL_TYPE=="VSC LCC_HVDC2T PROJECT")
    {
        VSC_HVDC_NETWORK_MODEL* model = vsc_hvdc->get_vsc_hvdc_network_model();
        if(model!=NULL)
            return model->get_model_data_with_name(PARAMETER_NAME);
        else
            return 0.0;
    }
    if(MODEL_TYPE=="CONVERTER" or MODEL_TYPE=="VSC LCC_HVDC2T CONVERTER")
    {
        VSC_HVDC_CONVERTER_MODEL* model = vsc_hvdc->get_vsc_hvdc_converter_model(converter_index);
        if(model!=NULL)
        {
            return model->get_model_data_with_name(PARAMETER_NAME);
        }

        else
            return 0.0;
    }
    show_model_type_not_supported_for_device_with_api(MODEL_TYPE, did, __FUNCTION__);
    return 0.0;
}

void api_set_vsc_hvdc_related_model_float_parameter(char* name, char* model_type, char* parameter_name, double value, size_t converter_index, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    VSC_HVDC* vsc_hvdc = psdb.get_vsc_hvdc(name);
    if(vsc_hvdc==NULL)
    {
        show_vsc_hvdc_not_exist_with_api(name, __FUNCTION__);
        return;
    }
    DEVICE_ID did = vsc_hvdc->get_device_id();

    string MODEL_TYPE = string2upper(model_type);
    string PARAMETER_NAME = string2upper(parameter_name);
    if(MODEL_TYPE=="PROJECT" or MODEL_TYPE=="VSC LCC_HVDC2T PROJECT")
    {
        VSC_HVDC_NETWORK_MODEL* model = vsc_hvdc->get_vsc_hvdc_network_model();
        if(model!=NULL)
            return model->set_model_data_with_name(PARAMETER_NAME, value);
        else
            return;
    }
    if(MODEL_TYPE=="CONVERTER" or MODEL_TYPE=="VSC LCC_HVDC2T CONVERTER")
    {
        VSC_HVDC_CONVERTER_MODEL* model = vsc_hvdc->get_vsc_hvdc_converter_model(converter_index);
        if(model!=NULL)
            return model->set_model_data_with_name(PARAMETER_NAME, value);
        else
            return;
    }
    show_model_type_not_supported_for_device_with_api(MODEL_TYPE, did, __FUNCTION__);
}

size_t api_get_vsc_hvdc_related_model_float_parameter_count(char* name, char* model_type, size_t converter_index, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    VSC_HVDC* vsc_hvdc = psdb.get_vsc_hvdc(name);
    if(vsc_hvdc==NULL)
    {
        show_vsc_hvdc_not_exist_with_api(name, __FUNCTION__);
        return 0;
    }
    DEVICE_ID did = vsc_hvdc->get_device_id();

    string MODEL_TYPE = string2upper(model_type);
    if(MODEL_TYPE=="PROJECT" or MODEL_TYPE=="VSC LCC_HVDC2T PROJECT")
    {
        VSC_HVDC_NETWORK_MODEL* model = vsc_hvdc->get_vsc_hvdc_network_model();
        if(model!=NULL)
            return model->get_model_float_parameter_count();
        else
            return 0;
    }
    if(MODEL_TYPE=="CONVERTER" or MODEL_TYPE=="VSC LCC_HVDC2T CONVERTER")
    {
        VSC_HVDC_CONVERTER_MODEL* model = vsc_hvdc->get_vsc_hvdc_converter_model(converter_index);
        if(model!=NULL)
            return model->get_model_float_parameter_count();
        else
            return 0;
    }
    show_model_type_not_supported_for_device_with_api(MODEL_TYPE, did, __FUNCTION__);
    return 0;
}

const char* api_get_vsc_hvdc_related_model_float_parameter_name(char* name, char* model_type, size_t parameter_index, size_t converter_index, size_t toolkit_index)
{
    string par_name = "";
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    VSC_HVDC* vsc_hvdc = psdb.get_vsc_hvdc(name);
    if(vsc_hvdc==NULL)
    {
        show_vsc_hvdc_not_exist_with_api(name, __FUNCTION__);
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", par_name.c_str());
        return toolkit.steps_char_buffer;
    }
    DEVICE_ID did = vsc_hvdc->get_device_id();

    string MODEL_TYPE = string2upper(model_type);
    if(MODEL_TYPE=="PROJECT" or MODEL_TYPE=="VSC LCC_HVDC2T PROJECT")
    {
        VSC_HVDC_NETWORK_MODEL* model = vsc_hvdc->get_vsc_hvdc_network_model();
        if(model!=NULL)
            par_name = model->get_model_data_name(parameter_index);
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", par_name.c_str());
        return toolkit.steps_char_buffer;
    }
    if(MODEL_TYPE=="CONVERTER" or MODEL_TYPE=="VSC LCC_HVDC2T CONVERTER")
    {
        VSC_HVDC_CONVERTER_MODEL* model = vsc_hvdc->get_vsc_hvdc_converter_model(converter_index);
        if(model!=NULL)
            par_name = model->get_model_data_name(parameter_index);
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", par_name.c_str());
        return toolkit.steps_char_buffer;
    }
    show_model_type_not_supported_for_device_with_api(MODEL_TYPE, did, __FUNCTION__);
    snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", par_name.c_str());
    return toolkit.steps_char_buffer;
}
