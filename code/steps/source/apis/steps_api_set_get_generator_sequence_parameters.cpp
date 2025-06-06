#include "header/apis/steps_api.h"
#include "header/apis/steps_api_common.h"
#include "header/basic/utility.h"
#include "header/steps_namespace.h"
#include "header/data_imexporter/psse_imexporter.h"
#include "header/data_imexporter/bpa_imexporter.h"


double api_get_generator_sequence_float_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    PF_DATA& psdb = toolkit.get_database();
    GENERATOR* generator = psdb.get_generator(did);

    if(generator==NULL)
    {
        show_device_not_exist_with_api(did, __FUNCTION__);
        return 0.0;
    }

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME == "RPOS_PU" or "POSITIVE SEQUENCE RESISTANCE IN PU")
        return generator->get_positive_sequence_resistance_in_pu();
    if(PARAMETER_NAME == "XPPDV_PU" or "POSITIVE SEQUENCE SUBTRANSIENT REACTANCE IN PU")
        return generator->get_positive_sequence_subtransient_reactance_in_pu();
    if(PARAMETER_NAME == "XPDV_PU" or "POSITIVE SEQUENCE TRANSIENT REACTANCE IN PU")
        return generator->get_positive_sequence_transient_reactance_in_pu();
    if(PARAMETER_NAME == "XSDV_PU" or "POSITIVE SEQUENCE SYNCHRONOUS REACTANCE IN PU")
        return generator->get_positive_sequence_syncronous_reactance_in_pu();
    if(PARAMETER_NAME == "RNEG_PU" or "NEGATIVE SEQUENCE RESISTANCE IN PU")
        return generator->get_negative_sequence_resistance_in_pu();
    if(PARAMETER_NAME == "XNEG_PU" or "NEGATIVE SEQUENCE REACTANCE IN PU")
        return generator->get_negative_sequence_reactance_in_pu();
    if(PARAMETER_NAME == "RZERO_PU" or "ZERO SEQUENCE RESISTANCE IN PU")
        return generator->get_zero_sequence_resistance_in_pu();
    if(PARAMETER_NAME == "XZERO_PU" or "ZERO SEQUENCE REACTANCE IN PU")
        return generator->get_zero_sequence_reactance_in_pu();
    if(PARAMETER_NAME == "RG_PU" or "GROUNDING RESISTANCE")
        return generator->get_grounding_resistance_in_pu();
    if(PARAMETER_NAME == "XG_PU" or "GROUNDING REACTANCE")
        return generator->get_grounding_reactance_in_pu();

    if(PARAMETER_NAME=="I1_PU" or "POSITIVE SEQUENCE CURRENT IN PU")
        return abs(generator->get_positive_sequence_complex_current_in_pu());
    if(PARAMETER_NAME=="I2_PU" or "NEGATIVE SEQUENCE CURRENT IN PU")
        return abs(generator->get_negative_sequence_complex_current_in_pu());
    if(PARAMETER_NAME=="I0_PU" or "ZERO SEQUENCE CURRENT IN PU")
        return abs(generator->get_zero_sequence_complex_current_in_pu());
    if(PARAMETER_NAME=="I1_KA" or "POSITIVE SEQUENCE CURRENT IN KA")
        return abs(generator->get_positive_sequence_complex_current_in_kA());
    if(PARAMETER_NAME=="I2_KA" or "NEGATIVE SEQUENCE CURRENT IN KA")
        return abs(generator->get_negative_sequence_complex_current_in_kA());
    if(PARAMETER_NAME=="I0_KA" or "ZERO SEQUENCE CURRENT IN KA")
        return abs(generator->get_zero_sequence_complex_current_in_kA());

    if(PARAMETER_NAME=="ANGLE_RAD" or PARAMETER_NAME=="ANGLE IN RAD" or PARAMETER_NAME=="POSITIVE ANGLE IN RAD")
        return arg(generator->get_positive_sequence_complex_current_in_pu());
    if(PARAMETER_NAME=="ANGLE_DEG" or PARAMETER_NAME=="ANGLE IN DEG" or PARAMETER_NAME=="POSITIVE ANGLE IN DEG")
        return rad2deg(arg(generator->get_positive_sequence_complex_current_in_pu()));
    if(PARAMETER_NAME=="NEGATIVE_ANGLE_RAD" or PARAMETER_NAME=="NEGATIVE ANGLE IN RAD")
        return arg(generator->get_negative_sequence_complex_current_in_pu());
    if(PARAMETER_NAME=="NEGATIVE_ANGLE_DEG" or PARAMETER_NAME=="NEGATIVE ANGLE IN DEG")
        return rad2deg(arg(generator->get_negative_sequence_complex_current_in_pu()));
    if(PARAMETER_NAME=="ZERO_ANGLE_RAD" or PARAMETER_NAME=="ZERO ANGLE IN RAD")
        return arg(generator->get_zero_sequence_complex_current_in_pu());
    if(PARAMETER_NAME=="ZERO_ANGLE_DEG" or PARAMETER_NAME=="ZERO ANGLE IN DEG")
        return rad2deg(arg(generator->get_zero_sequence_complex_current_in_pu()));

    show_parameter_not_supported_for_device_with_api(PARAMETER_NAME, did, __FUNCTION__);

    return 0.0; //Hengxu: PARAMETER_NAME not containing the given info

}
void api_set_generator_sequence_float_data(size_t bus, char* identifier, char* parameter_name, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    PF_DATA& psdb = toolkit.get_database();
    GENERATOR* generator = psdb.get_generator(did);

    if(generator==NULL)
    {
        show_device_not_exist_with_api(did, __FUNCTION__);
        return;
    }

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME == "RPOS_PU" or "POSITIVE SEQUENCE RESISTANCE IN PU")
        return generator->set_positive_sequence_resistance_in_pu(value);
    if(PARAMETER_NAME == "XPPDV_PU" or "POSITIVE SEQUENCE SUBTRANSIENT REACTANCE IN PU")
        return generator->set_positive_sequence_subtransient_reactance_in_pu(value);
    if(PARAMETER_NAME == "XPDV_PU" or "POSITIVE SEQUENCE TRANSIENT REACTANCE IN PU")
        return generator->set_positive_sequence_transient_reactance_in_pu(value);
    if(PARAMETER_NAME == "XSDV_PU" or "POSITIVE SEQUENCE SYNCHRONOUS REACTANCE IN PU")
        return generator->set_positive_sequence_syncronous_reactance_in_pu(value);
    if(PARAMETER_NAME == "RNEG_PU" or "NEGATIVE SEQUENCE RESISTANCE IN PU")
        return generator->set_negative_sequence_resistance_in_pu(value);
    if(PARAMETER_NAME == "XNEG_PU" or "NEGATIVE SEQUENCE REACTANCE IN PU")
        return generator->set_negative_sequence_reactance_in_pu(value);
    if(PARAMETER_NAME == "RZERO_PU" or "ZERO SEQUENCE RESISTANCE IN PU")
        return generator->set_zero_sequence_resistance_in_pu(value);
    if(PARAMETER_NAME == "XZERO_PU" or "ZERO SEQUENCE REACTANCE IN PU")
        return generator->set_zero_sequence_reactance_in_pu(value);
    if(PARAMETER_NAME == "RG_PU" or "GROUNDING RESISTANCE")
        return generator->set_grounding_resistance_in_pu(value);
    if(PARAMETER_NAME == "XG_PU" or "GROUNDING REACTANCE")
        return generator->set_grounding_reactance_in_pu(value);

    show_parameter_not_supported_for_device_with_api(PARAMETER_NAME, did, __FUNCTION__);
}
