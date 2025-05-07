#ifndef STEPS_API_COMMON_H
#define STEPS_API_COMMON_H


#include "header/iSTEPS.h"
#include "header/pf_database.h"
#include "header/toolkit/pf_solver.h"
#include "header/toolkit/dyn_simulator.h"

void show_side_not_supported_for_device_with_api(string side, DEVICE_ID did, string api_func, size_t toolkit_index=INDEX_NOT_EXIST);
void show_parameter_not_supported_for_device_with_api(string par_name, DEVICE_ID did, string api_func, size_t toolkit_index=INDEX_NOT_EXIST);
void show_parameter_not_supported_with_api(string par_name, string api_func, size_t toolkit_index=INDEX_NOT_EXIST);
void show_device_not_exist_with_api(DEVICE_ID did, string api_func, size_t toolkit_index=INDEX_NOT_EXIST);
void show_vsc_hvdc_not_exist_with_api(string name, string api_func, size_t toolkit_index=INDEX_NOT_EXIST);
void show_model_type_not_supported_for_device_with_api(string model_type, DEVICE_ID did, string api_func, size_t toolkit_index=INDEX_NOT_EXIST);

void show_parameter_not_supported_for_area_zone_owner_with_api(string par_name, size_t no, string api_func, size_t toolkit_index=INDEX_NOT_EXIST);
void show_area_zone_owner_not_exist_with_api(size_t no, string api_func, size_t toolkit_index=INDEX_NOT_EXIST);

size_t get_owner_of_nonbus_device(NONBUS_DEVICE* device, string parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
void set_owner_of_nonbus_device(NONBUS_DEVICE* device, string parameter_name, int value, size_t toolkit_index=INDEX_NOT_EXIST);
double get_owner_fraction_of_nonbus_device(NONBUS_DEVICE* device, string parameter_name, size_t toolkit_index=INDEX_NOT_EXIST);
void set_owner_fraction_of_nonbus_device(NONBUS_DEVICE* device, string parameter_name, double value, size_t toolkit_index=INDEX_NOT_EXIST);

#endif // STEPS_API_COMMON_H
