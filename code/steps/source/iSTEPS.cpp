#include "header/iSTEPS.h"
#include <iostream>
#include <chrono>
#include <thread>
#include "header/basic/utility.h"
#include "header/steps_namespace.h"
#include "header/basic/steps_enum.h"
#include "header/data_imexporter/psse_imexporter.h"

using namespace std;

iSTEPS::iSTEPS(const string& toolkit_name, const string& log_file_name, const string& cfg_file_name) : pf_data(*this),  dyn_data(*this), pf_solver(*this),	dyn_simulator(*this), sc_solver(*this), network(*this),  importer(*this), exporter(*this)
{
    clear_all_data();

    toolkit_name_ = toolkit_name;

    set_fast_math_logic(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (log_file_name != "")
    {
        log_file_name_ = log_file_name;
        open_log_file(log_file_name_, false);
    }

    detailed_log_enabled = false;
    optimize_network_enabled = true;
    correct_three_winding_transformer_impedance = false;

    clock_started_ = clock();

    pf_data.set_default_capacity(cfg_file_name);

    set_thread_number(1);
    set_dynamic_model_database_size_in_bytes(STEPS_10M);

    show_information("iSTEPS toolkit [" + toolkit_name + "] @ 0X" + num2hex_str(size_t(this)) + " is created.\n");
}

iSTEPS::~iSTEPS()
{
    report_toolkit_memory_usage();

//    if (toolkit_name_ != "TK DFLT")
//    {
//        show_information("iSTEPS simulation toolkit [" + toolkit_name + "] @ 0X" + num2hex_str(size_t(this)) + " is deleted.");
//    }
    close_log_file();
}

void iSTEPS::set_name(const string& name)
{
        toolkit_name_ = name;
        show_information(std::format("iSTEPS toolkit's name is set to:{:s} ", name));
}

string iSTEPS::get_name() const
{
    return toolkit_name_;
}


//void iSTEPS::enable_fast_math()
//{
//	use_fast_math = true;
//}
//
//void iSTEPS::disable_fast_math()
//{
//	use_fast_math = false;
//}

void iSTEPS::set_fast_math_logic(const bool flag)
{
    use_fast_math = flag;
}

bool iSTEPS::get_fast_math_logic()
{
    return use_fast_math;
}

bool iSTEPS::get_correct_three_winding_transformer_impedance_logic()
{
    return correct_three_winding_transformer_impedance;
}

void iSTEPS::set_correct_three_winding_transformer_impedance_logic(bool flag)
{
    correct_three_winding_transformer_impedance = flag;
}

void iSTEPS::set_dynamic_blocks_automatic_large_stepsize(bool flag)
{
    BLOCK::set_automatic_large_stepsize(flag);
}

bool iSTEPS::get_dynamic_blocks_automatic_large_time_step_logic()
{
    return BLOCK::is_automatic_large_time_step_logic_enabled();
}

void iSTEPS::set_thread_number(size_t n)
{
    thread_number = n;
    generator_thread_number = 1;
    wt_generator_thread_number = 1;
    pv_unit_thread_number = 1;
    energy_storage_thread_number = 1;
    load_thread_number = 1;
    fixed_shunt_thread_number = 1;
    line_thread_number = 1;
    transformer_thread_number = 1;
    lcc_hvdc2t_thread_number = 1;
    vsc_hvdc_thread_number = 1;;
    equivalent_device_thread_number = 1;

    if (pf_data.get_bus_count() != 0)
    {
        update_device_thread_number();
    }
}

size_t iSTEPS::get_thread_number() const
{
    return thread_number;
}

void iSTEPS::update_device_thread_number()
{
    PF_DATA& psdb = get_database();
    if (thread_number != 1)
    {
        generator_thread_number = thread_number;
        wt_generator_thread_number = thread_number;
        pv_unit_thread_number = thread_number;
        energy_storage_thread_number = thread_number;
        load_thread_number = thread_number;
        fixed_shunt_thread_number = thread_number;
        line_thread_number = thread_number;
        transformer_thread_number = thread_number;
        lcc_hvdc2t_thread_number = thread_number;
        vsc_hvdc_thread_number = thread_number;;
        equivalent_device_thread_number = thread_number;

        vector<BUS*> buses = psdb.get_all_buses();
        size_t n = buses.size();
        for (size_t i = 0; i < n; ++i)
        {
            BUS* bus = buses[i];
            if (bus->get_bus_type() != OUT_OF_SERVICE)
            {
                size_t bus_number = bus->get_bus_number();

                if (generator_thread_number != 1)
                {
                    vector<GENERATOR*> devices = psdb.get_generators_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        generator_thread_number = 1;
                    }
                }

                if (wt_generator_thread_number != 1)
                {
                    vector<WT_GENERATOR*> devices = psdb.get_wt_generators_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        wt_generator_thread_number = 1;
                    }
                }

                if (pv_unit_thread_number != 1)
                {
                    vector<PV_UNIT*> devices = psdb.get_pv_units_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        pv_unit_thread_number = 1;
                    }
                }

                if (energy_storage_thread_number != 1)
                {
                    vector<ENERGY_STORAGE*> devices = psdb.get_energy_storages_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        energy_storage_thread_number = 1;
                    }
                }

                if (load_thread_number != 1)
                {
                    vector<LOAD*> devices = psdb.get_loads_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        load_thread_number = 1;
                    }
                }

                if (fixed_shunt_thread_number != 1)
                {
                    vector<FIXED_SHUNT*> devices = psdb.get_fixed_shunts_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        fixed_shunt_thread_number = 1;
                    }
                }

                if (line_thread_number != 1)
                {
                    vector<AC_LINE*> devices = psdb.get_ac_lines_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        line_thread_number = 1;
                    }
                }

                if (transformer_thread_number != 1)
                {
                    vector<TRANSFORMER*> devices = psdb.get_transformers_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        transformer_thread_number = 1;
                    }
                }

                if (lcc_hvdc2t_thread_number != 1)
                {
                    vector<LCC_HVDC2T*> devices = psdb.get_2t_lcc_hvdcs_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        lcc_hvdc2t_thread_number = 1;
                    }
                }

                if (vsc_hvdc_thread_number != 1)
                {
                    vector<VSC_HVDC*> devices = psdb.get_vsc_hvdcs_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        vsc_hvdc_thread_number = 1;
                    }
                }

                if (equivalent_device_thread_number != 1)
                {
                    vector<EQUIVALENT_DEVICE*> devices = psdb.get_equivalent_devices_connecting_to_bus(bus_number);
                    if (devices.size() > 1)
                    {
                        equivalent_device_thread_number = 1;
                    }
                }
            }
        }
    }
}

size_t iSTEPS::get_bus_thread_number() const
{
    return thread_number;
}

size_t iSTEPS::get_generator_thread_number() const
{
    return generator_thread_number;
}

size_t iSTEPS::get_wt_generator_thread_number() const
{
    return wt_generator_thread_number;
}

size_t iSTEPS::get_pv_unit_thread_number() const
{
    return pv_unit_thread_number;
}

size_t iSTEPS::get_energy_storage_thread_number() const
{
    return energy_storage_thread_number;
}

size_t iSTEPS::get_load_thread_number() const
{
    return load_thread_number;
}

size_t iSTEPS::get_fixed_shunt_thread_number() const
{
    return fixed_shunt_thread_number;
}

size_t iSTEPS::get_ac_line_thread_number() const
{
    return line_thread_number;
}

size_t iSTEPS::get_transformer_thread_number() const
{
    return transformer_thread_number;
}

size_t iSTEPS::get_lcc_hvdc2t_thread_number() const
{
    return lcc_hvdc2t_thread_number;
}

size_t iSTEPS::get_vsc_hvdc_thread_number() const
{
    return vsc_hvdc_thread_number;
}

size_t iSTEPS::get_equivalent_device_thread_number() const
{
    return equivalent_device_thread_number;
}

void iSTEPS::set_dynamic_model_database_size_in_bytes(size_t n)
{
    dynamic_model_db_size = n;
}

size_t iSTEPS::get_dynamic_model_database_size_in_bytes()
{
    return dynamic_model_db_size;
}

void iSTEPS::report_toolkit_memory_usage()
{
    if (is_detailed_log_enabled())
    {
        ostringstream osstream;
        osstream << "Gross report of toolkit memory usage:\n"
                 << "Power System Database : " << setw(9) << pf_data.get_memory_usage() << " B\n"
                 << "Dynamic Model Database: " << setw(9) << dyn_data.get_memory_usage() << " B\n"
                 << "Powerflow Solver      : " << setw(9) << pf_solver.get_memory_usage() << " B\n"
                 << "Dynamic Simulator     : " << setw(9) << dyn_simulator.get_memory_usage() << " B\n"
                 << "Network Matrix        : " << setw(9) << network.get_memory_usage() << " B";
        show_information(osstream);
    }
}

void iSTEPS::clear_all_data()
{
    current_alphabeta = 'Z';

    toolkit_str_int_map.clear_all_data();
    pf_data.clear_all_data();
    dyn_data.clear_all_data();
    pf_solver.clear_all_data();
    dyn_simulator.reset_data();
    sc_solver.reset_data();
    network.clear_all_data();
}

void iSTEPS::open_log_file(const string& file_name, bool append_mode)
{
    if (log_file_handle_.is_open())
    {
        close_log_file();
    }

    log_file_name_ = string2upper(file_name);

    if ( log_file_name_ != "" and log_file_name_ != "BLACKHOLE")
    {
        if (append_mode)
        {
            log_file_handle_.open(log_file_name_, ios_base::app);
        }
        else
        {
            log_file_handle_.open(log_file_name_);
        }
    }
}

void iSTEPS::close_log_file()
{
    if (log_file_handle_.is_open())
    {
        log_file_handle_.close();
    }
}

void iSTEPS::enable_detailed_log()
{
    ostringstream osstream;
    osstream << "Detailed log is enabled";
    show_information(osstream);
    detailed_log_enabled = true;
}

void iSTEPS::disable_detailed_log()
{
    ostringstream osstream;
    osstream << "Detailed log is disabled";
    show_information(osstream);
    detailed_log_enabled = false;
}

bool iSTEPS::is_detailed_log_enabled()
{
    return detailed_log_enabled;
}

void iSTEPS::enable_optimize_network()
{
    optimize_network_enabled = true;
}

void iSTEPS::disable_optimize_network()
{
    optimize_network_enabled = false;
}

bool iSTEPS::is_optimize_network_enabled()
{
    return optimize_network_enabled;
}

//void iSTEPS::reset()
//{
//    ostringstream osstream;
//    osstream<<"iSTEPS simulation toolkit is reset.";
//    show_information(osstream);
//
//    pf_data.clear();
//}

//void iSTEPS::terminate()
//{
////    reset();
//    ostringstream osstream;
//    pf_data.clear();
//    osstream<<"iSTEPS simulation toolkit is terminated.";
//    show_information(osstream);
//}

PF_DATA& iSTEPS::get_database()
{
    return pf_data;
}

DYN_DATA& iSTEPS::get_dynamic_model_database()
{
    return dyn_data;
}

PF_SOLVER& iSTEPS::get_powerflow_solver()
{
    return pf_solver;
}

SC_SOLVER& iSTEPS::get_short_circuit_solver()
{
    return sc_solver;
}

DYN_SIMULATOR& iSTEPS::get_dynamic_simulator()
{
    return dyn_simulator;
}

NET_MATRIX& iSTEPS::get_network_matrix()
{
    return network;
}

double iSTEPS::get_system_base_power_in_MVA() const
{
    return pf_data.get_system_base_power_in_MVA();
}

double iSTEPS::get_one_over_system_base_power_in_one_over_MVA() const
{
    return pf_data.get_one_over_system_base_power_in_one_over_MVA();
}

void iSTEPS::set_dynamic_simulation_time_step_in_s(double delt)
{
    dyn_simulator.set_dynamic_simulation_time_step_in_s(delt);
}

double iSTEPS::get_dynamic_simulation_time_step_in_s()
{
    return dyn_simulator.get_dynamic_simulation_time_step_in_s();
}

void iSTEPS::set_dynamic_simulation_time_in_s(double time)
{
    dyn_simulator.set_dynamic_simulation_time_in_s(time);
}

double iSTEPS::get_dynamic_simulation_time_in_s()
{
    return dyn_simulator.get_dynamic_simulation_time_in_s();
}

void iSTEPS::show_information(ostringstream& stream)
{
    if (log_file_name_ != "BLACKHOLE")
    {
        show_information(stream.str());
    }

    stream.str("");
}

void iSTEPS::show_information(const string& out_info)
{

    if (log_file_name_ != "BLACKHOLE")
    {
        vector<string> splitted_info = split_string(out_info, "\n");

        size_t info_size = splitted_info.size();
        if (info_size != 0)
        {
            string info = "";
            string sys_time = get_system_time_stamp_string();
            info = sys_time + " " + splitted_info[0] + "\n";
            for (size_t i = 1; i != info_size; ++i)
            {
                info += (sys_time + " " + splitted_info[i] + "\n");
            }

            if (log_file_handle_.is_open())
            {
                log_file_handle_ << info;
            }
            else
            {
                cout << info;
            }
        }
    }
}

string iSTEPS::get_system_time_stamp_string(bool simplified)
{
    time_t tt = time(NULL);
    tm* local_time = localtime(&tt);
    time_t clock_now = clock();

    double elapsed_time_in_s = (1.0 / double(CLOCKS_PER_SEC)) * double(clock_now - clock_started_);

    char time_stamp[40];
    if (simplified)
    {
        snprintf(time_stamp, 40, "[% 6.3f]", elapsed_time_in_s);
    }
    else
        snprintf(time_stamp, 40, "[%d-%02d-%02d %02d:%02d:%02d][% 6.3f]", local_time->tm_year + 1900, local_time->tm_mon + 1,
                 local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec, elapsed_time_in_s);

    return string(time_stamp);
}


void iSTEPS::show_set_get_model_data_with_index_error(const string& device, const string& model, const string& func, size_t index)
{
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    if (func == "set_model_data_with_index")
    {
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Index %zu is out of range when calling %s:%s() for %s.\n0.0 will be returned.",
                 index, model.c_str(), func.c_str(), device.c_str());
        show_information(buffer);
        return;
    }
    if (func == "get_model_data_with_index")
    {
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Index %zu is out of range when calling %s:%s() for %s.\nNothing will be set.",
                 index, model.c_str(), func.c_str(), device.c_str());
        show_information(buffer);
        return;
    }
}

void iSTEPS::show_set_get_model_data_with_name_error(const string& device, const string& model, const string& func, const string& par_name)
{
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    if (func == "set_model_data_with_name")
    {
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s is not supported when calling %s:%s() of %s.\n0.0 will be returned.",
                 par_name.c_str(), model.c_str(), func.c_str(), device.c_str());
        show_information(buffer);
        return;
    }
    if (func == "get_model_data_with_name")
    {
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s is not supported when calling %s:%s() of %s.\nNothing will be set.",
                 par_name.c_str(), model.c_str(), func.c_str(), device.c_str());
        show_information(buffer);
        return;
    }
}
char iSTEPS::get_next_alphabeta()
{
    current_alphabeta ++;
    if (current_alphabeta > 'Z')
    {
        current_alphabeta = 'A';
    }
    return current_alphabeta;
}
