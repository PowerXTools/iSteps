#ifndef POWER_SYSTEM_DATABASE_H
#define POWER_SYSTEM_DATABASE_H

#include "header/device/bus.h"
#include "header/device/generator.h"
#include "header/device/wt_generator.h"
#include "header/device/pv_unit.h"
#include "header/device/energy_storage.h"
#include "header/device/load.h"
#include "header/device/ac_line.h"
#include "header/device/transformer.h"
#include "header/device/fixed_shunt.h"
#include "header/device/lcc_hvdc2t.h"
#include "header/device/vsc_hvdc.h"
#include "header/device/equivalent_device.h"
#include "header/device/source.h"
#include "header/device/lcc_hvdc.h"
#include "header/device/mutual_data.h"
#include "header/basic/area.h"
#include "header/basic/zone.h"
#include "header/basic/owner.h"
#include "header/basic/bus_index.h"
#include "header/basic/device_index_map.h"

#include <string>

using namespace std;

class MODEL;
class SOURCE;
template<class T> void check_all_items(std::vector<T>& item);

class PF_DATA
{
    public:
        PF_DATA(iSTEPS& toolkit);
        ~PF_DATA();

        iSTEPS& get_toolkit() const;

        void clear_all_data();
        void check_data();

        void set_zero_impedance_threshold_in_pu(double z);
        double get_zero_impedance_threshold_in_pu() const;

        void set_default_capacity(const string cfgfile);
        size_t get_device_capacity(const char* device_type);
        template<class T> size_t  get_item_capacity(std::vector<T>& item);

//        size_t get_area_capacity() const;
//        size_t get_zone_capacity() const;
//        size_t get_owner_capacity() const;

        void set_bus_capacity(size_t n);
        void set_generator_capacity(size_t n);
        void set_wt_generator_capacity(size_t n);
        void set_pv_unit_capacity(size_t n);
        void set_load_capacity(size_t n);
        void set_fixed_shunt_capacity(size_t n);
        void set_ac_line_capacity(size_t n);
        void set_transformer_capacity(size_t n);
        void set_2t_lcc_hvdc_capacity(size_t n);
        void set_vsc_hvdc_capacity(size_t n);
        void set_equivalent_device_capacity(size_t n);
        void set_energy_storage_capacity(size_t n);
        void set_lcc_hvdc_capacity(size_t n);
        void set_area_capacity(size_t n);
        void set_zone_capacity(size_t n);
        void set_owner_capacity(size_t n);

        void set_system_name(const string& name);
        string get_system_name() const;

        void set_max_bus_number(size_t max_bus_num);
        size_t get_allowed_max_bus_number() const;
        bool is_bus_in_allowed_range(size_t bus) const;

        void set_system_base_power_in_MVA(const double s);
        double get_system_base_power_in_MVA() const;
        double get_one_over_system_base_power_in_one_over_MVA() const;

        void set_case_information(const string& title);
        void set_case_additional_information(const string& title);

        string get_case_information() const;
        string get_case_additional_information() const;

        void append_bus(BUS& bus);
        void append_generator(GENERATOR& generator);
        void append_wt_generator(WT_GENERATOR& wt_generator);
        void append_pv_unit(PV_UNIT& pv_unit);
        void append_energy_storage(ENERGY_STORAGE& estorage);
        void append_load(LOAD& load);
        void append_ac_line(AC_LINE& line);
        void append_transformer(TRANSFORMER& transformer);
        void append_fixed_shunt(FIXED_SHUNT& shunt);
        void append_2t_lcc_hvdc(LCC_HVDC2T& hvdc);
        void append_vsc_hvdc(VSC_HVDC& vsc_hvdc);
        void append_equivalent_device(EQUIVALENT_DEVICE& edevice);
        void append_lcc_hvdc(LCC_HVDC& hvdc);
        void append_area(AREA& area);
        void append_zone(ZONE& zone);
        void append_owner(OWNER& owner);
        void append_mutual_data(MUTUAL_DATA& mutual_data);

        void update_device_id(const DEVICE_ID& did_old, const DEVICE_ID& did_new);

        void update_all_bus_base_frequency(double fbase_Hz);

        bool is_bus_exist(size_t bus) const;
        bool is_generator_exist(const DEVICE_ID& device_id) const;
        bool is_wt_generator_exist(const DEVICE_ID& device_id) const;
        bool is_pv_unit_exist(const DEVICE_ID& device_id) const;
        bool is_load_exist(const DEVICE_ID& device_id) const;
        bool is_ac_line_exist(const DEVICE_ID& device_id) const;
        bool is_transformer_exist(const DEVICE_ID& device_id) const;
        bool is_fixed_shunt_exist(const DEVICE_ID& device_id) const;
        bool is_2t_lcc_hvdc_exist(const DEVICE_ID& device_id) const;
        bool is_vsc_hvdc_exist(const DEVICE_ID& device_id) const;
        bool is_vsc_hvdc_exist(const string vsc_name) const;
        bool is_equivalent_device_exist(const DEVICE_ID& device_id) const;
        bool is_energy_storage_exist(const DEVICE_ID& device_id) const;
        bool is_lcc_hvdc_exist(const DEVICE_ID& device_id) const;
        bool is_area_exist(const size_t no) const;
        bool is_zone_exist(const size_t no) const;
        bool is_owner_exist(const size_t no) const;

        void change_bus_number(size_t original_bus_number, size_t new_bus_number);
        void change_bus_number_with_file(string file);

        DEVICE* get_device(const DEVICE_ID& device_id);
        NONBUS_DEVICE* get_nonbus_device(const DEVICE_ID& device_id);
        BUS* get_bus(size_t bus);
        BUS* get_bus(const DEVICE_ID & device_id);
        GENERATOR* get_generator(const DEVICE_ID & device_id);
        WT_GENERATOR* get_wt_generator(const DEVICE_ID & device_id);
        PV_UNIT* get_pv_unit(const DEVICE_ID & device_id);
        SOURCE* get_source(const DEVICE_ID & device_id);
        LOAD* get_load(const DEVICE_ID & device_id);
        AC_LINE* get_ac_line(const DEVICE_ID & device_id);
        TRANSFORMER* get_transformer(const DEVICE_ID & device_id);
        FIXED_SHUNT* get_fixed_shunt(const DEVICE_ID & device_id);
        LCC_HVDC2T* get_2t_lcc_hvdc(const DEVICE_ID & device_id);
        VSC_HVDC* get_vsc_hvdc(const DEVICE_ID & device_id);
        VSC_HVDC* get_vsc_hvdc(string name);
        EQUIVALENT_DEVICE* get_equivalent_device(const DEVICE_ID & device_id);
        ENERGY_STORAGE* get_energy_storage(const DEVICE_ID & device_id);
        LCC_HVDC* get_lcc_hvdc(const DEVICE_ID & device_id);
        AREA* get_area(const size_t no);
        ZONE* get_zone(const size_t no);
        OWNER* get_owner(const size_t no);

        vector<DEVICE*> get_all_devices_connecting_to_bus(const size_t bus);
        vector<GENERATOR*> get_generators_connecting_to_bus(const size_t bus);
        vector<WT_GENERATOR*> get_wt_generators_connecting_to_bus(const size_t bus);
        vector<PV_UNIT*> get_pv_units_connecting_to_bus(const size_t bus);
        vector<SOURCE*> get_sources_connecting_to_bus(const size_t bus);
        vector<LOAD*> get_loads_connecting_to_bus(const size_t bus);
        vector<AC_LINE*> get_ac_lines_connecting_to_bus(const size_t bus);
        vector<TRANSFORMER*> get_transformers_connecting_to_bus(const size_t bus);
        vector<FIXED_SHUNT*> get_fixed_shunts_connecting_to_bus(const size_t bus);
        vector<LCC_HVDC2T*> get_2t_lcc_hvdcs_connecting_to_bus(const size_t bus);
        vector<VSC_HVDC*> get_vsc_hvdcs_connecting_to_bus(const size_t bus);
        vector<EQUIVALENT_DEVICE*> get_equivalent_devices_connecting_to_bus(const size_t bus);
        vector<ENERGY_STORAGE*> get_energy_storages_connecting_to_bus(const size_t bus);
        vector<LCC_HVDC*> get_lcc_hvdcs_connecting_to_bus(const size_t bus);
        vector<MUTUAL_DATA*> get_mutual_data_with_line(const size_t ibus, const size_t jbus, string identifier);

        vector<DEVICE_ID> get_all_devices_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_generators_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_wt_generators_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_pv_units_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_sources_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_loads_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_ac_lines_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_transformers_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_fixed_shunts_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_2t_lcc_hvdcs_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_vsc_hvdcs_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_equivalent_devices_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_energy_storages_device_id_connecting_to_bus(const size_t bus);
        vector<DEVICE_ID> get_lcc_hvdcs_device_id_connecting_to_bus(const size_t bus);

        vector<DEVICE*> get_all_devices_in_area(const size_t area);
        vector<BUS*> get_buses_in_area(const size_t area);
        vector<GENERATOR*> get_generators_in_area(const size_t area);
        vector<WT_GENERATOR*> get_wt_generators_in_area(const size_t area);
        vector<PV_UNIT*> get_pv_units_in_area(const size_t area);
        vector<SOURCE*> get_sources_in_area(const size_t area);
        vector<LOAD*> get_loads_in_area(const size_t area);
        vector<AC_LINE*> get_ac_lines_in_area(const size_t area);
        vector<TRANSFORMER*> get_transformers_in_area(const size_t area);
        vector<FIXED_SHUNT*> get_fixed_shunts_in_area(const size_t area);
        vector<LCC_HVDC2T*> get_2t_lcc_hvdcs_in_area(const size_t area);
        vector<VSC_HVDC*> get_vsc_hvdcs_in_area(const size_t area);
        vector<EQUIVALENT_DEVICE*> get_equivalent_devices_in_area(const size_t area);
        vector<ENERGY_STORAGE*> get_energy_storages_in_area(const size_t area);
        vector<LCC_HVDC*> get_lcc_hvdcs_in_area(const size_t area);

        vector<DEVICE_ID> get_all_devices_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_buses_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_generators_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_wt_generators_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_pv_units_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_sources_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_loads_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_ac_lines_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_transformers_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_fixed_shunts_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_2t_lcc_hvdcs_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_vsc_hvdcs_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_equivalent_devices_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_energy_storages_device_id_in_area(const size_t area);
        vector<DEVICE_ID> get_lcc_hvdcs_device_id_in_area(const size_t area);

        vector<DEVICE*> get_all_devices_in_zone(const size_t zone);
        vector<BUS*> get_buses_in_zone(const size_t zone);
        vector<GENERATOR*> get_generators_in_zone(const size_t zone);
        vector<WT_GENERATOR*> get_wt_generators_in_zone(const size_t zone);
        vector<PV_UNIT*> get_pv_units_in_zone(const size_t zone);
        vector<SOURCE*> get_sources_in_zone(const size_t zone);
        vector<LOAD*> get_loads_in_zone(const size_t zone);
        vector<AC_LINE*> get_ac_lines_in_zone(const size_t zone);
        vector<TRANSFORMER*> get_transformers_in_zone(const size_t zone);
        vector<FIXED_SHUNT*> get_fixed_shunts_in_zone(const size_t zone);
        vector<LCC_HVDC2T*> get_2t_lcc_hvdcs_in_zone(const size_t zone);
        vector<VSC_HVDC*> get_vsc_hvdcs_in_zone(const size_t zone);
        vector<EQUIVALENT_DEVICE*> get_equivalent_devices_in_zone(const size_t zone);
        vector<ENERGY_STORAGE*> get_energy_storages_in_zone(const size_t zone);
        vector<LCC_HVDC*> get_lcc_hvdcs_in_zone(const size_t zone);

        vector<DEVICE_ID> get_all_devices_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_buses_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_generators_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_wt_generators_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_pv_units_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_sources_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_loads_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_ac_lines_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_transformers_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_fixed_shunts_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_2t_lcc_hvdcs_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_vsc_hvdcs_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_equivalent_devices_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_energy_storages_device_id_in_zone(const size_t zone);
        vector<DEVICE_ID> get_lcc_hvdcs_device_id_in_zone(const size_t zone);

        vector<DEVICE*> get_all_devices();
        vector<BUS*> get_all_buses();
        vector<BUS*> get_buses_with_constraints(double vbase_kV_min, double vbase_kV_max, double v_pu_min, double v_pu_max, size_t area, size_t zone, size_t owner);
        vector<BUS*> get_all_in_service_buses();
        vector<GENERATOR*> get_all_generators();
        vector<WT_GENERATOR*> get_all_wt_generators();
        vector<PV_UNIT*> get_all_pv_units();
        vector<SOURCE*> get_all_sources();
        vector<LOAD*> get_all_loads();
        vector<AC_LINE*> get_all_ac_lines();
        vector<TRANSFORMER*> get_all_transformers();
        vector<FIXED_SHUNT*> get_all_fixed_shunts();
        vector<LCC_HVDC2T*> get_all_2t_lcc_hvdcs();
        vector<VSC_HVDC*> get_all_vsc_hvdcs();
        vector<EQUIVALENT_DEVICE*> get_all_equivalent_devices();
        vector<ENERGY_STORAGE*> get_all_energy_storages();
        vector<LCC_HVDC*> get_all_lcc_hvdcs();
        vector<AREA*> get_all_areas();
        vector<ZONE*> get_all_zones();
        vector<OWNER*> get_all_owners();
        vector<MUTUAL_DATA*> get_all_mutual_data();

        vector<size_t> get_all_buses_number();
        vector<size_t> get_buses_number_with_constraints(double vbase_kV_min, double vbase_kV_max, double v_pu_min, double v_pu_max, size_t area, size_t zone, size_t owner);
        vector<size_t> get_all_in_service_buses_number();
        vector<DEVICE_ID> get_all_generators_device_id();
        vector<DEVICE_ID> get_all_wt_generators_device_id();
        vector<DEVICE_ID> get_all_pv_units_device_id();
        vector<DEVICE_ID> get_all_sources_device_id();
        vector<DEVICE_ID> get_all_loads_device_id();
        vector<DEVICE_ID> get_all_ac_lines_device_id();
        vector<DEVICE_ID> get_all_transformers_device_id();
        vector<DEVICE_ID> get_all_fixed_shunts_device_id();
        vector<DEVICE_ID> get_all_2t_lcc_hvdcs_device_id();
        vector<DEVICE_ID> get_all_vsc_hvdcs_device_id();
        vector<DEVICE_ID> get_all_equivalent_devices_device_id();
        vector<DEVICE_ID> get_all_energy_storages_device_id();
        vector<DEVICE_ID> get_all_lcc_hvdcs_device_id();
        vector<size_t> get_all_areas_number();
        vector<size_t> get_all_zones_number();
        vector<size_t> get_all_owners_number();

        size_t get_bus_count() const;
        size_t get_in_service_bus_count() const;
        size_t get_overshadowed_bus_count() const;
        size_t get_generator_count() const;
        size_t get_wt_generator_count() const;
        size_t get_pv_unit_count() const;
        size_t get_source_count() const;
        size_t get_load_count() const;
        size_t get_ac_line_count() const;
        size_t get_transformer_count() const;
        size_t get_fixed_shunt_count() const;
        size_t get_2t_lcc_hvdc_count() const;
        size_t get_vsc_hvdc_count() const;
        size_t get_switched_shunt_count() const;
        size_t get_equivalent_device_count() const;
        size_t get_energy_storage_count() const;
        size_t get_lcc_hvdc_count() const;
        size_t get_area_count() const;
        size_t get_zone_count() const;
        size_t get_owner_count() const;
        size_t get_mutual_data_count() const;

        size_t get_bus_index(size_t bus) const;
        size_t get_generator_index(const DEVICE_ID & device_id) const;
        size_t get_wt_generator_index(const DEVICE_ID & device_id) const;
        size_t get_pv_unit_index(const DEVICE_ID & device_id) const;
        size_t get_load_index(const DEVICE_ID & device_id) const;
        size_t get_ac_line_index(const DEVICE_ID & device_id) const;
        size_t get_transformer_index(const DEVICE_ID & device_id) const;
        size_t get_fixed_shunt_index(const DEVICE_ID & device_id) const;
        size_t get_2t_lcc_hvdc_index(const DEVICE_ID & device_id) const;
        size_t get_vsc_hvdc_index(const DEVICE_ID & device_id) const;
        size_t get_vsc_hvdc_index(const string vsc_name) const;
        size_t get_equivalent_device_index(const DEVICE_ID & device_id) const;
        size_t get_energy_storage_index(const DEVICE_ID & device_id) const;
        size_t get_lcc_hvdc_index(const DEVICE_ID & device_id) const;
        size_t get_area_index(const size_t no) const;
        size_t get_zone_index(const size_t no) const;
        size_t get_owner_index(const size_t no) const;

        size_t bus_name2bus_number(const string& name, double vbase=0) const;
        size_t area_name2area_number(const string& name) const;
        size_t zone_name2zone_number(const string& name) const;
        size_t owner_name2owner_number(const string& name) const;
        string bus_number2bus_name(size_t number);
        string area_number2area_name(size_t number);
        string zone_number2zone_name(size_t number);
        string owner_number2owner_name(size_t number);

        void check_dynamic_data();
        void check_generator_related_dynamic_data();
        void check_wt_generator_related_dynamic_data();
        void check_pv_unit_related_dynamic_data();
        void check_energy_storage_related_dynamic_data();
        void check_load_related_dynamic_data();
        void check_ac_line_related_dynamic_data();
        void check_2t_lcc_hvdc_related_dynamic_data();
        void check_vsc_hvdc_related_dynamic_data();
        void check_equivalent_device_related_dynamic_data();
        void check_lcc_hvdc_related_dynamic_data();

        void check_missing_models();
        void check_missing_generator_related_model();
        void check_missing_wt_generator_related_model();
        void check_missing_pv_unit_related_model();
        void check_missing_energy_storage_related_model();
        void check_missing_load_related_model();
        void check_missing_ac_line_related_model();
        void check_missing_2t_lcc_hvdc_related_model();
        void check_missing_vsc_hvdc_related_model();
        void check_missing_equivalent_device_related_model();
        void check_missing_lcc_hvdc_related_model();


        void scale_load_power(const DEVICE_ID& did, double scale);
        void scale_all_loads_power(double scale);
        void scale_loads_power_at_bus(size_t bus, double scale);
        void scale_loads_power_in_area(size_t area_number, double scale);
        void scale_loads_power_in_zone(size_t zone_number, double scale);

        void scale_source_power(const DEVICE_ID& did, double scale);
        void scale_all_sources_power(double scale);
        void scale_sources_power_at_bus(size_t bus, double scale);
        void scale_sources_power_in_area(size_t area_number, double scale);
        void scale_sources_power_in_zone(size_t zone_number, double scale);

        void scale_generator_power(const DEVICE_ID& did, double scale);
        void scale_all_generators_power(double scale);
        void scale_generators_power_at_bus(size_t bus, double scale);
        void scale_generators_power_in_area(size_t area_number, double scale);
        void scale_generators_power_in_zone(size_t zone_number, double scale);

        void scale_wt_generator_power(const DEVICE_ID& did, double scale);
        void scale_all_wt_generators_power(double scale);
        void scale_wt_generators_power_at_bus(size_t bus, double scale);
        void scale_wt_generators_power_in_area(size_t area_number, double scale);
        void scale_wt_generators_power_in_zone(size_t zone_number, double scale);

        void scale_pv_unit_power(const DEVICE_ID& did, double scale);
        void scale_all_pv_units_power(double scale);
        void scale_pv_units_power_at_bus(size_t bus, double scale);
        void scale_pv_units_power_in_area(size_t area_number, double scale);
        void scale_pv_units_power_in_zone(size_t zone_number, double scale);

        void scale_energy_storage_power(const DEVICE_ID& did, double scale);
        void scale_all_energy_storages_power(double scale);
        void scale_energy_storages_power_at_bus(size_t bus, double scale);
        void scale_energy_storages_power_in_area(size_t area_number, double scale);
        void scale_energy_storages_power_in_zone(size_t zone_number, double scale);

        void clear_bus(size_t bus);
        void clear_all_buses();

        void clear_generator(const DEVICE_ID& device_id);
        void clear_generators_connecting_to_bus(const size_t bus);
        void clear_all_generators();

        void clear_wt_generator(const DEVICE_ID& device_id);
        void clear_wt_generators_connecting_to_bus(const size_t bus);
        void clear_all_wt_generators();

        void clear_pv_unit(const DEVICE_ID& device_id);
        void clear_pv_units_connecting_to_bus(const size_t bus);
        void clear_all_pv_units();

        void clear_sources_connecting_to_bus(const size_t bus);
        void clear_all_sources();

        void clear_load(const DEVICE_ID& device_id);
        void clear_loads_connecting_to_bus(const size_t bus);
        void clear_all_loads();

        void clear_ac_line(const DEVICE_ID& device_id);
        void clear_ac_lines_connecting_to_bus(const size_t bus);
        void clear_all_ac_lines();

        void clear_transformer(const DEVICE_ID& device_id);
        void clear_transformers_connecting_to_bus(const size_t bus);
        void clear_all_transformers();

        void clear_fixed_shunt(const DEVICE_ID& device_id);
        void clear_fixed_shunts_connecting_to_bus(const size_t bus);
        void clear_all_fixed_shunts();

        void clear_2t_lcc_hvdc(const DEVICE_ID& device_id);
        void clear_2t_lcc_hvdcs_connecting_to_bus(const size_t bus);
        void clear_all_2t_lcc_hvdcs();

        void clear_vsc_hvdc(const DEVICE_ID& device_id);
        void clear_vsc_hvdc(const string vsc_name);
        void clear_vsc_hvdcs_connecting_to_bus(const size_t bus);
        void clear_all_vsc_hvdcs();

        void clear_equivalent_device(const DEVICE_ID& device_id);
        void clear_equivalent_devices_connecting_to_bus(const size_t bus);
        void clear_all_equivalent_devices();

        void clear_energy_storage(const DEVICE_ID& device_id);
        void clear_energy_storages_connecting_to_bus(const size_t bus);
        void clear_all_energy_storages();

        void clear_lcc_hvdc(const DEVICE_ID& device_id);
        void clear_lcc_hvdcs_connecting_to_bus(const size_t bus);
        void clear_all_lcc_hvdcs();

        void clear_area(size_t area);
        void clear_all_areas();

        void clear_zone(size_t zone);
        void clear_all_zones();

        void clear_owner(size_t owner);
        void clear_all_owners();

        void trip_bus(size_t bus);

        void check_device_status_for_out_of_service_buses();
        void calibrate_sources_pmax_and_pmin_with_dynamic_model();
        void calibrate_generators_pmax_and_pmin_with_turbine_governor_model();

        BUS_TYPE get_bus_type(size_t bus);
        double get_bus_base_frequency_in_Hz(size_t bus);
        double get_bus_base_voltage_in_kV(size_t bus);
        double get_bus_positive_sequence_voltage_in_pu(size_t bus);
        double get_bus_positive_sequence_voltage_in_kV(size_t bus);
        double get_bus_positive_sequence_angle_in_deg(size_t bus);
        double get_bus_positive_sequence_angle_in_rad(size_t bus);
        complex<double> get_bus_positive_sequence_complex_voltage_in_pu(size_t bus);
        complex<double> get_bus_positive_sequence_complex_voltage_in_kV(size_t bus);

        double get_bus_negative_sequence_voltage_in_pu(size_t bus);
        double get_bus_negative_sequence_voltage_in_kV(size_t bus);
        double get_bus_negative_sequence_angle_in_deg(size_t bus);
        double get_bus_negative_sequence_angle_in_rad(size_t bus);
        complex<double> get_bus_negative_sequence_complex_voltage_in_pu(size_t bus);
        complex<double> get_bus_negative_sequence_complex_voltage_in_kV(size_t bus);
        double get_bus_zero_sequence_voltage_in_pu(size_t bus);
        double get_bus_zero_sequence_voltage_in_kV(size_t bus);
        double get_bus_zero_sequence_angle_in_deg(size_t bus);
        double get_bus_zero_sequence_angle_in_rad(size_t bus);
        complex<double> get_bus_zero_sequence_complex_voltage_in_pu(size_t bus);
        complex<double> get_bus_zero_sequence_complex_voltage_in_kV(size_t bus);

        double get_bus_frequency_deviation_in_pu(size_t bus);
        double get_bus_frequency_deviation_in_Hz(size_t bus);
        double get_bus_frequency_in_Hz(size_t bus);
        double get_bus_frequency_in_pu(size_t bus);

        double get_voltage_to_regulate_of_physical_bus_in_pu(size_t bus);
        double get_regulatable_p_max_at_physical_bus_in_MW(size_t bus);
        double get_regulatable_p_min_at_physical_bus_in_MW(size_t bus);
        double get_regulatable_q_max_at_physical_bus_in_MVar(size_t bus);
        double get_regulatable_q_min_at_physical_bus_in_MVar(size_t bus);
        double get_total_regulating_p_generation_at_physical_bus_in_MW(size_t bus);
        double get_total_regulating_q_generation_at_physical_bus_in_MVar(size_t bus);
        double get_total_p_generation_at_physical_bus_in_MW(size_t bus);
        double get_total_q_generation_at_physical_bus_in_MVar(size_t bus);

        complex<double> get_total_load_power_in_MVA();
        complex<double> get_total_generation_power_in_MVA();
        complex<double> get_total_loss_power_in_MVA();


        void update_in_service_bus_count();
        void set_all_buses_un_overshadowed();
        void update_overshadowed_bus_count();
        size_t get_equivalent_bus_of_bus(size_t bus);

        size_t get_memory_usage();
    private:
        void check_source_status_for_out_of_service_bus(size_t bus);
        void check_load_status_for_out_of_service_bus(size_t bus);
        void check_fixed_shunt_status_for_out_of_service_bus(size_t bus);
        void check_ac_line_status_for_out_of_service_bus(size_t bus);
        void check_transformer_status_for_out_of_service_bus(size_t bus);
        void check_2t_lcc_hvdc_status_for_out_of_service_bus(size_t bus);
        void check_vsc_hvdc_status_for_out_of_service_bus(size_t bus);
        void check_lcc_hvdc_status_for_out_of_service_bus(size_t bus);

        iSTEPS* toolkit;

        string system_name;
        double system_base_power_in_MVA, one_over_system_base_power;

        string case_information, case_additional_information;

        vector<BUS> Bus;
        vector<GENERATOR> Generator;
        vector<WT_GENERATOR> WT_Generator;
        vector<PV_UNIT> PV_Unit;
        vector<LOAD> Load;
        vector<AC_LINE> Ac_line;
        vector<TRANSFORMER> Transformer;
        vector<FIXED_SHUNT> Fixed_shunt;
        vector<LCC_HVDC2T> Lcc_hvdc2t;
        vector<VSC_HVDC> Vsc_hvdc;
        vector<EQUIVALENT_DEVICE> Equivalent_device;
        vector<ENERGY_STORAGE> Energy_storage;
        vector<LCC_HVDC> Lcc_hvdc;
        vector<AREA> Area_;
        vector<ZONE> Zone_;
        vector<OWNER> Owner_;
        vector<MUTUAL_DATA> Mutual_data;

        double zero_impedance_threshold;

        size_t in_service_bus_count;
        size_t overshadowed_bus_count;

        BUS_INDEX bus_index;
        DEVICE_INDEX_MAP generator_index, wt_generator_index, pv_unit_index, load_index, fixed_shunt_index, switched_shunt_index,
                            ac_line_index, transformer_index, hvdc_index, vsc_hvdc_index, equivalent_device_index, energy_storage_index,
                            lcc_hvdc_index;
        map<size_t,  size_t> area_index, zone_index, owner_index;
};
#endif // POWER_SYSTEM_DATABASE_H
