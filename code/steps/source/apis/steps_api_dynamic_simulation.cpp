#include "header/apis/steps_api.h"
#include "header/basic/utility.h"
#include "header/toolkit/cct_searcher.h"
#include "header/steps_namespace.h"
#include <iostream>

using namespace std;

size_t api_get_dynamic_simulator_integer_parameter(char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME=="MAX_DAE_ITER" or PARAMETER_NAME=="MAX DAE ITERATION")
        return ds.get_max_DAE_iteration();
    if(PARAMETER_NAME=="MIN_DAE_ITER" or PARAMETER_NAME=="MIN DAE ITERATION")
        return ds.get_min_DAE_iteration();
    if(PARAMETER_NAME=="MAX_NET_ITER" or PARAMETER_NAME=="MAX NETWORK ITERATION")
        return ds.get_max_network_iteration();
    if(PARAMETER_NAME=="MAX_UPDATE_ITER" or PARAMETER_NAME=="MAX UPDATE ITERATION")
        return ds.get_max_update_iteration();
    if(PARAMETER_NAME=="MAX_EVENT_UPDATE_ITER" or PARAMETER_NAME=="MAX EVENT UPDATE ITERATION")
        return ds.get_max_event_update_iteration();
    if(PARAMETER_NAME=="MAX_NET_DIVERGENT_THRESHOLD" or PARAMETER_NAME=="MAX NETWORK DIVERGENT THRESHOLD")
        return ds.get_max_network_solution_divergent_threshold();

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for dynamic simulator with api %s.\n"
             "0 will be returned.", PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
    return 0;
}

void api_set_dynamic_simulator_integer_parameter(char* parameter_name, int value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME=="MAX_DAE_ITER" or PARAMETER_NAME=="MAX DAE ITERATION")
    {
        ds.set_max_DAE_iteration(value);
        return;
    }
    if(PARAMETER_NAME=="MIN_DAE_ITER" or PARAMETER_NAME=="MIN DAE ITERATION")
    {
        ds.set_min_DAE_iteration(value);
        return;
    }
    if(PARAMETER_NAME=="MAX_NET_ITER" or PARAMETER_NAME=="MAX NETWORK ITERATION")
    {
        ds.set_max_network_iteration(value);
        return;
    }
    if(PARAMETER_NAME=="MAX_UPDATE_ITER" or PARAMETER_NAME=="MAX UPDATE ITERATION")
    {
        ds.set_max_update_iteration(value);
        return;
    }
    if(PARAMETER_NAME=="MAX_EVENT_UPDATE_ITER" or PARAMETER_NAME=="MAX EVENT UPDATE ITERATION")
    {
        ds.set_max_event_update_iteration(value);
        return;
    }
    if(PARAMETER_NAME=="MAX_NET_DIVERGENT_THRESHOLD" or PARAMETER_NAME=="MAX NETWORK DIVERGENT THRESHOLD")
    {
        ds.set_max_network_solution_divergent_threshold(value);
        return;
    }

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for dynamic simulator with api %s.\n",
             PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
}

double api_get_dynamic_simulator_float_parameter(char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME=="MAX POWER IMBALANCE IN MVA")
        return ds.get_allowed_max_power_imbalance_in_MVA();
    if(PARAMETER_NAME=="ITERATION ACCELERATOR")
        return ds.get_iteration_accelerator();
    if(PARAMETER_NAME=="ANGLE STABILITY THRESHOLD IN DEG")
        return ds.get_rotor_angle_stability_threshold_in_deg();
    if(PARAMETER_NAME=="DELT THRESHOLD FOR LEADING PART OF BUS FREQ MODEL IN S")
        return ds.get_time_step_threshold_when_leading_part_of_bus_frequency_model_is_enabled_in_s();
    if(PARAMETER_NAME=="K FOR LEADING PART OF BUS FREQ MODEL")
        return ds.get_k_for_leading_part_of_bus_frequency_model();


    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for dynamic simulator with api %s.\n"
             "0 will be returned.", PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
    return 0.0;
}

void api_set_dynamic_simulator_float_parameter(char* parameter_name, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME=="MAX POWER IMBALANCE IN MVA")
    {
        ds.set_allowed_max_power_imbalance_in_MVA(value);
        return;
    }
    if(PARAMETER_NAME=="ITERATION ACCELERATOR")
    {
        ds.set_iteration_accelerator(value);
        return;
    }
    if(PARAMETER_NAME=="ANGLE STABILITY THRESHOLD IN DEG")
    {
        ds.set_rotor_angle_stability_threshold_in_deg(value);
        return;
    }
    if(PARAMETER_NAME=="DELT THRESHOLD FOR LEADING PART OF BUS FREQ MODEL IN S")
    {
        ds.set_time_step_threshold_when_leading_part_of_bus_frequency_model_is_enabled_in_s(value);
        return;
    }
    if(PARAMETER_NAME=="K FOR LEADING PART OF BUS FREQ MODEL")
    {
        ds.set_k_for_leading_part_of_bus_frequency_model(value);
        return;
    }
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for dynamic simulator with api %s.\n",
             PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
}

const char* api_get_dynamic_simulator_string_parameter(char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();

	snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", "");
    string PARAMETER_NAME = string2upper(parameter_name);
    if (PARAMETER_NAME == "OUTPUT FILENAME")
    {
        snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", (ds.get_output_file()).c_str());
        return toolkit.steps_char_buffer;
    }

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for dynamic simulator with api %s.\n"
             "0 will be returned.", PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
    return toolkit.steps_char_buffer;
}

void api_set_dynamic_simulator_string_parameter(char* parameter_name, char* value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME=="OUTPUT FILENAME")
    {
        ostringstream osstream;
        //osstream<<"Output file is to be set as : "<<value<<endl;
        ds.set_output_file(value);
        //osstream<<"Output file is set as : "<<ds.get_output_file()<<endl;
        //toolkit.show_information(osstream);
        return;
    }
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for dynamic simulator with api %s.\n",
             PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
}

bool api_get_dynamic_simulator_boolean_parameter(char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME=="ANGLE STABILITY SURVEILLANCE LOGIC")
        return ds.get_rotor_angle_stability_surveillance_flag();
    if(PARAMETER_NAME=="NON DIVERGENT SOLUTION LOGIC")
        return ds.get_non_divergent_solution_logic();
    if(PARAMETER_NAME=="AUTOMATIC ACCELERATOR TUNE LOGIC")
        return ds.get_automatic_iteration_accelerator_tune_logic();
    if(PARAMETER_NAME=="BIN EXPORT LOGIC")
        return ds.is_bin_file_export_enabled();
    if(PARAMETER_NAME=="CSV EXPORT LOGIC")
        return ds.is_csv_file_export_enabled();
    if(PARAMETER_NAME=="JSON EXPORT LOGIC")
        return ds.is_json_file_export_enabled();

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for dynamic simulator with api %s.\n"
             "0 will be returned.", PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
    return false;
}
void api_set_dynamic_simulator_boolean_parameter(char* parameter_name, bool value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME=="ANGLE STABILITY SURVEILLANCE LOGIC")
    {
        ds.set_rotor_angle_stability_surveillance_flag(value);
        return;
    }
    if(PARAMETER_NAME=="NON DIVERGENT SOLUTION LOGIC")
    {
        ds.set_non_divergent_solution_logic(value);
        return;
    }
    if(PARAMETER_NAME=="AUTOMATIC ACCELERATOR TUNE LOGIC")
    {
        ds.set_automatic_iteration_accelerator_tune_logic(value);
        return;
    }
    if(PARAMETER_NAME=="BIN EXPORT LOGIC")
    {
        ds.set_bin_file_export_enable_flag(value);
        return;
    }
    if(PARAMETER_NAME=="CSV EXPORT LOGIC")
    {
        ds.set_csv_file_export_enable_flag(value);
        return;
    }
    if(PARAMETER_NAME=="JSON EXPORT LOGIC")
    {
        ds.set_json_file_export_enable_flag(value);
        return;
    }
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for dynamic simulator with api %s.\n",
             PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
}

void api_set_dynamic_simulator_output_file(char* file, size_t toolkit_index)
{
    string parameter_name = "OUTPUT FILENAME";
    api_set_dynamic_simulator_string_parameter((char*)parameter_name.c_str(), file, toolkit_index);
}

const char* api_get_dynamic_simulator_output_file(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();

	snprintf(toolkit.steps_char_buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s", ds.get_output_file().c_str());
	return toolkit.steps_char_buffer;
}

void api_set_dynamic_simulation_time_step(double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    toolkit.set_dynamic_simulation_time_step_in_s(value);
}

double api_get_dynamic_simulation_time_step(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    return toolkit.get_dynamic_simulation_time_step_in_s();
}

double api_get_dynamic_simulation_time(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    return toolkit.get_dynamic_simulation_time_in_s();
}

void api_show_dynamic_simulation_configuration(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.show_dynamic_simulator_configuration();
}

void api_clear_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.clear_meters();
}

void api_prepare_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_meters();
}

void api_prepare_bus_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_bus_related_meters();
}

void api_prepare_generator_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_generator_related_meters();
}

void api_prepare_wt_generator_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_wt_generator_related_meters();
}

void api_prepare_pv_unit_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_pv_unit_related_meters();
}

void api_prepare_energy_storage_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_energy_storage_related_meters();
}

void api_prepare_load_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_load_related_meters();
}

void api_prepare_line_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_ac_line_related_meters();
}

void api_prepare_transformer_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_transformer_related_meters();
}

void api_prepare_hvdc_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_2t_lcc_hvdc_related_meters();
}

void api_prepare_vsc_hvdc_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_vsc_hvdc_related_meters();
}

void api_prepare_equivalent_device_related_meters(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_equivalent_device_related_meters();
}

void api_prepare_bus_related_meter(size_t bus, char* meter_type, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_bus_related_meter(bus, meter_type);
}

void api_prepare_generator_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, id);
    ds.prepare_generator_related_meter(did, meter_type, var_name);
}

void api_prepare_wt_generator_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_wt_generator_device_id(bus, id);
    ds.prepare_wt_generator_related_meter(did, meter_type, var_name);
}

void api_prepare_pv_unit_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_pv_unit_device_id(bus, id);
    ds.prepare_pv_unit_related_meter(did, meter_type, var_name);
}

void api_prepare_energy_storage_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_energy_storage_device_id(bus, id);
    ds.prepare_energy_storage_related_meter(did, meter_type, var_name);
}

void api_prepare_load_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_load_device_id(bus, id);
    ds.prepare_load_related_meter(did, meter_type, var_name);
}

void api_prepare_line_related_meter(size_t ibus, size_t jbus, char* id, char* meter_type, char* side, char* var_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_ac_line_device_id(ibus, jbus, id);
    ds.prepare_ac_line_related_meter(did, meter_type, side, var_name);
}

void api_prepare_transformer_related_meter(size_t ibus, size_t jbus, size_t kbus, char* id, char* meter_type, char* side, char* var_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_transformer_device_id(ibus, jbus, kbus, id);
    ds.prepare_transformer_related_meter(did, meter_type, side, var_name);
}

void api_prepare_hvdc_related_meter(size_t ibus, size_t jbus, char* id, char* meter_type, char* side, char* var_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_2t_lcc_hvdc_device_id(ibus, jbus, id);
    ds.prepare_2t_lcc_hvdc_related_meter(did, meter_type, side, var_name);
}

void api_prepare_vsc_hvdc_related_meter(char* vsc_name, char* meter_type, size_t side, char* var_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.prepare_vsc_hvdc_related_meter(vsc_name, meter_type, side, var_name);
}

void api_prepare_equivalent_device_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_equivalent_device_id(bus, id);
    ds.prepare_equivalent_device_related_meter(did, meter_type, var_name);
}

void api_start_dynamic_simulation(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.start();
}

void api_stop_dynamic_simulation(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.stop();
}

void api_run_simulation_to_time(double t_end, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.run_to(t_end);
}

void api_run_a_step(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.run_a_step();
}

bool api_get_system_angular_stable_flag(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    return ds.get_system_angular_stable_flag();
}

double api_get_basic_meter_value(const char* meter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    return ds.get_basic_meter_value(meter_name);
}

double api_get_user_meter_value(int index, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    return ds.get_user_meter_value(index);
}

void api_change_dynamic_simulation_time_step(double delt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    return ds.change_dynamic_simulator_time_step_in_s(delt);

}

void api_set_bus_fault(size_t bus, char* fault_type, double fault_G, double fault_B, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    string string_fault_type = string2upper(fault_type);
    if(string_fault_type=="THREE PHASE FAULT")
    {
        ds.set_bus_fault(bus, complex<double>(fault_G, fault_B));
        return;
    }
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Fault type '%s' is not supported for dynamic simulator with api %s.",
             string_fault_type.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
}

void api_clear_bus_fault(size_t bus, char* fault_type, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    string string_fault_type = string2upper(fault_type);
    if(string_fault_type=="THREE PHASE FAULT")
    {
        ds.clear_bus_fault(bus);
        return;
    }
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Fault type '%s' is not supported for dynamic simulator with api %s.",
             string_fault_type.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
}

void api_trip_bus(size_t bus, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    ds.trip_bus(bus);
}

void api_set_line_fault(size_t ibus, size_t jbus, char* identifier, char* fault_type, double fault_location, double fault_G, double fault_B, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_ac_line_device_id(ibus, jbus, identifier);

    if(not psdb.is_ac_line_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    string string_fault_type = string2upper(fault_type);
    if(string_fault_type=="THREE PHASE FAULT")
    {
        ds.set_ac_line_fault(did, ibus, fault_location, complex<double>(fault_G, fault_B));
        return;
    }
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Fault type '%s' is not supported for dynamic simulator with api %s.",
             string_fault_type.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
}

void api_clear_line_fault(size_t ibus, size_t jbus, char* identifier, char* fault_type, double fault_location, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_ac_line_device_id(ibus, jbus, identifier);

    if(not psdb.is_ac_line_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    string string_fault_type = string2upper(fault_type);
    if(string_fault_type=="THREE PHASE FAULT")
    {
        ds.clear_ac_line_fault(did, ibus, fault_location);
        return;
    }
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Fault type '%s' is not supported for dynamic simulator with api %s.",
             string_fault_type.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
}

void api_trip_line(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_ac_line_device_id(ibus, jbus, identifier);

    if(not psdb.is_ac_line_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.trip_ac_line(did);
}

void api_trip_line_breaker(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_ac_line_device_id(ibus, jbus, identifier);

    if(not psdb.is_ac_line_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.trip_ac_line_breaker(did, ibus);
}

void api_close_line(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_ac_line_device_id(ibus, jbus, identifier);

    if(not psdb.is_ac_line_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.close_ac_line(did);
}

void api_close_line_breaker(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_ac_line_device_id(ibus, jbus, identifier);

    if(not psdb.is_ac_line_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.close_ac_line_breaker(did, ibus);
}

void api_trip_transformer(size_t ibus, size_t jbus, size_t kbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_transformer_device_id(ibus, jbus, kbus, identifier);

    if(not psdb.is_transformer_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.trip_transformer(did);
}

void api_trip_transformer_breaker(size_t ibus, size_t jbus, size_t kbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_transformer_device_id(ibus, jbus, kbus, identifier);

    if(not psdb.is_transformer_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.trip_transformer_breaker(did, ibus);
}

void api_close_transformer(size_t ibus, size_t jbus, size_t kbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_transformer_device_id(ibus, jbus, kbus, identifier);

    if(not psdb.is_transformer_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.close_transformer(did);
}

void api_close_transformer_breaker(size_t ibus, size_t jbus, size_t kbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_transformer_device_id(ibus, jbus, kbus, identifier);

    if(not psdb.is_transformer_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.close_transformer_breaker(did, ibus);
}

void api_trip_generator(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_generator_device_id(bus, identifier);

    if(not psdb.is_generator_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.trip_generator(did);
}

void api_shed_generator(size_t bus, char* identifier, double percent, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_generator_device_id(bus, identifier);

    if(not psdb.is_generator_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.shed_generator(did, percent);
}

void api_trip_wt_generator(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_wt_generator_device_id(bus, identifier);

    if(not psdb.is_wt_generator_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.trip_wt_generator(did);
}

void api_shed_wt_generator(size_t bus, char* identifier, double percent, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_wt_generator_device_id(bus, identifier);

    if(not psdb.is_wt_generator_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.shed_wt_generator(did, percent);
}

void api_trip_load(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_load_device_id(bus, identifier);

    if(not psdb.is_load_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.trip_load(did);
}

void api_close_load(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_load_device_id(bus, identifier);

    if(not psdb.is_load_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.close_load(did);
}

void api_scale_load(size_t bus, char* identifier, double percent, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_load_device_id(bus, identifier);

    if(not psdb.is_load_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.scale_load(did, percent);
}

void api_scale_all_loads(double percent, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();

    ds.scale_all_load(percent);
}

void api_trip_fixed_shunt(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_fixed_shunt_device_id(bus, identifier);

    if(not psdb.is_fixed_shunt_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.trip_fixed_shunt(did);
}

void api_close_fixed_shunt(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_fixed_shunt_device_id(bus, identifier);

    if(not psdb.is_load_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.close_fixed_shunt(did);
}

void api_manually_bypass_hvdc(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_2t_lcc_hvdc_device_id(ibus, jbus, identifier);

    if(not psdb.is_2t_lcc_hvdc_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.manual_bypass_2t_lcc_hvdc(did);
}

void api_manually_unbypass_hvdc(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_2t_lcc_hvdc_device_id(ibus, jbus, identifier);

    if(not psdb.is_2t_lcc_hvdc_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    ds.manual_unbypass_2t_lcc_hvdc(did);
}

void api_manually_block_hvdc(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_2t_lcc_hvdc_device_id(ibus, jbus, identifier);

    if(not psdb.is_2t_lcc_hvdc_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }
    ds.manual_block_2t_lcc_hvdc(did);
}

void api_manually_unblock_hvdc(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_2t_lcc_hvdc_device_id(ibus, jbus, identifier);

    if(not psdb.is_2t_lcc_hvdc_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for dynamic simulator with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }
    ds.manual_unblock_2t_lcc_hvdc(did);
}

double api_get_generator_voltage_reference_in_pu(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    return ds.get_generator_voltage_reference_in_pu(did);
}

double api_get_generator_mechanical_power_reference_in_pu_based_on_mbase(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    return ds.get_generator_mechanical_power_reference_in_pu_based_on_mbase(did);
}

double api_get_generator_mechanical_power_reference_in_MW(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    return ds.get_generator_mechanical_power_reference_in_MW(did);
}

void api_set_generator_voltage_reference_in_pu(size_t bus, char* identifier, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    ds.change_generator_voltage_reference_in_pu(did, value);
}

void api_set_generator_mechanical_power_reference_in_pu_based_on_mbase(size_t bus, char* identifier, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    ds.change_generator_mechanical_power_reference_in_pu_based_on_mbase(did, value);
}

void api_set_generator_mechanical_power_reference_in_MW(size_t bus, char* identifier, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    ds.change_generator_mechanical_power_reference_in_MW(did, value);
}

double api_get_generator_excitation_voltage_in_pu(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    return ds.get_generator_excitation_voltage_in_pu(did);
}

double api_get_generator_mechanical_power_in_pu_based_on_mbase(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    return ds.get_generator_mechanical_power_in_pu_based_on_mbase(did);
}

double api_get_generator_mechanical_power_in_MW(size_t bus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    return ds.get_generator_mechanical_power_in_MW(did);
}

void api_set_generator_excitation_voltage_in_pu(size_t bus, char* identifier, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    ds.change_generator_excitation_voltage_in_pu(did, value);
}

void api_set_generator_mechanical_power_in_pu_based_on_mbase(size_t bus, char* identifier, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    ds.change_generator_mechanical_power_in_pu_based_on_mbase(did, value);
}

void api_set_generator_mechanical_power_in_MW(size_t bus, char* identifier, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_generator_device_id(bus, identifier);
    ds.change_generator_mechanical_power_in_MW(did, value);
}

double api_get_hvdc_power_order_in_MW(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_2t_lcc_hvdc_device_id(ibus, jbus, identifier);
    return ds.get_2t_lcc_hvdc_power_order_in_MW(did);
}

void api_set_hvdc_power_order_in_MW(size_t ibus, size_t jbus, char* identifier, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DEVICE_ID did = get_2t_lcc_hvdc_device_id(ibus, jbus, identifier);
    ds.change_2t_lcc_hvdc_power_order_in_MW(did, value);
}


void api_set_vsc_hvdc_dc_line_fault(char* vsc_name, size_t ibus, size_t jbus, char* identifier, double fault_location, double fault_R, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DC_DEVICE_ID did = get_dc_line_device_id(ibus, jbus, identifier);
    ds.set_vsc_hvdc_line_fault(vsc_name, did, ibus, fault_location, fault_R);
}

void api_clear_vsc_hvdc_dc_line_fault(char* vsc_name, size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DC_DEVICE_ID did = get_dc_line_device_id(ibus, jbus, identifier);
    ds.clear_vsc_hvdc_line_fault(vsc_name, did);
}

void api_trip_vsc_hvdc_dc_line(char* vsc_name, size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DC_DEVICE_ID did = get_dc_line_device_id(ibus, jbus, identifier);
    ds.trip_vsc_hvdc_line(vsc_name, did);
}

void api_close_vsc_hvdc_dc_line(char* vsc_name, size_t ibus, size_t jbus, char* identifier, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_SIMULATOR& ds = toolkit.get_dynamic_simulator();
    DC_DEVICE_ID did = get_dc_line_device_id(ibus, jbus, identifier);
    ds.close_vsc_hvdc_line(vsc_name, did);
}

double api_search_cct(char* pf_file, char* dy_file, size_t ibus, size_t jbus, char* id, size_t sidebus, size_t trip_line, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    ostringstream osstream;

    toolkit.set_dynamic_simulation_time_step_in_s(0.01);

    CCT_SEARCHER searcher;
    searcher.set_power_system_database_maximum_bus_number(10000);
    searcher.set_search_title("");
    searcher.set_powerflow_data_filename(pf_file);
    searcher.set_dynamic_data_filename(dy_file);

    DEVICE_ID did;
    TERMINAL terminal;
    terminal.append_bus(ibus);
    terminal.append_bus(jbus);
    did.set_device_type(STEPS_AC_LINE);
    did.set_device_terminal(terminal);
    did.set_device_identifier_index(get_index_of_string(id));

    searcher.set_fault_device(did);
    searcher.set_fault_side_bus(sidebus);
    searcher.set_fault_location_to_fault_side_bus_in_pu(0.0);
    searcher.set_fault_shunt_in_pu(complex<double>(0.0, -2e8));
    if(trip_line==0)
        searcher.set_flag_trip_ac_line_after_clearing_fault(false);
    else
        searcher.set_flag_trip_ac_line_after_clearing_fault(true);


    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Now go searching CCT for fault at side %zu of %s.",
             searcher.get_fault_side_bus(), (did.get_compound_device_name()).c_str());
    toolkit.show_information(buffer);
    double cct = searcher.search_cct();
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Now done searching CCT for fault at side %zu of %s.",
             searcher.get_fault_side_bus(), (did.get_compound_device_name()).c_str());
    toolkit.show_information(buffer);

    searcher.run_case_with_clearing_time(cct);
    searcher.run_case_with_clearing_time(cct+0.1);
    return cct;
}
