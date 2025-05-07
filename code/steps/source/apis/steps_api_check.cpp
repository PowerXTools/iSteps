#include "header/apis/steps_api.h"
#include "header/basic/utility.h"
#include "header/iSTEPS.h"
#include "header/pf_database.h"
void api_check_powerflow_data(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    psdb.check_data();
}

void api_check_dynamic_data(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    psdb.check_data();
}

void api_check_missing_models(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    PF_DATA& psdb = toolkit.get_database();
    psdb.check_missing_models();
}

void api_check_least_dynamic_time_constants(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    DYN_DATA& dmdb = toolkit.get_dynamic_model_database();
    dmdb.check_device_model_minimum_time_constants();
}

void api_check_network_connectivity(bool remove_void_island, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    NET_MATRIX& network = toolkit.get_network_matrix();
    network.build_network_Y_matrix();
    network.check_network_connectivity(remove_void_island);
}
