#include "header/pf_database.h"
#include <format>
#include <iostream>
#include "thirdparty/nlohmann/json.hpp"
#include "header/iSTEPS.h"
#include "header/steps_namespace.h"
#include "header/basic/utility.h"
#include "header/toolkit/dyn_simulator.h"
#include "header/data_imexporter/psse_imexporter.h"

using namespace std;
using json = nlohmann::json;
template<class T> void check_all_items(std::vector<T>& item)
{
    static_assert(std::is_member_function_pointer<decltype(&T::check)>::value,
                  "T must have a check() method");

    size_t n = item.size();
    for (size_t i = 0; i < n; ++i)
    {
        item[i].check();
    }
};

PF_DATA::PF_DATA(iSTEPS& toolkit)
{
    this->toolkit = (&toolkit);

//    set_system_name("");
//    set_system_base_power_in_MVA(100.0);
//    set_max_bus_number(1000);
//    set_zero_impedance_threshold_in_pu(0.0001);

    //clear_all_data();

    update_in_service_bus_count();
}


PF_DATA::~PF_DATA()
{
    clear_all_data();

    toolkit = NULL;
}

iSTEPS& PF_DATA::get_toolkit() const
{
    return *toolkit;
}

void PF_DATA::set_zero_impedance_threshold_in_pu(double z)
{
    zero_impedance_threshold = z;
}

double PF_DATA::get_zero_impedance_threshold_in_pu() const
{
    return zero_impedance_threshold;
}

void PF_DATA::set_default_capacity(const string cfg_file)
{
    size_t bus_capacity = 3000;
    size_t generator_capacity = (size_t)(round(bus_capacity * 0.5));
    size_t wt_generator_capacity = (size_t)(round(bus_capacity * 0.5));
    size_t pv_unit_capacity = (size_t)(round(bus_capacity * 0.5));
    size_t load_capacity = (size_t)(round(bus_capacity * 0.5));
    size_t fixedshunt_capacity = (size_t)(round(bus_capacity * 0.5));
    size_t line_capacity = (size_t)(round(bus_capacity * 3.0));
    size_t transformer_capacity = bus_capacity;
    size_t hvdc_capacity = (size_t)(round(bus_capacity * 0.1));
    size_t vsc_hvdc_capacity = (size_t)(round(bus_capacity * 0.01));
    size_t equivalentdevice_capacity = (size_t)(round(bus_capacity * 0.1));
    size_t energy_storage_capacity = (size_t)(round(bus_capacity * 0.1));
    size_t lcc_hvdc_capacity = hvdc_capacity;
    size_t area_capacity = (size_t)(round(bus_capacity * 0.1));
    size_t zone_capacity = (size_t)(round(bus_capacity * 0.1));
    size_t owner_capacity = (size_t)(round(bus_capacity * 0.1));

    std::ifstream instream(cfg_file);
    if (instream.is_open())
    {
        // 解析 JSON 文件
        json config;
        instream >> config;

        // 定义变量并从 JSON 中读取值
        bus_capacity = config.value("bus", 0);
        generator_capacity = config.value("generator", 0);
        wt_generator_capacity = config.value("wtGenerator", 0);
        pv_unit_capacity = config.value("pvUnit", 0);
        load_capacity = config.value("load", 0);
        fixedshunt_capacity = config.value("fixedShunt", 0);
        line_capacity = config.value("line", 0);
        transformer_capacity = config.value("transformer", 0);
        hvdc_capacity = config.value("hvdc", 0);
        vsc_hvdc_capacity = config.value("vschvdc", 0);
        equivalentdevice_capacity = config.value("equivalentDevice", 0);
        energy_storage_capacity = config.value("energyStorage", 0);
        lcc_hvdc_capacity = config.value("lccHvdc", 0);
        area_capacity = config.value("area", 0);
        zone_capacity = config.value("zone", 0);
        owner_capacity = config.value("owner", 0);
    }
    else
    {
        cout << "Config file" << cfg_file<< "is not found. Default capacity used." << endl;
        toolkit->show_information( std::format("Config file {:s} is not found. Default capacity used.", cfg_file));
    }

    set_bus_capacity(bus_capacity);
    set_generator_capacity(generator_capacity);
    set_wt_generator_capacity(wt_generator_capacity);
    set_pv_unit_capacity(pv_unit_capacity);
    set_load_capacity(load_capacity);
    set_fixed_shunt_capacity(fixedshunt_capacity);
    set_ac_line_capacity(line_capacity);
    set_transformer_capacity(transformer_capacity);
    set_2t_lcc_hvdc_capacity(hvdc_capacity);
    set_vsc_hvdc_capacity(vsc_hvdc_capacity);
    set_equivalent_device_capacity(equivalentdevice_capacity);
    set_energy_storage_capacity(energy_storage_capacity);
    set_lcc_hvdc_capacity(lcc_hvdc_capacity);
    set_area_capacity(area_capacity);
    set_zone_capacity(zone_capacity);
    set_owner_capacity(owner_capacity);
}

size_t PF_DATA::get_device_capacity(const char* device_type)
{
    string DEVICE_TYPE = string2upper(device_type);

    if(DEVICE_TYPE=="BUS")
        return get_item_capacity(Bus);

    if(DEVICE_TYPE=="GENERATOR")
        return get_item_capacity(Generator);

    if(DEVICE_TYPE=="WT GENERATOR")
        return get_item_capacity(WT_Generator);

    if(DEVICE_TYPE=="PV UNIT")
        return get_item_capacity(PV_Unit);

    if(DEVICE_TYPE=="LOAD")
        return get_item_capacity(Load);

    if(DEVICE_TYPE=="FIXED SHUNT")
        return get_item_capacity(Fixed_shunt);

    if(DEVICE_TYPE=="AC LINE")
        return get_item_capacity(Ac_line);

    if(DEVICE_TYPE=="TRANSFORMER")
        return get_item_capacity(Transformer);

    if(DEVICE_TYPE=="2T LCC HVDC")
        return get_item_capacity(Lcc_hvdc2t);

    if(DEVICE_TYPE=="VSC HVDC")
        return get_item_capacity(Vsc_hvdc);

    if(DEVICE_TYPE=="EQUIVALENT DEVICE")
        return get_item_capacity(Equivalent_device);

    if(DEVICE_TYPE=="ENERGY STORAGE")
        return get_item_capacity(Energy_storage);

    if(DEVICE_TYPE=="AREA")
        return get_item_capacity(Area_);

    if(DEVICE_TYPE=="ZONE")
        return get_item_capacity(Zone_);

    if(DEVICE_TYPE=="OWNER")
        return get_item_capacity(Owner_);

     cout<<"Device info input is mismatching, "<<DEVICE_TYPE<<"'s capacity is not available."<<endl;
     return 0;
};

template<class T> size_t  PF_DATA::get_item_capacity(std::vector<T>& item)
{
    static_assert(std::is_member_function_pointer<decltype(std::vector<T>::capacity)>::value,
                  "T must have a capacity() method");

    return item.capacity();
};

//size_t PF_DATA::get_bus_capacity() const
//{
//    return Bus.capacity();
//}
//
//size_t PF_DATA::get_generator_capacity() const
//{
//    return Generator.capacity();
//}
//
//size_t PF_DATA::get_wt_generator_capacity() const
//{
//    return WT_Generator.capacity();
//}
//
//size_t PF_DATA::get_pv_unit_capacity() const
//{
//    return PV_Unit.capacity();
//}
//
//size_t PF_DATA::get_load_capacity() const
//{
//    return Load.capacity();
//}
//
//size_t PF_DATA::get_fixed_shunt_capacity() const
//{
//    return Fixed_shunt.capacity();
//}
//
//size_t PF_DATA::get_ac_line_capacity() const
//{
//    return Ac_line.capacity();
//}
//
//size_t PF_DATA::get_transformer_capacity() const
//{
//    return Transformer.capacity();
//}
//
//size_t PF_DATA::get_2t_lcc_hvdc_capacity() const
//{
//    return Lcc_hvdc2t.capacity();
//}
//
//size_t PF_DATA::get_vsc_hvdc_capacity() const
//{
//    return Vsc_hvdc.capacity();
//}
//
//size_t PF_DATA::get_equivalent_device_capacity() const
//{
//    return Equivalent_device.capacity();
//}
//
//size_t PF_DATA::get_energy_storage_capacity() const
//{
//    return Energy_storage.capacity();
//}
//
//size_t PF_DATA::get_lcc_hvdc_capacity() const
//{
//    return Lcc_hvdc.capacity();
//}
//

//size_t PF_DATA::get_area_capacity() const
//{
//    return Area.capacity();
//}
//
//size_t PF_DATA::get_zone_capacity() const
//{
//    return Zone.capacity();
//}
//
//size_t PF_DATA::get_owner_capacity() const
//{
//    return Owner.capacity();
//}

void PF_DATA::set_bus_capacity(size_t n)
{
    Bus.reserve(n);
}

void PF_DATA::set_generator_capacity(size_t n)
{
    Generator.reserve(n);
}

void PF_DATA::set_wt_generator_capacity(size_t n)
{
    WT_Generator.reserve(n);
}

void PF_DATA::set_pv_unit_capacity(size_t n)
{
    PV_Unit.reserve(n);
}

void PF_DATA::set_load_capacity(size_t n)
{
    Load.reserve(n);
}

void PF_DATA::set_fixed_shunt_capacity(size_t n)
{
    Fixed_shunt.reserve(n);
}

void PF_DATA::set_ac_line_capacity(size_t n)
{
    Ac_line.reserve(n);
}

void PF_DATA::set_transformer_capacity(size_t n)
{
    Transformer.reserve(n);
}

void PF_DATA::set_2t_lcc_hvdc_capacity(size_t n)
{
    Lcc_hvdc2t.reserve(n);
}

void PF_DATA::set_vsc_hvdc_capacity(size_t n)
{
    Vsc_hvdc.reserve(n);
}

void PF_DATA::set_equivalent_device_capacity(size_t n)
{
    Equivalent_device.reserve(n);
}

void PF_DATA::set_energy_storage_capacity(size_t n)
{
    Energy_storage.reserve(n);
}

void PF_DATA::set_lcc_hvdc_capacity(size_t n)
{
    Lcc_hvdc.reserve(n);
}

void PF_DATA::set_area_capacity(size_t n)
{
    Area_.reserve(n);
}

void PF_DATA::set_zone_capacity(size_t n)
{
    Zone_.reserve(n);
}

void PF_DATA::set_owner_capacity(size_t n)
{
    Owner_.reserve(n);
}

void PF_DATA::clear_all_data()
{
    set_case_information("");
    set_case_additional_information("");
    clear_all_buses();
    clear_all_sources();
    clear_all_loads();
    clear_all_ac_lines();
    clear_all_transformers();
    clear_all_fixed_shunts();
    clear_all_2t_lcc_hvdcs();
    clear_all_vsc_hvdcs();
    clear_all_equivalent_devices();
    clear_all_lcc_hvdcs();
    clear_all_areas();
    clear_all_zones();
    clear_all_owners();
    update_in_service_bus_count();
}

void PF_DATA::set_system_name(const string& name)
{
    this->system_name = name;
}

string PF_DATA::get_system_name() const
{
    return this->system_name;
}

void PF_DATA::set_max_bus_number(size_t max_bus_num)
{
    bus_index.set_max_bus_number(max_bus_num);
}

size_t PF_DATA::get_allowed_max_bus_number() const
{
    return bus_index.get_max_bus_number();
}

bool PF_DATA::is_bus_in_allowed_range(size_t bus) const
{
    return (bus > 0 and bus <= this->get_allowed_max_bus_number());
}

void PF_DATA::set_system_base_power_in_MVA(const double s)
{
    if (s > 0.0)
    {
        system_base_power_in_MVA = s;
        one_over_system_base_power = 1.0 / s;
    }
    else
    {
        ostringstream osstream;
        osstream << "Warning. Non positive power (" << s << " MVA) is not allowed for setting system base power." << endl
                 << "Nothing will be changed.";
        toolkit->show_information(osstream);
    }
}

double PF_DATA::get_system_base_power_in_MVA() const
{
    return system_base_power_in_MVA;
}

double PF_DATA::get_one_over_system_base_power_in_one_over_MVA() const
{
    return one_over_system_base_power;
}

void PF_DATA::set_case_information(const string& title)
{
    case_information = title;
}

void PF_DATA::set_case_additional_information(const string& title)
{
    case_additional_information = title;
}

string PF_DATA::get_case_information() const
{
    return case_information;
}

string PF_DATA::get_case_additional_information() const
{
    return case_additional_information;
}

void PF_DATA::append_bus(BUS& bus)
{
    bus.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not bus.is_valid())
    {
        osstream << "Warning. Failed to append invalid bus " << bus.get_bus_number() << " due to either 0 bus number of 0 base voltage.";
        toolkit->show_information(osstream);
        return;
    }

    size_t bus_number = bus.get_bus_number();

    if (not this->is_bus_in_allowed_range(bus_number))
    {
        osstream << "Warning. Bus number " << bus_number << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << bus.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Bus will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    if (this->is_bus_exist(bus_number))
    {
        osstream << "Warning. Bus " << bus_number << " already exists in power system database '" << get_system_name() << "': Bus." << endl
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Bus.capacity() == Bus.size())
    {
        osstream << "Warning. Capacity limit (" << Bus.capacity() << ") reached when appending bus to power system database " << get_system_name() << "." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }
    size_t bus_count = get_bus_count();

    Bus.push_back(bus);

    bus_index.set_bus_with_index(bus_number, bus_count);

    update_in_service_bus_count();
}

void PF_DATA::append_generator(GENERATOR& generator)
{
    generator.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not generator.is_valid())
    {
        osstream << "Warning. Failed to append invalid generator to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    size_t generator_bus = generator.get_generator_bus();

    if (not this->is_bus_in_allowed_range(generator_bus))
    {
        osstream << "Warning. Bus number " << generator_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << generator.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Generator will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    string identifier = generator.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_GENERATOR);
    TERMINAL terminal;
    terminal.append_bus(generator_bus);
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (this->is_generator_exist(device_id))
    {
        osstream << "Warning. " << generator.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': Generator." << endl
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Generator.capacity() == Generator.size())
    {
        osstream << "Warning. Capacity limit (" << Generator.capacity() << ") reached when appending generator to power system database " << get_system_name() << "." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t generator_count = get_generator_count();
    Generator.push_back(generator);
    generator_index.set_device_index(device_id, generator_count);
}

void PF_DATA::append_wt_generator(WT_GENERATOR& wt_generator)
{
    wt_generator.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not wt_generator.is_valid())
    {
        osstream << "Warning. Failed to append invalid wt generator to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    size_t wt_generator_bus = wt_generator.get_source_bus();

    if (not this->is_bus_in_allowed_range(wt_generator_bus))
    {
        osstream << "Warning. Bus " << wt_generator_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << wt_generator.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "WT generator will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    string identifier = wt_generator.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_WT_GENERATOR);
    TERMINAL terminal;
    terminal.append_bus(wt_generator_bus);
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (this->is_wt_generator_exist(device_id))
    {
        osstream << "Warning. " << wt_generator.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': WT_Generator." << endl
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (WT_Generator.capacity() == WT_Generator.size())
    {
        osstream << "Warning. Capacity limit (" << WT_Generator.capacity() << ") reached when appending WT generator to power system database " << get_system_name() << "." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t wt_generator_count = get_wt_generator_count();
    WT_Generator.push_back(wt_generator);
    wt_generator_index.set_device_index(device_id, wt_generator_count);
}

void PF_DATA::append_pv_unit(PV_UNIT& pv_unit)
{
    pv_unit.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not pv_unit.is_valid())
    {
        osstream << "Warning. Failed to append invalid pv unit to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    size_t pv_unit_bus = pv_unit.get_unit_bus();

    if (not this->is_bus_in_allowed_range(pv_unit_bus))
    {
        osstream << "Warning. Bus " << pv_unit_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << pv_unit.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "PV unit will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    string identifier = pv_unit.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_PV_UNIT);
    TERMINAL terminal;
    terminal.append_bus(pv_unit_bus);
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (this->is_pv_unit_exist(device_id))
    {
        osstream << "Warning. " << pv_unit.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': PV_Unit." << endl
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (PV_Unit.capacity() == PV_Unit.size())
    {
        osstream << "Warning. Capacity limit (" << PV_Unit.capacity() << ") reached when appending PV Unit to power system database " << get_system_name() << "." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t pv_unit_count = get_pv_unit_count();
    PV_Unit.push_back(pv_unit);
    pv_unit_index.set_device_index(device_id, pv_unit_count);
}

void PF_DATA::append_energy_storage(ENERGY_STORAGE& estorage)
{
    estorage.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not estorage.is_valid())
    {
        osstream << "Warning. Failed to append invalid energy storage to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    size_t bus = estorage.get_energy_storage_bus();

    if (not this->is_bus_in_allowed_range(bus))
    {
        osstream << "Warning. Bus " << bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << estorage.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Energy storage will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    string identifier = estorage.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_ENERGY_STORAGE);
    TERMINAL terminal;
    terminal.append_bus(bus);
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (this->is_energy_storage_exist(device_id))
    {
        osstream << "Warning. " << estorage.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': Energy_storage.\n"
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Energy_storage.capacity() == Energy_storage.size())
    {
        osstream << "Warning. Capacity limit (" << Energy_storage.capacity() << ") reached when appending energy storage to power system database '" << get_system_name() << "'." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t energy_storage_count = get_energy_storage_count();
    Energy_storage.push_back(estorage);
    energy_storage_index.set_device_index(device_id, energy_storage_count);
}

void PF_DATA::append_load(LOAD& load)
{
    load.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not load.is_valid())
    {
        osstream << "Warning. Failed to append invalid load to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    size_t load_bus = load.get_load_bus();

    if (not this->is_bus_in_allowed_range(load_bus))
    {
        osstream << "Warning. Bus " << load_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << load.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Load will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    string identifier = load.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_LOAD);
    TERMINAL terminal;
    terminal.append_bus(load_bus);
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (is_load_exist(device_id))
    {
        osstream << "Warning. " << load.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': Load." << endl
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Load.capacity() == Load.size())
    {
        osstream << "Warning. Capacity limit (" << Load.capacity() << ") reached when appending load to power system database " << get_system_name() << "." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t load_count = get_load_count();
    Load.push_back(load);
    load_index.set_device_index(device_id, load_count);
}

void PF_DATA::append_ac_line(AC_LINE& line)
{
    line.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not line.is_valid())
    {
        osstream << "Warning. Failed to append invalid line to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    size_t sending_side_bus = line.get_sending_side_bus();
    size_t receiving_side_bus = line.get_receiving_side_bus();

    if (not this->is_bus_in_allowed_range(sending_side_bus))
    {
        osstream << "Warning. Bus " << sending_side_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << line.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Line will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }
    if (not this->is_bus_in_allowed_range(receiving_side_bus))
    {
        osstream << "Warning. Bus " << receiving_side_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << line.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Line will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    string identifier = line.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_AC_LINE);
    TERMINAL terminal;
    terminal.append_bus(sending_side_bus);
    terminal.append_bus(receiving_side_bus);
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (this->is_ac_line_exist(device_id))
    {
        osstream << "Warning. " << line.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': Ac_line." << endl
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Ac_line.capacity() == Ac_line.size())
    {
        osstream << "Warning. Capacity limit (" << Ac_line.capacity() << ") reached when appending line to power system database '" << get_system_name() << "'." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t line_count = get_ac_line_count();
    Ac_line.push_back(line);
    ac_line_index.set_device_index(device_id, line_count);
}

void PF_DATA::append_transformer(TRANSFORMER& transformer)
{
    transformer.set_toolkit(*toolkit);
    ostringstream osstream;

    if (not transformer.is_valid())
    {
        osstream << "Warning. Failed to append invalid transformer to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    size_t primary_winding_bus = transformer.get_winding_bus(PRIMARY_SIDE);
    size_t secondary_winding_bus = transformer.get_winding_bus(SECONDARY_SIDE);
    size_t tertiary_winding_bus = transformer.get_winding_bus(TERTIARY_SIDE);

    if (not is_bus_in_allowed_range(primary_winding_bus))
    {
        osstream << "Warning. Bus " << primary_winding_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << transformer.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Transformer will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }
    if (not is_bus_in_allowed_range(secondary_winding_bus))
    {
        osstream << "Warning. Bus " << secondary_winding_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << transformer.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Transformer will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }
    if (not(is_bus_in_allowed_range(tertiary_winding_bus) or tertiary_winding_bus == 0))
    {
        osstream << "Warning. Bus " << tertiary_winding_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << transformer.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Transformer will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    string identifier = transformer.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_TRANSFORMER);
    TERMINAL terminal;
    terminal.append_bus(primary_winding_bus);
    terminal.append_bus(secondary_winding_bus);
    terminal.append_bus(tertiary_winding_bus);
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (is_transformer_exist(device_id))
    {
        osstream << "Warning. " << transformer.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "':Transformer.\n"
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Transformer.capacity() == Transformer.size())
    {
        osstream << "Warning. Capacity limit (" << Transformer.capacity() << ") reached when appending transformer to power system database '" << get_system_name() << "'." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t transformer_count = get_transformer_count();
    Transformer.push_back(transformer);
    transformer_index.set_device_index(device_id, transformer_count);
}

void PF_DATA::append_fixed_shunt(FIXED_SHUNT& shunt)
{
    shunt.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not shunt.is_valid())
    {
        osstream << "Warning. Failed to append invalid fixed shunt to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    size_t shunt_bus = shunt.get_shunt_bus();

    if (not this->is_bus_in_allowed_range(shunt_bus))
    {
        osstream << "Warning. Bus " << shunt_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << shunt.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Fixed shunt will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    string identifier = shunt.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_FIXED_SHUNT);
    TERMINAL terminal;
    terminal.append_bus(shunt_bus);
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (this->is_fixed_shunt_exist(device_id))
    {
        osstream << "Warning. " << shunt.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': Fixed_shunt.\n"
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Fixed_shunt.capacity() == Fixed_shunt.size())
    {
        osstream << "Warning. Capacity limit (" << Fixed_shunt.capacity() << ") reached when appending fixed shunt to power system database '" << get_system_name() << "'." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t shunt_count = get_fixed_shunt_count();
    Fixed_shunt.push_back(shunt);
    fixed_shunt_index.set_device_index(device_id, shunt_count);
}

void PF_DATA::append_2t_lcc_hvdc(LCC_HVDC2T& hvdc)
{
    hvdc.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not hvdc.is_valid())
    {
        osstream << "Warning. Failed to append invalid hvdc to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    size_t rec_bus = hvdc.get_converter_bus(RECTIFIER);
    size_t inv_bus = hvdc.get_converter_bus(INVERTER);

    if (not this->is_bus_in_allowed_range(rec_bus))
    {
        osstream << "Warning. Bus " << rec_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << hvdc.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Hvdc will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }
    if (not this->is_bus_in_allowed_range(inv_bus))
    {
        osstream << "Warning. Bus " << inv_bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << hvdc.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Hvdc will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    string identifier = hvdc.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_LCC_HVDC2T);
    TERMINAL terminal;
    terminal.append_bus(rec_bus);
    terminal.append_bus(inv_bus);
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (this->is_2t_lcc_hvdc_exist(device_id))
    {
        osstream << "Warning. " << hvdc.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': Lcc_hvdc2t.\n"
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Lcc_hvdc2t.capacity() == Lcc_hvdc2t.size())
    {
        osstream << "Warning. Capacity limit (" << Lcc_hvdc2t.capacity() << ") reached when appending Hvdc to power system database '" << get_system_name() << "'." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t hvdc_count = get_2t_lcc_hvdc_count();
    Lcc_hvdc2t.push_back(hvdc);
    hvdc_index.set_device_index(device_id, hvdc_count);
}

void PF_DATA::append_vsc_hvdc(VSC_HVDC& vsc_hvdc)
{
    vsc_hvdc.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not vsc_hvdc.is_valid())
    {
        osstream << "Warning. Failed to append invalid VSC LCC_HVDC2T to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }
    string identifier = vsc_hvdc.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_VSC_HVDC);
    TERMINAL terminal;
    size_t n_converter = vsc_hvdc.get_converter_count();
    for (size_t i = 0; i != n_converter; ++i)
    {
        size_t bus_number = vsc_hvdc.get_converter_ac_bus(i);
        terminal.append_bus(bus_number);
    }
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (this->is_vsc_hvdc_exist(device_id))
    {
        osstream << "Warning. " << vsc_hvdc.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': Vsc_hvdc.\n"
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Vsc_hvdc.capacity() == Vsc_hvdc.size())
    {
        osstream << "Warning. Capacity limit (" << Vsc_hvdc.capacity() << ") reached when appending VSC LCC_HVDC2T to power system database '" << get_system_name() << "'." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }
    size_t vsc_hvdc_count = get_vsc_hvdc_count();
    Vsc_hvdc.push_back(vsc_hvdc);
    vsc_hvdc_index.set_device_index(device_id, vsc_hvdc_count);
}

void PF_DATA::append_equivalent_device(EQUIVALENT_DEVICE& edevice)
{
    edevice.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not edevice.is_valid())
    {
        osstream << "Warning. Failed to append invalid equivalent device to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    size_t bus = edevice.get_equivalent_device_bus();

    if (not this->is_bus_in_allowed_range(bus))
    {
        osstream << "Warning. Bus " << bus << " is not in the allowed range [1, " << get_allowed_max_bus_number() << "] when appending " << edevice.get_compound_device_name() << " to power system database '" << get_system_name() << "'." << endl
                 << "Equivalent device will not be appended into the database.";
        toolkit->show_information(osstream);
        return;
    }

    string identifier = edevice.get_identifier();
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_EQUIVALENT_DEVICE);
    TERMINAL terminal;
    terminal.append_bus(bus);
    device_id.set_device_terminal(terminal);
    device_id.set_device_identifier_index(get_index_of_string(identifier));

    if (this->is_equivalent_device_exist(device_id))
    {
        osstream << "Warning. " << edevice.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': Equivalent_device.\n"
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Equivalent_device.capacity() == Equivalent_device.size())
    {
        osstream << "Warning. Capacity limit (" << Equivalent_device.capacity() << ") reached when appending equivalent device to power system database '" << get_system_name() << "'." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t equivalent_device_count = get_equivalent_device_count();
    Equivalent_device.push_back(edevice);
    equivalent_device_index.set_device_index(device_id, equivalent_device_count);
}

void PF_DATA::append_lcc_hvdc(LCC_HVDC& lcc_hvdc)
{
    lcc_hvdc.set_toolkit(*toolkit);

    ostringstream osstream;

    if (not lcc_hvdc.is_valid())
    {
        osstream << "Warning. Failed to append invalid LCC LCC_HVDC2T to power system database '" << get_system_name() << "'.";
        toolkit->show_information(osstream);
        return;
    }

    string name = lcc_hvdc.get_name();
    add_string_to_str_int_map(name);
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_LCC_HVDC);
    device_id.set_device_name_index(get_index_of_string(name));

    if (this->is_lcc_hvdc_exist(device_id))
    {
        osstream << "Warning. " << lcc_hvdc.get_compound_device_name() << " already exists in power system database '" << get_system_name() << "': Lcc_hvdc.\n"
                 << "Duplicate copy is not allowed.";
        toolkit->show_information(osstream);
        return;
    }

    if (Lcc_hvdc.capacity() == Lcc_hvdc.size())
    {
        osstream << "Warning. Capacity limit (" << Lcc_hvdc.capacity() << ") reached when appending LCC LCC_HVDC2T to power system database '" << get_system_name() << "'." << endl
                 << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
        return;
    }

    size_t lcc_hvdc_count = get_lcc_hvdc_count();
    Lcc_hvdc.push_back(lcc_hvdc);
    lcc_hvdc_index.set_device_index(device_id, lcc_hvdc_count);
}

void PF_DATA::append_area(AREA& area)
{
    area.set_toolkit(*toolkit);

    if (area.is_valid() and
            (not this->is_area_exist(area.get_area_number())) and
            Area_.capacity() > Area_.size())
    {
        size_t area_count = get_area_count();
        Area_.push_back(area);
        area_index[area.get_area_number()] = area_count;
    }
    else
    {
        ostringstream osstream;
        if (not area.is_valid())
        {
            osstream << "Warning. Failed to append invalid area to power system database '" << get_system_name() << "'.";
        }
        if (this->is_area_exist(area.get_area_number()))
            osstream << "Warning. Area " << area.get_area_number() << " already exists in power system database '" << get_system_name() << "': Area." << endl
                     << "Duplicate copy is not allowed.";
        if (Area_.capacity() == Area_.size())
            osstream << "Warning. Capacity limit (" << Area_.capacity() << ") reached when appending area to power system database '" << get_system_name() << "'." << endl
                     << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
    }
}

void PF_DATA::append_zone(ZONE& zone)
{
    zone.set_toolkit(*toolkit);

    if (zone.is_valid() and
            (not this->is_zone_exist(zone.get_zone_number())) and
            Zone_.capacity() > Zone_.size())
    {
        size_t zone_count = get_zone_count();
        Zone_.push_back(zone);
        zone_index[zone.get_zone_number()] = zone_count;
    }
    else
    {
        ostringstream osstream;
        if (not zone.is_valid())
        {
            osstream << "Warning. Failed to append invalid zone to power system database '" << get_system_name() << "'.";
        }
        if (this->is_zone_exist(zone.get_zone_number()))
            osstream << "Warning. Zone " << zone.get_zone_number() << " already exists in power system database '" << get_system_name() << "': Zone." << endl
                     << "Duplicate copy is not allowed.";
        if (Zone_.capacity() == Zone_.size())
            osstream << "Warning. Capacity limit (" << Zone_.capacity() << ") reached when appending zone to power system database '" << get_system_name() << "'." << endl
                     << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
    }
}

void PF_DATA::append_owner(OWNER& owner)
{
    owner.set_toolkit(*toolkit);

    if (owner.is_valid() and
            (not this->is_owner_exist(owner.get_owner_number())) and
            Owner_.capacity() > Owner_.size())
    {
        size_t owner_count = get_owner_count();
        Owner_.push_back(owner);
        owner_index[owner.get_owner_number()] = owner_count;
    }
    else
    {
        ostringstream osstream;
        if (not owner.is_valid())
        {
            osstream << "Warning. Failed to append invalid owner to power system database '" << get_system_name() << "'.";
        }
        if (this->is_owner_exist(owner.get_owner_number()))
            osstream << "Warning. Owner " << owner.get_owner_number() << " already exists in power system database '" << get_system_name() << "': Owner." << endl
                     << "Duplicate copy is not allowed.";
        if (Owner_.capacity() == Owner_.size())
            osstream << "Warning. Capacity limit (" << Owner_.capacity() << ") reached when appending owner to power system database '" << get_system_name() << "'." << endl
                     << "Increase capacity by modified steps_config.json.";
        toolkit->show_information(osstream);
    }
}

void PF_DATA::append_mutual_data(MUTUAL_DATA& mutual_data)
{
    mutual_data.set_toolkit(*toolkit);
    Mutual_data.push_back(mutual_data);
}

void PF_DATA::update_device_id(const DEVICE_ID& did_old, const DEVICE_ID& did_new)
{
    ostringstream osstream;
    STEPS_DEVICE_TYPE old_device_type = did_old.get_device_type();
    STEPS_DEVICE_TYPE new_device_type = did_new.get_device_type();
    if (old_device_type != new_device_type)
    {
        osstream << "Device ID types are inconsistent with each other when calling " << __FUNCTION__ << ":\n"
                 << "Old device type is: " << device_type2string(old_device_type) << ". New device type is: " << device_type2string(new_device_type) << ".\n"
                 << "No device id will be updated.";
        toolkit->show_information(osstream);
        return;
    }
    STEPS_DEVICE_TYPE device_type = old_device_type;
    if (device_type != STEPS_GENERATOR and device_type != STEPS_WT_GENERATOR and device_type != STEPS_PV_UNIT and
            device_type != STEPS_ENERGY_STORAGE and device_type != STEPS_LOAD and device_type != STEPS_FIXED_SHUNT and
            device_type != STEPS_AC_LINE and device_type != STEPS_TRANSFORMER and device_type != STEPS_LCC_HVDC2T and
            device_type != STEPS_VSC_HVDC and device_type != STEPS_LCC_HVDC)
    {
        osstream << "Device ID type is not in the following allowed types when calling " << __FUNCTION__ << ":\n"
                 << "[GENERATOR, WT GENERATOR, PV UNIT, ENERGY STORAGE, LOAD, FIXED SHUNT, LINE, TRANSFORMER, 2T LCC HVDC, VSC HVDC, LCC HVDC].\n"
                 << "The input device type is: " << device_type2string(device_type) << ".\n"
                 << "No device id will be updated.";
        toolkit->show_information(osstream);
        return;
    }

    TERMINAL new_terminal = did_new.get_device_terminal();
    string new_id = did_new.get_device_identifier();
    if (device_type == STEPS_VSC_HVDC)
    {
        if (not is_vsc_hvdc_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_vsc_hvdc_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        VSC_HVDC* vsc_hvdc = get_vsc_hvdc(did_old);
        vsc_hvdc_index.swap_device_index(did_old, did_new);
        size_t n_bus = new_terminal.get_bus_count();
        for (size_t i = 0; i != n_bus; ++i)
        {
            vsc_hvdc->set_converter_ac_bus(i, new_terminal[i]);
        }

        vsc_hvdc->set_identifier(new_id);
        return;

    }
    // The following handle devices with buses <=3
    size_t ibus = new_terminal[0];
    size_t jbus = new_terminal[1];
    size_t kbus = new_terminal[2];

    if (device_type == STEPS_GENERATOR)
    {
        if (not is_generator_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_generator_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        GENERATOR* gen = get_generator(did_old);
        generator_index.swap_device_index(did_old, did_new);
        gen->set_generator_bus(ibus);
        gen->set_identifier(new_id);
        return;
    }

    if (device_type == STEPS_WT_GENERATOR)
    {
        if (not is_wt_generator_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_wt_generator_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        WT_GENERATOR* gen = get_wt_generator(did_old);
        wt_generator_index.swap_device_index(did_old, did_new);
        gen->set_generator_bus(ibus);
        gen->set_identifier(new_id);
        return;
    }

    if (device_type == STEPS_PV_UNIT)
    {
        if (not is_pv_unit_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_pv_unit_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        PV_UNIT* pv = get_pv_unit(did_old);
        pv_unit_index.swap_device_index(did_old, did_new);
        pv->set_unit_bus(ibus);
        pv->set_identifier(new_id);
        return;
    }

    if (device_type == STEPS_ENERGY_STORAGE)
    {
        if (not is_energy_storage_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_energy_storage_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        ENERGY_STORAGE* es = get_energy_storage(did_old);
        energy_storage_index.swap_device_index(did_old, did_new);
        es->set_energy_storage_bus(ibus);
        es->set_identifier(new_id);
        return;
    }

    if (device_type == STEPS_LOAD)
    {
        if (not is_load_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_load_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        LOAD* load = get_load(did_old);
        load_index.swap_device_index(did_old, did_new);
        load->set_load_bus(ibus);
        load->set_identifier(new_id);
        return;
    }

    if (device_type == STEPS_FIXED_SHUNT)
    {
        if (not is_fixed_shunt_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_fixed_shunt_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        FIXED_SHUNT* fixed_shunt = get_fixed_shunt(did_old);
        fixed_shunt_index.swap_device_index(did_old, did_new);
        fixed_shunt->set_shunt_bus(ibus);
        fixed_shunt->set_identifier(new_id);
        return;
    }

    if (device_type == STEPS_AC_LINE)
    {
        if (not is_ac_line_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_ac_line_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        AC_LINE* line = get_ac_line(did_old);
        ac_line_index.swap_device_index(did_old, did_new);
        line->set_sending_side_bus(ibus);
        line->set_receiving_side_bus(jbus);
        line->set_identifier(new_id);
        return;
    }

    if (device_type == STEPS_TRANSFORMER)
    {
        if (not is_transformer_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_transformer_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        TERMINAL old_terminal = did_old.get_device_terminal();
        if (old_terminal.get_bus_count() != new_terminal.get_bus_count())
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " bus count is inconsistent with the old device " << did_old.get_compound_device_name() << ". No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        TRANSFORMER* trans = get_transformer(did_old);
        transformer_index.swap_device_index(did_old, did_new);
        trans->set_winding_bus(PRIMARY_SIDE, ibus);
        trans->set_winding_bus(SECONDARY_SIDE, jbus);
        trans->set_winding_bus(TERTIARY_SIDE, kbus);
        trans->set_identifier(new_id);
        return;
    }

    if (device_type == STEPS_LCC_HVDC2T)
    {
        if (not is_2t_lcc_hvdc_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_2t_lcc_hvdc_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        LCC_HVDC2T* hvdc = get_2t_lcc_hvdc(did_old);
        hvdc_index.swap_device_index(did_old, did_new);
        hvdc->set_converter_bus(RECTIFIER, ibus);
        hvdc->set_converter_bus(INVERTER, jbus);
        hvdc->set_identifier(new_id);
        return;
    }

    if (device_type == STEPS_LCC_HVDC)
    {
        if (not is_lcc_hvdc_exist(did_old))
        {
            osstream << "Warning. The old device " << did_old.get_compound_device_name() << " does not exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        if (is_lcc_hvdc_exist(did_new))
        {
            osstream << "Warning. The new device " << did_new.get_compound_device_name() << " already exist in database. No device id will be updated.";
            toolkit->show_information(osstream);
            return;
        }
        LCC_HVDC* hvdc = get_lcc_hvdc(did_old);
        lcc_hvdc_index.swap_device_index(did_old, did_new);
        hvdc->set_name(did_new.get_device_name());
        return;
    }
}

void PF_DATA::update_all_bus_base_frequency(double fbase_Hz)
{
    size_t n = Bus.size();
    for (size_t i = 0; i != n; ++i)
    {
        Bus[i].set_base_frequency_in_Hz(fbase_Hz);
    }
}


bool PF_DATA::is_bus_exist(size_t bus) const
{
    if (bus != 0)
    {
        size_t index = this->get_bus_index(bus);
        if (index != INDEX_NOT_EXIST)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_generator_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_generator_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_wt_generator_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_wt_generator_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_pv_unit_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_pv_unit_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_load_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_load_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_ac_line_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_ac_line_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_transformer_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_transformer_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_fixed_shunt_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_fixed_shunt_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_2t_lcc_hvdc_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_2t_lcc_hvdc_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_vsc_hvdc_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_vsc_hvdc_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_vsc_hvdc_exist(const string vsc_name) const
{
    size_t index = get_vsc_hvdc_index(vsc_name);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_equivalent_device_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_equivalent_device_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_energy_storage_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_energy_storage_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_lcc_hvdc_exist(const DEVICE_ID& device_id) const
{
    size_t index = get_lcc_hvdc_index(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_area_exist(const size_t no) const
{
    size_t index = get_area_index(no);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_zone_exist(const size_t no) const
{
    size_t index = get_zone_index(no);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PF_DATA::is_owner_exist(const size_t no) const
{
    size_t index = get_owner_index(no);
    if (index != INDEX_NOT_EXIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PF_DATA::change_bus_number(size_t original_bus_number, size_t new_bus_number)
{
    ostringstream osstream;

    if (is_bus_exist(original_bus_number) and
            (not is_bus_exist(new_bus_number)) and
            is_bus_in_allowed_range(new_bus_number))
    {
        BUS* bus = get_bus(original_bus_number);

        bus->set_bus_number(new_bus_number);

        bus_index.set_bus_with_index(new_bus_number, get_bus_index(original_bus_number));
        bus_index.set_bus_with_index(original_bus_number, INDEX_NOT_EXIST);

        vector<GENERATOR*> gens = get_generators_connecting_to_bus(original_bus_number);
        size_t ngens = gens.size();
        for (size_t i = 0; i != ngens; ++i)
        {
            GENERATOR* gen = gens[i];
            DEVICE_ID old_did = gen->get_device_id();
            size_t index = get_generator_index(old_did);
            generator_index.set_device_index(old_did, INDEX_NOT_EXIST);

            gen->set_generator_bus(new_bus_number);
            DEVICE_ID new_did = gen->get_device_id();
            generator_index.set_device_index(new_did, index);
        }

        vector<WT_GENERATOR*> wt_generators = get_wt_generators_connecting_to_bus(original_bus_number);
        size_t nwt_generator = wt_generators.size();
        for (size_t i = 0; i != nwt_generator; ++i)
        {
            WT_GENERATOR* wt_generator = wt_generators[i];
            DEVICE_ID old_did = wt_generator->get_device_id();
            size_t index = get_wt_generator_index(old_did);
            wt_generator_index.set_device_index(old_did, INDEX_NOT_EXIST);

            wt_generator->set_source_bus(new_bus_number);
            DEVICE_ID new_did = wt_generator->get_device_id();
            wt_generator_index.set_device_index(new_did, index);
        }

        vector<PV_UNIT*> pv_units = get_pv_units_connecting_to_bus(original_bus_number);
        size_t npv_unit = pv_units.size();
        for (size_t i = 0; i != npv_unit; ++i)
        {
            PV_UNIT* pv_unit = pv_units[i];
            DEVICE_ID old_did = pv_unit->get_device_id();
            size_t index = get_pv_unit_index(old_did);
            pv_unit_index.set_device_index(old_did, INDEX_NOT_EXIST);

            pv_unit->set_unit_bus(new_bus_number);
            DEVICE_ID new_did = pv_unit->get_device_id();
            pv_unit_index.set_device_index(new_did, index);
        }

        vector<ENERGY_STORAGE*> estorages = get_energy_storages_connecting_to_bus(original_bus_number);
        size_t nestorage = estorages.size();
        for (size_t i = 0; i != nestorage; ++i)
        {
            ENERGY_STORAGE* estorage = estorages[i];
            DEVICE_ID old_did = estorage->get_device_id();
            size_t index = get_energy_storage_index(old_did);
            energy_storage_index.set_device_index(old_did, INDEX_NOT_EXIST);

            estorage->set_energy_storage_bus(new_bus_number);
            DEVICE_ID new_did = estorage->get_device_id();
            energy_storage_index.set_device_index(new_did, index);
        }

        vector<LOAD*> loads = get_loads_connecting_to_bus(original_bus_number);
        size_t nload = loads.size();
        for (size_t i = 0; i != nload; ++i)
        {
            LOAD* load = loads[i];
            DEVICE_ID old_did = load->get_device_id();
            size_t index = get_load_index(old_did);
            load_index.set_device_index(old_did, INDEX_NOT_EXIST);

            load->set_load_bus(new_bus_number);
            DEVICE_ID new_did = load->get_device_id();
            load_index.set_device_index(new_did, index);
        }

        vector<FIXED_SHUNT*> fshunts = get_fixed_shunts_connecting_to_bus(original_bus_number);
        size_t nfshunt = fshunts.size();
        for (size_t i = 0; i != nfshunt; ++i)
        {
            FIXED_SHUNT* fshunt = fshunts[i];
            DEVICE_ID old_did = fshunt->get_device_id();
            size_t index = get_fixed_shunt_index(old_did);
            fixed_shunt_index.set_device_index(old_did, INDEX_NOT_EXIST);

            fshunt->set_shunt_bus(new_bus_number);
            DEVICE_ID new_did = fshunt->get_device_id();
            fixed_shunt_index.set_device_index(new_did, index);
        }

        vector<AC_LINE*> lines = get_ac_lines_connecting_to_bus(original_bus_number);
        size_t nline = lines.size();
        for (size_t i = 0; i != nline; ++i)
        {
            AC_LINE* line = lines[i];
            DEVICE_ID old_did = line->get_device_id();
            size_t index = get_ac_line_index(old_did);
            ac_line_index.set_device_index(old_did, INDEX_NOT_EXIST);

            if (line->get_sending_side_bus() == original_bus_number)
            {
                line->set_sending_side_bus(new_bus_number);
            }
            else
            {
                line->set_receiving_side_bus(new_bus_number);
            }

            DEVICE_ID new_did = line->get_device_id();
            ac_line_index.set_device_index(new_did, index);

        }

        vector<LCC_HVDC2T*> hvdcs = get_2t_lcc_hvdcs_connecting_to_bus(original_bus_number);
        size_t nhvdc = hvdcs.size();
        for (size_t i = 0; i != nhvdc; ++i)
        {
            LCC_HVDC2T* hvdc = hvdcs[i];
            DEVICE_ID old_did = hvdc->get_device_id();
            size_t index = get_2t_lcc_hvdc_index(old_did);
            hvdc_index.set_device_index(old_did, INDEX_NOT_EXIST);

            if (hvdc->get_converter_bus(RECTIFIER) == original_bus_number)
            {
                hvdc->set_converter_bus(RECTIFIER, new_bus_number);
            }
            else
            {
                hvdc->set_converter_bus(INVERTER, new_bus_number);
            }

            DEVICE_ID new_did = hvdc->get_device_id();
            hvdc_index.set_device_index(new_did, index);
        }

        vector<TRANSFORMER*> transformers = get_transformers_connecting_to_bus(original_bus_number);
        size_t ntrans = transformers.size();
        for (size_t i = 0; i != ntrans; ++i)
        {
            TRANSFORMER* trans = transformers[i];
            DEVICE_ID old_did = trans->get_device_id();
            size_t index = get_transformer_index(old_did);
            transformer_index.set_device_index(old_did, INDEX_NOT_EXIST);

            if (trans->get_winding_bus(PRIMARY_SIDE) == original_bus_number)
            {
                trans->set_winding_bus(PRIMARY_SIDE, new_bus_number);
            }
            else
            {
                if (trans->get_winding_bus(SECONDARY_SIDE) == original_bus_number)
                {
                    trans->set_winding_bus(SECONDARY_SIDE, new_bus_number);
                }
                else
                {
                    trans->set_winding_bus(TERTIARY_SIDE, new_bus_number);
                }
            }

            DEVICE_ID new_did = trans->get_device_id();
            transformer_index.set_device_index(new_did, index);
        }

        vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_connecting_to_bus(original_bus_number);
        size_t nvsc = vsc_hvdcs.size();
        for (size_t i = 0; i != nvsc; ++i)
        {
            VSC_HVDC* vsc_hvdc = vsc_hvdcs[i];
            DEVICE_ID old_did = vsc_hvdc->get_device_id();
            size_t index = get_vsc_hvdc_index(old_did);
            vsc_hvdc_index.set_device_index(old_did, INDEX_NOT_EXIST);

            size_t n_converter = vsc_hvdc->get_converter_count();
            for (size_t j = 0; j != n_converter; ++j)
            {
                if (vsc_hvdc->get_converter_ac_bus(j) == original_bus_number)
                {
                    vsc_hvdc->set_converter_ac_bus(j, new_bus_number);
                    break;
                }
            }

            DEVICE_ID new_did = vsc_hvdc->get_device_id();
            vsc_hvdc_index.set_device_index(new_did, index);
        }

        vector<AREA*> areas = get_all_areas();
        size_t narea = areas.size();
        for (size_t i = 0; i != narea; ++i)
        {
            AREA* area = areas[i];
            if (area->get_area_swing_bus() == original_bus_number)
            {
                area->set_area_swing_bus(new_bus_number);
            }
        }
    }
    else
    {
        if (not is_bus_exist(original_bus_number))
        {
            osstream << "Warning. The original bus number to change (" << original_bus_number << ") does not exist in the power system database. No bus number will be changed.";
        }
        if (is_bus_exist(new_bus_number))
        {
            osstream << "Warning. The new bus number to change (" << new_bus_number << ") already exists in the power system database. No bus number will be changed.";
        }
        if (not is_bus_in_allowed_range(new_bus_number))
        {
            osstream << "Warning. The new bus number to change (" << new_bus_number << ") exceeds the allowed maximum bus number range [1, " << get_allowed_max_bus_number() << "] in the power system database. No bus number will be changed.";
        }
        toolkit->show_information(osstream);
    }
}

void PF_DATA::change_bus_number_with_file(string file)
{
    FILE* fid = fopen(file.c_str(), "rt");
    if (fid == NULL)
    {
        ostringstream osstream;
        osstream << "Bus number pair file '" << file << "' is not accessible. Change bus number with file is failed.";
        toolkit->show_information(osstream);
        return;
    }

    const size_t buffer_size = 64;
    char buffer[buffer_size];
    string sbuffer;
    vector<string> splitted_str;
    while (true)
    {
        if (fgets(buffer, buffer_size, fid) == NULL)
        {
            break;
        }
        sbuffer = trim_string(buffer);
        splitted_str = split_string(sbuffer, ", ");
        if (splitted_str.size() == 2)
        {
            size_t original_bus_number = str2int(splitted_str[0]);
            size_t new_bus_number = str2int(splitted_str[1]);
            change_bus_number(original_bus_number, new_bus_number);
        }
        else
        {
            break;
        }
    }
    fclose(fid);
}

DEVICE* PF_DATA::get_device(const DEVICE_ID& device_id)
{
    // this function is not tested
    if (device_id.is_valid())
    {
        STEPS_DEVICE_TYPE dtype = device_id.get_device_type();
        if (dtype == STEPS_BUS)
        {
            return get_bus(device_id);
        }
        else
        {
            return get_nonbus_device(device_id);
        }
    }
    else
    {
        return NULL;
    }
}

NONBUS_DEVICE* PF_DATA::get_nonbus_device(const DEVICE_ID& device_id)
{
    // this function is not tested
    if (device_id.is_valid())
    {
        STEPS_DEVICE_TYPE dtype = device_id.get_device_type();
        if (dtype == STEPS_GENERATOR)
        {
            return get_generator(device_id);
        }

        if (dtype == STEPS_WT_GENERATOR)
        {
            return get_wt_generator(device_id);
        }

        if (dtype == STEPS_PV_UNIT)
        {
            return get_pv_unit(device_id);
        }

        if (dtype == STEPS_ENERGY_STORAGE)
        {
            return get_energy_storage(device_id);
        }

        if (dtype == STEPS_LOAD)
        {
            return get_load(device_id);
        }

        if (dtype == STEPS_FIXED_SHUNT)
        {
            return get_fixed_shunt(device_id);
        }

        if (dtype == STEPS_AC_LINE)
        {
            return get_ac_line(device_id);
        }

        if (dtype == STEPS_LCC_HVDC2T)
        {
            return get_2t_lcc_hvdc(device_id);
        }

        if (dtype == STEPS_VSC_HVDC)
        {
            return get_vsc_hvdc(device_id);
        }

        if (dtype == STEPS_TRANSFORMER)
        {
            return get_transformer(device_id);
        }

        if (dtype == STEPS_EQUIVALENT_DEVICE)
        {
            return get_equivalent_device(device_id);
        }

        if (dtype == STEPS_LCC_HVDC)
        {
            return get_lcc_hvdc(device_id);
        }

        return NULL;
    }
    else
    {
        return NULL;
    }
}

BUS* PF_DATA::get_bus(size_t bus)
{
    size_t index = bus_index.get_index_of_bus(bus);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Bus[index]);
    }
    else
    {
        return NULL;
    }
}

BUS* PF_DATA::get_bus(const DEVICE_ID & device_id)
{
    if (device_id.is_valid())
    {
        if (device_id.get_device_type() == STEPS_BUS)
        {
            TERMINAL terminal = device_id.get_device_terminal();
            size_t bus = terminal[0];
            return get_bus(bus);
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

GENERATOR* PF_DATA::get_generator(const DEVICE_ID & device_id)
{
    size_t index = generator_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Generator[index]);
    }
    else
    {
        return NULL;
    }
}

WT_GENERATOR* PF_DATA::get_wt_generator(const DEVICE_ID & device_id)
{
    size_t index = wt_generator_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(WT_Generator[index]);
    }
    else
    {
        return NULL;
    }
}

PV_UNIT* PF_DATA::get_pv_unit(const DEVICE_ID & device_id)
{
    size_t index = pv_unit_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(PV_Unit[index]);
    }
    else
    {
        return NULL;
    }
}

SOURCE* PF_DATA::get_source(const DEVICE_ID & device_id)
{
    SOURCE* source = NULL;
    source = (SOURCE*) get_generator(device_id);
    if (source != NULL)
    {
        return source;
    }
    else
    {
        source = (SOURCE*) get_wt_generator(device_id);
        if (source != NULL)
        {
            return source;
        }
        else
        {
            source = (SOURCE*) get_pv_unit(device_id);
            if (source != NULL)
            {
                return source;
            }
            else
            {
                source = (SOURCE*) get_energy_storage(device_id);
                return source;
            }
        }
    }
}

LOAD* PF_DATA::get_load(const DEVICE_ID & device_id)
{
    size_t index = load_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Load[index]);
    }
    else
    {
        return NULL;
    }
}

AC_LINE* PF_DATA::get_ac_line(const DEVICE_ID & device_id)
{
    size_t index = ac_line_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Ac_line[index]);
    }
    else
    {
        return NULL;
    }
}

TRANSFORMER* PF_DATA::get_transformer(const DEVICE_ID & device_id)
{
    size_t index = transformer_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Transformer[index]);
    }
    else
    {
        return NULL;
    }
}

FIXED_SHUNT* PF_DATA::get_fixed_shunt(const DEVICE_ID & device_id)
{
    size_t index = fixed_shunt_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Fixed_shunt[index]);
    }
    else
    {
        return NULL;
    }
}

LCC_HVDC2T* PF_DATA::get_2t_lcc_hvdc(const DEVICE_ID & device_id)
{
    size_t index = hvdc_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Lcc_hvdc2t[index]);
    }
    else
    {
        return NULL;
    }
}

VSC_HVDC* PF_DATA::get_vsc_hvdc(const DEVICE_ID & device_id)
{
    size_t index = vsc_hvdc_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Vsc_hvdc[index]);
    }
    else
    {
        return NULL;
    }
}

VSC_HVDC* PF_DATA::get_vsc_hvdc(string name)
{
    size_t n = Vsc_hvdc.size();
    for (size_t index = 0; index != n; ++index)
    {
        if (Vsc_hvdc[index].get_name() == name)
        {
            return &(Vsc_hvdc[index]);
        }
    }
    return NULL;
}

EQUIVALENT_DEVICE* PF_DATA::get_equivalent_device(const DEVICE_ID & device_id)
{
    size_t index = equivalent_device_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Equivalent_device[index]);
    }
    else
    {
        return NULL;
    }
}

ENERGY_STORAGE* PF_DATA::get_energy_storage(const DEVICE_ID & device_id)
{
    size_t index = energy_storage_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Energy_storage[index]);
    }
    else
    {
        return NULL;
    }
}

LCC_HVDC* PF_DATA::get_lcc_hvdc(const DEVICE_ID & device_id)
{
    size_t index = lcc_hvdc_index.get_index_of_device(device_id);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Lcc_hvdc[index]);
    }
    else
    {
        return NULL;
    }
}

AREA* PF_DATA::get_area(const size_t no)
{
    size_t index = get_area_index(no);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Area_[index]);
    }
    else
    {
        return NULL;
    }
}

ZONE* PF_DATA::get_zone(const size_t no)
{
    size_t index = get_zone_index(no);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Zone_[index]);
    }
    else
    {
        return NULL;
    }
}

OWNER* PF_DATA::get_owner(const size_t no)
{
    size_t index = get_owner_index(no);
    if (index != INDEX_NOT_EXIST)
    {
        return &(Owner_[index]);
    }
    else
    {
        return NULL;
    }
}

vector<DEVICE*> PF_DATA::get_all_devices_connecting_to_bus(const size_t bus)
{
    vector<DEVICE*> devices;
    size_t n = 0;

    vector<SOURCE*> sources = get_sources_connecting_to_bus(bus);
    n = sources.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(sources[i]);
    }

    vector<LOAD*> loads = get_loads_connecting_to_bus(bus);
    n = loads.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(loads[i]);
    }

    vector<AC_LINE*> lines = get_ac_lines_connecting_to_bus(bus);
    n = lines.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(lines[i]);
    }

    vector<TRANSFORMER*> transes = get_transformers_connecting_to_bus(bus);
    n = transes.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(transes[i]);
    }

    vector<FIXED_SHUNT*> fshunts = get_fixed_shunts_connecting_to_bus(bus);
    n = fshunts.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(fshunts[i]);
    }

    vector<LCC_HVDC2T*> hvdcs = get_2t_lcc_hvdcs_connecting_to_bus(bus);
    n = hvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(hvdcs[i]);
    }

    vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_connecting_to_bus(bus);
    n = vsc_hvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(vsc_hvdcs[i]);
    }

    vector<EQUIVALENT_DEVICE*> edevices = get_equivalent_devices_connecting_to_bus(bus);
    n = edevices.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(edevices[i]);
    }

    vector<LCC_HVDC*> lhvdcs = get_lcc_hvdcs_connecting_to_bus(bus);
    n = lhvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(lhvdcs[i]);
    }

    return devices;
}

vector<GENERATOR*> PF_DATA::get_generators_connecting_to_bus(const size_t bus)
{
    vector<GENERATOR*> device;
    device.reserve(8);

    size_t n = get_generator_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Generator[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(Generator[i]));
        }
    }
    return device;
}

vector<WT_GENERATOR*> PF_DATA::get_wt_generators_connecting_to_bus(const size_t bus)
{
    vector<WT_GENERATOR*> device;
    device.reserve(8);

    size_t n = get_wt_generator_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not WT_Generator[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(WT_Generator[i]));
        }
    }
    return device;
}


vector<PV_UNIT*> PF_DATA::get_pv_units_connecting_to_bus(const size_t bus)
{
    vector<PV_UNIT*> device;
    device.reserve(8);

    size_t n = get_pv_unit_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not PV_Unit[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(PV_Unit[i]));
        }
    }
    return device;
}

vector<SOURCE*> PF_DATA::get_sources_connecting_to_bus(const size_t bus)
{
    vector<SOURCE*> device;
    vector<GENERATOR*> gen_device;
    vector<WT_GENERATOR*> wt_generator_device;
    vector<PV_UNIT*> pv_unit_device;
    vector<ENERGY_STORAGE*> estorage_device;
    device.reserve(8);

    gen_device = get_generators_connecting_to_bus(bus);
    size_t ngens = gen_device.size();
    for (size_t i = 0; i != ngens; ++i)
    {
        device.push_back(gen_device[i]);
    }

    wt_generator_device = get_wt_generators_connecting_to_bus(bus);
    size_t nwt_generator = wt_generator_device.size();
    for (size_t i = 0; i != nwt_generator; ++i)
    {
        device.push_back(wt_generator_device[i]);
    }

    pv_unit_device = get_pv_units_connecting_to_bus(bus);
    size_t npv_unit = pv_unit_device.size();
    for (size_t i = 0; i != npv_unit; ++i)
    {
        device.push_back(pv_unit_device[i]);
    }

    estorage_device = get_energy_storages_connecting_to_bus(bus);
    size_t nes = estorage_device.size();
    for (size_t i = 0; i != nes; ++i)
    {
        device.push_back(estorage_device[i]);
    }

    return device;
}

vector<LOAD*> PF_DATA::get_loads_connecting_to_bus(const size_t bus)
{
    vector<LOAD*> device;
    device.reserve(8);

    size_t n = get_load_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Load[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(Load[i]));
        }
    }
    return device;
}

vector<AC_LINE*> PF_DATA::get_ac_lines_connecting_to_bus(const size_t bus)
{
    vector<AC_LINE*> device;
    device.reserve(8);

    size_t n = get_ac_line_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Ac_line[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(Ac_line[i]));
        }
    }
    return device;
}

vector<TRANSFORMER*> PF_DATA::get_transformers_connecting_to_bus(const size_t bus)
{
    vector<TRANSFORMER*> device;
    device.reserve(8);

    size_t n = get_transformer_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Transformer[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(Transformer[i]));
        }
    }
    return device;
}

vector<FIXED_SHUNT*> PF_DATA::get_fixed_shunts_connecting_to_bus(const size_t bus)
{
    vector<FIXED_SHUNT*> device;
    device.reserve(8);

    size_t n = get_fixed_shunt_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Fixed_shunt[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(Fixed_shunt[i]));
        }
    }
    return device;
}

vector<LCC_HVDC2T*> PF_DATA::get_2t_lcc_hvdcs_connecting_to_bus(const size_t bus)
{
    vector<LCC_HVDC2T*> device;
    device.reserve(8);

    size_t n = get_2t_lcc_hvdc_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Lcc_hvdc2t[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(Lcc_hvdc2t[i]));
        }
    }
    return device;
}

vector<VSC_HVDC*> PF_DATA::get_vsc_hvdcs_connecting_to_bus(const size_t bus)
{
    vector<VSC_HVDC*> device;
    device.reserve(8);

    size_t n = get_vsc_hvdc_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Vsc_hvdc[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(Vsc_hvdc[i]));
        }
    }
    return device;
}

vector<EQUIVALENT_DEVICE*> PF_DATA::get_equivalent_devices_connecting_to_bus(const size_t bus)
{
    vector<EQUIVALENT_DEVICE*> device;
    device.reserve(8);

    size_t n = get_equivalent_device_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Equivalent_device[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(Equivalent_device[i]));
        }
    }
    return device;
}

vector<ENERGY_STORAGE*> PF_DATA::get_energy_storages_connecting_to_bus(const size_t bus)
{
    vector<ENERGY_STORAGE*> device;
    device.reserve(8);

    size_t n = get_energy_storage_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Energy_storage[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(Energy_storage[i]));
        }
    }
    return device;
}

vector<LCC_HVDC*> PF_DATA::get_lcc_hvdcs_connecting_to_bus(const size_t bus)
{
    vector<LCC_HVDC*> device;
    device.reserve(8);

    size_t n = get_lcc_hvdc_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Lcc_hvdc[i].is_connected_to_bus(bus))
        {
            continue;
        }
        else
        {
            device.push_back(&(Lcc_hvdc[i]));
        }
    }
    return device;
}

vector<MUTUAL_DATA*> PF_DATA::get_mutual_data_with_line(const size_t ibus, const size_t jbus, string identifier)
{
    vector<MUTUAL_DATA*> device;
    device.reserve(4);
    size_t n = get_mutual_data_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (Mutual_data[i].is_related_to_ac_line(ibus, jbus, identifier))
        {
            device.push_back(&(Mutual_data[i]));
        }
        else
        {
            continue;
        }
    }
    return device;
}


vector<DEVICE_ID> PF_DATA::get_all_devices_device_id_connecting_to_bus(const size_t bus)
{
    vector<DEVICE*> devices = get_all_devices_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_generators_device_id_connecting_to_bus(const size_t bus)
{
    vector<GENERATOR*> devices = get_generators_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_wt_generators_device_id_connecting_to_bus(const size_t bus)
{
    vector<WT_GENERATOR*> devices = get_wt_generators_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_pv_units_device_id_connecting_to_bus(const size_t bus)
{
    vector<PV_UNIT*> devices = get_pv_units_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_sources_device_id_connecting_to_bus(const size_t bus)
{
    vector<SOURCE*> devices = get_sources_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_loads_device_id_connecting_to_bus(const size_t bus)
{
    vector<LOAD*> devices = get_loads_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_ac_lines_device_id_connecting_to_bus(const size_t bus)
{
    vector<AC_LINE*> devices = get_ac_lines_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_transformers_device_id_connecting_to_bus(const size_t bus)
{
    vector<TRANSFORMER*> devices = get_transformers_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_fixed_shunts_device_id_connecting_to_bus(const size_t bus)
{
    vector<FIXED_SHUNT*> devices = get_fixed_shunts_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_2t_lcc_hvdcs_device_id_connecting_to_bus(const size_t bus)
{
    vector<LCC_HVDC2T*> devices = get_2t_lcc_hvdcs_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_vsc_hvdcs_device_id_connecting_to_bus(const size_t bus)
{
    vector<VSC_HVDC*> devices = get_vsc_hvdcs_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_equivalent_devices_device_id_connecting_to_bus(const size_t bus)
{
    vector<EQUIVALENT_DEVICE*> devices = get_equivalent_devices_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_energy_storages_device_id_connecting_to_bus(const size_t bus)
{
    vector<ENERGY_STORAGE*> devices = get_energy_storages_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_lcc_hvdcs_device_id_connecting_to_bus(const size_t bus)
{
    vector<LCC_HVDC*> devices = get_lcc_hvdcs_connecting_to_bus(bus);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}


vector<DEVICE*> PF_DATA::get_all_devices_in_area(const size_t area)
{
    vector<DEVICE*> devices;
    size_t n = 0;

    vector<BUS*> buses = get_buses_in_area(area);
    n = buses.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(buses[i]);
    }

    vector<SOURCE*> sources = get_sources_in_area(area);
    n = sources.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(sources[i]);
    }

    vector<LOAD*> loads = get_loads_in_area(area);
    n = loads.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(loads[i]);
    }

    vector<AC_LINE*> lines = get_ac_lines_in_area(area);
    n = lines.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(lines[i]);
    }

    vector<TRANSFORMER*> transes = get_transformers_in_area(area);
    n = transes.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(transes[i]);
    }

    vector<FIXED_SHUNT*> fshunts = get_fixed_shunts_in_area(area);
    n = fshunts.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(fshunts[i]);
    }

    vector<LCC_HVDC2T*> hvdcs = get_2t_lcc_hvdcs_in_area(area);
    n = hvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(hvdcs[i]);
    }

    vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_in_area(area);
    n = vsc_hvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(vsc_hvdcs[i]);
    }

    vector<EQUIVALENT_DEVICE*> edevices = get_equivalent_devices_in_area(area);
    n = edevices.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(edevices[i]);
    }

    vector<LCC_HVDC*> lhvdcs = get_lcc_hvdcs_in_area(area);
    n = lhvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(lhvdcs[i]);
    }

    return devices;
}

vector<BUS*> PF_DATA::get_buses_in_area(const size_t area)
{
    vector<BUS*> device;
    device.reserve(8);

    size_t n = get_bus_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Bus[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Bus[i]));
        }
    }
    return device;
}

vector<GENERATOR*> PF_DATA::get_generators_in_area(const size_t area)
{
    vector<GENERATOR*> device;
    device.reserve(8);

    size_t n = get_generator_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Generator[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Generator[i]));
        }
    }
    return device;
}

vector<WT_GENERATOR*> PF_DATA::get_wt_generators_in_area(const size_t area)
{
    vector<WT_GENERATOR*> device;
    device.reserve(8);

    size_t n = get_wt_generator_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not WT_Generator[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(WT_Generator[i]));
        }
    }
    return device;
}


vector<PV_UNIT*> PF_DATA::get_pv_units_in_area(const size_t area)
{
    vector<PV_UNIT*> device;
    device.reserve(8);

    size_t n = get_pv_unit_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not PV_Unit[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(PV_Unit[i]));
        }
    }
    return device;
}

vector<SOURCE*> PF_DATA::get_sources_in_area(const size_t area)
{
    vector<SOURCE*> device;
    vector<GENERATOR*> gen_device;
    vector<WT_GENERATOR*> wt_generator_device;
    vector<PV_UNIT*> pv_unit_device;
    vector<ENERGY_STORAGE*> estorage_device;
    device.reserve(8);

    gen_device = get_generators_in_area(area);
    size_t ngens = gen_device.size();
    for (size_t i = 0; i != ngens; ++i)
    {
        device.push_back(gen_device[i]);
    }

    wt_generator_device = get_wt_generators_in_area(area);
    size_t nwt_generator = wt_generator_device.size();
    for (size_t i = 0; i != nwt_generator; ++i)
    {
        device.push_back(wt_generator_device[i]);
    }

    pv_unit_device = get_pv_units_in_area(area);
    size_t npv_unit = pv_unit_device.size();
    for (size_t i = 0; i != npv_unit; ++i)
    {
        device.push_back(pv_unit_device[i]);
    }

    estorage_device = get_energy_storages_in_area(area);
    size_t nes = estorage_device.size();
    for (size_t i = 0; i != nes; ++i)
    {
        device.push_back(estorage_device[i]);
    }

    return device;
}

vector<LOAD*> PF_DATA::get_loads_in_area(const size_t area)
{
    vector<LOAD*> device;
    device.reserve(8);

    size_t n = get_load_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Load[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Load[i]));
        }
    }
    return device;
}

vector<AC_LINE*> PF_DATA::get_ac_lines_in_area(const size_t area)
{
    vector<AC_LINE*> device;
    device.reserve(8);

    size_t n = get_ac_line_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Ac_line[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Ac_line[i]));
        }
    }
    return device;
}

vector<TRANSFORMER*> PF_DATA::get_transformers_in_area(const size_t area)
{
    vector<TRANSFORMER*> device;
    device.reserve(8);

    size_t n = get_transformer_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Transformer[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Transformer[i]));
        }
    }
    return device;
}

vector<FIXED_SHUNT*> PF_DATA::get_fixed_shunts_in_area(const size_t area)
{
    vector<FIXED_SHUNT*> device;
    device.reserve(8);

    size_t n = get_fixed_shunt_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Fixed_shunt[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Fixed_shunt[i]));
        }
    }
    return device;
}

vector<LCC_HVDC2T*> PF_DATA::get_2t_lcc_hvdcs_in_area(const size_t area)
{
    vector<LCC_HVDC2T*> device;
    device.reserve(8);

    size_t n = get_2t_lcc_hvdc_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Lcc_hvdc2t[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Lcc_hvdc2t[i]));
        }
    }
    return device;
}

vector<VSC_HVDC*> PF_DATA::get_vsc_hvdcs_in_area(const size_t area)
{
    vector<VSC_HVDC*> device;
    device.reserve(8);

    size_t n = get_vsc_hvdc_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Vsc_hvdc[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Vsc_hvdc[i]));
        }
    }
    return device;
}

vector<EQUIVALENT_DEVICE*> PF_DATA::get_equivalent_devices_in_area(const size_t area)
{
    vector<EQUIVALENT_DEVICE*> device;
    device.reserve(8);

    size_t n = get_equivalent_device_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Equivalent_device[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Equivalent_device[i]));
        }
    }
    return device;
}

vector<ENERGY_STORAGE*> PF_DATA::get_energy_storages_in_area(const size_t area)
{
    vector<ENERGY_STORAGE*> device;
    device.reserve(8);

    size_t n = get_energy_storage_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Energy_storage[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Energy_storage[i]));
        }
    }
    return device;
}

vector<LCC_HVDC*> PF_DATA::get_lcc_hvdcs_in_area(const size_t area)
{
    vector<LCC_HVDC*> device;
    device.reserve(8);

    size_t n = get_lcc_hvdc_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Lcc_hvdc[i].is_in_area(area))
        {
            continue;
        }
        else
        {
            device.push_back(&(Lcc_hvdc[i]));
        }
    }
    return device;
}



vector<DEVICE_ID> PF_DATA::get_all_devices_device_id_in_area(const size_t area)
{
    vector<DEVICE*> devices = get_all_devices_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_buses_device_id_in_area(const size_t area)
{
    vector<BUS*> devices = get_buses_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_generators_device_id_in_area(const size_t area)
{
    vector<GENERATOR*> devices = get_generators_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_wt_generators_device_id_in_area(const size_t area)
{
    vector<WT_GENERATOR*> devices = get_wt_generators_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_pv_units_device_id_in_area(const size_t area)
{
    vector<PV_UNIT*> devices = get_pv_units_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_sources_device_id_in_area(const size_t area)
{
    vector<SOURCE*> devices = get_sources_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_loads_device_id_in_area(const size_t area)
{
    vector<LOAD*> devices = get_loads_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_ac_lines_device_id_in_area(const size_t area)
{
    vector<AC_LINE*> devices = get_ac_lines_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_transformers_device_id_in_area(const size_t area)
{
    vector<TRANSFORMER*> devices = get_transformers_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_fixed_shunts_device_id_in_area(const size_t area)
{
    vector<FIXED_SHUNT*> devices = get_fixed_shunts_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_2t_lcc_hvdcs_device_id_in_area(const size_t area)
{
    vector<LCC_HVDC2T*> devices = get_2t_lcc_hvdcs_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_vsc_hvdcs_device_id_in_area(const size_t area)
{
    vector<VSC_HVDC*> devices = get_vsc_hvdcs_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_equivalent_devices_device_id_in_area(const size_t area)
{
    vector<EQUIVALENT_DEVICE*> devices = get_equivalent_devices_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_energy_storages_device_id_in_area(const size_t area)
{
    vector<ENERGY_STORAGE*> devices = get_energy_storages_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_lcc_hvdcs_device_id_in_area(const size_t area)
{
    vector<LCC_HVDC*> devices = get_lcc_hvdcs_in_area(area);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE*> PF_DATA::get_all_devices_in_zone(const size_t zone)
{
    vector<DEVICE*> devices;
    size_t n = 0;

    vector<BUS*> buses = get_buses_in_zone(zone);
    n = buses.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(buses[i]);
    }

    vector<SOURCE*> sources = get_sources_in_zone(zone);
    n = sources.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(sources[i]);
    }

    vector<LOAD*> loads = get_loads_in_zone(zone);
    n = loads.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(loads[i]);
    }

    vector<AC_LINE*> lines = get_ac_lines_in_zone(zone);
    n = lines.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(lines[i]);
    }

    vector<TRANSFORMER*> transes = get_transformers_in_zone(zone);
    n = transes.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(transes[i]);
    }

    vector<FIXED_SHUNT*> fshunts = get_fixed_shunts_in_zone(zone);
    n = fshunts.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(fshunts[i]);
    }

    vector<LCC_HVDC2T*> hvdcs = get_2t_lcc_hvdcs_in_zone(zone);
    n = hvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(hvdcs[i]);
    }

    vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_in_zone(zone);
    n = vsc_hvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(vsc_hvdcs[i]);
    }

    vector<EQUIVALENT_DEVICE*> edevices = get_equivalent_devices_in_zone(zone);
    n = edevices.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(edevices[i]);
    }

    vector<LCC_HVDC*> lhvdcs = get_lcc_hvdcs_in_zone(zone);
    n = lhvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(lhvdcs[i]);
    }

    return devices;
}

vector<BUS*> PF_DATA::get_buses_in_zone(const size_t zone)
{
    vector<BUS*> device;
    device.reserve(8);

    size_t n = get_bus_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Bus[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Bus[i]));
        }
    }
    return device;
}

vector<GENERATOR*> PF_DATA::get_generators_in_zone(const size_t zone)
{
    vector<GENERATOR*> device;
    device.reserve(8);

    size_t n = get_generator_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Generator[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Generator[i]));
        }
    }
    return device;
}

vector<WT_GENERATOR*> PF_DATA::get_wt_generators_in_zone(const size_t zone)
{
    vector<WT_GENERATOR*> device;
    device.reserve(8);

    size_t n = get_wt_generator_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not WT_Generator[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(WT_Generator[i]));
        }
    }
    return device;
}

vector<PV_UNIT*> PF_DATA::get_pv_units_in_zone(const size_t zone)
{
    vector<PV_UNIT*> device;
    device.reserve(8);

    size_t n = get_pv_unit_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not PV_Unit[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(PV_Unit[i]));
        }
    }
    return device;
}

vector<SOURCE*> PF_DATA::get_sources_in_zone(const size_t zone)
{
    vector<SOURCE*> device;
    vector<GENERATOR*> gen_device;
    vector<WT_GENERATOR*> wt_generator_device;
    vector<PV_UNIT*> pv_unit_device;
    vector<ENERGY_STORAGE*> estorage_device;
    device.reserve(8);

    gen_device = get_generators_in_zone(zone);
    size_t ngens = gen_device.size();
    for (size_t i = 0; i != ngens; ++i)
    {
        device.push_back(gen_device[i]);
    }

    wt_generator_device = get_wt_generators_in_zone(zone);
    size_t nwt_generator = wt_generator_device.size();
    for (size_t i = 0; i != nwt_generator; ++i)
    {
        device.push_back(wt_generator_device[i]);
    }

    pv_unit_device = get_pv_units_in_zone(zone);
    size_t npv_unit = pv_unit_device.size();
    for (size_t i = 0; i != npv_unit; ++i)
    {
        device.push_back(pv_unit_device[i]);
    }

    estorage_device = get_energy_storages_in_zone(zone);
    size_t nes = estorage_device.size();
    for (size_t i = 0; i != nes; ++i)
    {
        device.push_back(estorage_device[i]);
    }

    return device;
}

vector<LOAD*> PF_DATA::get_loads_in_zone(const size_t zone)
{
    vector<LOAD*> device;
    device.reserve(8);

    size_t n = get_load_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Load[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Load[i]));
        }
    }
    return device;
}

vector<AC_LINE*> PF_DATA::get_ac_lines_in_zone(const size_t zone)
{
    vector<AC_LINE*> device;
    device.reserve(8);

    size_t n = get_ac_line_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Ac_line[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Ac_line[i]));
        }
    }
    return device;
}

vector<TRANSFORMER*> PF_DATA::get_transformers_in_zone(const size_t zone)
{
    vector<TRANSFORMER*> device;
    device.reserve(8);

    size_t n = get_transformer_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Transformer[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Transformer[i]));
        }
    }
    return device;
}

vector<FIXED_SHUNT*> PF_DATA::get_fixed_shunts_in_zone(const size_t zone)
{
    vector<FIXED_SHUNT*> device;
    device.reserve(8);

    size_t n = get_fixed_shunt_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Fixed_shunt[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Fixed_shunt[i]));
        }
    }
    return device;
}

vector<LCC_HVDC2T*> PF_DATA::get_2t_lcc_hvdcs_in_zone(const size_t zone)
{
    vector<LCC_HVDC2T*> device;
    device.reserve(8);

    size_t n = get_2t_lcc_hvdc_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Lcc_hvdc2t[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Lcc_hvdc2t[i]));
        }
    }
    return device;
}

vector<VSC_HVDC*> PF_DATA::get_vsc_hvdcs_in_zone(const size_t zone)
{
    vector<VSC_HVDC*> device;
    device.reserve(8);

    size_t n = get_vsc_hvdc_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Vsc_hvdc[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Vsc_hvdc[i]));
        }
    }
    return device;
}

vector<EQUIVALENT_DEVICE*> PF_DATA::get_equivalent_devices_in_zone(const size_t zone)
{
    vector<EQUIVALENT_DEVICE*> device;
    device.reserve(8);

    size_t n = get_equivalent_device_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Equivalent_device[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Equivalent_device[i]));
        }
    }
    return device;
}

vector<ENERGY_STORAGE*> PF_DATA::get_energy_storages_in_zone(const size_t zone)
{
    vector<ENERGY_STORAGE*> device;
    device.reserve(8);

    size_t n = get_energy_storage_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Energy_storage[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Energy_storage[i]));
        }
    }
    return device;
}

vector<LCC_HVDC*> PF_DATA::get_lcc_hvdcs_in_zone(const size_t zone)
{
    vector<LCC_HVDC*> device;
    device.reserve(8);

    size_t n = get_lcc_hvdc_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Lcc_hvdc[i].is_in_zone(zone))
        {
            continue;
        }
        else
        {
            device.push_back(&(Lcc_hvdc[i]));
        }
    }
    return device;
}

vector<DEVICE_ID> PF_DATA::get_all_devices_device_id_in_zone(const size_t zone)
{
    vector<DEVICE*> devices = get_all_devices_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_buses_device_id_in_zone(const size_t zone)
{
    vector<BUS*> devices = get_buses_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_generators_device_id_in_zone(const size_t zone)
{
    vector<GENERATOR*> devices = get_generators_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_wt_generators_device_id_in_zone(const size_t zone)
{
    vector<WT_GENERATOR*> devices = get_wt_generators_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_pv_units_device_id_in_zone(const size_t zone)
{
    vector<PV_UNIT*> devices = get_pv_units_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_sources_device_id_in_zone(const size_t zone)
{
    vector<SOURCE*> devices = get_sources_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_loads_device_id_in_zone(const size_t zone)
{
    vector<LOAD*> devices = get_loads_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_ac_lines_device_id_in_zone(const size_t zone)
{
    vector<AC_LINE*> devices = get_ac_lines_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_transformers_device_id_in_zone(const size_t zone)
{
    vector<TRANSFORMER*> devices = get_transformers_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_fixed_shunts_device_id_in_zone(const size_t zone)
{
    vector<FIXED_SHUNT*> devices = get_fixed_shunts_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_2t_lcc_hvdcs_device_id_in_zone(const size_t zone)
{
    vector<LCC_HVDC2T*> devices = get_2t_lcc_hvdcs_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_vsc_hvdcs_device_id_in_zone(const size_t zone)
{
    vector<VSC_HVDC*> devices = get_vsc_hvdcs_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_equivalent_devices_device_id_in_zone(const size_t zone)
{
    vector<EQUIVALENT_DEVICE*> devices = get_equivalent_devices_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_energy_storages_device_id_in_zone(const size_t zone)
{
    vector<ENERGY_STORAGE*> devices = get_energy_storages_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}

vector<DEVICE_ID> PF_DATA::get_lcc_hvdcs_device_id_in_zone(const size_t zone)
{
    vector<LCC_HVDC*> devices = get_lcc_hvdcs_in_zone(zone);
    size_t n = devices.size();

    vector<DEVICE_ID> dids;
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(devices[i]->get_device_id());
    }

    return dids;
}


vector<DEVICE*> PF_DATA::get_all_devices()
{
    vector<DEVICE*> devices;
    size_t n = 0;

    vector<BUS*> buses = get_all_buses();
    n = buses.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(buses[i]);
    }

    vector<SOURCE*> sources = get_all_sources();
    n = sources.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(sources[i]);
    }

    vector<LOAD*> loads = get_all_loads();
    n = loads.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(loads[i]);
    }

    vector<FIXED_SHUNT*> fshunts = get_all_fixed_shunts();
    n = fshunts.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(fshunts[i]);
    }

    vector<AC_LINE*> lines = get_all_ac_lines();
    n = lines.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(lines[i]);
    }

    vector<TRANSFORMER*> transes = get_all_transformers();
    n = transes.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(transes[i]);
    }

    vector<LCC_HVDC2T*> hvdcs = get_all_2t_lcc_hvdcs();
    n = hvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(hvdcs[i]);
    }

    vector<VSC_HVDC*> vsc_hvdcs = get_all_vsc_hvdcs();
    n = vsc_hvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(vsc_hvdcs[i]);
    }

    vector<EQUIVALENT_DEVICE*> edevices = get_all_equivalent_devices();
    n = edevices.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(edevices[i]);
    }

    vector<LCC_HVDC*> lcc_hvdcs = get_all_lcc_hvdcs();
    n = lcc_hvdcs.size();
    for (size_t i = 0; i != n; ++i)
    {
        devices.push_back(lcc_hvdcs[i]);
    }

    return devices;
}

vector<BUS*> PF_DATA::get_all_buses()
{
    vector<BUS*> device;
    size_t n = get_bus_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Bus[i]));
    }
    return device;
}

vector<BUS*> PF_DATA::get_buses_with_constraints(double vbase_kV_min, double vbase_kV_max, double v_pu_min, double v_pu_max, size_t area, size_t zone, size_t owner)
{
    vector<BUS*> device;
    size_t n = get_bus_count();
    device.reserve(n);
    double vbase = 0.0, v = 0.0;
    size_t this_area = 0, this_zone = 0, this_owner = 0;
    for (size_t i = 0; i != n; ++i)
    {
        vbase = Bus[i].get_base_voltage_in_kV();
        v = Bus[i].get_positive_sequence_voltage_in_pu();
        this_area = Bus[i].get_area_number();
        this_zone = Bus[i].get_zone_number();
        this_owner = Bus[i].get_owner_number();

        if ((area != 0 and this_area != area) or
                (zone != 0 and this_zone != zone) or
                (owner != 0 and this_owner != owner))
        {
            continue;
        }
        if (vbase >= vbase_kV_min and vbase <= vbase_kV_max and v >= v_pu_min and v <= v_pu_max)
        {
            device.push_back(&(Bus[i]));
        }
    }
    return device;
}

vector<BUS*> PF_DATA::get_all_in_service_buses()
{
    vector<BUS*> device;
    size_t n = get_bus_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        if (Bus[i].get_bus_type() != OUT_OF_SERVICE)
        {
            device.push_back(&(Bus[i]));
        }
    }
    return device;
}

vector<GENERATOR*> PF_DATA::get_all_generators()
{
    vector<GENERATOR*> device;
    size_t n = get_generator_count();
    device.reserve(n);

    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Generator[i]));
    }
    return device;
}

vector<WT_GENERATOR*> PF_DATA::get_all_wt_generators()
{
    vector<WT_GENERATOR*> device;
    size_t n = get_wt_generator_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(WT_Generator[i]));
    }
    return device;
}

vector<PV_UNIT*> PF_DATA::get_all_pv_units()
{
    vector<PV_UNIT*> device;
    size_t n = get_pv_unit_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(PV_Unit[i]));
    }
    return device;
}

vector<SOURCE*> PF_DATA::get_all_sources()
{
    vector<SOURCE*> device;
    vector<GENERATOR*> gen_device;
    vector<WT_GENERATOR*> wt_generator_device;
    vector<PV_UNIT*> pv_unit_device;
    vector<ENERGY_STORAGE*> estorage_device;
    device.reserve(get_source_count());

    gen_device = get_all_generators();
    size_t ngens = gen_device.size();
    for (size_t i = 0; i != ngens; ++i)
    {
        device.push_back(gen_device[i]);
    }

    wt_generator_device = get_all_wt_generators();
    size_t nwt_generator = wt_generator_device.size();
    for (size_t i = 0; i != nwt_generator; ++i)
    {
        device.push_back(wt_generator_device[i]);
    }

    pv_unit_device = get_all_pv_units();
    size_t npv_unit = pv_unit_device.size();
    for (size_t i = 0; i != npv_unit; ++i)
    {
        device.push_back(pv_unit_device[i]);
    }

    estorage_device = get_all_energy_storages();
    size_t nes = estorage_device.size();
    for (size_t i = 0; i != nes; ++i)
    {
        device.push_back(estorage_device[i]);
    }

    return device;
}

vector<LOAD*> PF_DATA::get_all_loads()
{
    vector<LOAD*> device;
    size_t n = get_load_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Load[i]));
    }
    return device;
}

vector<AC_LINE*> PF_DATA::get_all_ac_lines()
{
    vector<AC_LINE*> device;
    size_t n = get_ac_line_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Ac_line[i]));
    }
    return device;
}

vector<TRANSFORMER*> PF_DATA::get_all_transformers()
{
    vector<TRANSFORMER*> device;
    size_t n = get_transformer_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Transformer[i]));
    }
    return device;
}

vector<FIXED_SHUNT*> PF_DATA::get_all_fixed_shunts()
{
    vector<FIXED_SHUNT*> device;
    size_t n = get_fixed_shunt_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Fixed_shunt[i]));
    }
    return device;
}

vector<LCC_HVDC2T*> PF_DATA::get_all_2t_lcc_hvdcs()
{
    vector<LCC_HVDC2T*> device;
    size_t n = get_2t_lcc_hvdc_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Lcc_hvdc2t[i]));
    }
    return device;
}

vector<VSC_HVDC*> PF_DATA::get_all_vsc_hvdcs()
{
    vector<VSC_HVDC*> device;
    size_t n = get_vsc_hvdc_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Vsc_hvdc[i]));
    }
    return device;
}

vector<EQUIVALENT_DEVICE*> PF_DATA::get_all_equivalent_devices()
{
    vector<EQUIVALENT_DEVICE*> device;
    size_t n = get_equivalent_device_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Equivalent_device[i]));
    }
    return device;
}

vector<ENERGY_STORAGE*> PF_DATA::get_all_energy_storages()
{
    vector<ENERGY_STORAGE*> device;
    size_t n = get_energy_storage_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Energy_storage[i]));
    }
    return device;
}

vector<LCC_HVDC*> PF_DATA::get_all_lcc_hvdcs()
{
    vector<LCC_HVDC*> device;
    size_t n = get_lcc_hvdc_count();
    device.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        device.push_back(&(Lcc_hvdc[i]));
    }
    return device;
}

vector<AREA*> PF_DATA::get_all_areas()
{
    vector<AREA*> areas;
    areas.reserve(get_area_count());

    size_t n = get_area_count();
    for (size_t i = 0; i != n; ++i)
    {
        areas.push_back(&(Area_[i]));
    }
    return areas;
}

vector<ZONE*> PF_DATA::get_all_zones()
{
    vector<ZONE*> zones;
    zones.reserve(get_zone_count());

    size_t n = get_zone_count();
    for (size_t i = 0; i != n; ++i)
    {
        zones.push_back(&(Zone_[i]));
    }
    return zones;
}

vector<OWNER*> PF_DATA::get_all_owners()
{
    vector<OWNER*> owners;
    owners.reserve(get_owner_count());

    size_t n = get_owner_count();
    for (size_t i = 0; i != n; ++i)
    {
        owners.push_back(&(Owner_[i]));
    }
    return owners;
}

vector<MUTUAL_DATA*> PF_DATA::get_all_mutual_data()
{
    vector <MUTUAL_DATA*> mutuals;
    mutuals.reserve(get_mutual_data_count());

    size_t n = get_mutual_data_count();
    for (size_t i = 0; i != n; ++i)
    {
        mutuals.push_back(&(Mutual_data[i]));
    }
    return mutuals;
}



vector<size_t> PF_DATA::get_all_buses_number()
{
    vector<size_t> buses_number;

    size_t n = get_bus_count();
    buses_number.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        buses_number.push_back(Bus[i].get_bus_number());
    }

    return buses_number;
}

vector<size_t> PF_DATA::get_buses_number_with_constraints(double vbase_kV_min, double vbase_kV_max, double v_pu_min, double v_pu_max, size_t area, size_t zone, size_t owner)
{
    vector<size_t> buses_number;
    size_t n = get_bus_count();
    buses_number.reserve(n);
    double vbase = 0.0, v = 0.0;
    size_t this_area = 0, this_zone = 0, this_owner = 0;
    for (size_t i = 0; i != n; ++i)
    {
        vbase = Bus[i].get_base_voltage_in_kV();
        v = Bus[i].get_positive_sequence_voltage_in_pu();
        this_area = Bus[i].get_area_number();
        this_zone = Bus[i].get_zone_number();
        this_owner = Bus[i].get_owner_number();

        if ((area != 0 and this_area != area) or
                (zone != 0 and this_zone != zone) or
                (owner != 0 and this_owner != owner))
        {
            continue;
        }
        if (vbase >= vbase_kV_min and vbase <= vbase_kV_max and v >= v_pu_min and v <= v_pu_max)
        {
            buses_number.push_back(Bus[i].get_bus_number());
        }
    }
    return buses_number;
}

vector<size_t> PF_DATA::get_all_in_service_buses_number()
{
    vector<size_t> buses_number;
    size_t n = get_bus_count();
    buses_number.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        if (Bus[i].get_bus_type() != OUT_OF_SERVICE)
        {
            buses_number.push_back(Bus[i].get_bus_number());
        }
        else
        {
            continue;
        }
    }
    return buses_number;
}

vector<DEVICE_ID> PF_DATA::get_all_generators_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_generator_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(Generator[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_wt_generators_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_wt_generator_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(WT_Generator[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_pv_units_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_pv_unit_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(PV_Unit[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_sources_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_source_count();
    dids.reserve(n);
    size_t ngens = get_generator_count();
    for (size_t i = 0; i != ngens; ++i)
    {
        dids.push_back(Generator[i].get_device_id());
    }

    size_t nwt_generator = get_wt_generator_count();
    for (size_t i = 0; i != nwt_generator; ++i)
    {
        dids.push_back(WT_Generator[i].get_device_id());
    }

    size_t npv_unit = get_pv_unit_count();
    for (size_t i = 0; i != npv_unit; ++i)
    {
        dids.push_back(PV_Unit[i].get_device_id());
    }

    size_t nes = get_energy_storage_count();
    for (size_t i = 0; i != nes; ++i)
    {
        dids.push_back(Energy_storage[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_loads_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_load_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(Load[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_ac_lines_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_ac_line_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(Ac_line[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_transformers_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_transformer_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(Transformer[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_fixed_shunts_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_fixed_shunt_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(Fixed_shunt[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_2t_lcc_hvdcs_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_2t_lcc_hvdc_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(Lcc_hvdc2t[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_vsc_hvdcs_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_vsc_hvdc_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(Vsc_hvdc[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_equivalent_devices_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_equivalent_device_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(Equivalent_device[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_energy_storages_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_energy_storage_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(Energy_storage[i].get_device_id());
    }
    return dids;
}

vector<DEVICE_ID> PF_DATA::get_all_lcc_hvdcs_device_id()
{
    vector<DEVICE_ID> dids;
    size_t n = get_lcc_hvdc_count();
    dids.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        dids.push_back(Lcc_hvdc[i].get_device_id());
    }
    return dids;
}

vector<size_t> PF_DATA::get_all_areas_number()
{
    vector<size_t> areas_number;
    size_t n = get_area_count();
    areas_number.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        areas_number.push_back(Area_[i].get_area_number());
    }
    return areas_number;
}

vector<size_t> PF_DATA::get_all_zones_number()
{
    vector<size_t> zones_number;
    size_t n = get_zone_count();
    zones_number.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        zones_number.push_back(Zone_[i].get_zone_number());
    }
    return zones_number;
}

vector<size_t> PF_DATA::get_all_owners_number()
{
    vector<size_t> owners_number;
    size_t n = get_owner_count();
    owners_number.reserve(n);
    for (size_t i = 0; i != n; ++i)
    {
        owners_number.push_back(Owner_[i].get_owner_number());
    }
    return owners_number;
}


size_t PF_DATA::get_bus_count() const
{
    return Bus.size();
}

size_t PF_DATA::get_in_service_bus_count() const
{
    return in_service_bus_count;
}

void PF_DATA::update_in_service_bus_count()
{
    size_t n = Bus.size();
    size_t n_out_of_service = 0;
    for (size_t i = 0; i != n; ++i)
    {
        if (Bus[i].get_bus_type() != OUT_OF_SERVICE)
        {
            continue;
        }
        else
        {
            ++n_out_of_service;
        }
    }

    in_service_bus_count = n - n_out_of_service;
}

size_t PF_DATA::get_overshadowed_bus_count() const
{
    return overshadowed_bus_count;
}

void PF_DATA::set_all_buses_un_overshadowed()
{
    size_t n = Bus.size();
    for (size_t i = 0; i != n; ++i)
    {
        Bus[i].set_equivalent_bus_number(0);
    }
}

void PF_DATA::update_overshadowed_bus_count()
{
    set_all_buses_un_overshadowed();

    overshadowed_bus_count = 0;
    size_t n = Ac_line.size();
    while (true)
    {
        bool new_bus_is_overshadowed = false;
        for (size_t i = 0; i != n; ++i)
        {
            if (not Ac_line[i].is_zero_impedance_line())
            {
                continue;
            }
            else
            {
                bool istatus = Ac_line[i].get_sending_side_breaker_status();
                bool jstatus = Ac_line[i].get_receiving_side_breaker_status();
                if (istatus != false and jstatus != false)
                {
                    size_t ibus = Ac_line[i].get_sending_side_bus();
                    size_t jbus = Ac_line[i].get_receiving_side_bus();

                    size_t iequvilent_bus = get_equivalent_bus_of_bus(ibus);
                    size_t jequvilent_bus = get_equivalent_bus_of_bus(jbus);

                    if (iequvilent_bus == 0)
                    {
                        iequvilent_bus = ibus;
                    }
                    if (jequvilent_bus == 0)
                    {
                        jequvilent_bus = jbus;
                    }

                    if (iequvilent_bus == jequvilent_bus)
                    {
                        continue;
                    }
                    else
                    {
                        if (iequvilent_bus < jequvilent_bus)
                        {
                            BUS* busptr = get_bus(jbus);
                            busptr->set_equivalent_bus_number(iequvilent_bus);
                        }
                        else
                        {
                            BUS* busptr = get_bus(ibus);
                            busptr->set_equivalent_bus_number(jequvilent_bus);
                        }
                        if (new_bus_is_overshadowed == false)
                        {
                            new_bus_is_overshadowed = true;
                        }
                    }
                }
                else
                {
                    continue;
                }
            }
        }
        if (new_bus_is_overshadowed == false)
        {
            break;
        }
    }

    n = Bus.size();
    for (size_t i = 0; i != n; ++i)
    {
        if (not Bus[i].is_bus_overshadowed())
        {
            continue;
        }
        else
        {
            ++overshadowed_bus_count;
        }
    }
}

size_t PF_DATA::get_equivalent_bus_of_bus(size_t bus)
{
    BUS* busptr = get_bus(bus);
    if (busptr != NULL)
    {
        return busptr->get_equivalent_bus_number();
    }
    else
    {
        return 0;
    }
}

size_t PF_DATA::get_generator_count() const
{
    return Generator.size();
}

size_t PF_DATA::get_wt_generator_count() const
{
    return WT_Generator.size();
}

size_t PF_DATA::get_pv_unit_count() const
{
    return PV_Unit.size();
}

size_t PF_DATA::get_source_count() const
{
    size_t n = 0;
    n += get_generator_count();
    n += get_wt_generator_count();
    n += get_pv_unit_count();
    n += get_energy_storage_count();
    return n;
}

size_t PF_DATA::get_load_count() const
{
    return Load.size();
}

size_t PF_DATA::get_ac_line_count() const
{
    return Ac_line.size();
}

size_t PF_DATA::get_transformer_count() const
{
    return Transformer.size();
}

size_t PF_DATA::get_fixed_shunt_count() const
{
    return Fixed_shunt.size();
}

size_t PF_DATA::get_2t_lcc_hvdc_count() const
{
    return Lcc_hvdc2t.size();
}

size_t PF_DATA::get_vsc_hvdc_count() const
{
    return Vsc_hvdc.size();
}

size_t PF_DATA::get_equivalent_device_count() const
{
    return Equivalent_device.size();
}

size_t PF_DATA::get_energy_storage_count() const
{
    return Energy_storage.size();
}

size_t PF_DATA::get_lcc_hvdc_count() const
{
    return Lcc_hvdc.size();
}

size_t PF_DATA::get_area_count() const
{
    return Area_.size();
}

size_t PF_DATA::get_zone_count() const
{
    return Zone_.size();
}

size_t PF_DATA::get_owner_count() const
{
    return Owner_.size();
}

size_t PF_DATA::get_mutual_data_count() const
{
    return Mutual_data.size();
}

size_t PF_DATA::get_bus_index(size_t bus) const
{
    return bus_index.get_index_of_bus(bus);
}

size_t PF_DATA::get_generator_index(const DEVICE_ID & device_id) const
{
    return generator_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_wt_generator_index(const DEVICE_ID & device_id) const
{
    return wt_generator_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_pv_unit_index(const DEVICE_ID & device_id) const
{
    return pv_unit_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_load_index(const DEVICE_ID & device_id) const
{
    return load_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_ac_line_index(const DEVICE_ID & device_id) const
{
    return ac_line_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_transformer_index(const DEVICE_ID & device_id) const
{
    return transformer_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_fixed_shunt_index(const DEVICE_ID & device_id) const
{
    return fixed_shunt_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_2t_lcc_hvdc_index(const DEVICE_ID & device_id) const
{
    return hvdc_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_vsc_hvdc_index(const DEVICE_ID & device_id) const
{
    return vsc_hvdc_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_vsc_hvdc_index(const string vsc_name) const
{
    return vsc_hvdc_index.get_index_of_device(vsc_name);
}

size_t PF_DATA::get_equivalent_device_index(const DEVICE_ID & device_id) const
{
    return equivalent_device_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_energy_storage_index(const DEVICE_ID & device_id) const
{
    return energy_storage_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_lcc_hvdc_index(const DEVICE_ID & device_id) const
{
    return lcc_hvdc_index.get_index_of_device(device_id);
}

size_t PF_DATA::get_area_index(const size_t no) const
{
    map<size_t, size_t>::const_iterator iter = area_index.begin();
    iter = area_index.find(no);
    if (iter != area_index.end())
    {
        return iter->second;
    }
    else
    {
        return INDEX_NOT_EXIST;
    }
}

size_t PF_DATA::get_zone_index(const size_t no) const
{
    map<size_t, size_t>::const_iterator iter = zone_index.begin();
    iter = zone_index.find(no);
    if (iter != zone_index.end())
    {
        return iter->second;
    }
    else
    {
        return INDEX_NOT_EXIST;
    }
}

size_t PF_DATA::get_owner_index(const size_t no) const
{
    map<size_t, size_t>::const_iterator iter = owner_index.begin();
    iter = owner_index.find(no);
    if (iter != owner_index.end())
    {
        return iter->second;
    }
    else
    {
        return INDEX_NOT_EXIST;
    }
}

size_t PF_DATA::bus_name2bus_number(const string&name, double vbase) const
{
    string trimmed_name = trim_string(name);
    size_t n = get_bus_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (Bus[i].get_bus_name() != trimmed_name)
        {
            continue;
        }
        else
        {
            if (vbase == 0.0 or Bus[i].get_base_voltage_in_kV() == vbase)
            {
                return Bus[i].get_bus_number();
            }
        }

    }
    return 0;
}

size_t PF_DATA::area_name2area_number(const string& name) const
{
    string trimmed_name = trim_string(name);
    size_t n = get_area_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (Area_[i].get_area_name() != trimmed_name)
        {
            continue;
        }
        else
        {
            return Area_[i].get_area_number();
        }
    }
    return 0;
}

size_t PF_DATA::zone_name2zone_number(const string& name) const
{
    string trimmed_name = trim_string(name);
    size_t n = get_zone_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (Zone_[i].get_zone_name() != trimmed_name)
        {
            continue;
        }
        else
        {
            return Zone_[i].get_zone_number();
        }
    }
    return 0;
}

size_t PF_DATA::owner_name2owner_number(const string& name) const
{
    string trimmed_name = trim_string(name);
    size_t n = get_owner_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (Owner_[i].get_owner_name() != trimmed_name)
        {
            continue;
        }
        else
        {
            return Owner_[i].get_owner_number();
        }
    }
    return 0;
}

string PF_DATA::bus_number2bus_name(size_t number)
{
    if (is_bus_in_allowed_range(number))
    {
        BUS* bus = get_bus(number);
        if (bus != NULL)
        {
            return bus->get_bus_name();
        }
        else
        {
            return "";
        }
    }
    else
    {
        return "";
    }
}


string PF_DATA::area_number2area_name(size_t number)
{
    AREA* area = get_area(number);
    if (area != NULL)
    {
        return area->get_area_name();
    }
    else
    {
        return "";
    }
}

string PF_DATA::zone_number2zone_name(size_t number)
{
    ZONE* zone = get_zone(number);
    if (zone != NULL)
    {
        return zone->get_zone_name();
    }
    else
    {
        return "";
    }
}

string PF_DATA::owner_number2owner_name(size_t number)
{
    OWNER* owner = get_owner(number);
    if (owner != NULL)
    {
        return owner->get_owner_name();
    }
    else
    {
        return "";
    }
}


void PF_DATA::check_data()
{
    check_all_items(Bus);
    check_all_items(Generator);
    check_all_items(WT_Generator);
    check_all_items(PV_Unit);
    check_all_items(Energy_storage);
    check_all_items(Load);
    check_all_items(Fixed_shunt);
    check_all_items(Ac_line);
    check_all_items(Transformer);
    check_all_items(Lcc_hvdc2t);
    check_all_items(Equivalent_device);
    check_all_items(Lcc_hvdc);
    check_all_items(Vsc_hvdc);
    check_all_items(Area_);
    check_all_items(Zone_);
    check_all_items(Owner_);
}

void PF_DATA::check_dynamic_data()
{
    check_generator_related_dynamic_data();
    check_wt_generator_related_dynamic_data();
    check_pv_unit_related_dynamic_data();
    check_energy_storage_related_dynamic_data();
    check_load_related_dynamic_data();
    check_ac_line_related_dynamic_data();
    check_2t_lcc_hvdc_related_dynamic_data();
    check_equivalent_device_related_dynamic_data();
}

void PF_DATA::check_generator_related_dynamic_data()
{
    ostringstream osstream;
    vector<GENERATOR*> generators = get_all_generators();
    size_t n = generators.size();
    GENERATOR* generator;
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];

        SYNC_GENERATOR_MODEL* genmodel = generator->get_sync_generator_model();
        if (genmodel != NULL)
        {
            genmodel->check();
        }

        EXCITER_MODEL* avrmodel = generator->get_exciter_model();
        if (avrmodel != NULL)
        {
            avrmodel->check();
        }

        STABILIZER_MODEL* pssmodel = generator->get_stabilizer_model();
        if (pssmodel != NULL)
        {
            pssmodel->check();
        }

        COMPENSATOR_MODEL* compmodel = generator->get_compensator_model();
        if (compmodel != NULL)
        {
            compmodel->check();
        }

        TURBINE_GOVERNOR_MODEL* govmodel = generator->get_turbine_governor_model();
        if (govmodel != NULL)
        {
            govmodel->check();
        }

        TURBINE_LOAD_CONTROLLER_MODEL* tlcmodel = generator->get_turbine_load_controller_model();
        if (tlcmodel != NULL)
        {
            tlcmodel->check();
        }

        if (toolkit->is_detailed_log_enabled())
        {
            osstream << "Model entry address of " << generator->get_compound_device_name() << ":\n"
                     << "Compensator: " << compmodel << "\n"
                     << "PSS:         " << pssmodel << "\n"
                     << "Exciter:     " << avrmodel << "\n"
                     << "TurbineLCtrl:" << tlcmodel << "\n"
                     << "Turbine:     " << govmodel << "\n"
                     << "Generator:   " << genmodel;
            toolkit->show_information(osstream);
        }
    }
}

void PF_DATA::check_wt_generator_related_dynamic_data()
{
    vector<WT_GENERATOR*> generators = get_all_wt_generators();
    size_t n = generators.size();
    WT_GENERATOR* generator;
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];

        WT_GENERATOR_MODEL* genmodel = generator->get_wt_generator_model();
        if (genmodel != NULL)
        {
            genmodel->check();
        }

        WT_ELECTRICAL_MODEL* elecmodel = generator->get_wt_electrical_model();
        if (elecmodel != NULL)
        {
            elecmodel->check();
        }

        WT_AERODYNAMIC_MODEL* aerdmodel = generator->get_wt_aerodynamic_model();
        if (aerdmodel != NULL)
        {
            aerdmodel->check();
        }

        WT_TURBINE_MODEL* turbmodel = generator->get_wt_turbine_model();
        if (turbmodel != NULL)
        {
            turbmodel->check();
        }

        WT_PITCH_MODEL* pitchmodel = generator->get_wt_pitch_model();
        if (pitchmodel != NULL)
        {
            pitchmodel->check();
        }

        WT_VRT_MODEL * vrtmodel = generator->get_wt_vrt_model();
        if (vrtmodel != NULL)
        {
            vrtmodel->check();
        }

        WT_RELAY_MODEL* relaymodel = generator->get_wt_relay_model();
        if (relaymodel != NULL)
        {
            relaymodel->check();
        }

        WIND_SPEED_MODEL* windmodel = generator->get_wind_speed_model();
        if (windmodel != NULL)
        {
            windmodel->check();
        }


        if (toolkit->is_detailed_log_enabled())
        {
            ostringstream osstream;
            osstream << "Model entry address of " << generator->get_compound_device_name() << ":\n"
                     << "WT Generator: " << genmodel << "\n"
                     << "Electrical:   " << elecmodel << "\n"
                     << "Aero dyanmic: " << aerdmodel << "\n"
                     << "Turbine:      " << turbmodel << "\n"
                     << "Pitch:        " << pitchmodel << "\n"
                     << "VRT:          " << vrtmodel << "\n"
                     << "Relay:        " << relaymodel << "\n"
                     << "Wind:         " << windmodel;
            toolkit->show_information(osstream);
        }
    }
}

void PF_DATA::check_pv_unit_related_dynamic_data()
{
    vector<PV_UNIT*> pvs = get_all_pv_units();
    size_t n = pvs.size();
    PV_UNIT* pv;
    for (size_t i = 0; i != n; ++i)
    {
        pv = pvs[i];

        PV_CONVERTER_MODEL* convmodel = pv->get_pv_converter_model();
        if (convmodel != NULL)
        {
            convmodel->check();
        }

        PV_PANEL_MODEL* panelmodel = pv->get_pv_panel_model();
        if (panelmodel != NULL)
        {
            panelmodel->check();
        }

        PV_ELECTRICAL_MODEL* elecmodel = pv->get_pv_electrical_model();
        if (elecmodel != NULL)
        {
            elecmodel->check();
        }

        PV_IRRADIANCE_MODEL* irradmodel = pv->get_pv_irradiance_model();
        if (irradmodel != NULL)
        {
            irradmodel->check();
        }
    }
}

void PF_DATA::check_energy_storage_related_dynamic_data()
{
    vector<ENERGY_STORAGE*> estorages = get_all_energy_storages();
    size_t n = estorages.size();
    ENERGY_STORAGE* estorage;
    for (size_t i = 0; i != n; ++i)
    {
        estorage = estorages[i];

        ES_CONVERTER_MODEL* conv = estorage->get_es_converter_model();
        if (conv != NULL)
        {
            conv->check();
        }
    }
}

void PF_DATA::check_load_related_dynamic_data()
{
    ostringstream osstream;
    vector<LOAD*> loads = get_all_loads();
    size_t n = loads.size();
    LOAD* load;
    for (size_t i = 0; i != n; ++i)
    {
        load = loads[i];

        LOAD_MODEL* loadmodel = load->get_load_model();
        if (loadmodel != NULL)
        {
            loadmodel->check();
        }

        LOAD_VOLTAGE_RELAY_MODEL* uvlsmodel = load->get_load_voltage_relay_model();
        if (uvlsmodel != NULL)
        {
            uvlsmodel->check();
        }

        LOAD_FREQUENCY_RELAY_MODEL* uflsmodel = load->get_load_frequency_relay_model();
        if (uflsmodel != NULL)
        {
            uflsmodel->check();
        }

        if (toolkit->is_detailed_log_enabled())
        {
            osstream << "Model entry address of " << load->get_compound_device_name() << ":\n"
                     << "uvls: " << uvlsmodel << "\n"
                     << "ufls: " << uflsmodel << "\n"
                     << "load: " << loadmodel;
            toolkit->show_information(osstream);
        }
    }
}

void PF_DATA::check_ac_line_related_dynamic_data()
{
    /*vector<AC_LINE*> lines = get_all_ac_lines();
    size_t n = lines.size();
    for(size_t i=0; i!=n; ++i)
    {
        AC_LINE *line = lines[i];
    }*/
}

void PF_DATA::check_2t_lcc_hvdc_related_dynamic_data()
{
    vector<LCC_HVDC2T*> hvdcs = get_all_2t_lcc_hvdcs();
    size_t n = hvdcs.size();
    LCC_HVDC2T* hvdc;
    for (size_t i = 0; i != n; ++i)
    {
        hvdc = hvdcs[i];

        LCC_HVDC2T_MODEL* hvdcmodel = hvdc->get_2t_lcc_hvdc_model();
        if (hvdcmodel != NULL)
        {
            hvdcmodel->check();
        }
    }
}

void PF_DATA::check_vsc_hvdc_related_dynamic_data()
{
    vector<VSC_HVDC*> vsc_hvdcs = get_all_vsc_hvdcs();
    size_t n = vsc_hvdcs.size();
    VSC_HVDC* vsc_hvdc;
    for (size_t i = 0; i != n; ++i)
    {
        vsc_hvdc = vsc_hvdcs[i];

        VSC_HVDC_NETWORK_MODEL* vsc_project_model = vsc_hvdc->get_vsc_hvdc_network_model();
        if (vsc_project_model != NULL)
        {
            vsc_project_model->check();
        }
        vector<VSC_HVDC_CONVERTER_MODEL*> vsc_converter_models = vsc_hvdc->get_vsc_hvdc_converter_models();
        size_t n = vsc_converter_models.size();
        for (size_t i = 0; i != n; ++i)
        {
            VSC_HVDC_CONVERTER_MODEL* model = vsc_converter_models[i];
            if (model != NULL)
            {
                model->check();
            }
        }
    }
}

void PF_DATA::check_equivalent_device_related_dynamic_data()
{
    vector<EQUIVALENT_DEVICE*> edevices = get_all_equivalent_devices();
    size_t n = edevices.size();
    EQUIVALENT_DEVICE* edevice;
    for (size_t i = 0; i != n; ++i)
    {
        edevice = edevices[i];

        EQUIVALENT_MODEL* eqmodel = edevice->get_equivalent_model();
        if (eqmodel != NULL)
        {
            eqmodel->check();
        }
    }
}


void PF_DATA::check_lcc_hvdc_related_dynamic_data()
{
    ostringstream osstream;
    osstream << "No LCC LCC_HVDC2T is checked for dynamic data.";
    toolkit->show_information(osstream);
    return;
}


void PF_DATA::check_missing_models()
{
    check_missing_generator_related_model();
    check_missing_wt_generator_related_model();
    check_missing_pv_unit_related_model();
    check_missing_energy_storage_related_model();
    check_missing_load_related_model();
    check_missing_ac_line_related_model();
    check_missing_2t_lcc_hvdc_related_model();
    check_missing_equivalent_device_related_model();
}

void PF_DATA::check_missing_generator_related_model()
{
    ostringstream osstream;
    vector<GENERATOR*> generators = get_all_generators();
    size_t n = generators.size();
    GENERATOR* generator;

    bool model_missing_detected = false;
    osstream << "Error. Synchronous generator model is missing for the following generators:\n";
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];
        SYNC_GENERATOR_MODEL* genmodel = generator->get_sync_generator_model();
        if (genmodel == NULL)
        {
            osstream << generator->get_compound_device_name() << " [" << bus_number2bus_name(generator->get_generator_bus()) << "]\n"
                     << "Use default GENCLS model with H=1.0, D=0.0\n";
            model_missing_detected = true;
            string model_string = num2str(generator->get_generator_bus()) + ", GENCLS, " + generator->get_identifier() + ", 1.0, 0.0/";
            PSSE_IMEXPORTER importer(*toolkit);
            importer.load_one_model(model_string);
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }

    model_missing_detected = false;
    osstream << "Warning. Exciter model is missing for the following generators:\n";
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];
        SYNC_GENERATOR_MODEL* genmodel = generator->get_sync_generator_model();
        if (genmodel != NULL and genmodel->get_model_name() != "GENCLS")
        {
            EXCITER_MODEL* avrmodel = generator->get_exciter_model();
            if (avrmodel == NULL)
            {
                osstream << generator->get_compound_device_name() << " [" << bus_number2bus_name(generator->get_generator_bus()) << "]";
                model_missing_detected = true;
            }
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }

    model_missing_detected = false;
    osstream << "Warning. Turbine governor model is missing for the following generators:\n";
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];
        TURBINE_GOVERNOR_MODEL* govmodel = generator->get_turbine_governor_model();
        if (govmodel == NULL)
        {
            osstream << generator->get_compound_device_name() << " [" << bus_number2bus_name(generator->get_generator_bus()) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }
}

void PF_DATA::check_missing_wt_generator_related_model()
{
    ostringstream osstream;
    vector<WT_GENERATOR*> generators = get_all_wt_generators();
    size_t n = generators.size();
    WT_GENERATOR* generator;

    bool model_missing_detected = false;
    osstream << "Error. WT generator model is missing for the following wt generators:\n";
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];

        WT_GENERATOR_MODEL* genmodel = generator->get_wt_generator_model();
        if (genmodel == NULL)
        {
            osstream << generator->get_compound_device_name() << " [" << bus_number2bus_name(generator->get_generator_bus()) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }

    model_missing_detected = false;
    osstream << "Error. WT turbine model is missing for the following wt generators:\n";
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];

        WT_TURBINE_MODEL* turbmodel = generator->get_wt_turbine_model();
        if (turbmodel == NULL)
        {
            osstream << generator->get_compound_device_name() << " [" << bus_number2bus_name(generator->get_generator_bus()) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }

    model_missing_detected = false;
    osstream << "Warning. WT electrical model is missing for the following wt generators:\n";
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];

        WT_ELECTRICAL_MODEL* elecmodel = generator->get_wt_electrical_model();
        if (elecmodel == NULL)
        {
            osstream << generator->get_compound_device_name() << " [" << bus_number2bus_name(generator->get_generator_bus()) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }

    model_missing_detected = false;
    osstream << "Warning. WT aerodynamic model is missing for the following wt generators:\n";
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];

        WT_AERODYNAMIC_MODEL* aerdmodel = generator->get_wt_aerodynamic_model();
        if (aerdmodel == NULL)
        {
            osstream << generator->get_compound_device_name() << " [" << bus_number2bus_name(generator->get_generator_bus()) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }

    model_missing_detected = false;
    osstream << "Warning. WT electrical model is missing for the following wt generators:\n";
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];

        WT_ELECTRICAL_MODEL* elecmodel = generator->get_wt_electrical_model();
        if (elecmodel == NULL)
        {
            osstream << generator->get_compound_device_name() << " [" << bus_number2bus_name(generator->get_generator_bus()) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }

    model_missing_detected = false;
    osstream << "Warning. WT pitch model is missing for the following wt generators:\n";
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];

        WT_PITCH_MODEL* pitchmodel = generator->get_wt_pitch_model();
        if (pitchmodel == NULL)
        {
            osstream << generator->get_compound_device_name() << " [" << bus_number2bus_name(generator->get_generator_bus()) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }
}

void PF_DATA::check_missing_pv_unit_related_model()
{
    ostringstream osstream;
    vector<PV_UNIT*> pvs = get_all_pv_units();
    size_t n = pvs.size();
    PV_UNIT* pv;

    bool model_missing_detected = false;
    osstream << "Error. PV converter model is missing for the following PV units:\n";
    for (size_t i = 0; i != n; ++i)
    {
        pv = pvs[i];

        PV_CONVERTER_MODEL* convmodel = pv->get_pv_converter_model();
        if (convmodel == NULL)
        {
            osstream << pv->get_compound_device_name() << " [" << bus_number2bus_name(pv->get_unit_bus()) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }
}

void PF_DATA::check_missing_energy_storage_related_model()
{
    ostringstream osstream;
    osstream << "No energy storage is checked for model missing.";
    toolkit->show_information(osstream);
    return;

    vector<ENERGY_STORAGE*> estorages = get_all_energy_storages();
    size_t n = estorages.size();
    ENERGY_STORAGE* estorage;

    bool model_missing_detected = false;
    osstream << "Error. Energy storage model is missing for the following energy storages:\n";
    for (size_t i = 0; i != n; ++i)
    {
        estorage = estorages[i];

        ES_CONVERTER_MODEL* conv = estorage->get_es_converter_model();
        if (conv == NULL)
        {
            osstream << estorage->get_compound_device_name() << " [" << bus_number2bus_name(estorage->get_energy_storage_bus()) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }
}

void PF_DATA::check_missing_load_related_model()
{
    ostringstream osstream;
    vector<LOAD*> loads = get_all_loads();
    size_t n = loads.size();
    LOAD* load;

    bool model_missing_detected = false;
    osstream << "Error. Load model is missing for the following loads:\n";
    for (size_t i = 0; i != n; ++i)
    {
        load = loads[i];

        LOAD_MODEL* loadmodel = load->get_load_model();
        if (loadmodel == NULL)
        {
            osstream << load->get_compound_device_name() << " [" << bus_number2bus_name(load->get_load_bus()) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }
}

void PF_DATA::check_missing_ac_line_related_model()
{
    ostringstream osstream;
    osstream << "No line is checked for model missing.";
    toolkit->show_information(osstream);
    return;
}

void PF_DATA::check_missing_2t_lcc_hvdc_related_model()
{
    ostringstream osstream;
    vector<LCC_HVDC2T*> hvdcs = get_all_2t_lcc_hvdcs();
    size_t n = hvdcs.size();
    LCC_HVDC2T* hvdc;

    bool model_missing_detected = false;
    osstream << "Error. LCC_HVDC2T model is missing for the following LCC_HVDC2T links:\n";
    for (size_t i = 0; i != n; ++i)
    {
        hvdc = hvdcs[i];

        LCC_HVDC2T_MODEL* hvdcmodel = hvdc->get_2t_lcc_hvdc_model();
        if (hvdcmodel == NULL)
        {
            osstream << hvdc->get_compound_device_name() << " [" << bus_number2bus_name(hvdc->get_converter_bus(RECTIFIER)) << " -- " << bus_number2bus_name(hvdc->get_converter_bus(INVERTER)) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }
}

void PF_DATA::check_missing_vsc_hvdc_related_model()
{
    ostringstream osstream;
    vector<VSC_HVDC*> vsc_hvdcs = get_all_vsc_hvdcs();
    size_t n = vsc_hvdcs.size();
    VSC_HVDC* vsc_hvdc;

    bool model_missing_detected = false;
    osstream << "Error. VSC_HVDC model is missing for the following VSC_HVDC links:\n";
    for (size_t i = 0; i != n; ++i)
    {
        vsc_hvdc = vsc_hvdcs[i];

        VSC_HVDC_NETWORK_MODEL* vscmodel = vsc_hvdc->get_vsc_hvdc_network_model();
        size_t n_converter = vsc_hvdc->get_converter_count();
        if (vscmodel == NULL)
        {
            osstream << vsc_hvdc->get_compound_device_name() << "[";
            for (size_t j = 0; j != n_converter - 1; ++j)
            {
                osstream << bus_number2bus_name(vsc_hvdc->get_converter_ac_bus(j)) << " -- ";
            }
            osstream << bus_number2bus_name(vsc_hvdc->get_converter_ac_bus(n_converter - 1)) << "]";
            model_missing_detected = true;
        }
    }
    if (model_missing_detected == true)
    {
        toolkit->show_information(osstream);
    }
    else
    {
        osstream.str("");
    }
}

void PF_DATA::check_missing_equivalent_device_related_model()
{
    ostringstream osstream;
    osstream << "No equivalent device is checked for model missing.";
    toolkit->show_information(osstream);

    return;
}

void PF_DATA::check_missing_lcc_hvdc_related_model()
{
    ostringstream osstream;
    osstream << "No LCC LCC_HVDC2T is checked for model missing.";
    toolkit->show_information(osstream);

    return;
}


void PF_DATA::scale_load_power(const DEVICE_ID& did, double scale)
{
    LOAD* load = get_load(did);
    if (load != NULL)
    {
        scale += 1.0;

        load->set_nominal_constant_power_load_in_MVA(load->get_nominal_constant_power_load_in_MVA()*scale);
        load->set_nominal_constant_current_load_in_MVA(load->get_nominal_constant_current_load_in_MVA()*scale);
        load->set_nominal_constant_impedance_load_in_MVA(load->get_nominal_constant_impedance_load_in_MVA()*scale);
    }
}


void PF_DATA::scale_all_loads_power(double scale)
{
    vector<DEVICE_ID> loads = get_all_loads_device_id();
    size_t n = loads.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_load_power(loads[i], scale);
    }
}

void PF_DATA::scale_loads_power_at_bus(size_t bus, double scale)
{
    vector<DEVICE_ID> loads = get_loads_device_id_connecting_to_bus(bus);
    size_t n = loads.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_load_power(loads[i], scale);
    }
}

void PF_DATA::scale_loads_power_in_area(size_t area_number, double scale)
{
    vector<DEVICE_ID> loads = get_loads_device_id_in_area(area_number);
    size_t n = loads.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_load_power(loads[i], scale);
    }
}

void PF_DATA::scale_loads_power_in_zone(size_t zone_number, double scale)
{
    vector<DEVICE_ID> loads = get_loads_device_id_in_zone(zone_number);
    size_t n = loads.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_load_power(loads[i], scale);
    }
}



void PF_DATA::scale_source_power(const DEVICE_ID& did, double scale)
{
    SOURCE* source = get_source(did);
    if (source != NULL)
    {
        scale += 1.0;

        double P = source->get_p_generation_in_MW();
        double Q = source->get_q_generation_in_MVar();

        P *= scale;
        double pmax = source->get_p_max_in_MW();
        double pmin = source->get_p_min_in_MW();
        if (P > pmax)
        {
            source->set_p_generation_in_MW(pmax);
        }
        else
        {
            if (P < pmin)
            {
                source->set_p_generation_in_MW(pmin);
            }
            else
            {
                source->set_p_generation_in_MW(P);
            }
        }

        Q *= scale;
        double qmax = source->get_q_max_in_MVar();
        double qmin = source->get_q_min_in_MVar();
        if (Q > qmax)
        {
            source->set_q_generation_in_MVar(qmax);
        }
        else
        {
            if (Q < qmin)
            {
                source->set_q_generation_in_MVar(qmin);
            }
            else
            {
                source->set_q_generation_in_MVar(Q);
            }
        }
    }
}

void PF_DATA::scale_all_sources_power(double scale)
{
    vector<DEVICE_ID> sources = get_all_sources_device_id();
    size_t n = sources.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_source_power(sources[i], scale);
    }
}

void PF_DATA::scale_sources_power_at_bus(size_t bus, double scale)
{
    vector<DEVICE_ID> sources = get_sources_device_id_connecting_to_bus(bus);
    size_t n = sources.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_source_power(sources[i], scale);
    }
}

void PF_DATA::scale_sources_power_in_area(size_t area_number, double scale)
{
    vector<DEVICE_ID> sources = get_sources_device_id_in_area(area_number);
    size_t n = sources.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_source_power(sources[i], scale);
    }
}

void PF_DATA::scale_sources_power_in_zone(size_t zone_number, double scale)
{
    vector<DEVICE_ID> sources = get_sources_device_id_in_zone(zone_number);
    size_t n = sources.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_source_power(sources[i], scale);
    }
}

void PF_DATA::scale_generator_power(const DEVICE_ID& did, double scale)
{
    scale_source_power(did, scale);
}

void PF_DATA::scale_all_generators_power(double scale)
{
    vector<DEVICE_ID> generators = get_all_generators_device_id();
    size_t n = generators.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_generator_power(generators[i], scale);
    }
}

void PF_DATA::scale_generators_power_at_bus(size_t bus, double scale)
{
    vector<DEVICE_ID> generators = get_generators_device_id_connecting_to_bus(bus);
    size_t n = generators.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_generator_power(generators[i], scale);
    }
}

void PF_DATA::scale_generators_power_in_area(size_t area_number, double scale)
{
    vector<DEVICE_ID> generators = get_generators_device_id_in_area(area_number);
    size_t n = generators.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_generator_power(generators[i], scale);
    }
}

void PF_DATA::scale_generators_power_in_zone(size_t zone_number, double scale)
{
    vector<DEVICE_ID> generators = get_generators_device_id_in_zone(zone_number);
    size_t n = generators.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_generator_power(generators[i], scale);
    }
}

void PF_DATA::scale_wt_generator_power(const DEVICE_ID& did, double scale)
{
    scale_source_power(did, scale);
}

void PF_DATA::scale_all_wt_generators_power(double scale)
{
    vector<DEVICE_ID> wt_generators = get_all_wt_generators_device_id();
    size_t n = wt_generators.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_wt_generator_power(wt_generators[i], scale);
    }
}

void PF_DATA::scale_wt_generators_power_at_bus(size_t bus, double scale)
{
    vector<DEVICE_ID> wt_generators = get_wt_generators_device_id_connecting_to_bus(bus);
    size_t n = wt_generators.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_wt_generator_power(wt_generators[i], scale);
    }
}

void PF_DATA::scale_wt_generators_power_in_area(size_t area_number, double scale)
{
    vector<DEVICE_ID> wt_generators = get_wt_generators_device_id_in_area(area_number);
    size_t n = wt_generators.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_wt_generator_power(wt_generators[i], scale);
    }
}

void PF_DATA::scale_wt_generators_power_in_zone(size_t zone_number, double scale)
{
    vector<DEVICE_ID> wt_generators = get_wt_generators_device_id_in_zone(zone_number);
    size_t n = wt_generators.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_wt_generator_power(wt_generators[i], scale);
    }
}


void PF_DATA::scale_pv_unit_power(const DEVICE_ID& did, double scale)
{
    scale_source_power(did, scale);
}

void PF_DATA::scale_all_pv_units_power(double scale)
{
    vector<DEVICE_ID> pv_units = get_all_pv_units_device_id();
    size_t n = pv_units.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_pv_unit_power(pv_units[i], scale);
    }
}

void PF_DATA::scale_pv_units_power_at_bus(size_t bus, double scale)
{
    vector<DEVICE_ID> pv_units = get_pv_units_device_id_connecting_to_bus(bus);
    size_t n = pv_units.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_pv_unit_power(pv_units[i], scale);
    }
}

void PF_DATA::scale_pv_units_power_in_area(size_t area_number, double scale)
{
    vector<DEVICE_ID> pv_units = get_pv_units_device_id_in_area(area_number);
    size_t n = pv_units.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_pv_unit_power(pv_units[i], scale);
    }
}

void PF_DATA::scale_pv_units_power_in_zone(size_t zone_number, double scale)
{
    vector<DEVICE_ID> pv_units = get_pv_units_device_id_in_zone(zone_number);
    size_t n = pv_units.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_pv_unit_power(pv_units[i], scale);
    }
}

void PF_DATA::scale_energy_storage_power(const DEVICE_ID& did, double scale)
{
    scale_source_power(did, scale);
}

void PF_DATA::scale_all_energy_storages_power(double scale)
{
    vector<DEVICE_ID> ess = get_all_energy_storages_device_id();
    size_t n = ess.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_energy_storage_power(ess[i], scale);
    }
}

void PF_DATA::scale_energy_storages_power_at_bus(size_t bus, double scale)
{
    vector<DEVICE_ID> ess = get_energy_storages_device_id_connecting_to_bus(bus);
    size_t n = ess.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_energy_storage_power(ess[i], scale);
    }
}

void PF_DATA::scale_energy_storages_power_in_area(size_t area_number, double scale)
{
    vector<DEVICE_ID> ess = get_energy_storages_device_id_in_area(area_number);
    size_t n = ess.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_energy_storage_power(ess[i], scale);
    }
}

void PF_DATA::scale_energy_storages_power_in_zone(size_t zone_number, double scale)
{
    vector<DEVICE_ID> ess = get_energy_storages_device_id_in_zone(zone_number);
    size_t n = ess.size();
    for (size_t i = 0; i != n; ++i)
    {
        scale_energy_storage_power(ess[i], scale);
    }
}

void PF_DATA::clear_bus(size_t bus)
{
    if (is_bus_exist(bus))
    {
        clear_sources_connecting_to_bus(bus);
        clear_loads_connecting_to_bus(bus);
        clear_ac_lines_connecting_to_bus(bus);
        clear_transformers_connecting_to_bus(bus);
        clear_fixed_shunts_connecting_to_bus(bus);
        clear_2t_lcc_hvdcs_connecting_to_bus(bus);
        clear_equivalent_devices_connecting_to_bus(bus);

        size_t current_index = get_bus_index(bus);

        vector<BUS>::iterator iter = Bus.begin();
        std::advance(iter, current_index);
        Bus.erase(iter);
        bus_index.set_bus_with_index(bus, INDEX_NOT_EXIST);

        size_t max_bus_number = bus_index.get_max_bus_number();

        size_t max_bus_number_plus_1 = max_bus_number + 1;
        for (size_t i = 1; i != max_bus_number_plus_1; ++i)
        {
            size_t index = bus_index.get_index_of_bus(i);
            if (index != INDEX_NOT_EXIST and index > current_index)
            {
                bus_index.set_bus_with_index(i, index - 1);
            }
        }
        update_in_service_bus_count();
    }
}

void PF_DATA::clear_all_buses()
{
    Bus.clear();
    bus_index.clear();

    clear_all_sources();
    clear_all_loads();
    clear_all_ac_lines();
    clear_all_transformers();
    clear_all_fixed_shunts();
    clear_all_2t_lcc_hvdcs();
    clear_all_equivalent_devices();
    clear_all_areas();
    clear_all_zones();
    clear_all_owners();
    update_in_service_bus_count();
}


void PF_DATA::clear_generator(const DEVICE_ID& device_id)
{
    if (is_generator_exist(device_id))
    {
        size_t current_index = get_generator_index(device_id);

        vector<GENERATOR>::iterator iter_generator = Generator.begin();

        std::advance(iter_generator, current_index);

        Generator.erase(iter_generator);

        generator_index.set_device_index(device_id, INDEX_NOT_EXIST);

        generator_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_generators_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    TERMINAL terminal;
    device_id.set_device_type(STEPS_GENERATOR);
    while (true)
    {
        vector<GENERATOR*> generator = get_generators_connecting_to_bus(bus);
        if (generator.size() != 0)
        {
            device_id = generator[0]->get_device_id();
            clear_generator(device_id);
        }
        else
        {
            break;
        }
    }
}

void PF_DATA::clear_all_generators()
{
    Generator.clear();
    generator_index.clear();
}


void PF_DATA::clear_wt_generator(const DEVICE_ID& device_id)
{
    if (is_wt_generator_exist(device_id))
    {
        size_t current_index = get_wt_generator_index(device_id);

        vector<WT_GENERATOR>::iterator iter_wt_generator = WT_Generator.begin();

        std::advance(iter_wt_generator, current_index);
        WT_Generator.erase(iter_wt_generator);
        wt_generator_index.set_device_index(device_id, INDEX_NOT_EXIST);

        wt_generator_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_wt_generators_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    TERMINAL terminal;
    device_id.set_device_type(STEPS_WT_GENERATOR);
    while (true)
    {
        vector<WT_GENERATOR*> wt_generator = get_wt_generators_connecting_to_bus(bus);
        if (wt_generator.size() != 0)
        {
            device_id = wt_generator[0]->get_device_id();
            clear_wt_generator(device_id);
        }
        else
        {
            break;
        }
    }
}

void PF_DATA::clear_all_wt_generators()
{
    WT_Generator.clear();
    wt_generator_index.clear();
}

void PF_DATA::clear_pv_unit(const DEVICE_ID& device_id)
{
    if (is_pv_unit_exist(device_id))
    {
        size_t current_index = get_pv_unit_index(device_id);

        vector<PV_UNIT>::iterator iter_pv_unit = PV_Unit.begin();

        std::advance(iter_pv_unit, current_index);
        PV_Unit.erase(iter_pv_unit);
        pv_unit_index.set_device_index(device_id, INDEX_NOT_EXIST);

        pv_unit_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}


void PF_DATA::clear_pv_units_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    TERMINAL terminal;
    device_id.set_device_type(STEPS_PV_UNIT);
    while (true)
    {
        vector<PV_UNIT*> pv_unit = get_pv_units_connecting_to_bus(bus);
        if (pv_unit.size() != 0)
        {
            device_id = pv_unit[0]->get_device_id();
            clear_pv_unit(device_id);
        }
        else
        {
            break;
        }
    }
}

void PF_DATA::clear_all_pv_units()
{
    PV_Unit.clear();
    pv_unit_index.clear();
}

void PF_DATA::clear_sources_connecting_to_bus(const size_t bus)
{
    clear_generators_connecting_to_bus(bus);
    clear_wt_generators_connecting_to_bus(bus);
    clear_pv_units_connecting_to_bus(bus);
    clear_energy_storages_connecting_to_bus(bus);
}

void PF_DATA::clear_all_sources()
{
    clear_all_generators();
    clear_all_wt_generators();
    clear_all_pv_units();
    clear_all_energy_storages();
}

void PF_DATA::clear_load(const DEVICE_ID& device_id)
{
    if (is_load_exist(device_id))
    {
        size_t current_index = get_load_index(device_id);

        vector<LOAD>::iterator iter_load = Load.begin();


        std::advance(iter_load, current_index);
        Load.erase(iter_load);
        load_index.set_device_index(device_id, INDEX_NOT_EXIST);

        load_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_loads_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    TERMINAL terminal;
    device_id.set_device_type(STEPS_LOAD);
    while (true)
    {
        vector<LOAD*> load = get_loads_connecting_to_bus(bus);
        if (load.size() != 0)
        {
            device_id = load[0]->get_device_id();
            clear_load(device_id);
        }
        else
        {
            break;
        }

    }
}

void PF_DATA::clear_all_loads()
{
    Load.clear();
    load_index.clear();
}


void PF_DATA::clear_ac_line(const DEVICE_ID& device_id)
{
    if (is_ac_line_exist(device_id))
    {
        size_t current_index = get_ac_line_index(device_id);

        vector<AC_LINE>::iterator iter_line = Ac_line.begin();


        std::advance(iter_line, current_index);
        Ac_line.erase(iter_line);
        ac_line_index.set_device_index(device_id, INDEX_NOT_EXIST);

        ac_line_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_ac_lines_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    TERMINAL terminal;
    device_id.set_device_type(STEPS_AC_LINE);
    while (true)
    {
        vector<AC_LINE*> line = get_ac_lines_connecting_to_bus(bus);
        if (line.size() != 0)
        {
            device_id = line[0]->get_device_id();
            clear_ac_line(device_id);
        }
        else
        {
            break;
        }

    }
}

void PF_DATA::clear_all_ac_lines()
{
    Ac_line.clear();
    ac_line_index.clear();
}

void PF_DATA::clear_transformer(const DEVICE_ID& device_id)
{
    if (is_transformer_exist(device_id))
    {
        size_t current_index = get_transformer_index(device_id);

        vector<TRANSFORMER>::iterator iter_transformer = Transformer.begin();

        std::advance(iter_transformer, current_index);
        Transformer.erase(iter_transformer);
        transformer_index.set_device_index(device_id, INDEX_NOT_EXIST);

        transformer_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_transformers_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    TERMINAL terminal;
    device_id.set_device_type(STEPS_TRANSFORMER);
    while (true)
    {
        vector<TRANSFORMER*> transformer = get_transformers_connecting_to_bus(bus);

        if (transformer.size() != 0)
        {
            device_id = transformer[0]->get_device_id();
            clear_transformer(device_id);
        }
        else
        {
            break;
        }
    }
}

void PF_DATA::clear_all_transformers()
{
    Transformer.clear();
    transformer_index.clear();
}


void PF_DATA::clear_fixed_shunt(const DEVICE_ID& device_id)
{
    if (is_fixed_shunt_exist(device_id))
    {
        size_t current_index = get_fixed_shunt_index(device_id);

        vector<FIXED_SHUNT>::iterator iter_fs = Fixed_shunt.begin();


        std::advance(iter_fs, current_index);
        Fixed_shunt.erase(iter_fs);
        fixed_shunt_index.set_device_index(device_id, INDEX_NOT_EXIST);

        fixed_shunt_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}


void PF_DATA::clear_fixed_shunts_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_FIXED_SHUNT);
    TERMINAL terminal;
    terminal.append_bus(bus);
    device_id.set_device_terminal(terminal);
    while (true)
    {
        vector<FIXED_SHUNT*> fixed_shunt = get_fixed_shunts_connecting_to_bus(bus);
        if (fixed_shunt.size() != 0)
        {
            /*device_id.set_device_identifier_index(fixed_shunt[0]->get_identifier_index());*/
            device_id = fixed_shunt[0]->get_device_id();
            clear_fixed_shunt(device_id);
            fixed_shunt.clear();
        }
        else
        {
            break;
        }
    }
}

void PF_DATA::clear_all_fixed_shunts()
{
    Fixed_shunt.clear();
    fixed_shunt_index.clear();
}

void PF_DATA::clear_2t_lcc_hvdc(const DEVICE_ID& device_id)
{
    if (is_2t_lcc_hvdc_exist(device_id))
    {
        size_t current_index = get_2t_lcc_hvdc_index(device_id);

        vector<LCC_HVDC2T>::iterator iter_hvdc = Lcc_hvdc2t.begin();


        std::advance(iter_hvdc, current_index);
        Lcc_hvdc2t.erase(iter_hvdc);
        hvdc_index.set_device_index(device_id, INDEX_NOT_EXIST);

        hvdc_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_2t_lcc_hvdcs_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    TERMINAL terminal;
    device_id.set_device_type(STEPS_LCC_HVDC2T);
    while (true)
    {
        vector<LCC_HVDC2T*> hvdc = get_2t_lcc_hvdcs_connecting_to_bus(bus);
        if (hvdc.size() != 0)
        {
            device_id = hvdc[0]->get_device_id();
            clear_2t_lcc_hvdc(device_id);
        }
        else
        {
            break;
        }
    }
}

void PF_DATA::clear_all_2t_lcc_hvdcs()
{
    Lcc_hvdc2t.clear();
    hvdc_index.clear();
}

void PF_DATA::clear_vsc_hvdc(const DEVICE_ID& device_id)
{
    if (is_vsc_hvdc_exist(device_id))
    {
        size_t current_index = get_vsc_hvdc_index(device_id);

        vector<VSC_HVDC>::iterator iter_vsc_hvdc = Vsc_hvdc.begin();


        std::advance(iter_vsc_hvdc, current_index);
        Vsc_hvdc.erase(iter_vsc_hvdc);
        vsc_hvdc_index.set_device_index(device_id, INDEX_NOT_EXIST);

        vsc_hvdc_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_vsc_hvdc(const string vsc_name)
{
    if (is_vsc_hvdc_exist(vsc_name))
    {
        size_t current_index = get_vsc_hvdc_index(vsc_name);

        VSC_HVDC* vsc_hvdc = get_vsc_hvdc(vsc_name);
        DEVICE_ID device_id = vsc_hvdc->get_device_id();

        vector<VSC_HVDC>::iterator iter_vsc_hvdc = Vsc_hvdc.begin();

        std::advance(iter_vsc_hvdc, current_index);
        Vsc_hvdc.erase(iter_vsc_hvdc);
        vsc_hvdc_index.set_device_index(device_id, INDEX_NOT_EXIST);

        vsc_hvdc_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_vsc_hvdcs_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    TERMINAL terminal;
    device_id.set_device_type(STEPS_VSC_HVDC);
    while (true)
    {
        vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_connecting_to_bus(bus);
        if (vsc_hvdcs.size() != 0)
        {
            device_id = vsc_hvdcs[0]->get_device_id();
            clear_vsc_hvdc(device_id);
            vsc_hvdcs.clear();
        }
        else
        {
            break;
        }
    }
}

void PF_DATA::clear_all_vsc_hvdcs()
{
    Vsc_hvdc.clear();
    vsc_hvdc_index.clear();
}

void PF_DATA::clear_equivalent_device(const DEVICE_ID& device_id)
{
    if (is_equivalent_device_exist(device_id))
    {
        size_t current_index = get_equivalent_device_index(device_id);

        vector<EQUIVALENT_DEVICE>::iterator iter_edevice = Equivalent_device.begin();


        std::advance(iter_edevice, current_index);
        Equivalent_device.erase(iter_edevice);
        equivalent_device_index.set_device_index(device_id, INDEX_NOT_EXIST);

        equivalent_device_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_equivalent_devices_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    TERMINAL terminal;
    device_id.set_device_type(STEPS_EQUIVALENT_DEVICE);
    while (true)
    {
        vector<EQUIVALENT_DEVICE*> edevice = get_equivalent_devices_connecting_to_bus(bus);
        if (edevice.size() != 0)
        {
            /*terminal.clear();
            terminal.append_bus(edevice[0]->get_equivalent_device_bus());
            device_id.set_device_terminal(terminal);
            device_id.set_device_identifier_index(edevice[0]->get_identifier_index());*/
            device_id = edevice[0]->get_device_id();
            clear_equivalent_device(device_id);
        }
        else
        {
            break;
        }
    }
}

void PF_DATA::clear_all_equivalent_devices()
{
    Equivalent_device.clear();
    equivalent_device_index.clear();
}


void PF_DATA::clear_energy_storage(const DEVICE_ID& device_id)
{
    if (is_energy_storage_exist(device_id))
    {
        size_t current_index = get_energy_storage_index(device_id);

        vector<ENERGY_STORAGE>::iterator iter_edevice = Energy_storage.begin();


        std::advance(iter_edevice, current_index);
        Energy_storage.erase(iter_edevice);
        energy_storage_index.set_device_index(device_id, INDEX_NOT_EXIST);

        energy_storage_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_energy_storages_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    TERMINAL terminal;
    device_id.set_device_type(STEPS_ENERGY_STORAGE);
    while (true)
    {
        vector<ENERGY_STORAGE*> estorage = get_energy_storages_connecting_to_bus(bus);
        if (estorage.size() != 0)
        {
            /*terminal.clear();
            terminal.append_bus(estorage[0]->get_energy_storage_bus());
            device_id.set_device_terminal(terminal);
            device_id.set_device_identifier_index(estorage[0]->get_identifier_index());*/
            device_id = estorage[0]->get_device_id();
            clear_energy_storage(device_id);
        }
        else
        {
            break;
        }
    }
}

void PF_DATA::clear_all_energy_storages()
{
    Energy_storage.clear();
    energy_storage_index.clear();
}

void PF_DATA::clear_lcc_hvdc(const DEVICE_ID& device_id)
{
    if (is_lcc_hvdc_exist(device_id))
    {
        size_t current_index = get_lcc_hvdc_index(device_id);

        vector<LCC_HVDC>::iterator iter_edevice = Lcc_hvdc.begin();


        std::advance(iter_edevice, current_index);
        Lcc_hvdc.erase(iter_edevice);
        lcc_hvdc_index.set_device_index(device_id, INDEX_NOT_EXIST);

        lcc_hvdc_index.decrease_index_by_1_for_device_with_index_greater_than(current_index);
    }
}

void PF_DATA::clear_lcc_hvdcs_connecting_to_bus(const size_t bus)
{
    DEVICE_ID device_id;
    device_id.set_device_type(STEPS_LCC_HVDC);
    while (true)
    {
        vector<LCC_HVDC*> hvdcs = get_lcc_hvdcs_connecting_to_bus(bus);
        if (hvdcs.size() != 0)
        {
            /*device_id.set_device_name_index(hvdcs[0]->get_name_index());*/
            device_id = hvdcs[0]->get_device_id();
            clear_lcc_hvdc(device_id);
        }
        else
        {
            break;
        }
    }

}

void PF_DATA::clear_all_lcc_hvdcs()
{
    Lcc_hvdc.clear();
    lcc_hvdc_index.clear();
}

void PF_DATA::clear_area(size_t area)
{
    ;
}

void PF_DATA::clear_all_areas()
{
    Area_.clear();
    area_index.clear();
}

void PF_DATA::clear_zone(size_t zone)
{
    ;
}

void PF_DATA::clear_all_zones()
{
    Zone_.clear();
    zone_index.clear();
}

void PF_DATA::clear_owner(size_t owner)
{
    ;
}

void PF_DATA::clear_all_owners()
{
    Owner_.clear();
    owner_index.clear();
}


void PF_DATA::trip_bus(size_t bus)
{
    if (is_bus_exist(bus))
    {
        size_t index = get_bus_index(bus);
        if (Bus[index].get_bus_type() != OUT_OF_SERVICE)
        {
            ostringstream osstream;

            osstream << "The following devices are tripped before bus " << bus << " is tripped." << endl;

            size_t n = 0;

            vector<GENERATOR*> gens = get_generators_connecting_to_bus(bus);
            n = gens.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (gens[i]->get_status() == true)
                {
                    gens[i]->set_status(false);
                    osstream << gens[i]->get_compound_device_name() << endl;
                }
            }

            vector<WT_GENERATOR*> wts = get_wt_generators_connecting_to_bus(bus);
            n = wts.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (wts[i]->get_status() == true)
                {
                    wts[i]->set_status(false);
                    osstream << wts[i]->get_compound_device_name() << endl;
                }
            }

            vector<PV_UNIT*> pvs = get_pv_units_connecting_to_bus(bus);
            n = pvs.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (pvs[i]->get_status() == true)
                {
                    pvs[i]->set_status(false);
                    osstream << pvs[i]->get_compound_device_name() << endl;
                }
            }

            vector<ENERGY_STORAGE*> ess = get_energy_storages_connecting_to_bus(bus);
            n = ess.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (ess[i]->get_status() == true)
                {
                    ess[i]->set_status(false);
                    osstream << ess[i]->get_compound_device_name() << endl;
                }
            }

            vector<LOAD*> loads = get_loads_connecting_to_bus(bus);
            n = loads.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (loads[i]->get_status() == true)
                {
                    loads[i]->set_status(false);
                    osstream << loads[i]->get_compound_device_name() << endl;
                }
            }

            vector<FIXED_SHUNT*> fshunts = get_fixed_shunts_connecting_to_bus(bus);
            n = fshunts.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (fshunts[i]->get_status() == true)
                {
                    fshunts[i]->set_status(false);
                    osstream << fshunts[i]->get_compound_device_name() << endl;
                }
            }

            vector<AC_LINE*> lines = get_ac_lines_connecting_to_bus(bus);
            n = lines.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (lines[i]->get_sending_side_breaker_status() == true or lines[i]->get_receiving_side_breaker_status() == true)
                {
                    lines[i]->set_sending_side_breaker_status(false);
                    lines[i]->set_receiving_side_breaker_status(false);
                    osstream << lines[i]->get_compound_device_name() << endl;
                }
            }

            vector<TRANSFORMER*> transes = get_transformers_connecting_to_bus(bus);
            n = transes.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (transes[i]->get_winding_breaker_status(PRIMARY_SIDE) == true or transes[i]->get_winding_breaker_status(SECONDARY_SIDE) == true or
                        (transes[i]->is_three_winding_transformer() and transes[i]->get_winding_breaker_status(TERTIARY_SIDE) == true) )
                {
                    transes[i]->set_winding_breaker_status(PRIMARY_SIDE, false);
                    transes[i]->set_winding_breaker_status(SECONDARY_SIDE, false);
                    if (transes[i]->is_two_winding_transformer())
                    {
                        continue;
                    }
                    else
                    {
                        transes[i]->set_winding_breaker_status(TERTIARY_SIDE, false);
                    }

                    osstream << transes[i]->get_compound_device_name() << endl;
                }
            }

            vector<LCC_HVDC2T*> hvdcs = get_2t_lcc_hvdcs_connecting_to_bus(bus);
            n = hvdcs.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (hvdcs[i]->get_status() == true)
                {
                    hvdcs[i]->set_status(false);
                    osstream << hvdcs[i]->get_compound_device_name() << endl;
                }
            }

            vector<VSC_HVDC*> vscs = get_vsc_hvdcs_connecting_to_bus(bus);
            n = vscs.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (vscs[i]->get_status() == true)
                {
                    vscs[i]->set_status(false);
                    osstream << vscs[i]->get_compound_device_name() << endl;
                }
            }

            vector<EQUIVALENT_DEVICE*> edevices = get_equivalent_devices_connecting_to_bus(bus);
            n = edevices.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (edevices[i]->get_status() == true)
                {
                    edevices[i]->set_status(false);
                    osstream << edevices[i]->get_compound_device_name() << endl;
                }
            }

            Bus[index].set_bus_type(OUT_OF_SERVICE);
            osstream << "Bus " << bus << " is tripped.";
            toolkit->show_information(osstream);

            update_in_service_bus_count();
        }
    }
}

void PF_DATA::check_device_status_for_out_of_service_buses()
{
    vector<BUS*> buses = get_all_buses();
    BUS* bus;
    size_t nbus = buses.size();
    for (size_t i = 0; i != nbus; ++i)
    {
        bus = buses[i];
        if (bus->get_bus_type() != OUT_OF_SERVICE)
        {
            continue;
        }
        else
        {
            size_t bus_number = bus->get_bus_number();
            check_source_status_for_out_of_service_bus(bus_number);
            check_load_status_for_out_of_service_bus(bus_number);
            check_fixed_shunt_status_for_out_of_service_bus(bus_number);
            check_ac_line_status_for_out_of_service_bus(bus_number);
            check_transformer_status_for_out_of_service_bus(bus_number);
            check_2t_lcc_hvdc_status_for_out_of_service_bus(bus_number);
            check_vsc_hvdc_status_for_out_of_service_bus(bus_number);
            check_lcc_hvdc_status_for_out_of_service_bus(bus_number);
        }
    }
}

void PF_DATA::check_source_status_for_out_of_service_bus(size_t bus)
{
    vector<SOURCE*> sources = get_sources_connecting_to_bus(bus);
    SOURCE* source;
    size_t nsource = sources.size();
    for (size_t i = 0; i != nsource; ++i)
    {
        source = sources[i];
        if (source->get_status() == true)
        {
            source->set_status(false);
        }
    }
}

void PF_DATA::check_load_status_for_out_of_service_bus(size_t bus)
{
    vector<LOAD*> loads = get_loads_connecting_to_bus(bus);
    LOAD* load;
    size_t nload = loads.size();
    for (size_t i = 0; i != nload; ++i)
    {
        load = loads[i];
        if (load->get_status() == true)
        {
            load->set_status(false);
        }
    }
}

void PF_DATA::check_fixed_shunt_status_for_out_of_service_bus(size_t bus)
{
    vector<FIXED_SHUNT*> shunts = get_fixed_shunts_connecting_to_bus(bus);
    FIXED_SHUNT* shunt;
    size_t nshunt = shunts.size();
    for (size_t i = 0; i != nshunt; ++i)
    {
        shunt = shunts[i];
        if (shunt->get_status() == true)
        {
            shunt->set_status(false);
        }
    }
}

void PF_DATA::check_ac_line_status_for_out_of_service_bus(size_t bus)
{
    vector<AC_LINE*> lines = get_ac_lines_connecting_to_bus(bus);
    AC_LINE* line;
    size_t nline = lines.size();
    for (size_t i = 0; i != nline; ++i)
    {
        line = lines[i];
        if (line->get_sending_side_bus() == bus)
        {
            if (line->get_sending_side_breaker_status() == true)
            {
                line->set_sending_side_breaker_status(false);
            }
        }
        else
        {
            if (line->get_receiving_side_breaker_status() == true)
            {
                line->set_receiving_side_breaker_status(false);
            }
        }
    }
}

void PF_DATA::check_transformer_status_for_out_of_service_bus(size_t bus)
{
    vector<TRANSFORMER*> transformers = get_transformers_connecting_to_bus(bus);
    TRANSFORMER* transformer;
    size_t ntrans = transformers.size();
    for (size_t i = 0; i != ntrans; ++i)
    {
        transformer = transformers[i];
        if (transformer->get_winding_bus(PRIMARY_SIDE) == bus)
        {
            if (transformer->get_winding_breaker_status(PRIMARY_SIDE) == true)
            {
                transformer->set_winding_breaker_status(PRIMARY_SIDE, false);
            }
        }
        else
        {
            if (transformer->get_winding_bus(SECONDARY_SIDE) == bus)
            {
                if (transformer->get_winding_breaker_status(SECONDARY_SIDE) == true)
                {
                    transformer->set_winding_breaker_status(SECONDARY_SIDE, false);
                }
            }
            else
            {
                if (transformer->get_winding_breaker_status(TERTIARY_SIDE) == true)
                {
                    transformer->set_winding_breaker_status(TERTIARY_SIDE, false);
                }
            }
        }
    }
}

void PF_DATA::check_2t_lcc_hvdc_status_for_out_of_service_bus(size_t bus)
{
    vector<LCC_HVDC2T*> hvdcs = get_2t_lcc_hvdcs_connecting_to_bus(bus);
    LCC_HVDC2T* hvdc;
    size_t nhvdc = hvdcs.size();
    for (size_t i = 0; i != nhvdc; ++i)
    {
        hvdc = hvdcs[i];
        if (hvdc->get_status() == true)
        {
            hvdc->set_status(false);
        }
    }
}

void PF_DATA::check_vsc_hvdc_status_for_out_of_service_bus(size_t bus)
{
    vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_connecting_to_bus(bus);
    VSC_HVDC* vsc_hvdc;
    size_t nvsc = vsc_hvdcs.size();
    for (size_t i = 0; i != nvsc; ++i)
    {
        vsc_hvdc = vsc_hvdcs[i];
        if (vsc_hvdc->get_status() == true)
        {
            vsc_hvdc->set_status(false);
        }
    }
}

void PF_DATA::check_lcc_hvdc_status_for_out_of_service_bus(size_t bus)
{
    vector<LCC_HVDC*> hvdcs = get_lcc_hvdcs_connecting_to_bus(bus);
    LCC_HVDC* hvdc;
    size_t nhvdc = hvdcs.size();
    for (size_t i = 0; i != nhvdc; ++i)
    {
        hvdc = hvdcs[i];
        if (hvdc->get_status() == true)
        {
            hvdc->set_status(false);
        }
    }
}

void PF_DATA::calibrate_sources_pmax_and_pmin_with_dynamic_model()
{
    calibrate_generators_pmax_and_pmin_with_turbine_governor_model();
}

void PF_DATA::calibrate_generators_pmax_and_pmin_with_turbine_governor_model()
{
    vector<GENERATOR*> generators = get_all_generators();
    size_t n = generators.size();
    GENERATOR* generator;
    for (size_t i = 0; i != n; ++i)
    {
        generator = generators[i];
        double mbase = generator->get_mbase_in_MVA();
        TURBINE_GOVERNOR_MODEL* model = generator->get_turbine_governor_model();
        if (model != NULL)
        {
            double pmax = model->get_mechanical_power_upper_limit_in_pu_based_on_mbase() * mbase;
            double pmin = model->get_mechanical_power_lower_limit_in_pu_based_on_mbase() * mbase;
            generator->set_p_max_in_MW(pmax);
            generator->set_p_min_in_MW(pmin);
        }
    }
}

BUS_TYPE PF_DATA::get_bus_type(size_t bus)
{
    BUS* busptr = get_bus(bus);
    if (busptr != NULL)
    {
        return busptr->get_bus_type();
    }
    else
    {
        return OUT_OF_SERVICE;
    }
}

double PF_DATA::get_bus_base_frequency_in_Hz(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_base_frequency_in_Hz();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_base_voltage_in_kV(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_base_voltage_in_kV();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_positive_sequence_voltage_in_pu(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_positive_sequence_voltage_in_pu();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_positive_sequence_voltage_in_kV(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_positive_sequence_voltage_in_kV();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_positive_sequence_angle_in_deg(size_t bus_index)
{
    return rad2deg(get_bus_positive_sequence_angle_in_rad(bus_index));
}

double PF_DATA::get_bus_positive_sequence_angle_in_rad(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_positive_sequence_angle_in_rad();
    }
    else
    {
        return 0.0;
    }
}

complex<double> PF_DATA::get_bus_positive_sequence_complex_voltage_in_pu(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_positive_sequence_complex_voltage_in_pu();
    }
    else
    {
        return 0.0;
    }
}

complex<double> PF_DATA::get_bus_positive_sequence_complex_voltage_in_kV(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_positive_sequence_complex_voltage_in_kV();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_negative_sequence_voltage_in_pu(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_negative_sequence_voltage_in_pu();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_negative_sequence_voltage_in_kV(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_negative_sequence_voltage_in_kV();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_negative_sequence_angle_in_deg(size_t bus_index)
{
    return rad2deg(get_bus_negative_sequence_angle_in_rad(bus_index));
}

double PF_DATA::get_bus_negative_sequence_angle_in_rad(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_negative_sequence_angle_in_rad();
    }
    else
    {
        return 0.0;
    }
}

complex<double> PF_DATA::get_bus_negative_sequence_complex_voltage_in_pu(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_negative_sequence_complex_voltage_in_pu();
    }
    else
    {
        return 0.0;
    }
}

complex<double> PF_DATA::get_bus_negative_sequence_complex_voltage_in_kV(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_negative_sequence_complex_voltage_in_kV();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_zero_sequence_voltage_in_pu(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_zero_sequence_voltage_in_pu();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_zero_sequence_voltage_in_kV(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_zero_sequence_voltage_in_kV();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_zero_sequence_angle_in_deg(size_t bus_index)
{
    return rad2deg(get_bus_zero_sequence_angle_in_rad(bus_index));
}

double PF_DATA::get_bus_zero_sequence_angle_in_rad(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_zero_sequence_angle_in_rad();
    }
    else
    {
        return 0.0;
    }
}

complex<double> PF_DATA::get_bus_zero_sequence_complex_voltage_in_pu(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_zero_sequence_complex_voltage_in_pu();
    }
    else
    {
        return 0.0;
    }
}

complex<double> PF_DATA::get_bus_zero_sequence_complex_voltage_in_kV(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_zero_sequence_complex_voltage_in_kV();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_frequency_deviation_in_pu(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        BUS_FREQUENCY_MODEL* fmodel = busptr->get_bus_frequency_model();

        return fmodel->get_frequency_deviation_in_pu();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_frequency_deviation_in_Hz(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        BUS_FREQUENCY_MODEL* fmodel = busptr->get_bus_frequency_model();

        return fmodel->get_frequency_deviation_in_Hz();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_frequency_in_Hz(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        BUS_FREQUENCY_MODEL* fmodel = busptr->get_bus_frequency_model();

        return fmodel->get_frequency_in_Hz();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_bus_frequency_in_pu(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        BUS_FREQUENCY_MODEL* fmodel = busptr->get_bus_frequency_model();

        return fmodel->get_frequency_in_pu();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_voltage_to_regulate_of_physical_bus_in_pu(size_t bus_index)
{
    BUS* busptr = get_bus(bus_index);
    if (busptr != NULL)
    {
        return busptr->get_voltage_to_regulate_in_pu();
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_regulatable_p_max_at_physical_bus_in_MW(size_t bus_index)
{
    if (is_bus_exist(bus_index))
    {
        BUS* busptr = get_bus(bus_index);
        BUS_TYPE btype = busptr->get_bus_type();
        if (btype == SLACK_TYPE)
        {
            double total_p_max_in_MW = 0.0;

            vector<SOURCE*> sources = get_sources_connecting_to_bus(bus_index);

            size_t n = sources.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (sources[i]->get_status() == false)
                {
                    continue;
                }
                total_p_max_in_MW += sources[i]->get_p_max_in_MW();
            }

            vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_connecting_to_bus(bus_index);

            n = vsc_hvdcs.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (vsc_hvdcs[i]->get_status() == false)
                {
                    continue;
                }
                size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(bus_index);
                if (vsc_hvdcs[i]->get_converter_status(index) == true and
                        vsc_hvdcs[i]->get_converter_active_power_operation_mode(index) == VSC_AC_VOLTAGE_ANGLE_CONTROL)
                {
                    total_p_max_in_MW += vsc_hvdcs[i]->get_converter_Pmax_in_MW(index);
                }
            }

            return total_p_max_in_MW;
        }
        else
        {
            return 0.0;
        }
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_regulatable_p_min_at_physical_bus_in_MW(size_t bus)
{
    if (is_bus_exist(bus))
    {
        BUS* busptr = get_bus(bus);
        BUS_TYPE btype = busptr->get_bus_type();
        if (btype == SLACK_TYPE)
        {
            double total_p_min_in_MW = 0.0;

            vector<SOURCE*> sources = get_sources_connecting_to_bus(bus);

            size_t n = sources.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (sources[i]->get_status() == false)
                {
                    continue;
                }
                total_p_min_in_MW += sources[i]->get_p_min_in_MW();
            }

            vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_connecting_to_bus(bus);

            n = vsc_hvdcs.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (vsc_hvdcs[i]->get_status() == false)
                {
                    continue;
                }
                size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(bus);
                if (vsc_hvdcs[i]->get_converter_status(index) == true and
                        vsc_hvdcs[i]->get_converter_active_power_operation_mode(index) == VSC_AC_VOLTAGE_ANGLE_CONTROL)
                {
                    total_p_min_in_MW += vsc_hvdcs[i]->get_converter_Pmin_in_MW(index);
                }
            }

            return total_p_min_in_MW;
        }
        else
        {
            return 0.0;
        }
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_regulatable_q_max_at_physical_bus_in_MVar(size_t bus)
{
    if (is_bus_exist(bus))
    {
        BUS* busptr = get_bus(bus);
        BUS_TYPE btype = busptr->get_bus_type();
        if (btype == PV_TYPE or
                btype == PV_TO_PQ_TYPE_1 or btype == PV_TO_PQ_TYPE_2 or
                btype == PV_TO_PQ_TYPE_3 or btype == PV_TO_PQ_TYPE_4 or
                btype == SLACK_TYPE)
        {
            double total_q_max_in_MVar = 0.0;

            vector<SOURCE*> sources = get_sources_connecting_to_bus(bus);

            size_t n = sources.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (sources[i]->get_status() == false)
                {
                    continue;
                }
                total_q_max_in_MVar += sources[i]->get_q_max_in_MVar();
            }

            vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_connecting_to_bus(bus);

            n = vsc_hvdcs.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (vsc_hvdcs[i]->get_status() == false)
                {
                    continue;
                }
                size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(bus);
                if (vsc_hvdcs[i]->get_converter_status(index) == true and
                        vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL)
                {
                    total_q_max_in_MVar += vsc_hvdcs[i]->get_converter_Qmax_in_MVar(index);
                }
            }

            return total_q_max_in_MVar;
        }
        else
        {
            return 0.0;
        }
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_regulatable_q_min_at_physical_bus_in_MVar(size_t bus)
{
    if (is_bus_exist(bus))
    {
        BUS* busptr = get_bus(bus);
        BUS_TYPE btype = busptr->get_bus_type();
        if (btype == PV_TYPE or
                btype == PV_TO_PQ_TYPE_1 or btype == PV_TO_PQ_TYPE_2 or
                btype == PV_TO_PQ_TYPE_3 or btype == PV_TO_PQ_TYPE_4 or
                btype == SLACK_TYPE )
        {
            double total_q_min_in_MVar = 0.0;

            vector<SOURCE*> sources = get_sources_connecting_to_bus(bus);

            size_t n = sources.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (sources[i]->get_status() == true)
                {
                    total_q_min_in_MVar += sources[i]->get_q_min_in_MVar();
                }
            }

            vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_connecting_to_bus(bus);

            n = vsc_hvdcs.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (vsc_hvdcs[i]->get_status() == false)
                {
                    continue;
                }
                size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(bus);
                if (vsc_hvdcs[i]->get_converter_status(index) == true and
                        vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL)
                {
                    total_q_min_in_MVar += vsc_hvdcs[i]->get_converter_Qmin_in_MVar(index);
                }
            }
            return total_q_min_in_MVar;
        }
        else
        {
            return 0.0;
        }
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_total_regulating_p_generation_at_physical_bus_in_MW(size_t bus)
{
    if (is_bus_exist(bus))
    {
        BUS* busptr = get_bus(bus);
        BUS_TYPE btype = busptr->get_bus_type();
        if (btype == SLACK_TYPE)
        {
            double total_p_in_MW = 0.0;

            vector<SOURCE*> sources = get_sources_connecting_to_bus(bus);

            size_t n = sources.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (sources[i]->get_status() == true)
                {
                    total_p_in_MW += sources[i]->get_p_generation_in_MW();
                }
            }

            vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_connecting_to_bus(bus);

            n = vsc_hvdcs.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (vsc_hvdcs[i]->get_status() == false)
                {
                    continue;
                }
                size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(bus);
                if (vsc_hvdcs[i]->get_converter_status(index) == true and
                        vsc_hvdcs[i]->get_converter_active_power_operation_mode(index) == VSC_AC_VOLTAGE_ANGLE_CONTROL)
                {
                    total_p_in_MW += vsc_hvdcs[i]->get_converter_P_to_AC_bus_in_MW(index);
                }
            }
            return total_p_in_MW;
        }
        else
        {
            return 0.0;
        }
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_total_regulating_q_generation_at_physical_bus_in_MVar(size_t bus)
{

    if (is_bus_exist(bus))
    {
        BUS* busptr = get_bus(bus);
        BUS_TYPE btype = busptr->get_bus_type();
        if (btype == PV_TYPE or
                btype == PV_TO_PQ_TYPE_1 or btype == PV_TO_PQ_TYPE_2 or
                btype == PV_TO_PQ_TYPE_3 or btype == PV_TO_PQ_TYPE_4 or
                btype == SLACK_TYPE)
        {
            double total_q_in_MVar = 0.0;

            vector<SOURCE*> sources = get_sources_connecting_to_bus(bus);

            size_t n = sources.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (sources[i]->get_status() == true)
                {
                    total_q_in_MVar += sources[i]->get_q_generation_in_MVar();
                }
            }
            vector<VSC_HVDC*> vsc_hvdcs = get_vsc_hvdcs_connecting_to_bus(bus);

            n = vsc_hvdcs.size();
            for (size_t i = 0; i != n; ++i)
            {
                if (vsc_hvdcs[i]->get_status() == false)
                {
                    continue;
                }
                size_t index = vsc_hvdcs[i]->get_converter_index_with_ac_bus(bus);
                if (vsc_hvdcs[i]->get_converter_status(index) == true and
                        vsc_hvdcs[i]->get_converter_reactive_power_operation_mode(index) == VSC_AC_VOLTAGE_CONTROL)
                {
                    total_q_in_MVar += vsc_hvdcs[i]->get_converter_Q_to_AC_bus_in_MVar(index);
                }
            }
            return total_q_in_MVar;
        }
        else
        {
            return 0.0;
        }
    }
    else
    {
        return 0.0;
    }
}

double PF_DATA::get_total_p_generation_at_physical_bus_in_MW(size_t bus)
{
    double total_p_in_MW = 0.0;

    vector<SOURCE*> sources = get_sources_connecting_to_bus(bus);

    size_t n = sources.size();
    for (size_t i = 0; i != n; ++i)
    {
        if (sources[i]->get_status() == true)
        {
            total_p_in_MW += sources[i]->get_p_generation_in_MW();
        }
    }
    return total_p_in_MW;
}

double PF_DATA::get_total_q_generation_at_physical_bus_in_MVar(size_t bus)
{
    double total_q_in_MVar = 0.0;

    vector<SOURCE*> sources = get_sources_connecting_to_bus(bus);

    size_t n = sources.size();
    for (size_t i = 0; i != n; ++i)
    {
        if (sources[i]->get_status() == true)
        {
            total_q_in_MVar += sources[i]->get_q_generation_in_MVar();
        }
    }
    return total_q_in_MVar;
}

complex<double> PF_DATA::get_total_load_power_in_MVA()
{
    size_t n = get_load_count();
    complex<double> S = 0.0;
    for (size_t i = 0; i != n; ++i)
    {
        if (Load[i].get_status() == true)
        {
            S += Load[i].get_actual_total_load_in_MVA();
        }
    }
    return S;
}

complex<double> PF_DATA::get_total_generation_power_in_MVA()
{
    size_t n = get_generator_count();
    complex<double> S = 0.0;
    for (size_t i = 0; i != n; ++i)
    {
        if (Generator[i].get_status() == true)
        {
            S += Generator[i].get_complex_generation_in_MVA();
        }
    }
    n = get_wt_generator_count();
    for (size_t i = 0; i != n; ++i)
    {
        if (WT_Generator[i].get_status() == true)
        {
            S += WT_Generator[i].get_complex_generation_in_MVA();
        }
    }
    return S;
}

complex<double> PF_DATA::get_total_loss_power_in_MVA()
{
    return get_total_generation_power_in_MVA() - get_total_load_power_in_MVA();
}

size_t PF_DATA::get_memory_usage()
{
    ostringstream osstream;

    osstream << "Size of some basic types:\n"
             << "ownership   : " << setw(9) << sizeof(OWNERSHIP) << "B\n"
             << "fault       : " << setw(9) << sizeof(FAULT) << "B\n"
             << "bus freq mdl: " << setw(9) << sizeof(BUS_FREQUENCY_MODEL) << "B\n"
             << "size_t: " << setw(9) << sizeof(size_t) << "B\n"
             << "pointer     : " << setw(9) << sizeof(BUS*) << "B\n"
             << "bool        : " << setw(9) << sizeof(bool) << "B\n"
             << "string      : " << setw(9) << sizeof(string) << "B";
    toolkit->show_information(osstream);

    osstream << "Power system database device used:\n"
             << "Bus         : " << setw(9) << Bus.capacity()*sizeof(BUS) << "B, " << setw(9) << Bus.size()*sizeof(BUS) << "B, " << setw(9) << sizeof(BUS) << "B\n"
             << "Generator   : " << setw(9) << Generator.capacity()*sizeof(GENERATOR) << "B, " << setw(9) << Generator.size()*sizeof(GENERATOR) << "B, " << setw(9) << sizeof(GENERATOR) << "B\n"
             << "WT Generator: " << setw(9) << WT_Generator.capacity()*sizeof(WT_GENERATOR) << "B, " << setw(9) << WT_Generator.size()*sizeof(WT_GENERATOR) << "B, " << setw(9) << sizeof(WT_GENERATOR) << "B\n"
             << "PV unit     : " << setw(9) << PV_Unit.capacity()*sizeof(PV_UNIT) << "B, " << setw(9) << PV_Unit.size()*sizeof(PV_UNIT) << "B, " << setw(9) << sizeof(PV_UNIT) << "B\n"
             << "Load        : " << setw(9) << Load.capacity()*sizeof(LOAD) << "B, " << setw(9) << Load.size()*sizeof(LOAD) << "B, " << setw(9) << sizeof(LOAD) << "B\n"
             << "Line        : " << setw(9) << Ac_line.capacity()*sizeof(AC_LINE) << "B, " << setw(9) << Ac_line.size()*sizeof(AC_LINE) << "B, " << setw(9) << sizeof(AC_LINE) << "B\n"
             << "Transformer : " << setw(9) << Transformer.capacity()*sizeof(TRANSFORMER) << "B, " << setw(9) << Transformer.size()*sizeof(TRANSFORMER) << "B, " << setw(9) << sizeof(TRANSFORMER) << "B\n"
             << "Fixed shunt : " << setw(9) << Fixed_shunt.capacity()*sizeof(FIXED_SHUNT) << "B, " << setw(9) << Fixed_shunt.size()*sizeof(FIXED_SHUNT) << "B, " << setw(9) << sizeof(FIXED_SHUNT) << "B\n"
             << "Equiv device: " << setw(9) << Equivalent_device.capacity()*sizeof(EQUIVALENT_DEVICE) << "B, " << setw(9) << Equivalent_device.size()*sizeof(EQUIVALENT_DEVICE) << "B, " << setw(9) << sizeof(EQUIVALENT_DEVICE) << "B\n"
             << "Energy strge: " << setw(9) << Energy_storage.capacity()*sizeof(ENERGY_STORAGE) << "B, " << setw(9) << Energy_storage.size()*sizeof(ENERGY_STORAGE) << "B, " << setw(9) << sizeof(ENERGY_STORAGE) << "B\n"
             << "Hvdc        : " << setw(9) << Lcc_hvdc2t.capacity()*sizeof(LCC_HVDC2T) << "B, " << setw(9) << Lcc_hvdc2t.size()*sizeof(LCC_HVDC2T) << "B, " << setw(9) << sizeof(LCC_HVDC2T) << "B\n"
             << "Area        : " << setw(9) << Area_.capacity()*sizeof(AREA) << "B, " << setw(9) << Area_.size()*sizeof(AREA) << "B, " << setw(9) << sizeof(AREA) << "B\n"
             << "Zone        : " << setw(9) << Zone_.capacity()*sizeof(ZONE) << "B, " << setw(9) << Zone_.size()*sizeof(ZONE) << "B, " << setw(9) << sizeof(ZONE) << "B\n"
             << "Owner       : " << setw(9) << Owner_.capacity()*sizeof(OWNER) << "B, " << setw(9) << Owner_.size()*sizeof(OWNER) << "B, " << setw(9) << sizeof(OWNER) << "B";
    toolkit->show_information(osstream);

    return Bus.capacity() * sizeof(BUS) +
           Generator.capacity() * sizeof(GENERATOR) +
           WT_Generator.capacity() * sizeof(WT_GENERATOR) +
           PV_Unit.capacity() * sizeof(PV_UNIT) +
           Load.capacity() * sizeof(LOAD) +
           Ac_line.capacity() * sizeof(AC_LINE) +
           Transformer.capacity() * sizeof(TRANSFORMER) +
           Fixed_shunt.capacity() * sizeof(FIXED_SHUNT) +
           Lcc_hvdc2t.capacity() * sizeof(LCC_HVDC2T) +
           Equivalent_device.capacity() * sizeof(EQUIVALENT_DEVICE) +
           Energy_storage.capacity() * sizeof(ENERGY_STORAGE) +
           Area_.capacity() * sizeof(AREA) +
           Zone_.capacity() * sizeof(ZONE) +
           Owner_.capacity() * sizeof(OWNER) +
           bus_index.get_max_bus_number() * sizeof(size_t) +
           generator_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           wt_generator_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           pv_unit_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           load_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           fixed_shunt_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           switched_shunt_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           ac_line_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           transformer_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           hvdc_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           equivalent_device_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           energy_storage_index.get_map_size() * (sizeof(DEVICE_ID) + sizeof(size_t)) * 2 +
           area_index.size() * sizeof(size_t) * 2 +
           zone_index.size() * sizeof(size_t) * 2 +
           owner_index.size() * sizeof(size_t) * 2;
}
