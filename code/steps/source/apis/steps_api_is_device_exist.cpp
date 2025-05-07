#include "header/apis/steps_api.h"
#include "header/basic/utility.h"
#include "header/steps_namespace.h"


bool api_is_bus_exist(size_t bus, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    return psdb.is_bus_exist(bus);
}

bool api_is_generator_exist(size_t bus, char* ickt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    DEVICE_ID did = get_generator_device_id(bus, ickt);
    return psdb.is_generator_exist(did);
}

bool api_is_wt_generator_exist(size_t bus, char* ickt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    DEVICE_ID did = get_wt_generator_device_id(bus, ickt);
    return psdb.is_wt_generator_exist(did);
}

bool api_is_pv_unit_exist(size_t bus, char* ickt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    DEVICE_ID did = get_pv_unit_device_id(bus, ickt);
    return psdb.is_pv_unit_exist(did);
}

bool api_is_load_exist(size_t bus, char* ickt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    DEVICE_ID did = get_load_device_id(bus, ickt);
    return psdb.is_load_exist(did);
}

bool api_is_fixed_shunt_exist(size_t bus, char* ickt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    DEVICE_ID did = get_fixed_shunt_device_id(bus, ickt);
    return psdb.is_fixed_shunt_exist(did);
}

bool api_is_line_exist(size_t ibus, size_t jbus, char* ickt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    DEVICE_ID did = get_ac_line_device_id(ibus, jbus, ickt);
    return psdb.is_ac_line_exist(did);
}

bool api_is_transformer_exist(size_t ibus, size_t jbus, size_t kbus, char* ickt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    DEVICE_ID did = get_transformer_device_id(ibus, jbus, kbus, ickt);
    return psdb.is_transformer_exist(did);
}

bool api_is_hvdc_exist(size_t ibus, size_t jbus, char* ickt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    DEVICE_ID did = get_2t_lcc_hvdc_device_id(ibus, jbus, ickt);
    return psdb.is_2t_lcc_hvdc_exist(did);
}

bool api_is_vsc_hvdc_exist(char* vsc_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    return psdb.is_vsc_hvdc_exist(vsc_name);
}

bool api_is_equivalent_device_exist(size_t bus, char* ickt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    DEVICE_ID did = get_equivalent_device_id(bus, ickt);
    return psdb.is_equivalent_device_exist(did);
}

bool api_is_energy_storage_exist(size_t bus, char* ickt, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    DEVICE_ID did = get_energy_storage_device_id(bus, ickt);
    return psdb.is_energy_storage_exist(did);
}

