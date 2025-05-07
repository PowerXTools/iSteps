#ifndef STEPS_API_SEARCH_BUFFER_H
#define STEPS_API_SEARCH_BUFFER_H

#include "header/pf_database.h"
#include "header/basic/constants.h"
#include <ctime>
#include <string>

using namespace std;

class API_SEARCH_BUFFER
{
    public:
        API_SEARCH_BUFFER();
        ~API_SEARCH_BUFFER();

        vector<BUS*> buses;
        size_t bus_pointer;
        vector<GENERATOR*> generators;
        size_t generator_pointer;
        vector<WT_GENERATOR*> wt_generators;
        size_t wt_generator_pointer;
        vector<PV_UNIT*> pv_units;
        size_t pv_unit_pointer;
        vector<LOAD*> loads;
        size_t load_pointer;
        vector<FIXED_SHUNT*> fixed_shunts;
        size_t fixed_shunt_pointer;
        vector<AC_LINE*> ac_lines;
        size_t ac_line_pointer;
        vector<TRANSFORMER*> transformers;
        size_t transformer_pointer;
        vector<LCC_HVDC2T*> hvdcs;
        size_t hvdc_pointer;
        vector<VSC_HVDC*> vscs;
        size_t vsc_pointer;
        vector<EQUIVALENT_DEVICE*> equivalent_devices;
        size_t equivalent_device_pointer;
        vector<ENERGY_STORAGE*> energy_storages;
        size_t energy_storage_pointer;
        vector<AREA*> areas;
        size_t area_pointer;
        vector<ZONE*> zones;
        size_t zone_pointer;
        vector<OWNER*> owners;
        size_t owner_pointer;
};
#endif // STEPS_API_SEARCH_BUFFER_H
