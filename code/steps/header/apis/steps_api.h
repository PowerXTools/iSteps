#ifndef STEPS_API_H
#define STEPS_API_H

#ifdef _MSC_VER
#define EXPORT_STEPS_DLL __declspec(dllexport)
#else
#define EXPORT_STEPS_DLL
#endif

#include <cstdlib>
#include "header/basic/constants.h"
#include "header/basic/dc_device_id.h"

#ifdef  __cplusplus
extern "C" {
#endif

EXPORT_STEPS_DLL size_t api_get_api_major_version();
EXPORT_STEPS_DLL size_t api_get_api_minor_version();
EXPORT_STEPS_DLL size_t api_get_api_patch_version();

EXPORT_STEPS_DLL size_t api_get_const_INDEX_NOT_EXIST();

EXPORT_STEPS_DLL void api_set_toolkit_log_file(char* log_file, bool log_file_append_mode=false, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL size_t api_generate_new_toolkit(char* log_fie=NULL);
EXPORT_STEPS_DLL void api_delete_toolkit(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_initialize_toolkit(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_clear_toolkit(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_set_toolkit_parallel_thread_number(size_t n, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_toolkit_parallel_thread_number(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_set_toolkit_dynamic_model_database_capacity(size_t n, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_toolkit_dynamic_model_database_capacity(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL double api_get_toolkit_float_data(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_toolkit_float_data(char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_toolkit_string_data(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_toolkit_string_data(char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_toolkit_bool_data(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_toolkit_bool_data(char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL size_t api_get_allowed_maximum_bus_number(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_allowed_maximum_bus_number(size_t max_bus, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL size_t api_get_device_capacity(const char* device_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_area_capacity(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_zone_capacity(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_owner_capacity(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_device_capacity(const char* device_type, size_t cap, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_area_capacity(size_t cap, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_zone_capacity(size_t cap, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_owner_capacity(size_t cap, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL void api_load_powerflow_data_from_file(char* file, char* file_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_load_vsc_hvdc_powerflow_data_from_file(char* file, char* file_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_load_powerflow_result_from_file(char* file, char* file_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_powerflow_data_to_file(char* file, char* file_type, bool export_zero_impedance_line=true, bool export_out_of_service_bus=true, bool export_internal_bus_number=false, size_t powerflow_data_save_mode=0, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_vsc_hvdc_powerflow_data_to_file(char* file, char* file_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_load_dynamic_data_from_file(char* file, char* file_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_dynamic_data_to_file(char* file, char* file_type, bool export_internal_bus_number=false, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_load_sequence_data_from_file(char* file, char* file_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_sequence_data_to_file(char* file, char* file_type, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL void api_add_bus(size_t bus_number, char* bus_name, double base_voltage_in_kV, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_generator(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_wt_generator(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_pv_unit(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_load(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_fixed_shunt(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_line(size_t sending_side_bus_number, size_t receiving_side_bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_hvdc(size_t rectifier_bus_number, size_t inverter_bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_vsc_hvdc(char* vsc_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_transformer(size_t primary_side_bus_number, size_t secondary_side_bus_number, size_t tertiary_side_bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_equivalent_device(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_energy_storage(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_area(size_t area_number, char* area_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_zone(size_t zone_number, char* zone_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_add_owner(size_t owner_number, char* owner_name, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL void api_remove_bus(size_t bus_number, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_generator(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_wt_generator(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_pv_unit(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_load(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_fixed_shunt(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_line(size_t sending_side_bus_number, size_t receiving_side_bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_hvdc(size_t rectifier_bus_number, size_t inverter_bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_vsc_hvdc(char* vsc_hvdc, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_transformer(size_t primary_side_bus_number, size_t secondary_side_bus_number, size_t tertiary_side_bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_equivalent_device(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_energy_storage(size_t bus_number, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_area(size_t area_number, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_zone(size_t zone_number, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_remove_owner(size_t owner_number, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_change_bus_number(size_t original_bus_number, size_t new_bus_number, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_change_bus_number_with_file(char* file, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL bool api_is_bus_exist(size_t bus, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_generator_exist(size_t bus, char* ickt, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_wt_generator_exist(size_t bus, char* ickt, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_pv_unit_exist(size_t bus, char* ickt, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_load_exist(size_t bus, char* ickt, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_fixed_shunt_exist(size_t bus, char* ickt, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_line_exist(size_t ibus, size_t jbus, char* ickt, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_transformer_exist(size_t ibus, size_t jbus, size_t kbus, char* ickt, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_hvdc_exist(size_t ibus, size_t jbus, char* ickt, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_vsc_hvdc_exist(char* vsc_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_equivalent_device_exist(size_t bus, char* ickt, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_energy_storage_exist(size_t bus, char* ickt, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL size_t api_get_device_count(const char* device_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_area_count(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_zone_count(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_owner_count(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_in_service_bus_count(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_update_overshadowed_buses(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_all_buses_un_overshadowed(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_overshadowed_bus_count(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_show_device_data(const char* device_type, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL size_t api_bus_name2bus_number(const char* bus_name, double vbase=0.0, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_bus_number2bus_name(size_t bus_number, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL void api_initialize_bus_search(double vbase_kV_min, double vbase_kV_max, double v_pu_min, double v_pu_max, size_t area, size_t zone, size_t owner, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_initialize_all_bus_search(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_current_bus_number(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_goto_next_bus(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_initialize_device_search(const char* device_type, size_t bus, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_current_device_bus_number(const char* device_type, const char* side, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_current_device_identifier(const char* device_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_goto_next_device(const char* device_type, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_initialize_area_search(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_current_area_number(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_goto_next_area(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_initialize_zone_search(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_current_zone_number(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_goto_next_zone(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_initialize_owner_search(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_current_owner_number(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_goto_next_owner(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_bus_integer_data(size_t bus, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_bus_integer_data(size_t bus, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_bus_float_data(size_t bus, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_bus_float_data(size_t bus, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_bus_string_data(size_t bus, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_bus_string_data(size_t bus, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_bus_boolean_data(size_t bus, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_bus_boolean_data(size_t bus, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_source_integer_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_source_integer_data(size_t bus, char* identifier, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_source_float_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_source_float_data(size_t bus, char* identifier, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_source_string_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_source_string_data(size_t bus, char* identifier, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_source_boolean_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_source_boolean_data(size_t bus, char* identifier, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL double api_get_generator_sequence_float_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_generator_sequence_float_data(size_t bus, char* identifier, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_wt_generator_sequence_float_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_wt_generator_sequence_float_data(size_t bus, char* identifier, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_pv_unit_sequence_float_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_pv_unit_sequence_float_data(size_t bus, char* identifier, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL int api_get_load_integer_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_load_integer_data(size_t bus, char* identifier, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_load_float_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_load_float_data(size_t bus, char* identifier, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_load_string_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_load_string_data(size_t bus, char* identifier, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_load_boolean_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_load_boolean_data(size_t bus, char* identifier, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_fixed_shunt_integer_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_fixed_shunt_integer_data(size_t bus, char* identifier, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_fixed_shunt_float_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_fixed_shunt_float_data(size_t bus, char* identifier, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_fixed_shunt_string_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_fixed_shunt_string_data(size_t bus, char* identifier, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_fixed_shunt_boolean_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_fixed_shunt_boolean_data(size_t bus, char* identifier, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_line_integer_data(size_t ibus, size_t jbus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_line_integer_data(size_t ibus, size_t jbus, char* identifier, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_line_float_data(size_t ibus, size_t jbus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_line_float_data(size_t ibus, size_t jbus, char* identifier, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_line_string_data(size_t ibus, size_t jbus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_line_string_data(size_t ibus, size_t jbus, char* identifier, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_line_boolean_data(size_t ibus, size_t jbus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_line_boolean_data(size_t ibus, size_t jbus, char* identifier, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_transformer_integer_data(size_t ibus, size_t jbus, size_t kbus, char* identifier, char* side, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_transformer_integer_data(size_t ibus, size_t jbus, size_t kbus, char* identifier, char* side, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_transformer_float_data(size_t ibus, size_t jbus, size_t kbus, char* identifier, char* side, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_transformer_float_data(size_t ibus, size_t jbus, size_t kbus, char* identifier, char* side, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_transformer_string_data(size_t ibus, size_t jbus, size_t kbus, char* identifier, char* side, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_transformer_string_data(size_t ibus, size_t jbus, size_t kbus, char* identifier, char* side, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_transformer_boolean_data(size_t ibus, size_t jbus, size_t kbus, char* identifier, char* side, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_transformer_boolean_data(size_t ibus, size_t jbus, size_t kbus, char* identifier, char* side, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_hvdc_integer_data(size_t ibus, size_t jbus, char* identifier, char* side, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_hvdc_integer_data(size_t ibus, size_t jbus, char* identifier, char* side, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_hvdc_float_data(size_t ibus, size_t jbus, char* identifier, char* side, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_hvdc_float_data(size_t ibus, size_t jbus, char* identifier, char* side, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_hvdc_string_data(size_t ibus, size_t jbus, char* identifier, char* side, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_hvdc_string_data(size_t ibus, size_t jbus, char* identifier, char* side, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_hvdc_boolean_data(size_t ibus, size_t jbus, char* identifier, char* side, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_hvdc_boolean_data(size_t ibus, size_t jbus, char* identifier, char* side, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_vsc_hvdc_integer_data(char* vsc_name, char* device_type, char* parameter_name, size_t index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_vsc_hvdc_integer_data(char* vsc_name, char* device_type, char* parameter_name, int value, size_t index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_vsc_hvdc_float_data(char* vsc_name, char* device_type, char* parameter_name, size_t index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_vsc_hvdc_float_data(char* vsc_name, char* device_type, char* parameter_name, double value, size_t index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_vsc_hvdc_string_data(char* vsc_name, char* device_type, char* parameter_name, size_t index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_vsc_hvdc_string_data(char* vsc_name, char* device_type, char* parameter_name, char* value, size_t index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_vsc_hvdc_boolean_data(char* vsc_name, char* device_type, char* parameter_name, size_t index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_vsc_hvdc_boolean_data(char* vsc_name, char* device_type, char* parameter_name, bool value, size_t index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_equivalent_device_integer_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_equivalent_device_integer_data(size_t bus, char* identifier, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_equivalent_device_float_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_equivalent_device_float_data(size_t bus, char* identifier, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_equivalent_device_string_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_equivalent_device_string_data(size_t bus, char* identifier, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_equivalent_device_boolean_data(size_t bus, char* identifier, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_equivalent_device_boolean_data(size_t bus, char* identifier, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_area_integer_data(size_t area, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_area_integer_data(size_t area, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_area_float_data(size_t area, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_area_float_data(size_t area, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_area_string_data(size_t area, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_area_string_data(size_t area, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_area_boolean_data(size_t area, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_area_boolean_data(size_t area, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_zone_integer_data(size_t zone, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_zone_integer_data(size_t zone, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_zone_float_data(size_t zone, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_zone_float_data(size_t zone, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_zone_string_data(size_t zone, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_zone_string_data(size_t zone, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_zone_boolean_data(size_t zone, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_zone_boolean_data(size_t zone, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL int api_get_owner_integer_data(size_t owner, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_owner_integer_data(size_t owner, char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_owner_float_data(size_t owner, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_owner_float_data(size_t owner, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_owner_string_data(size_t owner, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_owner_string_data(size_t owner, char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_owner_boolean_data(size_t owner, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_owner_boolean_data(size_t owner, char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_set_dynamic_model(char* model_string, char* file_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_disable_generator_related_model(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_enable_generator_related_model(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL const char* api_get_generator_related_model_name(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_generator_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_generator_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_generator_related_model_float_parameter_count(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_generator_related_model_float_parameter_name(size_t bus, char* identifier, char* model_type, size_t parameter_index, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_generator_related_model_internal_variable_count(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_generator_related_model_internal_variable_with_name(size_t bus, char* identifier, char* model_type, char* variable_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_generator_related_model_internal_variable_with_index(size_t bus, char* identifier, char* model_type, size_t variable_index, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_generator_related_model_internal_variable_name(size_t bus, char* identifier, char* model_type, size_t variable_index, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL const char* api_get_wt_generator_related_model_name(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_wt_generator_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_wt_generator_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_wt_generator_related_model_float_parameter_count(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_wt_generator_related_model_float_parameter_name(size_t bus, char* identifier, char* model_type, size_t parameter_index, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL const char* api_get_pv_unit_related_model_name(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_pv_unit_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_pv_unit_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_pv_unit_related_model_float_parameter_count(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_pv_unit_related_model_float_parameter_name(size_t bus, char* identifier, char* model_type, size_t parameter_index, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL const char* api_get_load_related_model_name(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_load_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_load_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_load_related_model_float_parameter_count(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_load_related_model_float_parameter_name(size_t bus, char* identifier, char* model_type, size_t parameter_index, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL const char* api_get_line_related_model_name(size_t ibus, size_t jbus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_line_related_model_float_parameter(size_t ibus, size_t jbus, char* identifier, char* model_type, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_line_related_model_float_parameter(size_t ibus, size_t jbus, char* identifier, char* model_type, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_line_related_model_float_parameter_count(size_t ibus, size_t jbus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_line_related_model_float_parameter_name(size_t ibus, size_t jbus, char* identifier, char* model_type, size_t parameter_index, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL const char* api_get_hvdc_related_model_name(size_t ibus, size_t jbus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_hvdc_related_model_float_parameter(size_t ibus, size_t jbus, char* identifier, char* model_type, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_hvdc_related_model_float_parameter(size_t ibus, size_t jbus, char* identifier, char* model_type, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_hvdc_related_model_float_parameter_count(size_t ibus, size_t jbus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_hvdc_related_model_float_parameter_name(size_t ibus, size_t jbus, char* identifier, char* model_type, size_t parameter_index, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL const char* api_get_vsc_hvdc_related_model_name(char* name, char* model_type, size_t converter_index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_vsc_hvdc_related_model_float_parameter(char* name, char* model_type, char* parameter_name, size_t converter_index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_vsc_hvdc_related_model_float_parameter(char* name, char* model_type, char* parameter_name, double value, size_t converter_index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_vsc_hvdc_related_model_float_parameter_count(char* name, char* model_type, size_t converter_index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_vsc_hvdc_related_model_float_parameter_name(char* name, char* model_type, size_t parameter_index, size_t converter_index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL const char* api_get_energy_storage_related_model_name(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_energy_storage_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_energy_storage_related_model_float_parameter(size_t bus, char* identifier, char* model_type, char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_energy_storage_related_model_float_parameter_count(size_t bus, char* identifier, char* model_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_energy_storage_related_model_float_parameter_name(size_t bus, char* identifier, char* model_type, size_t parameter_index, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL double api_get_generator_related_model_float_variable(size_t bus, char* identifier, char* model_type, char* variable_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_wt_generator_related_model_float_variable(size_t bus, char* identifier, char* model_type, char* variable_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_pv_unit_related_model_float_variable(size_t bus, char* identifier, char* model_type, char* variable_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_load_related_model_float_variable(size_t bus, char* identifier, char* model_type, char* variable_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_line_related_model_float_variable(size_t ibus, size_t jbus, char* identifier, char* model_type, char* variable_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_hvdc_related_model_float_variable(size_t ibus, size_t jbus, char* identifier, char* model_type, char* variable_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_vsc_hvdc_related_model_float_variable(char* name, char* model_type, char* variable_name, size_t converter_index=INDEX_NOT_EXIST, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_energy_storage_related_model_float_variable(size_t bus, char* identifier, char* model_type, char* variable_name, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL size_t api_get_powerflow_solver_integer_parameter(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_powerflow_solver_integer_parameter(char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_powerflow_solver_float_parameter(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_powerflow_solver_float_parameter(char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_powerflow_solver_boolean_parameter(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_powerflow_solver_boolean_parameter(char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_show_powerflow_solver_configuration(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_solve_powerflow(char* method, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_powerflow_converged(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_is_nan_detected_in_powerflow_solution(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_show_powerflow_result(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_powerflow_result(char* file, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_extended_powerflow_result(char* file, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_jacobian_matrix(char* file, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_build_network_Y_matrix(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_build_decoupled_network_B_matrix(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_build_dc_network_B_matrix(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_build_dynamic_network_Y_matrix(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_build_network_Z_matrix(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_build_sequence_network_Y_matrix(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_save_network_Y_matrix(char* file, bool export_full=false, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_decoupled_network_B_matrix(char* file, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_dc_network_B_matrix(char* file, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_dynamic_network_Y_matrix(char* file, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_network_Z_matrix(char* file, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_positive_sequence_network_Y_matrix(char* file, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_negative_sequence_network_Y_matrix(char* file, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_zero_sequence_network_Y_matrix(char* file, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL void api_set_short_circuit_solver_integer_parameter(char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL size_t api_get_short_circuit_solver_integer_parameter(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_short_circuit_solver_float_parameter(char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_short_circuit_solver_float_parameter(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_short_circuit_solver_boolean_parameter(char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_short_circuit_solver_boolean_parameter(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_short_circuit_set_bus_fault(size_t bus, char* fault_type, double fault_G, double fault_B, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_short_circuit_set_line_fault(size_t ibus, size_t jbus, char* identifier, char* fault_type, double fault_location, double fault_G, double fault_B, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_short_circuit_clear_fault(size_t toolkit_index);
EXPORT_STEPS_DLL void api_solve_short_circuit(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_short_circuit_result_float_data(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_show_short_circuit_result(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_short_circuit_result_to_file(char* file, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_save_extended_short_circuit_result_to_file(char* file, size_t toolkit_index=INDEX_NOT_EXIST);



EXPORT_STEPS_DLL size_t api_get_dynamic_simulator_integer_parameter(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_dynamic_simulator_integer_parameter(char* parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_dynamic_simulator_float_parameter(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_dynamic_simulator_float_parameter(char* parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_dynamic_simulator_string_parameter(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_dynamic_simulator_string_parameter(char* parameter_name, char* value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_dynamic_simulator_boolean_parameter(char* parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_dynamic_simulator_boolean_parameter(char* parameter_name, bool value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_dynamic_simulator_output_file(char* file, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL const char* api_get_dynamic_simulator_output_file(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_set_dynamic_simulation_time_step(double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_dynamic_simulation_time_step(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_dynamic_simulation_time(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_show_dynamic_simulation_configuration(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_clear_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_bus_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_generator_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_wt_generator_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_pv_unit_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_energy_storage_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_load_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_line_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_transformer_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_hvdc_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_vsc_hvdc_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_equivalent_device_related_meters(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_prepare_bus_related_meter(size_t bus, char* meter_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_generator_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_wt_generator_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_pv_unit_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_energy_storage_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_load_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_line_related_meter(size_t ibus, size_t jbus, char* id, char* meter_type, char* side, char* var_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_transformer_related_meter(size_t ibus, size_t jbus, size_t kbus, char* id, char* meter_type, char* side, char* var_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_hvdc_related_meter(size_t ibus, size_t jbus, char* id, char* meter_type, char* side, char* var_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_vsc_hvdc_related_meter(char* vsc_name, char* meter_type, size_t side, char* var_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_prepare_equivalent_device_related_meter(size_t bus, char* id, char* meter_type, char* var_name, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_start_dynamic_simulation(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_stop_dynamic_simulation(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_run_simulation_to_time(double t_end, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_run_a_step(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL bool api_get_system_angular_stable_flag(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL double api_get_basic_meter_value(const char* meter_name, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_user_meter_value(int index, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_change_dynamic_simulation_time_step(double delt, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_set_bus_fault(size_t bus, char* fault_type, double fault_G, double fault_B, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_clear_bus_fault(size_t bus, char* fault_type, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_trip_bus(size_t bus, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_set_line_fault(size_t ibus, size_t jbus, char* identifier, char* fault_type, double fault_location, double fault_G, double fault_B, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_clear_line_fault(size_t ibus, size_t jbus, char* identifier, char* fault_type, double fault_location, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_trip_line(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_trip_line_breaker(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_close_line(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_close_line_breaker(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_trip_transformer(size_t ibus, size_t jbus, size_t kbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_trip_transformer_breaker(size_t ibus, size_t jbus, size_t kbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_close_transformer(size_t ibus, size_t jbus, size_t kbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_close_transformer_breaker(size_t ibus, size_t jbus, size_t kbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_trip_generator(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_shed_generator(size_t bus, char* identifier, double percent, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_trip_wt_generator(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_shed_wt_generator(size_t bus, char* identifier, double percent, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_trip_load(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_close_load(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_scale_load(size_t bus, char* identifier, double percent, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_scale_all_loads(double percent, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_trip_fixed_shunt(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_close_fixed_shunt(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_manually_bypass_hvdc(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_manually_unbypass_hvdc(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_manually_block_hvdc(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_manually_unblock_hvdc(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL double api_get_generator_voltage_reference_in_pu(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_generator_mechanical_power_reference_in_pu_based_on_mbase(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_generator_mechanical_power_reference_in_MW(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_set_generator_voltage_reference_in_pu(size_t bus, char* identifier, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_generator_mechanical_power_reference_in_pu_based_on_mbase(size_t bus, char* identifier, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_generator_mechanical_power_reference_in_MW(size_t bus, char* identifier, double value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL double api_get_generator_excitation_voltage_in_pu(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_generator_mechanical_power_in_pu_based_on_mbase(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL double api_get_generator_mechanical_power_in_MW(size_t bus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_set_generator_excitation_voltage_in_pu(size_t bus, char* identifier, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_generator_mechanical_power_in_pu_based_on_mbase(size_t bus, char* identifier, double value, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_generator_mechanical_power_in_MW(size_t bus, char* identifier, double value, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL double api_get_hvdc_power_order_in_MW(size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_set_hvdc_power_order_in_MW(size_t ibus, size_t jbus, char* identifier, double value, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL void api_set_vsc_hvdc_dc_line_fault(char* vsc_name, size_t ibus, size_t jbus, char* identifier, double fault_location, double fault_R, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_clear_vsc_hvdc_dc_line_fault(char* vsc_name, size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_trip_vsc_hvdc_dc_line(char* vsc_name, size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_close_vsc_hvdc_dc_line(char* vsc_name, size_t ibus, size_t jbus, char* identifier, size_t toolkit_index=INDEX_NOT_EXIST);


EXPORT_STEPS_DLL void api_check_powerflow_data(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_check_dynamic_data(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_check_missing_models(size_t toolkit_index=INDEX_NOT_EXIST);
EXPORT_STEPS_DLL void api_check_least_dynamic_time_constants(size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL void api_check_network_connectivity(bool remove_void_island=false, size_t toolkit_index=INDEX_NOT_EXIST);

EXPORT_STEPS_DLL double api_search_cct(char* pf_file, char* dy_file, size_t ibus, size_t jbus, char* id, size_t sidebus, size_t trip_line, size_t toolkit_index=INDEX_NOT_EXIST);

#ifdef  __cplusplus
}
#endif

#endif // STEPS_API_H
