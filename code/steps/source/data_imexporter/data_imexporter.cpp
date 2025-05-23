#include "header/data_imexporter/data_imexporter.h"
#include "header/iSTEPS.h"
#include "header/basic/utility.h"
#include "header/model/sg_models/sync_generator_model/sync_generator_models.h"
#include "header/model/wtg_models/wt_generator_model/wt_generator_models.h"
#include "header/model/load_model/load_models.h"
#include <cstdio>
#include <iostream>

using namespace std;

DATA_IMEXPORTER::DATA_IMEXPORTER(iSTEPS& toolkit)
{
    ptoolkit = (&toolkit);

    set_base_frequency_in_Hz(50.0);
    set_export_zero_impedance_line_logic(true);
    set_export_zero_impedance_line_logic(true);
    set_export_internal_bus_number_logic(false);
    set_powerflow_data_save_mode(SAVE_TO_KEEP_ORIGINAL_BUS_ORDER);
    set_supplement_sequence_model_from_dynamic_model_logic(true);
}

DATA_IMEXPORTER::~DATA_IMEXPORTER()
{
    ptoolkit = NULL;
}

iSTEPS& DATA_IMEXPORTER::get_toolkit() const
{
    return *ptoolkit;
}
void DATA_IMEXPORTER::set_base_frequency_in_Hz(double fbase)
{
    base_frequency_in_Hz = fbase;
}

double DATA_IMEXPORTER::get_base_frequency_in_Hz() const
{
    return base_frequency_in_Hz;
}


void DATA_IMEXPORTER::set_export_zero_impedance_line_logic(bool logic)
{
    export_zero_impedance_line = logic;
}

void DATA_IMEXPORTER::set_powerflow_data_save_mode(POWERFLOW_DATA_SAVE_MODE mode)
{
    powerflow_data_save_mode = mode;
}

void DATA_IMEXPORTER::set_supplement_sequence_model_from_dynamic_model_logic(bool logic)
{
    supplement_sequence_model_from_dynamic_model_logic = logic;
}

bool DATA_IMEXPORTER::get_export_zero_impedance_line_logic() const
{
    return export_zero_impedance_line;
}

void DATA_IMEXPORTER::set_export_out_of_service_bus_logic(bool logic)
{
    export_out_of_service_bus = logic;
}

void DATA_IMEXPORTER::set_export_internal_bus_number_logic(bool logic)
{
    export_internal_bus_number = logic;
}

bool DATA_IMEXPORTER::get_export_out_of_service_bus_logic() const
{
    return export_out_of_service_bus;
}

bool DATA_IMEXPORTER::get_export_internal_bus_number_logic() const
{
    return export_internal_bus_number;
}

POWERFLOW_DATA_SAVE_MODE DATA_IMEXPORTER::get_powerflow_data_save_mode() const
{
    return powerflow_data_save_mode;
}

bool DATA_IMEXPORTER::get_supplement_sequence_model_from_dynamic_model_logic() const
{
    return supplement_sequence_model_from_dynamic_model_logic;
}

void DATA_IMEXPORTER::export_shadowed_bus_pair(string file) const
{
    ostringstream osstream;
    PF_DATA& psdb = ptoolkit->get_database();

    ofstream ofs(file);
    if(!ofs)
    {
        osstream<<"Warning. Shadowed bus pair file "<<file<<" cannot be opened for exporting shadowed bus data.";
        ptoolkit->show_information(osstream);
        return;
    }
    map<size_t, vector<size_t> > bus_pairs;

    bus_pairs.clear();

    vector<BUS*> buses = psdb.get_all_buses();
    size_t n = buses.size();
    for(size_t i=0; i!=n; ++i)
    {
        BUS* bus = buses[i];
        size_t bus_number = bus->get_bus_number();
        size_t equiv_bus_number = bus->get_equivalent_bus_number();
        if(equiv_bus_number!=0 and bus_number!=equiv_bus_number)
        {
            map<size_t, vector<size_t> >::iterator iter = bus_pairs.begin();
            iter = bus_pairs.find(equiv_bus_number);
            if(iter!=bus_pairs.end())
            {
                (iter->second).push_back(bus_number);
            }
            else
            {
                vector<size_t> new_pair;
                new_pair.push_back(bus_number);
                bus_pairs.insert(pair<size_t, vector<size_t> >(equiv_bus_number, new_pair));
            }
        }
    }
    for(map<size_t, vector<size_t> >::const_iterator iter = bus_pairs.begin(); iter!=bus_pairs.end(); ++iter)
    {
        size_t equiv_bus_number = iter->first;
        vector<size_t> bus_pair = iter->second;
        size_t n = bus_pair.size();
        if(n==0)
            continue;
        ostringstream oss;
        oss<<equiv_bus_number<<":";
        for(size_t i=0; i!=(n-1); ++i)
            oss<<bus_pair[i]<<",";
        oss<<bus_pair[n-1]<<"\n";
        ofs<<oss.str();
    }
    ofs.close();
}

void DATA_IMEXPORTER::setup_ordered_buses_with_powerflow_data_save_mode(POWERFLOW_DATA_SAVE_MODE mode)
{
    switch(mode)
    {
        case SAVE_TO_ORDER_BUS_WITH_BUS_NUMBER:
        {
            setup_ordered_buses_with_mode_SAVE_TO_ORDER_BUS_WITH_BUS_NUMBER();
            break;
        }
        case SAVE_TO_ORDER_BUS_WITH_BUS_NAME:
        {
            setup_ordered_buses_with_mode_SAVE_TO_ORDER_BUS_WITH_BUS_NAME();
            break;
        }
        case SAVE_TO_ORDER_BUS_WITH_DYNAMIC_DEVICE_ORDER:
        {
            setup_ordered_buses_with_mode_SAVE_TO_ORDER_BUS_WITH_DYNAMIC_DEVICE_ORDER();
            break;
        }
        case SAVE_TO_KEEP_ORIGINAL_BUS_ORDER:
        default:
        {
            setup_ordered_buses_with_mode_SAVE_TO_KEEP_ORIGINAL_BUS_ORDER();
            break;
        }
    }
}

const vector<size_t>& DATA_IMEXPORTER::get_ordered_buses() const
{
    return ordered_buses_to_export;
}

void DATA_IMEXPORTER::setup_ordered_buses_with_mode_SAVE_TO_KEEP_ORIGINAL_BUS_ORDER()
{
    PF_DATA& psdb = ptoolkit->get_database();

    ordered_buses_to_export = psdb.get_all_buses_number();
}

void DATA_IMEXPORTER::setup_ordered_buses_with_mode_SAVE_TO_ORDER_BUS_WITH_BUS_NUMBER()
{
    PF_DATA& psdb = ptoolkit->get_database();

    ordered_buses_to_export = psdb.get_all_buses_number();
    size_t n = ordered_buses_to_export.size();

    bool swap_found = false;
    for(size_t i=0; i<n-1; ++i)
    {
        swap_found = false;
        for(size_t j=1; j<n; ++j)
        {
            size_t previous_bus = ordered_buses_to_export[j-1];
            size_t next_bus = ordered_buses_to_export[j];
            if(next_bus<previous_bus)
            {
                ordered_buses_to_export[j] = previous_bus;
                ordered_buses_to_export[j-1] = next_bus;
                swap_found = true;
            }
        }
        if(swap_found==false)
            break;
    }
}

void DATA_IMEXPORTER::setup_ordered_buses_with_mode_SAVE_TO_ORDER_BUS_WITH_BUS_NAME()
{
    PF_DATA& psdb = ptoolkit->get_database();

    ordered_buses_to_export = psdb.get_all_buses_number();
    size_t n = ordered_buses_to_export.size();

    bool swap_found = false;
    for(size_t i=0; i<n-1; ++i)
    {
        swap_found = false;
        for(size_t j=1; j<n; ++j)
        {
            size_t previous_bus = ordered_buses_to_export[j-1];
            size_t next_bus = ordered_buses_to_export[j];
            if(psdb.bus_number2bus_name(next_bus) < psdb.bus_number2bus_name(previous_bus) )
            {
                ordered_buses_to_export[j] = previous_bus;
                ordered_buses_to_export[j-1] = next_bus;
                swap_found = true;
            }
        }
        if(swap_found==false)
            break;
    }
}

void DATA_IMEXPORTER::setup_ordered_buses_with_mode_SAVE_TO_ORDER_BUS_WITH_DYNAMIC_DEVICE_ORDER()
{
    PF_DATA& psdb = ptoolkit->get_database();

    size_t n = psdb.get_bus_count();
    ordered_buses_to_export.reserve(n);
    ordered_buses_to_export.clear();

    set<size_t> existing_buses;
    existing_buses.clear();

    vector<size_t> buses;

    buses = get_all_generator_buses();
    append_buses_to_ordered_buses(buses, existing_buses);

    buses = get_all_wt_generator_buses();
    append_buses_to_ordered_buses(buses, existing_buses);

    buses = get_all_pv_unit_buses();
    append_buses_to_ordered_buses(buses, existing_buses);

    buses = get_all_energy_storage_buses();
    append_buses_to_ordered_buses(buses, existing_buses);

    buses = get_all_load_buses();
    append_buses_to_ordered_buses(buses, existing_buses);

    buses = get_all_2t_lcc_hvdc_buses();
    append_buses_to_ordered_buses(buses, existing_buses);

    buses = get_all_fixed_shunt_buses();
    append_buses_to_ordered_buses(buses, existing_buses);

    buses = psdb.get_all_buses_number();
    append_buses_to_ordered_buses(buses, existing_buses);
}

vector<size_t> DATA_IMEXPORTER::get_all_generator_buses()
{
    PF_DATA& psdb = ptoolkit->get_database();

    vector<size_t> buses;
    vector<GENERATOR*> devices = psdb.get_all_generators();
    size_t n = devices.size();
    buses.reserve(n);
    buses.clear();

    for(size_t i=0; i<n; ++i)
    {
        GENERATOR* dvc = devices[i];
        buses.push_back(dvc->get_generator_bus());
    }
    return buses;
}

vector<size_t> DATA_IMEXPORTER::get_all_wt_generator_buses()
{
    PF_DATA& psdb = ptoolkit->get_database();

    vector<size_t> buses;
    vector<WT_GENERATOR*> devices = psdb.get_all_wt_generators();
    size_t n = devices.size();
    buses.reserve(n);
    buses.clear();

    for(size_t i=0; i<n; ++i)
    {
        WT_GENERATOR* dvc = devices[i];
        buses.push_back(dvc->get_generator_bus());
    }
    return buses;
}

vector<size_t> DATA_IMEXPORTER::get_all_pv_unit_buses()
{
    PF_DATA& psdb = ptoolkit->get_database();

    vector<size_t> buses;
    vector<PV_UNIT*> devices = psdb.get_all_pv_units();
    size_t n = devices.size();
    buses.reserve(n);
    buses.clear();

    for(size_t i=0; i<n; ++i)
    {
        PV_UNIT* dvc = devices[i];
        buses.push_back(dvc->get_unit_bus());
    }
    return buses;
}

vector<size_t> DATA_IMEXPORTER::get_all_energy_storage_buses()
{
    PF_DATA& psdb = ptoolkit->get_database();

    vector<size_t> buses;
    vector<ENERGY_STORAGE*> devices = psdb.get_all_energy_storages();
    size_t n = devices.size();
    buses.reserve(n);
    buses.clear();

    for(size_t i=0; i<n; ++i)
    {
        ENERGY_STORAGE* dvc = devices[i];
        buses.push_back(dvc->get_energy_storage_bus());
    }
    return buses;
}

vector<size_t> DATA_IMEXPORTER::get_all_load_buses()
{
    PF_DATA& psdb = ptoolkit->get_database();

    vector<size_t> buses;
    vector<LOAD*> devices = psdb.get_all_loads();
    size_t n = devices.size();
    buses.reserve(n);
    buses.clear();

    for(size_t i=0; i<n; ++i)
    {
        LOAD* dvc = devices[i];
        buses.push_back(dvc->get_load_bus());
    }
    return buses;
}

vector<size_t> DATA_IMEXPORTER::get_all_2t_lcc_hvdc_buses()
{
    PF_DATA& psdb = ptoolkit->get_database();

    vector<size_t> buses;
    vector<LCC_HVDC2T*> devices = psdb.get_all_2t_lcc_hvdcs();
    size_t n = devices.size();
    buses.reserve(n);
    buses.clear();

    for(size_t i=0; i<n; ++i)
    {
        LCC_HVDC2T* dvc = devices[i];
        buses.push_back(dvc->get_converter_bus(RECTIFIER));
        buses.push_back(dvc->get_converter_bus(INVERTER));
    }
    return buses;
}

vector<size_t> DATA_IMEXPORTER::get_all_fixed_shunt_buses()
{
    PF_DATA& psdb = ptoolkit->get_database();

    vector<size_t> buses;
    vector<FIXED_SHUNT*> devices = psdb.get_all_fixed_shunts();
    size_t n = devices.size();
    buses.reserve(n);
    buses.clear();

    for(size_t i=0; i<n; ++i)
    {
        FIXED_SHUNT* dvc = devices[i];
        buses.push_back(dvc->get_shunt_bus());
    }
    return buses;
}

void DATA_IMEXPORTER::append_buses_to_ordered_buses(const vector<size_t>& buses, set<size_t>& existing_buses)
{
    size_t n = buses.size();
    for(size_t i=0; i<n; ++i)
    {
        size_t bus = buses[i];

        set<size_t>::iterator iter = existing_buses.find(bus);
        if(iter == existing_buses.end())
        {
            ordered_buses_to_export.push_back(bus);
            existing_buses.insert(bus);
        }
        else
            continue;
    }
}


void DATA_IMEXPORTER::check_and_supplement_sequence_model_from_dynamic_model()
{
    check_ac_line_sequence_data();
    check_transformer_sequence_data();
    check_fixed_shunt_sequence_data();

    check_and_supplement_load_sequence_data();
    check_and_supplement_generator_sequence_data();
    check_and_supplement_wt_generator_sequence_data();
    check_and_supplement_pv_unit_sequence_data();
    check_and_supplement_energy_storage_sequence_data();
    check_and_supplement_vsc_hvdc_sequence_data();
}

void DATA_IMEXPORTER::check_ac_line_sequence_data()
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();

    vector<AC_LINE*> lines = psdb.get_all_ac_lines();
    vector<AC_LINE*> lines_with_no_seq_data;
    size_t n = lines.size();
    if(n==0)
        return;

    for(size_t i=0; i<n; i++)
        if(lines[i]->get_sequence_parameter_import_flag()==false)
            lines_with_no_seq_data.push_back(lines[i]);

    if(lines_with_no_seq_data.size()==0)
    {
        osstream<<"All lines' zero sequence model have been imported from seq file."<<endl;
        toolkit.show_information(osstream);
    }
    else
    {
        //osstream;
        for(size_t i=0; i<lines_with_no_seq_data.size(); i++)
        {
            osstream<<"Line"<<" ["<<lines_with_no_seq_data[i]->get_sending_side_bus()<<" "<<lines_with_no_seq_data[i]->get_receiving_side_bus()
                    <<" id:"<<lines_with_no_seq_data[i]->get_identifier()<<"]";
            osstream<<" is not imported from seq file."<<endl;
            toolkit.show_information(osstream);

            osstream<<"+ The device will be ignored in zero sequence network during short circuit calculation."<<endl;
            toolkit.show_information(osstream);
        }
    }
}
void DATA_IMEXPORTER::check_transformer_sequence_data()
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    vector<TRANSFORMER*> transformers = psdb.get_all_transformers();
    vector<TRANSFORMER*> transformers_with_no_seq_data;

    size_t n = transformers.size();
    if(n==0)
        return;
    for(size_t i=0; i<n; i++)
        if(transformers[i]->get_sequence_parameter_import_flag()==false)
            transformers_with_no_seq_data.push_back(transformers[i]);

    if(transformers_with_no_seq_data.size()==0)
    {
        osstream<<"All transformer sequence model have been imported from seq file."<<endl;
        toolkit.show_information(osstream);
    }
    else
    {
        for(size_t i=0; i<transformers_with_no_seq_data.size(); i++)
        {
            osstream<<"Transformer ["<<transformers_with_no_seq_data[i]->get_winding_bus(PRIMARY_SIDE)<<" "
                          <<transformers_with_no_seq_data[i]->get_winding_bus(SECONDARY_SIDE)<<" "
                          <<transformers_with_no_seq_data[i]->get_winding_bus(TERTIARY_SIDE)
                          <<" id:"<<transformers_with_no_seq_data[i]->get_identifier()<<"] is not imported from seq file."<<endl;
            toolkit.show_information(osstream);

            osstream<<"+ The device will be ignored in zero sequence network during short circuit calculation."<<endl;
            toolkit.show_information(osstream);
        }
    }
}
void DATA_IMEXPORTER::check_fixed_shunt_sequence_data()
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    vector<FIXED_SHUNT*> shunts = psdb.get_all_fixed_shunts();
    vector<FIXED_SHUNT*> shunts_with_no_seq_data;

    size_t n = shunts.size();
    if(n==0)
        return;
    for(size_t i=0; i<n; i++)
        if(shunts[i]->get_sequence_parameter_import_flag()==false)
            shunts_with_no_seq_data.push_back(shunts[i]);

    if(shunts_with_no_seq_data.size()==0)
    {
        osstream<<"All fixed shunt sequence model have been imported from seq file."<<endl;
        toolkit.show_information(osstream);
    }
    else
    {
        for(size_t i=0; i<shunts_with_no_seq_data.size(); i++)
        {
            osstream<<"Fixed shunt [bus:"<<shunts_with_no_seq_data[i]->get_shunt_bus()<<" id:"<<shunts_with_no_seq_data[i]->get_identifier()<<"]"
                    <<" is not imported from seq file."<<endl;
            toolkit.show_information(osstream);

            osstream<<"+ The device will be ignored in zero sequence network during short circuit calculation."<<endl;
            toolkit.show_information(osstream);
        }
    }
}

void DATA_IMEXPORTER::check_and_supplement_load_sequence_data()
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    vector<LOAD*> loads = psdb.get_all_loads();
    vector<LOAD*> loads_with_no_seq_data;

    size_t n = loads.size();
    if(n==0)
        return;
    for(size_t i=0; i<n; i++)
        if(loads[i]->get_sequence_parameter_import_flag()==false)
            loads_with_no_seq_data.push_back(loads[i]);

    if(loads_with_no_seq_data.size()==0)
    {
        osstream<<"All load sequence model have been imported from seq file."<<endl;
        toolkit.show_information(osstream);
    }
    else
    {
        for(size_t i=0; i<loads_with_no_seq_data.size(); i++)
        {
            osstream<<"Load [bus:"<<loads_with_no_seq_data[i]->get_load_bus()<<" id:"<<loads_with_no_seq_data[i]->get_identifier()<<"]"
                    <<" is not imported from seq file."<<endl;
            toolkit.show_information(osstream);

            supplement_load_sequence_model_from_dynamic_model(*loads_with_no_seq_data[i]);
        }
    }
}
void DATA_IMEXPORTER::check_and_supplement_generator_sequence_data()
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    vector<GENERATOR*> generators = psdb.get_all_generators();
    vector<GENERATOR*> generators_with_no_seq_data;

    size_t n = generators.size();
    if(n==0)
        return;
    for(size_t i=0; i<n; i++)
        if(generators[i]->get_sequence_parameter_import_flag()==false)
            generators_with_no_seq_data.push_back(generators[i]);

    if(generators_with_no_seq_data.size()==0)
    {
        osstream<<"All generator sequence model have been imported from seq file."<<endl;
        toolkit.show_information(osstream);
    }
    else
    {
        for(size_t i=0; i<generators_with_no_seq_data.size(); i++)
        {
            osstream<<"Generator [bus:"<<generators_with_no_seq_data[i]->get_generator_bus()<<" id:"<<generators_with_no_seq_data[i]->get_identifier()<<"]"
                    <<" is not imported from seq file."<<endl;
            toolkit.show_information(osstream);

            supplement_generator_sequence_model_from_dynamic_model(*generators_with_no_seq_data[i]);
        }
    }
}
void DATA_IMEXPORTER::check_and_supplement_wt_generator_sequence_data()
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    vector<WT_GENERATOR*> wt_generators = psdb.get_all_wt_generators();
    vector<WT_GENERATOR*> wt_generators_with_no_seq_data;

    size_t n = wt_generators.size();
    if(n==0)
        return;
    for(size_t i=0; i<n; i++)
        if(wt_generators[i]->get_sequence_parameter_import_flag()==false)
            wt_generators_with_no_seq_data.push_back(wt_generators[i]);

    if(wt_generators_with_no_seq_data.size()==0)
    {
        osstream<<"All wt generator sequence model have been imported from seq file."<<endl;
        toolkit.show_information(osstream);
    }
    else
    {
        for(size_t i=0; i<wt_generators_with_no_seq_data.size(); i++)
        {
            osstream<<"WT generator [bus:"<<wt_generators_with_no_seq_data[i]->get_source_bus()<<" id:"<<wt_generators_with_no_seq_data[i]->get_identifier()<<"]"
                    <<" is not imported from seq file."<<endl;
            toolkit.show_information(osstream);

            supplement_wt_generator_sequence_model_from_dynamic_model(*wt_generators_with_no_seq_data[i]);
        }
    }
}
void DATA_IMEXPORTER::check_and_supplement_pv_unit_sequence_data()
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    vector<PV_UNIT*> pv_units = psdb.get_all_pv_units();
    vector<PV_UNIT*> pv_units_with_no_seq_data;

    size_t n = pv_units.size();
    if(n==0)
        return;
    for(size_t i=0; i<n; i++)
        if(pv_units[i]->get_sequence_parameter_import_flag()==false)
            pv_units_with_no_seq_data.push_back(pv_units[i]);

    if(pv_units_with_no_seq_data.size()==0)
    {
        osstream<<"All pv unit sequence model have been imported from seq file."<<endl;
        toolkit.show_information(osstream);
    }
    else
    {
        for(size_t i=0; i<pv_units_with_no_seq_data.size(); i++)
        {
            osstream<<"Pv unit [bus:"<<pv_units_with_no_seq_data[i]->get_source_bus()<<" id:"<<pv_units_with_no_seq_data[i]->get_identifier()<<"]"
                    <<" is not imported from seq file."<<endl;
            toolkit.show_information(osstream);
        }
    }
}
void DATA_IMEXPORTER::check_and_supplement_energy_storage_sequence_data()
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    vector<ENERGY_STORAGE*> estorages = psdb.get_all_energy_storages();
    vector<ENERGY_STORAGE*> estorages_with_no_seq_data;

    size_t n = estorages.size();
    if(n==0)
        return;
    for(size_t i=0; i<n; i++)
        if(estorages[i]->get_sequence_parameter_import_flag()==false)
            estorages_with_no_seq_data.push_back(estorages[i]);

    if(estorages_with_no_seq_data.size()==0)
    {
        osstream<<"All energy storage sequence model have been imported from seq file."<<endl;
        toolkit.show_information(osstream);
    }
    else
    {
        osstream<<"Energy storage";
        for(size_t i=0; i<estorages_with_no_seq_data.size(); i++)
        {
            osstream<<"Energy storage [bus:"<<estorages_with_no_seq_data[i]->get_source_bus()<<" id:"<<estorages_with_no_seq_data[i]->get_identifier()<<"]"
                    <<" is not imported from seq file."<<endl;
            toolkit.show_information(osstream);
        }
    }
}
void DATA_IMEXPORTER::check_and_supplement_vsc_hvdc_sequence_data()
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    vector<VSC_HVDC*> vsc_hvdcs = psdb.get_all_vsc_hvdcs();
    vector<VSC_HVDC*> vsc_hvdcs_with_no_seq_data;

    size_t n = vsc_hvdcs.size();
    if(n==0)
        return;
    for(size_t i=0; i<n; i++)
        if(vsc_hvdcs[i]->get_sequence_parameter_import_flag()==false)
            vsc_hvdcs_with_no_seq_data.push_back(vsc_hvdcs[i]);

    if(vsc_hvdcs_with_no_seq_data.size()==0)
    {
        osstream<<"All vsc hvdc sequence model have been imported from seq file."<<endl;
        toolkit.show_information(osstream);
    }
    else
    {
        for(size_t i=0; i<vsc_hvdcs_with_no_seq_data.size(); i++)
        {
            osstream<<"Vsc hvdc [id:"<<vsc_hvdcs_with_no_seq_data[i]->get_identifier()<<"] is not imported from seq file."<<endl;
            toolkit.show_information(osstream);

            supplement_vsc_hvdc_sequence_model_from_dynamic_model(*vsc_hvdcs_with_no_seq_data[i]);
        }
    }
}
void DATA_IMEXPORTER::supplement_load_sequence_model_from_dynamic_model(LOAD& load)
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();

    osstream<<"+ Import sequence parameter from dynamic model for "<<load.get_compound_device_name()<<endl;
    toolkit.show_information(osstream);

    LOAD_MODEL* load_model = load.get_load_model();
    if(load_model!=NULL)
    {
        string model_name = load_model->get_model_name();
        if(model_name=="CIM6")
        {
            CIM6* cim6_model = (CIM6*) load_model;
            osstream<<"+ Import parameters from CIM6 model."<<endl;

            double Ra = cim6_model->get_Ra_in_pu();
            double Xa = cim6_model->get_Xa_in_pu();
            double Xm = cim6_model->get_Xm_in_pu();
            double R1 = cim6_model->get_R1_in_pu();
            double X1 = cim6_model->get_X1_in_pu();
            double R2 = cim6_model->get_R2_in_pu();
            double X2 = cim6_model->get_X2_in_pu();
            double mbase = cim6_model->get_Mbase_in_MVA();
            double Pmult = cim6_model->get_Pmult();

            double total_load_active_power = load.get_actual_total_load_in_MVA().real();
            if(mbase==0.0)
                mbase = Pmult * total_load_active_power;

            BUS* busptr = load.get_bus_pointer();
            double baseV = busptr->get_base_voltage_in_kV();

            load.set_ratio_of_motor_active_power(mbase/total_load_active_power);
            load.set_motor_rated_voltage_in_kV(baseV);
            load.set_mbase_code(MACHINE_BASE_POWER);
            load.set_mbase_in_MVA(mbase);
            load.set_motor_Ra_in_pu(Ra);
            load.set_motor_Xa_in_pu(Xa);
            load.set_motor_Xm_in_pu(Xm);
            load.set_motor_R1_in_pu(R1);
            load.set_motor_X1_in_pu(X1);
            load.set_motor_R2_in_pu(R2);
            load.set_motor_X2_in_pu(X2);

            load.set_sequence_parameter_import_flag(true);
        }
        else if(model_name=="IEEL")
        {
            osstream<<"+ Cannot import sequence parameters from "<<model_name<<" model. The device will be ignored during short circuit calculation."<<endl;
        }
        else
        {
            osstream<<"+ Cannot import sequence parameters from "<<model_name<<" model. The device will be ignored during short circuit calculation."<<endl;
        }
    }
    else
    {
        osstream<<"+ Dynamic model is not existed. The device will be ignored during short circuit calculation."<<endl;
    }
    toolkit.show_information(osstream);
}

void DATA_IMEXPORTER::supplement_generator_sequence_model_from_dynamic_model(GENERATOR& gen)
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();

    osstream<<"+ Import sequence parameter from dynamic model for "<<gen.get_compound_device_name()<<endl;
    toolkit.show_information(osstream);

    SYNC_GENERATOR_MODEL* sync_model = gen.get_sync_generator_model();
    if(sync_model!=NULL)
    {
        string model_name = sync_model->get_model_name();
        if(model_name=="GENCLS")
        {
            osstream<<"+ Cannot import sequence parameters from GENCLS model. The device will be ignored during short circuit calculation."<<endl;
        }
        else if(model_name=="GENROU")
        {
            GENROU* genrou_model = (GENROU*) sync_model;
            osstream<<"+ Import parameters from GENROU model."<<endl;

            double Xpp = genrou_model->get_Xdpp();
            gen.set_positive_sequence_subtransient_reactance_in_pu(Xpp);

            gen.set_sequence_parameter_import_flag(true);
        }
        else if(model_name=="GENSAL")
        {
            GENSAL* gensal_model = (GENSAL*) sync_model;
            osstream<<"+ Import parameters from GENSAL model."<<endl;

            double Xpp = gensal_model->get_Xdpp();
            gen.set_positive_sequence_subtransient_reactance_in_pu(Xpp);

            gen.set_sequence_parameter_import_flag(true);
        }
        else
        {
            osstream<<"+ Cannot import sequence parameters from "<<model_name<<" model. The device will be ignored during short circuit calculation."<<endl;
        }
    }
    else
    {
        osstream<<"+ Dynamic model is not existed. The device will be ignored during short circuit calculation."<<endl;
    }
    toolkit.show_information(osstream);
}
void DATA_IMEXPORTER::supplement_wt_generator_sequence_model_from_dynamic_model(WT_GENERATOR& wt_gen)
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();

    osstream<<"+ Import sequence parameter from dynamic model for "<<wt_gen.get_compound_device_name()<<endl;
    toolkit.show_information(osstream);

    WT_GENERATOR_MODEL* wtg_model = wt_gen.get_wt_generator_model();
    if(wtg_model!=NULL)
    {
        string model_name = wtg_model->get_model_name();
        if(model_name=="WT3G0")
        {
            osstream<<"+ Cannot import sequence parameters from "<<model_name<<" model. The device will be ignored during short circuit calculation."<<endl;
        }
        else if(model_name=="WT3G1")
        {
            osstream<<"+ Cannot import sequence parameters from "<<model_name<<" model. The device will be ignored during short circuit calculation."<<endl;
        }
        else if(model_name=="WT3G2")
        {
            osstream<<"+ Cannot import sequence parameters from "<<model_name<<" model. The device will be ignored during short circuit calculation."<<endl;
        }
        else
        {
            osstream<<"+ Cannot import sequence parameters from "<<model_name<<" model. The device will be ignored during short circuit calculation."<<endl;
        }
    }
    else
    {
        osstream<<"+ Dynamic model is not existed. The device will be ignored during short circuit calculation."<<endl;
    }
    toolkit.show_information(osstream);
}
void DATA_IMEXPORTER::supplement_vsc_hvdc_sequence_model_from_dynamic_model(VSC_HVDC& vsc_hvdc)
{
    ostringstream osstream;
    iSTEPS& toolkit = get_toolkit();

    osstream<<"+ Import sequence parameter from dynamic model for "<<vsc_hvdc.get_compound_device_name()<<endl;
    toolkit.show_information(osstream);

    vector<VSC_HVDC_CONVERTER_MODEL*> models = vsc_hvdc.get_vsc_hvdc_converter_models();
    if(models.size()==0)
    {
        osstream<<"+ Dynamic model is not existed, the device will be ignored during short circuit calculation."<<endl;
        toolkit.show_information(osstream);
    }
    else
    {
        size_t ncon = vsc_hvdc.get_converter_count();
        for(size_t i=0; i<ncon; i++)
        {
            if(vsc_hvdc.is_converter_a_dynamic_voltage_source(i))
                vsc_hvdc.set_converter_control_mode(i, VIRTUAL_SYNCHRONOUS_GENERATOR_CONTROL);
            else
                vsc_hvdc.set_converter_control_mode(i, CURRENT_VECTOR_CONTROL);
        }
        vsc_hvdc.set_sequence_parameter_import_flag(true);
    }
}
