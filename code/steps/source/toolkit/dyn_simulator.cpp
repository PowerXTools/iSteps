#include "header/toolkit/dyn_simulator.h"
#include "header/basic/constants.h"
#include "header/basic/utility.h"
#include "header/steps_namespace.h"
#include "header/meter/meter_setter.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <istream>
#include <iostream>
#include <ctime>
#include <chrono>

using namespace std;
using namespace chrono;

class iSTEPS;

#define ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR

#define USE_DYNAMIC_CURRENT_MISMATCH_CONTROL

DYN_SIMULATOR::DYN_SIMULATOR(iSTEPS& toolkit)
{
    this->toolkit = (&toolkit);

    DELT = 0.01;
    reset_data();
    detailed_log_enabled = false;
}

DYN_SIMULATOR::~DYN_SIMULATOR()
{
    stop();
    toolkit = NULL;
}

iSTEPS& DYN_SIMULATOR::get_toolkit() const
{
    return *toolkit;
}

void DYN_SIMULATOR::reset_data()
{
    clear_meters();

    set_output_file("");

    close_meter_output_files();

    set_bin_file_export_enable_flag(false);
    set_csv_file_export_enable_flag(true);
    set_json_file_export_enable_flag(false);

    set_dynamic_simulation_time_in_s(0.0);

    set_max_DAE_iteration(100);
    set_min_DAE_iteration(3);
    set_max_network_iteration(50);
    set_max_update_iteration(10);
    set_max_event_update_iteration(10);
    set_max_network_solution_divergent_threshold(3);
    set_allowed_max_power_imbalance_in_MVA(0.001);
    set_iteration_accelerator(1.0);
    set_non_divergent_solution_logic(false);
    set_automatic_iteration_accelerator_tune_logic(false);
    set_rotor_angle_stability_surveillance_flag(false);
    set_rotor_angle_stability_threshold_in_deg(360.0);
    generators_in_islands.clear();
    flag_rotor_angle_stable = true;
    set_time_step_threshold_when_leading_part_of_bus_frequency_model_is_enabled_in_s(0.1);
    set_k_for_leading_part_of_bus_frequency_model(3.0);
}

void DYN_SIMULATOR::set_dynamic_simulation_time_step_in_s(double delt)
{
    DELT = delt;
    ostringstream osstream;
    osstream<<"System dynamic simulation time step is set as :"<<DELT<<" s.";
    toolkit->show_information(osstream);
}

double DYN_SIMULATOR::get_dynamic_simulation_time_step_in_s() const
{
    return DELT;
}

void DYN_SIMULATOR::set_dynamic_simulation_time_in_s(double time)
{
    TIME = time;
}

double DYN_SIMULATOR::get_dynamic_simulation_time_in_s() const
{
    return TIME;
}

NET_MATRIX& DYN_SIMULATOR::get_network_matrix()
{
    return toolkit->get_network_matrix();
}

void DYN_SIMULATOR::set_csv_file_export_enable_flag(bool flag)
{
    csv_file_export_enabled = flag;
}

void DYN_SIMULATOR::set_json_file_export_enable_flag(bool flag)
{
    json_file_export_enabled = flag;
}

void DYN_SIMULATOR::set_bin_file_export_enable_flag(bool flag)
{
    bin_file_export_enabled = flag;
}

bool DYN_SIMULATOR::is_csv_file_export_enabled() const
{
    return csv_file_export_enabled;
}

bool DYN_SIMULATOR::is_json_file_export_enabled() const
{
    return json_file_export_enabled;
}

bool DYN_SIMULATOR::is_bin_file_export_enabled() const
{
    return bin_file_export_enabled;
}

void DYN_SIMULATOR::set_max_DAE_iteration(size_t iteration)
{
    if(iteration>0)
        this->max_DAE_iteration = iteration;
}

void DYN_SIMULATOR::set_min_DAE_iteration(size_t iteration)
{
    if(iteration>0)
    {
        if(iteration<2) iteration = 2;

        this->min_DAE_iteration = iteration;
    }
}

void DYN_SIMULATOR::set_max_network_iteration(size_t iteration)
{
    if(iteration>0)
        this->max_network_iteration = iteration;
}

void DYN_SIMULATOR::set_max_update_iteration(size_t iteration)
{
    if(iteration>0)
        this->max_update_iteration = iteration;
}

void DYN_SIMULATOR::set_max_event_update_iteration(size_t iteration)
{
    if(iteration>0)
        this->max_event_update_iteration = iteration;
}

void DYN_SIMULATOR::set_max_network_solution_divergent_threshold(size_t div_th)
{
    if(div_th>0)
        this->max_network_solution_divergent_threshold = div_th;
}

void DYN_SIMULATOR::set_allowed_max_power_imbalance_in_MVA(double tol)
{
    if(tol>0.0)
    {
        P_threshold_in_MW = tol;
        Q_threshold_in_MVar = tol;
    }
}

void DYN_SIMULATOR::set_iteration_accelerator(double iter_alpha)
{
    if(iter_alpha>0.0)
        this->alpha = iter_alpha;
}

void DYN_SIMULATOR::set_non_divergent_solution_logic(bool logic)
{
    non_divergent_solution_enabled = logic;
}

void DYN_SIMULATOR::set_automatic_iteration_accelerator_tune_logic(bool logic)
{
    automatic_iteration_accelerator_tune_enabled = logic;
    if(logic==true)
    {

        ostringstream osstream;
        osstream<<"*******************************\n"
                <<"******************************\n"
                <<"******************************\n"
                <<"ALERT!\n"
                <<"Automatic iteration accelerator tune is enabled for dynamic simulation. Your simulation may malfunction.\n"
                <<"You are supposed to modify function DYNAMIC_SIMULATOR::tune_iteration_accelerator_based_on_maximum_current_mismatch() for you specific simulation.\n"
                <<"******************************\n"
                <<"******************************\n"
                <<"******************************";
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::set_rotor_angle_stability_surveillance_flag(bool flag)
{
    this->flag_rotor_angle_stability_surveillance = flag;
}

void DYN_SIMULATOR::set_rotor_angle_stability_threshold_in_deg(double angle_th)
{
    if(angle_th<0.0) angle_th = - angle_th;
    this->rotor_angle_stability_threshold_in_deg = angle_th;
}

void DYN_SIMULATOR::set_time_step_threshold_when_leading_part_of_bus_frequency_model_is_enabled_in_s(double delt)
{
    time_step_threshold_when_leading_part_of_bus_frequency_model_is_enabled = delt;
}

void DYN_SIMULATOR::set_k_for_leading_part_of_bus_frequency_model(double k)
{
    k_for_leading_part_of_bus_frequency_model = k;
}

/*void DYN_SIMULATOR::set_current_simulation_time_in_s(double time)
{
    TIME = time;
}

double DYN_SIMULATOR::get_current_simulation_time_in_s() const
{
    return TIME;
}*/

size_t DYN_SIMULATOR::get_max_DAE_iteration() const
{
    return max_DAE_iteration;
}

size_t DYN_SIMULATOR::get_min_DAE_iteration() const
{
    return min_DAE_iteration;
}

size_t DYN_SIMULATOR::get_max_network_iteration() const
{
    return max_network_iteration;
}

size_t DYN_SIMULATOR::get_max_update_iteration() const
{
    return max_update_iteration;
}

size_t DYN_SIMULATOR::get_max_event_update_iteration() const
{
    return max_event_update_iteration;
}

size_t DYN_SIMULATOR::get_max_network_solution_divergent_threshold() const
{
    return max_network_solution_divergent_threshold;
}

double DYN_SIMULATOR::get_allowed_max_power_imbalance_in_MVA() const
{
    return P_threshold_in_MW;
}

double DYN_SIMULATOR::get_iteration_accelerator() const
{
    return alpha;
}

bool DYN_SIMULATOR::get_non_divergent_solution_logic() const
{
    return non_divergent_solution_enabled;
}

bool DYN_SIMULATOR::get_automatic_iteration_accelerator_tune_logic() const
{
    return automatic_iteration_accelerator_tune_enabled;
}

bool DYN_SIMULATOR::get_rotor_angle_stability_surveillance_flag() const
{
    return this->flag_rotor_angle_stability_surveillance;
}

double DYN_SIMULATOR::get_rotor_angle_stability_threshold_in_deg() const
{
    return this->rotor_angle_stability_threshold_in_deg;
}

double DYN_SIMULATOR::get_time_step_threshold_when_leading_part_of_bus_frequency_model_is_enabled_in_s() const
{
    return time_step_threshold_when_leading_part_of_bus_frequency_model_is_enabled;
}

double DYN_SIMULATOR::get_k_for_leading_part_of_bus_frequency_model() const
{
    return k_for_leading_part_of_bus_frequency_model;
}

void DYN_SIMULATOR::show_dynamic_simulator_configuration() const
{
    ostringstream osstream;
    osstream<<"Configuration of dynamic simulator:\n"
            <<"Fast sin/cos/tan function: "<<(use_fast_math?"Enabled":"Disabled")<<"\n"
            <<"Time step: "<<get_dynamic_simulation_time_step_in_s()<<" s\n"
            <<"Allowed maximum power imbalance: "<<get_allowed_max_power_imbalance_in_MVA()<<" MVA\n"
            <<"Maximum iteration for DAE solution: "<<get_max_DAE_iteration()<<"\n"
            <<"Minimum iteration for DAE solution: "<<get_min_DAE_iteration()<<"\n"
            <<"Maximum iteration for network: "<<get_max_network_iteration()<<"\n"
            <<"Maximum iteration for updating: "<<get_max_update_iteration()<<"\n"
            <<"Maximum iteration for event updating: "<<get_max_event_update_iteration()<<"\n"
            <<"Maximum network solution divergent threshold: "<<get_max_network_solution_divergent_threshold()<<"\n"
            <<"Network solution accelerator: "<<get_iteration_accelerator()<<"\n"
            <<"Rotor angle stability surveillance: "<<(get_rotor_angle_stability_surveillance_flag()?"Enabled":"Disabled")<<"\n"
            <<"Rotor angle stability threshold: "<<get_rotor_angle_stability_threshold_in_deg()<<" deg\n"
            <<"Time step threshold when leading part of bus frequency model is enabled: "<<get_time_step_threshold_when_leading_part_of_bus_frequency_model_is_enabled_in_s()<<" s\n"
            <<"K for leading part of bus frequency model: "<<get_k_for_leading_part_of_bus_frequency_model()<<"\n"
            <<"CSV export: "<<(is_csv_file_export_enabled()?"Enabled":"Disabled")<<"\n"
            <<"BIN export: "<<(is_bin_file_export_enabled()?"Enabled":"Disabled")<<"\n"
            <<"JSON export: "<<(is_json_file_export_enabled()?"Enabled":"Disabled")<<"\n"
            <<"Output file name: "<<get_output_file()<<"\n";

    size_t n = meters.size();
    osstream<<"Channel count: "<<n<<"\n";
    for(size_t i=0; i!=n; ++i)
        osstream<<"("<<setw(4)<<i+1<<") "<<meters[i].get_meter_name()<<"\n";

    toolkit->show_information(osstream);
}

size_t DYN_SIMULATOR::get_memory_usage()
{
    return in_service_buses.capacity()*sizeof(BUS*)+
           generators.capacity()*sizeof(GENERATOR*)+
           wt_generators.capacity()*sizeof(WT_GENERATOR*)+
           pv_units.capacity()*sizeof(PV_UNIT*)+
           e_storages.capacity()*sizeof(ENERGY_STORAGE*)+
           loads.capacity()*sizeof(LOAD*)+
           lines.capacity()*sizeof(AC_LINE*)+
           hvdcs.capacity()*sizeof(LCC_HVDC2T*)+
           vsc_hvdcs.capacity()*sizeof(VSC_HVDC*)+
           e_devices.capacity()*sizeof(EQUIVALENT_DEVICE*)+

           internal_bus_pointers.capacity()*sizeof(BUS*)+
           internal_bus_voltage_in_pu.capacity()*sizeof(double)+
           internal_bus_complex_voltage_in_pu.capacity()*sizeof(complex<double>)+
           I_mismatch.capacity()*sizeof(complex<double>)+
           S_mismatch.capacity()*sizeof(complex<double>)+
           I_vec.capacity()*sizeof(double)+
           delta_V.capacity()*sizeof(complex<double>)+

           jacobian.get_memory_usage()+

           meters.capacity()*sizeof(METER)+
           meter_values.capacity()*sizeof(double);
}

void DYN_SIMULATOR::set_network_matrix_update_as_unrequired()
{
    network_matrix_update_required = false;
}

void DYN_SIMULATOR::set_network_matrix_update_as_required()
{
    network_matrix_update_required = true;
}

bool DYN_SIMULATOR::is_network_matrix_update_required() const
{
    return network_matrix_update_required;
}

void DYN_SIMULATOR::append_meter(const METER& meter)
{
    if(meter.is_valid())
    {
        meters.push_back(meter);
        //os<< "Meter added. %s.",(meter.get_meter_name()).c_str());
        //show_information(osstream);
    }
    else
    {
        ostringstream osstream;
        osstream<<"Warning. Invalid meter is given. No meter will be set in DYNAMIC_SIMULATOR::"<<__FUNCTION__<<"().";
        toolkit->show_information(osstream);
    }
}

size_t DYN_SIMULATOR::get_meter_count() const
{
    return meters.size();
}

METER DYN_SIMULATOR::get_meter(size_t i)
{
    METER voidmeter(get_toolkit());

    if(meters.size()!=0)
    {
        if(i<meters.size())
            return meters[i];
        else
            return voidmeter;
    }
    else
        return voidmeter;
}

void DYN_SIMULATOR::update_all_meters_value()
{
    size_t n = meters.size();
    if(n!=0)
    {
        if(meter_values.size()!=n)
            meter_values.resize(n,0.0);

        #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
            set_openmp_number_of_threads(toolkit->get_thread_number());
            #pragma omp parallel for schedule(static)
            //#pragma omp parallel for num_threads(2)
        #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        for(size_t i=0; i<n; ++i)
            meter_values[i]=meters[i].get_meter_value();
        /*
        if(meter_values.size()==n)
        {
            for(size_t i=0; i!=n; ++i)
                meter_values[i]=meters[i].get_meter_value();
        }
        else
            meter_values.resize(n,0.0);
        */
    }
}

vector<double> DYN_SIMULATOR::get_all_meters_value()
{
    update_all_meters_value();
    return meter_values;
}

double DYN_SIMULATOR::get_user_meter_value(size_t i)
{
    if(i>0 and i<=meters.size())
        return meter_values[i-1];
    else
        return 0.0;
}

double DYN_SIMULATOR::get_basic_meter_value(string meter_name)
{
    meter_name = string2upper(meter_name);

    if(meter_name=="DAE ITERATION")
        return ITER_DAE;
    if(meter_name=="NETWORK ITERATION")
        return ITER_NET;
    if(meter_name=="POWER MISMATCH IN MVA")
        return max_power_mismatch_MVA;
    if(meter_name=="BUS WITH MAX POWER MISMATCH")
        return max_mismatch_bus;
    if(meter_name=="ELAPSED TIME OF INTEGRATION IN MS")
        return time_elapse_of_differential_equations_in_a_step;
    if(meter_name=="ELAPSED TIME OF NETWORK SOLUTION IN MS")
        return time_elapse_of_network_solution_in_a_step;
    if(meter_name=="TOTAL ELAPSED TIME IN MS")
        return time_elapse_in_a_step;
    return 0.0;
}

void DYN_SIMULATOR::clear_meters()
{
    meters.clear();
    meter_values.clear();
}

void DYN_SIMULATOR::set_output_file(string filename)
{
    output_filename = filename;
}

string DYN_SIMULATOR::get_output_file() const
{
    return output_filename;
}

void DYN_SIMULATOR::open_meter_output_files()
{
    ostringstream osstream;
    if(csv_output_file.is_open())
    {
        osstream<<"CSV meter output file stream was connected to some file. Stream will be closed before reopen for exporting dynamic simulation meter values.";
        toolkit->show_information(osstream);
    }
    if(json_output_file.is_open())
    {
        osstream<<"JSON meter output file stream was connected to some file. Stream will be closed before reopen for exporting dynamic simulation meter values.";
        toolkit->show_information(osstream);
    }
    if(bin_output_file.is_open())
    {
        osstream<<"BIN meter output file stream was connected to some file. Stream will be closed before reopen for exporting dynamic simulation meter values.";
        toolkit->show_information(osstream);
    }
    close_meter_output_files();

    //osstream<<"output file name is : "<<output_filename<<endl;
    //toolkit->show_information(osstream);
    if(output_filename!="")
    {
        if(is_bin_file_export_enabled())
        {
            string bin_filename = output_filename+".bin";
            bin_output_file.open(bin_filename, ios::binary);
            if(not bin_output_file.is_open())
            {
                osstream<<"BIN meter output file "<<bin_filename<<" cannot be opened for exporting dynamic simulation meter values.";
                toolkit->show_information(osstream);
            }
        }

        if(is_csv_file_export_enabled())
        {
            string csv_filename = output_filename+".csv";
            csv_output_file.open(csv_filename);
            if(not csv_output_file.is_open())
            {
                osstream<<"CSV meter output file "<<csv_filename<<" cannot be opened for exporting dynamic simulation meter values.";
                toolkit->show_information(osstream);
            }
        }

        if(is_json_file_export_enabled())
        {
            string json_filename = output_filename+".json";
            json_output_file.open(json_filename);
            if(not json_output_file.is_open())
            {
                osstream<<"JSON meter output file "<<json_filename<<" cannot be opened for exporting dynamic simulation meter values.";
                toolkit->show_information(osstream);
            }
        }
    }
    else
    {
        return;

        time_t tt = time(NULL);
        tm* local_time= localtime(&tt);

        char time_stamp[60];
        snprintf(time_stamp, 60, "dynamic_result_%d-%02d-%02d~%02d:%02d:%02d", local_time->tm_year + 1900, local_time->tm_mon + 1,
                local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
        output_filename = string(time_stamp);

        osstream<<"Meter output filename is not properly set. Meters will be automatically exported to the following file:"<<endl
          <<output_filename;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::close_meter_output_files()
{
    ostringstream osstream;
    if(is_csv_file_export_enabled() and csv_output_file.is_open())
    {
        //osstream<<"CSV meter output file stream will be closed.";
        //show_information(osstream);

        csv_output_file.close();

        //osstream<<"CSV meter output file stream is closed.";
        //show_information(osstream);
    }
    if(is_json_file_export_enabled() and json_output_file.is_open())
    {
        //osstream<<"JSON meter output file stream will be closed.";
        //show_information(osstream);

        json_output_file.close();

        //osstream<<"JSON meter output file stream is closed.";
        //show_information(osstream);
    }

    if(is_bin_file_export_enabled() and bin_output_file.is_open())
    {
        //osstream<<"BIN meter output file stream will be closed.";
        //show_information(osstream);

        bin_output_file.close();

        //osstream<<"BIN meter output file stream is closed.";
        //show_information(osstream);
    }
}

void DYN_SIMULATOR::save_meter_information()
{
    ostringstream osstream;

    size_t n = meters.size();
    if(n!=0)
    {
        open_meter_output_files();

        if(csv_output_file.is_open() or json_output_file.is_open() or bin_output_file.is_open())
        {
            // save header to csv file
            if(is_csv_file_export_enabled() and csv_output_file.is_open())
            {
                //csv_output_file<<"TIME,ITERATION,MISMATCH IN MVA";
                //for(size_t i=0; i!=n; ++i)
                //    csv_output_file<<","<<meters[i].get_meter_type();
                //csv_output_file<<endl;

                csv_output_file<<"TIME,DAE INTEGRATION,NETWORK ITERATION,MISMATCH IN MVA,MISMATCH BUS,D TIME ELAPSE IN MS,A TIME ELAPSE IN MS,TOTAL TIME ELAPSE IN MS";

                for(size_t i=0; i!=n; ++i)
                    csv_output_file<<","<<meters[i].get_meter_name();
                csv_output_file<<"\n";
            }

            // save header to json file
            if(is_json_file_export_enabled() and json_output_file.is_open())
            {
                json_output_file<<"{"<<"\n";

                //json_output_file<<"    \"meter_type\" : [\"TIME\", \"ITERATION\", \"MISMATCH IN MVA\"";
                //for(size_t i=0; i!=n; ++i)
                //    json_output_file<<", \""<<meters[i].get_meter_type()<<"\"";
                //json_output_file<<"],"<<endl<<endl;

                json_output_file<<"    \"meter_name\" : [\"TIME\", \"DAE INTEGRATION\", \"NETWORK ITERATION\", \"MISMATCH IN MVA\", \"MISMATCH BUS\", \"D TIME ELAPSE IN MS\", \"A TIME ELAPSE IN MS\", \"TOTAL TIME ELAPSE IN MS\"";
                for(size_t i=0; i!=n; ++i)
                    json_output_file<<", \""<<meters[i].get_meter_name()<<"\"";
                json_output_file<<"],"<<"\n\n";
                json_output_file<<"    \"meter_value\" : ["<<"\n";
            }

            // save header to bin file

            if(is_bin_file_export_enabled() and bin_output_file.is_open())
            {
                size_t bin_version=0;
                bin_output_file.write((char *)(&bin_version), sizeof(bin_version));

                time_t tt = time(NULL);
                tm* local_time= localtime(&tt);

                size_t year = local_time->tm_year + 1900;
                size_t month = local_time->tm_mon + 1;
                size_t day = local_time->tm_mday;
                size_t hour = local_time->tm_hour;
                size_t minute = local_time->tm_min;
                size_t second = local_time->tm_sec;

                bin_output_file.write((char *)(&year), sizeof(year));
                bin_output_file.write((char *)(&month), sizeof(month));
                bin_output_file.write((char *)(&day), sizeof(day));
                bin_output_file.write((char *)(&hour), sizeof(hour));
                bin_output_file.write((char *)(&minute), sizeof(minute));
                bin_output_file.write((char *)(&second), sizeof(second));

                size_t float_size=sizeof(float);
                bin_output_file.write((char *)(&float_size), sizeof(float_size));

                size_t m = 4+n;
                bin_output_file.write((char *)(&m), sizeof(m));
                string meter_names ="";

                meter_names += "TIME\n";
                meter_names += "DAE INTEGRATION\n";
                meter_names += "NETWORK INTEGRATION\n";
                meter_names += "MISMATCH IN MVA\n";
                meter_names += "MISMATCH BUS\n";
                meter_names += "D TIME ELAPSE IN MS\n";
                meter_names += "A TIME ELAPSE IN MS\n";
                meter_names += "TOTAL TIME ELAPSE IN MS\n";
                for(size_t i=0; i!=n; ++i)
                    meter_names += (meters[i].get_meter_name()+"\n");

                size_t n_meter_string_size = strlen(meter_names.c_str());
                bin_output_file.write((char *)(&n_meter_string_size), sizeof(n_meter_string_size));

                char * pmeter_name = new char[n_meter_string_size+1];
                sprintf(pmeter_name, meter_names.c_str());
                pmeter_name[n_meter_string_size]='\0';

                bin_output_file.write((char *)(pmeter_name), strlen(pmeter_name));
                delete [] pmeter_name;
            }
        }
    }
}

void DYN_SIMULATOR::save_meter_values()
{
    #ifdef USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
    max_power_mismatch_MVA = max_current_mismatch_pu*toolkit->get_database().get_bus_positive_sequence_voltage_in_pu(max_mismatch_bus)*toolkit->get_system_base_power_in_MVA();
    #endif // USE_DYNAMIC_CURRENT_MISMATCH_CONTROL

    update_all_meters_value();
    size_t n = meters.size();
    if(n!=0 and (csv_output_file.is_open() or json_output_file.is_open() or bin_output_file.is_open()))
    {
        //get_bus_current_mismatch();
        //calculate_bus_power_mismatch_in_MVA();
        //GREATEST_POWER_CURRENT_MISMATCH_STRUCT s_mismatch = get_max_power_mismatch_struct();
        //double smax = s_mismatch.greatest_power_mismatch_in_MVA;
        //size_t smax_bus = s_mismatch.bus_with_greatest_power_mismatch;

        if(is_bin_file_export_enabled() and bin_output_file.is_open())
        {
            float fvalue=0.0;
            char* fdata = (char *)(&fvalue);
            int ivalue=0;
            char* idata = (char *)(&ivalue);

            fvalue = TIME;
            bin_output_file.write(fdata, sizeof(float));

            ivalue = ITER_DAE;
            bin_output_file.write(fdata, sizeof(int));

            ivalue = ITER_NET;
            bin_output_file.write(fdata, sizeof(int));

            fvalue = max_power_mismatch_MVA;
            bin_output_file.write(fdata, sizeof(float));

            ivalue = max_mismatch_bus;
            bin_output_file.write(idata, sizeof(int));

            fvalue = time_elapse_of_differential_equations_in_a_step;
            bin_output_file.write(fdata, sizeof(float));

            fvalue = time_elapse_of_network_solution_in_a_step;
            bin_output_file.write(fdata, sizeof(float));

            fvalue = time_elapse_in_a_step;
            bin_output_file.write(fdata, sizeof(float));

            for (size_t i = 0; i != n; ++i)
            {
                fvalue = meter_values[i];
                bin_output_file.write(fdata, sizeof(float));
            }
        }

        if(is_csv_file_export_enabled() or is_json_file_export_enabled())
        {
            char temp_buffer[50];
            string temp_str = "";
            snprintf(temp_buffer, 50, "%8.4f",TIME);
            temp_str += temp_buffer;
            snprintf(temp_buffer, 50, ",%3zu",ITER_DAE);
            temp_str += temp_buffer;
            snprintf(temp_buffer, 50, ",%3zu",ITER_NET);
            temp_str += temp_buffer;
            snprintf(temp_buffer, 50, ",%6.3f",max_power_mismatch_MVA);
            temp_str += temp_buffer;
            snprintf(temp_buffer, 50, ",%6zu",max_mismatch_bus);
            temp_str += temp_buffer;
            snprintf(temp_buffer, 50, ",%6.3f",time_elapse_of_differential_equations_in_a_step);
            temp_str += temp_buffer;
            snprintf(temp_buffer, 50, ",%6.3f",time_elapse_of_network_solution_in_a_step);
            temp_str += temp_buffer;
            snprintf(temp_buffer, 50, ",%6.3f",time_elapse_in_a_step);
            temp_str += temp_buffer;
            for (size_t i = 0; i != n; ++i)
            {
                snprintf(temp_buffer, 50, ",%16.12f", meter_values[i]);
                temp_str += temp_buffer;
            }

            if(is_csv_file_export_enabled() and csv_output_file.is_open())
                csv_output_file<<temp_str<<"\n";

            if(is_json_file_export_enabled() and json_output_file.is_open())
                json_output_file<<","<<"\n"
                                <<"                       ["<<temp_str<<"]";
        }
    }
}

void DYN_SIMULATOR::optimize_network_ordering()
{
    NET_MATRIX& network_matrix = get_network_matrix();

    network_matrix.optimize_network_ordering();

    set_internal_bus_pointer();
}

void DYN_SIMULATOR::set_internal_bus_pointer()
{
    PF_DATA& psdb = toolkit->get_database();
    size_t nbus = psdb.get_in_service_bus_count();

    NET_MATRIX& network_matrix = get_network_matrix();
    internal_bus_pointers.clear();
    for(size_t internal_bus=0; internal_bus!=nbus; ++internal_bus)
    {
        size_t physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(internal_bus);
        internal_bus_pointers.push_back(psdb.get_bus(physical_bus));
    }

    internal_bus_complex_voltage_in_pu.resize(nbus, 0.0);
    initialize_internal_bus_voltage_vector();
}

void DYN_SIMULATOR::initialize_internal_bus_voltage_vector()
{
    size_t nbus = internal_bus_complex_voltage_in_pu.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_bus_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t internal_bus=0; internal_bus<nbus; ++internal_bus)
        internal_bus_complex_voltage_in_pu[internal_bus] = internal_bus_pointers[internal_bus]->get_positive_sequence_complex_voltage_in_pu();
}

void DYN_SIMULATOR::build_vsc_hvdc_dynamic_dc_network_matrix()
{
    size_t nvsc_hvdc = vsc_hvdcs.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_bus_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<nvsc_hvdc; ++i)
    {
        VSC_HVDC* vsc_hvdc = vsc_hvdcs[i];
        VSC_HVDC_NETWORK_MODEL* model = vsc_hvdc->get_vsc_hvdc_network_model();
        model->build_dynamic_dc_network_matrix();
    }
}

complex<double> DYN_SIMULATOR::get_bus_voltage_in_pu(size_t internal_bus) const
{
    return internal_bus_complex_voltage_in_pu[internal_bus];

    /*
    BUS* busptr = internal_bus_pointers[internal_bus];
    complex<double> v= busptr->get_positive_sequence_complex_voltage_in_pu();
    if(v!=internal_bus_complex_voltage_in_pu[internal_bus])
    {
        ostringstream osstream;
        osstream<<"voltage inconsistent of internal bus "<<internal_bus<<", v = "<<v<<", internal array = "<<internal_bus_complex_voltage_in_pu[internal_bus];
        toolkit->show_information(osstream);
    }
    return v;
    */
}

void DYN_SIMULATOR::start()
{
    microseconds_elapse_of_differential_equations_in_a_step = 0;
    microseconds_elapse_of_network_solution_in_a_step = 0;

    toolkit->update_device_thread_number();

    PF_SOLVER& pf_solver = toolkit->get_powerflow_solver();
    NET_MATRIX& network_matrix = get_network_matrix();

    detailed_log_enabled = toolkit->is_detailed_log_enabled();

    show_dynamic_simulator_configuration();

    auto clock_start = system_clock::now();

    ostringstream osstream;
    osstream<<"Dynamics initialization starts.";
    toolkit->show_information(osstream);

    if(not pf_solver.is_converged())
    {
        osstream<<"Warning. Powerflow is not converged. Please go check powerflow solution.\n"
                <<"Any further simulation cannot be trusted.";
        toolkit->show_information(osstream);
    }
    pf_solver.clear_all_data();


    meter_values.resize(meters.size(), 0.0);

    TIME = -2.0*DELT;
    flag_rotor_angle_stable = true;

    optimize_network_ordering();

    prepare_devices_for_run();

    run_all_models(DYNAMIC_INITIALIZE_MODE);
    run_bus_frequency_blocks(DYNAMIC_INITIALIZE_MODE);

    network_matrix.build_dynamic_network_Y_matrix();
    build_jacobian();

    //const SPARSE_MATRIX& Y = network_matrix.get_dynamic_network_Y_matrix();
    //Y.report_brief();
    //network_matrix.report_physical_internal_bus_number_pair();


    if(get_rotor_angle_stability_surveillance_flag()==true)
        update_generators_in_islands();

    ITER_DAE = 0;
    ITER_NET = 0;

    bool converged = false;
    size_t iter_count  = 0;
    current_max_network_iteration = get_max_network_iteration();
    while(true)
    {
		++iter_count;
		//osstream<<"Network iteration "<<iter_count;
		//toolkit->show_information(osstream);

        if(detailed_log_enabled)
        {
            ostringstream osstream;
            osstream<<"Initialization DAE iteration "<<iter_count<<":";
            toolkit->show_information(osstream);
        }
        converged = solve_network();
        ITER_NET += network_iteration_count;
        if(converged or iter_count>get_max_DAE_iteration())
            break;
    }
    ITER_DAE = iter_count;
    if(ITER_NET>1)
    {
        osstream<<"Warning. Network solution converged in "<<ITER_NET<<" iterations for dynamics simulation initialization.\n"
                <<"Any solution with more than 1 iteration is indicating bad initial powerflow result or exceeding dynamic upper or lower limits.";
        toolkit->show_information(osstream);
    }

    osstream<<"Dynamics initialization finished.";
    toolkit->show_information(osstream);


    auto tduration = duration_cast<microseconds>(system_clock::now()-clock_start);
    time_elapse_in_a_step = tduration.count()*0.001;
    time_elapse_of_differential_equations_in_a_step = 0.001*microseconds_elapse_of_differential_equations_in_a_step;
    time_elapse_of_network_solution_in_a_step = 0.001*microseconds_elapse_of_network_solution_in_a_step;

    save_meter_information();
    save_meter_values();
}

void DYN_SIMULATOR::prepare_devices_for_run()
{
    PF_DATA& psdb = toolkit->get_database();

    in_service_buses = psdb.get_all_in_service_buses();
    generators = psdb.get_all_generators();
    wt_generators = psdb.get_all_wt_generators();
    pv_units = psdb.get_all_pv_units();
    e_storages = psdb.get_all_energy_storages();

    loads = psdb.get_all_loads();

    lines = psdb.get_all_ac_lines();
    hvdcs = psdb.get_all_2t_lcc_hvdcs();

    vsc_hvdcs = psdb.get_all_vsc_hvdcs();

    e_devices = psdb.get_all_equivalent_devices();
}

void DYN_SIMULATOR::stop()
{
    //if(TIME >0.0)
    ostringstream osstream;
    osstream<<"Dynamics simulation stops at simulation time: "<<TIME<<"s.";
    toolkit->show_information(osstream);

    if(json_output_file.is_open())
        json_output_file<<endl
                        <<"                    ]"<<endl
                        <<"}";
    close_meter_output_files();
}


double DYN_SIMULATOR::get_system_max_angle_difference_in_deg()
{
    size_t n = generators.size();
    GENERATOR* gen;
    SYNC_GENERATOR_MODEL* model;
    double max_angle = -1e10, min_angle = 1e10;
    double angle;
    for(size_t i=0; i!=n; ++i)
    {
        gen = generators[i];
        if(gen->get_status()==true)
        {
            model = gen->get_sync_generator_model();
            if(model!=NULL)
            {
                angle = model->get_rotor_angle_in_deg();
                if(angle>max_angle)
                    max_angle = angle;
                if(angle<min_angle)
                    min_angle = angle;
            }
        }
    }
    return max_angle-min_angle;
}

void DYN_SIMULATOR::run_to(double time)
{
    NET_MATRIX& network_matrix = toolkit->get_network_matrix();

    network_matrix.build_dynamic_network_Y_matrix();
    build_jacobian();

    build_vsc_hvdc_dynamic_dc_network_matrix();

    update_with_event(FAULT_OR_OPERATION_EVENT);
    if(get_rotor_angle_stability_surveillance_flag()==false)
    {
        while(TIME<=time-DOUBLE_EPSILON)
            run_a_step();
    }
    else
    {
        while(TIME<=time-DOUBLE_EPSILON)
        {
            run_a_step();

            update_generators_in_islands();
            flag_rotor_angle_stable = is_system_angular_stable();
            if(flag_rotor_angle_stable==true)
                ;
            else
            {
                ostringstream osstream;
                osstream<<"At time "<<TIME<<"s, "
                        <<"system is detected to be unstable due to rotor angular stability surveillance logic."<<endl
                        <<"No further simulation will be performed.";
                toolkit->show_information(osstream);
                break;
            }
        }
    }
}

void DYN_SIMULATOR::run_a_step()
{
    auto clock_start = system_clock::now();
    microseconds_elapse_of_differential_equations_in_a_step = 0;
    microseconds_elapse_of_network_solution_in_a_step = 0;
    ostringstream osstream;

    TIME += DELT;
    if(detailed_log_enabled)
    {
        osstream<<"Run dynamic simulation at time "<<TIME<<"s.";
        toolkit->show_information(osstream);
    }
    update_equivalent_devices_buffer();
    update_equivalent_devices_output();

    if(is_network_matrix_update_required())
    {
        NET_MATRIX& network = get_network_matrix();
        network.build_dynamic_network_Y_matrix();
        set_network_matrix_update_as_unrequired();
    }
    //bool network_converged = false;
    //bool DAE_converged = false;
    ITER_DAE = 0;
    ITER_NET = 0;
    current_max_network_iteration = get_max_network_iteration();
    while(true)
    {
        ++ITER_DAE;
        if(detailed_log_enabled)
        {
            osstream<<"Solve DAE with iteration "<<ITER_DAE<<" at time "<<TIME<<"s.";
            toolkit->show_information(osstream);
        }

        if(ITER_DAE<=max_DAE_iteration)
        {
            integrate();
            bool network_converged = solve_network();
            run_bus_frequency_blocks(DYNAMIC_INTEGRATE_MODE);

            ITER_NET += network_iteration_count;

            if(detailed_log_enabled and not network_converged)
            {
                osstream<<"Failed to solve network in "<<max_network_iteration<<" iterations during DAE iteration "<<ITER_DAE<<" when integrating at time "<<TIME<<" s.";
                toolkit->show_information(osstream);
            }
            if(ITER_DAE>=get_min_DAE_iteration() and network_converged) // DAE solution converged
                break;
        }
        else
        {
            if(max_DAE_iteration!=2)
            {
                char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
                snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Failed to solve DAE in %zu iterations when integrating at time %f s.",
                         max_DAE_iteration, TIME);
                toolkit->show_information(buffer);
            }
            --ITER_DAE;
            break;
        }
    }
    update();
    run_bus_frequency_blocks(DYNAMIC_INTEGRATE_MODE);
    run_bus_frequency_blocks(DYNAMIC_UPDATE_MODE);
    check_relay_events();

    auto tduration = duration_cast<microseconds>(system_clock::now()-clock_start);
    time_elapse_in_a_step = tduration.count()*0.001;
    time_elapse_of_differential_equations_in_a_step = 0.001*microseconds_elapse_of_differential_equations_in_a_step;
    time_elapse_of_network_solution_in_a_step = 0.001*microseconds_elapse_of_network_solution_in_a_step;

    save_meter_values();
}

void DYN_SIMULATOR::update_with_event(DYNAMIC_EVENT_TYPE event_type)
{
    auto clock_start = system_clock::now();
    microseconds_elapse_of_differential_equations_in_a_step = 0;
    microseconds_elapse_of_network_solution_in_a_step = 0;

    ostringstream osstream;

    bool network_converged = false;
    size_t update_event_iter_max = get_max_event_update_iteration();

    ITER_DAE = 0;
    ITER_NET = 0;

    size_t iter = 0;
    current_max_network_iteration = get_max_network_iteration();
    while(true)
    {
        network_converged = solve_network();
        ++iter;
        ITER_NET += network_iteration_count;
        if(not network_converged)
        {
            if(iter<=update_event_iter_max)
                continue;
            else
            {
                char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
                snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Failed to solve network in %zu iterations when updating with event at time %f s.",
                         update_event_iter_max, TIME);
                toolkit->show_information(buffer);
                break;
            }
        }
        else
            break;
    }
    switch(event_type)
    {
        case FAULT_OR_OPERATION_EVENT:
            update();
            break;
        case TIME_STEP_CHANGE_EVENT:
        default:
            update_when_time_step_is_change();
            break;
    }
    update_bus_frequency_blocks_when_applying_event(event_type);

    auto tduration = duration_cast<microseconds>(system_clock::now()-clock_start);
    time_elapse_in_a_step = tduration.count()*0.001;
    time_elapse_of_differential_equations_in_a_step = 0.001*microseconds_elapse_of_differential_equations_in_a_step;
    time_elapse_of_network_solution_in_a_step = 0.001*microseconds_elapse_of_network_solution_in_a_step;

    save_meter_values();
}

void DYN_SIMULATOR::integrate()
{
    //clock_t start = clock();
    run_all_models(DYNAMIC_INTEGRATE_MODE);
    //cout<<"    elapsed time for integration: "<<double(clock()-start)/CLOCKS_PER_SEC*1000.0<<" ms"<<endl;
}

void DYN_SIMULATOR::update()
{
    update_with_specific_mode(DYNAMIC_UPDATE_MODE);
}

void DYN_SIMULATOR::update_when_time_step_is_change()
{
    update_with_specific_mode(DYNAMIC_UPDATE_TIME_STEP_MODE);
}

void DYN_SIMULATOR::update_with_specific_mode(DYNAMIC_MODE mode)
{
    if(mode==DYNAMIC_UPDATE_MODE or mode==DYNAMIC_UPDATE_TIME_STEP_MODE)
    {
        ostringstream osstream;

        update_equivalent_devices_buffer();
        run_all_models(mode);

        bool network_converged = false;

        current_max_network_iteration = get_max_network_iteration();

        size_t max_update_iter = get_max_update_iteration();
        size_t iter = 0;
        while(iter<max_update_iter)
        {
            network_converged = solve_network();
            ITER_NET += network_iteration_count;

            if(network_converged)
                break;
            else
                ++iter;
        }

        if(not network_converged and current_max_network_iteration>1)
        {
            char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
            if(mode==DYNAMIC_UPDATE_MODE)
                snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Failed to solve network in %zu iterations when updating at time %f s in DYNAMIC_UPDATE_MODE.",
                         current_max_network_iteration, TIME);
            else
                snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Failed to solve network in %zu iterations when updating at time %f s in DYNAMIC_UPDATE_TIME_STEP_MODE.",
                         current_max_network_iteration, TIME);
            toolkit->show_information(buffer);
        }
    }
}

void DYN_SIMULATOR::check_relay_events()
{
    disable_relay_action_flag();
    run_all_models(DYNAMIC_RELAY_MODE);

    if(is_relay_action_detected()==true)
        update();
}

void DYN_SIMULATOR::run_all_models(DYNAMIC_MODE mode)
{
    //ostringstream osstream;
    auto clock_start = steady_clock::now();

    //auto clock_1 = steady_clock::now();
    size_t n = 0;

    n = generators.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_generator_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
        generators[i]->run(mode);

    /*auto clock_2 = steady_clock::now();
    auto tduration = duration_cast<nanoseconds>(clock_2-clock_1);
    osstream<<"at time "<<TIME<<", generator models run takes "<<tduration.count()<<" ns.";
    toolkit->show_information(osstream);
    clock_1 = clock_2;*/

    n = wt_generators.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_wt_generator_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
        wt_generators[i]->run(mode);

    /*clock_2 = steady_clock::now();
    tduration = duration_cast<nanoseconds>(clock_2-clock_1);
    osstream<<"at time "<<TIME<<", wt generator models run takes "<<tduration.count()<<" ns.";
    toolkit->show_information(osstream);
    clock_1 = clock_2;*/

    n = pv_units.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_pv_unit_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
        pv_units[i]->run(mode);

    /*clock_2 = steady_clock::now();
    tduration = duration_cast<nanoseconds>(clock_2-clock_1);
    osstream<<"at time "<<TIME<<", pv models run takes "<<tduration.count()<<" ns.";
    toolkit->show_information(osstream);
    clock_1 = clock_2;*/

    n = loads.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_load_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
        loads[i]->run(mode);

    /*clock_2 = steady_clock::now();
    tduration = duration_cast<nanoseconds>(clock_2-clock_1);
    osstream<<"at time "<<TIME<<", load models run takes "<<tduration.count()<<" ns.";
    toolkit->show_information(osstream);
    clock_1 = clock_2;*/

    n = hvdcs.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_lcc_hvdc2t_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        LCC_HVDC2T* hvdc = hvdcs[i];
        if(hvdc->get_status()==true)
            hvdc->run(mode);
    }

    n = vsc_hvdcs.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_lcc_hvdc2t_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        VSC_HVDC* vsc_hvdc = vsc_hvdcs[i];
        if(vsc_hvdc->get_status()==true)
            vsc_hvdc->run(mode);
    }

    /*clock_2 = steady_clock::now();
    tduration = duration_cast<nanoseconds>(clock_2-clock_1);
    osstream<<"at time "<<TIME<<", hvdc models run takes "<<tduration.count()<<" ns.";
    toolkit->show_information(osstream);
    clock_1 = clock_2;*/

    n = e_devices.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_equivalent_device_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        EQUIVALENT_DEVICE* edevice = e_devices[i];
        if(edevice->get_status()==true)
            edevice->run(mode);
    }

    /*clock_2 = steady_clock::now();
    tduration = duration_cast<nanoseconds>(clock_2-clock_1);
    osstream<<"at time "<<TIME<<", equivalent device models run takes "<<tduration.count()<<" ns.";
    toolkit->show_information(osstream);
    clock_1 = clock_2;*/

    /*clock_2 = steady_clock::now();
    tduration = duration_cast<nanoseconds>(clock_2-clock_1);
    osstream<<"at time "<<TIME<<", bus frequency models run takes "<<tduration.count()<<" ns.";
    toolkit->show_information(osstream);
    clock_1 = clock_2;

    tduration = duration_cast<nanoseconds>(steady_clock::now()-clock_start);
    osstream<<"at time "<<TIME<<", all models run takes "<<tduration.count()<<" ns.";
    toolkit->show_information(osstream);*/

    auto tdurationx = duration_cast<microseconds>(steady_clock::now()-clock_start);
    microseconds_elapse_of_differential_equations_in_a_step += tdurationx.count();
}

void DYN_SIMULATOR::run_bus_frequency_blocks(DYNAMIC_MODE mode)
{
    auto clock_start = steady_clock::now();

    size_t n = in_service_buses.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_bus_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        (in_service_buses[i]->get_bus_frequency_model())->run(mode);
        /*BUS* bus = in_service_buses[i];
        BUS_FREQUENCY_MODEL* model = bus->get_bus_frequency_model();
        model->run(mode);*/
    }
    auto tdurationx = duration_cast<microseconds>(steady_clock::now()-clock_start);
    microseconds_elapse_of_differential_equations_in_a_step += tdurationx.count();
}

void DYN_SIMULATOR::update_bus_frequency_blocks_when_applying_event(DYNAMIC_EVENT_TYPE type)
{
    size_t n = in_service_buses.size();

    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_bus_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        (in_service_buses[i]->get_bus_frequency_model())->update_for_applying_event(type);
        /*BUS* bus = in_service_buses[i];
        BUS_FREQUENCY_MODEL* model = bus->get_bus_frequency_model();
        model->update_for_applying_event();*/
    }
}

void DYN_SIMULATOR::enable_relay_action_flag()
{
    relay_action_flag = true;
}

void DYN_SIMULATOR::disable_relay_action_flag()
{
    relay_action_flag = false;
}

bool DYN_SIMULATOR::is_relay_action_detected() const
{
    return relay_action_flag;
}

void DYN_SIMULATOR::update_equivalent_devices_buffer()
{
    size_t n = e_devices.size();

    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_equivalent_device_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        EQUIVALENT_DEVICE* edevice = e_devices[i];
        edevice->update_meter_buffer();
    }
}

void DYN_SIMULATOR::update_equivalent_devices_output()
{
    size_t n = e_devices.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_equivalent_device_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        ostringstream osstream;
        EQUIVALENT_DEVICE* edevice = e_devices[i];
        edevice->update_equivalent_outputs();

        osstream<<"At time "<<TIME<<" s, equivalent load of "<<edevice->get_compound_device_name()<<" is: "<<edevice->get_total_equivalent_power_as_load_in_MVA()<<"MVA";
        toolkit->show_information(osstream);
    }
}

bool DYN_SIMULATOR::get_system_angular_stable_flag() const
{
    return flag_rotor_angle_stable;
}

bool DYN_SIMULATOR::solve_network()
{
    auto clock_start = steady_clock::now();

    max_current_mismatch_pu = 0.0;
    max_power_mismatch_MVA = 0.0;
    max_mismatch_bus = 0;

    ostringstream osstream;
    bool converged = false;

    network_iteration_count = 0;

    size_t network_iter_max = current_max_network_iteration;
    size_t network_iter_count = 0;

    size_t network_solution_divergent_count = 0;

    double smax_th_MVA = get_allowed_max_power_imbalance_in_MVA();
    double imax_th_pu = smax_th_MVA*toolkit->get_one_over_system_base_power_in_one_over_MVA()*0.5;

    double original_alpha = get_iteration_accelerator();

    GREATEST_POWER_CURRENT_MISMATCH_STRUCT greatest_mismatch_struct;


    solve_2t_lcc_hvdcs_without_integration();

    update_vsc_hvdcs_converter_model();

    get_bus_current_mismatch();


    #ifndef USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
    calculate_bus_power_mismatch_in_MVA();
    greatest_mismatch_struct = get_max_power_mismatch_struct();
    max_power_mismatch_MVA = greatest_mismatch_struct.greatest_power_mismatch_in_MVA;
    max_mismatch_bus = greatest_mismatch_struct.bus_with_greatest_power_mismatch;
    if(max_power_mismatch_MVA<get_allowed_max_power_imbalance_in_MVA())
    #else
    greatest_mismatch_struct = get_max_current_mismatch_struct();
    max_current_mismatch_pu = greatest_mismatch_struct.greatest_current_mismatch_in_pu;
    max_mismatch_bus = greatest_mismatch_struct.bus_with_greatest_current_mismatch;
    if(max_current_mismatch_pu<imax_th_pu)
    #endif // USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
    {
        return true;
    }
    else
    {
        while(true)
        {
            if(network_iter_count<network_iter_max)
            {
                //if(get_automatic_iteration_accelerator_tune_logic()==true)
                //    tune_iteration_accelerator_based_on_maximum_current_mismatch(max_current_mismatch_pu);

                delta_V = I_mismatch/jacobian;
                if(jacobian.is_lu_factorization_failed())
                {
                    ostringstream osstream;
                    osstream<<"No further network solution will be attempted since LU factorization of dynamic Jacobian matrix is failed at time "<<TIME<<"s.";
                    toolkit->show_information(osstream);
                    break;
                }

                update_bus_voltage();
                if(get_non_divergent_solution_logic()==true)
                {
                    size_t n_delta_V = delta_V.size();
                    for(size_t i=0; i<n_delta_V; ++i)
                        delta_V[i] = -delta_V[i];

                    double original_alpha = get_iteration_accelerator();
                    for(size_t i=0; i<5; ++i)
                    {
                        //++network_iter_count;
                        solve_2t_lcc_hvdcs_without_integration();
                        update_vsc_hvdcs_converter_model();
                        get_bus_current_mismatch();
                        #ifndef USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
                        calculate_bus_power_mismatch_in_MVA();
                        greatest_mismatch_struct = get_max_power_mismatch_struct();
                        double new_smax = greatest_mismatch_struct.greatest_power_mismatch_in_MVA;
                        if(new_smax>=max_power_mismatch_MVA)
                        #else
                        greatest_mismatch_struct = get_max_current_mismatch_struct();
                        double new_imax = greatest_mismatch_struct.greatest_current_mismatch_in_pu;
                        if(new_imax>=max_current_mismatch_pu)
                        #endif // USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
                        {
                            alpha *= 0.5;
                            update_bus_voltage();
                        }
                        else
                            break;
                    }
                    set_iteration_accelerator(original_alpha);
                }
                ++network_iter_count;

                solve_2t_lcc_hvdcs_without_integration();
                update_vsc_hvdcs_converter_model();
                get_bus_current_mismatch();
                #ifndef USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
                calculate_bus_power_mismatch_in_MVA();
                greatest_mismatch_struct = get_max_power_mismatch_struct();
                double new_smax = greatest_mismatch_struct.greatest_power_mismatch_in_MVA;
                max_mismatch_bus = greatest_mismatch_struct.bus_with_greatest_power_mismatch;
                if(new_smax>max_power_mismatch_MVA)
                #else
                double new_imax = get_max_current_mismatch_struct().greatest_current_mismatch_in_pu;
                max_mismatch_bus = greatest_mismatch_struct.bus_with_greatest_current_mismatch;

                if(new_imax>max_current_mismatch_pu)
                #endif // USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
                {
                    ++network_solution_divergent_count;
                    if(network_solution_divergent_count>get_max_network_solution_divergent_threshold())
                    {
                        #ifndef USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
                        max_power_mismatch_MVA = new_smax;
                        #else
                        max_current_mismatch_pu = new_imax;
                        #endif // USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
                        converged = false;
                        break;
                    }
                }
                #ifndef USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
                max_power_mismatch_MVA = new_smax;
                if(max_power_mismatch_MVA>get_allowed_max_power_imbalance_in_MVA())
                #else
                max_current_mismatch_pu = new_imax;
                if(max_current_mismatch_pu>imax_th_pu)
                #endif // USE_DYNAMIC_CURRENT_MISMATCH_CONTROL
                    continue;
                else
                {
                    converged = true;
                    break;
                }
            }
            else
            {
                //--network_iter_count;
                break;
            }
        }
    }

    set_iteration_accelerator(original_alpha);

    network_iteration_count = network_iter_count;

    auto tduration = duration_cast<microseconds>(steady_clock::now()-clock_start);
    microseconds_elapse_of_network_solution_in_a_step += tduration.count();
    return converged;
}

void DYN_SIMULATOR::tune_iteration_accelerator_based_on_maximum_current_mismatch(double imax)
{
    double mbase = toolkit->get_system_base_power_in_MVA();
    double smax = imax*mbase;
    if(smax<0.1)
        set_iteration_accelerator(1.0);
    else
    {
        if(smax<1.0)
            set_iteration_accelerator(0.8);
        else
        {
            if(smax<5.0)
                set_iteration_accelerator(0.7);
            else
            {
                if(smax<10.0)
                    set_iteration_accelerator(0.65);
                else
                {
                    if(smax<20.0)
                        set_iteration_accelerator(0.625);
                    else
                        set_iteration_accelerator(0.6);
                    /*{
                        if(smax<50.0)
                            set_iteration_accelerator(0.75);
                        else
                        {
                            if(smax<75.0)
                                set_iteration_accelerator(0.7);
                            else
                            {
                                if(smax<100.0)
                                    set_iteration_accelerator(0.65);
                                else
                                    set_iteration_accelerator(0.6);
                            }
                        }
                    }*/
                }
            }
        }
    }
    //ostringstream osstream;
    //osstream<<"At time "<<TIME<<", dynamic simulation iteration accelerator is automatically tuned: ("<<smax<<"MVA, "<<get_iteration_accelerator()<<").";
    //toolkit->show_information(osstream);
}

void DYN_SIMULATOR::solve_2t_lcc_hvdcs_without_integration()
{
    size_t n = hvdcs.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_lcc_hvdc2t_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        LCC_HVDC2T_MODEL* model = hvdcs[i]->get_2t_lcc_hvdc_model();
        if(model!=NULL)
            model->solve_2t_lcc_hvdc_model_without_integration();
    }
}

void DYN_SIMULATOR::update_vsc_hvdcs_converter_model()
{
    size_t n = vsc_hvdcs.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        //set_openmp_number_of_threads(toolkit->get_lcc_hvdc2t_thread_number());
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        vector<VSC_HVDC_CONVERTER_MODEL*> models = vsc_hvdcs[i]->get_vsc_hvdc_converter_models();
        size_t m = models.size();
        for(size_t j=0; j!=m ; ++j)
        {
            if(models[j]!=NULL)
                models[j]->run(DYNAMIC_UPDATE_MODE);
        }
    }
}

void DYN_SIMULATOR::get_bus_current_mismatch()
{
    ostringstream osstream;
    NET_MATRIX& net = get_network_matrix();

    get_bus_currnet_into_network();

    size_t n = I_mismatch.size();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_bus_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i = 0; i<n; ++i)
        I_mismatch[i] = -I_mismatch[i];

    if(not detailed_log_enabled)
        ;
    else
    {
        for(size_t i = 0; i<n; ++i)
        {
            if(std::isnan(I_mismatch[i].real()) or std::isnan(I_mismatch[i].imag()))
            {
                osstream<<"warning. NAN is detected when getting bus current mismatch after get_bus_currnet_into_network() when calling DYN_SIMULATOR::"<<__FUNCTION__<<"():"<<endl;
                for(size_t j = 0; j<n; ++j)
                {
                    if(std::isnan(I_mismatch[j].real()) or std::isnan(I_mismatch[j].imag()))
                    {
                        size_t ibus = net.get_physical_bus_number_of_internal_bus(j);
                        osstream<<"Physical bus: "<<ibus<<", internal bus: "<<j<<", "<<I_mismatch[i].real()<<","<<I_mismatch[i].imag()<<endl;
                    }
                }
                toolkit->show_information(osstream);
                break;
            }
        }
    }

    add_generators_to_bus_current_mismatch();
    if(not detailed_log_enabled)
        ;
    else
    {
        for(size_t i = 0; i<n; ++i)
        {
            if(std::isnan(I_mismatch[i].real()) or std::isnan(I_mismatch[i].imag()))
            {
                osstream<<"warning. NAN is detected when getting bus current mismatch after adding generators when calling DYN_SIMULATOR::"<<__FUNCTION__<<"():"<<endl;
                for(size_t j = 0; j<n; ++j)
                {
                    if(std::isnan(I_mismatch[j].real()) or std::isnan(I_mismatch[j].imag()))
                    {
                        size_t ibus = net.get_physical_bus_number_of_internal_bus(j);
                        osstream<<"Physical bus: "<<ibus<<", internal bus: "<<j<<", "<<I_mismatch[i].real()<<","<<I_mismatch[i].imag()<<endl;
                    }
                }
                toolkit->show_information(osstream);
                break;
            }
        }
    }
    add_wt_generators_to_bus_current_mismatch();
    if(not detailed_log_enabled)
        ;
    else
    {
        for(size_t i = 0; i<n; ++i)
        {
            if(std::isnan(I_mismatch[i].real()) or std::isnan(I_mismatch[i].imag()))
            {
                osstream<<"warning. NAN is detected when getting bus current mismatch after adding wt generators when calling DYN_SIMULATOR::"<<__FUNCTION__<<"():"<<endl;
                for(size_t j = 0; j<n; ++j)
                {
                    if(std::isnan(I_mismatch[j].real()) or std::isnan(I_mismatch[j].imag()))
                    {
                        size_t ibus = net.get_physical_bus_number_of_internal_bus(j);
                        osstream<<"Physical bus: "<<ibus<<", internal bus: "<<j<<", "<<I_mismatch[i].real()<<","<<I_mismatch[i].imag()<<endl;
                    }
                }
                toolkit->show_information(osstream);
                break;
            }
        }
    }
    add_pv_units_to_bus_current_mismatch();
    if(not detailed_log_enabled)
        ;
    else
    {
        for(size_t i = 0; i<n; ++i)
        {
            if(std::isnan(I_mismatch[i].real()) or std::isnan(I_mismatch[i].imag()))
            {
                osstream<<"warning. NAN is detected when getting bus current mismatch after adding pv units when calling DYN_SIMULATOR::"<<__FUNCTION__<<"():"<<endl;
                for(size_t j = 0; j<n; ++j)
                {
                    if(std::isnan(I_mismatch[j].real()) or std::isnan(I_mismatch[j].imag()))
                    {
                        size_t ibus = net.get_physical_bus_number_of_internal_bus(j);
                        osstream<<"Physical bus: "<<ibus<<", internal bus: "<<j<<", "<<I_mismatch[i].real()<<","<<I_mismatch[i].imag()<<endl;
                    }
                }
                toolkit->show_information(osstream);
                break;
            }
        }
    }
    add_loads_to_bus_current_mismatch();
    if(not detailed_log_enabled)
        ;
    else
    {
        for(size_t i = 0; i<n; ++i)
        {
            if(std::isnan(I_mismatch[i].real()) or std::isnan(I_mismatch[i].imag()))
            {
                osstream<<"warning. NAN is detected when getting bus current mismatch after adding loads when calling DYN_SIMULATOR::"<<__FUNCTION__<<"():"<<endl;
                for(size_t j = 0; j<n; ++j)
                {
                    if(std::isnan(I_mismatch[j].real()) or std::isnan(I_mismatch[j].imag()))
                    {
                        size_t ibus = net.get_physical_bus_number_of_internal_bus(j);
                        osstream<<"Physical bus: "<<ibus<<", internal bus: "<<j<<", "<<I_mismatch[i].real()<<","<<I_mismatch[i].imag()<<endl;
                    }
                }
                toolkit->show_information(osstream);
                break;
            }
        }
    }
    add_2t_lcc_hvdcs_to_bus_current_mismatch();
    if(not detailed_log_enabled)
        ;
    else
    {
        for(size_t i = 0; i<n; ++i)
        {
            if(std::isnan(I_mismatch[i].real()) or std::isnan(I_mismatch[i].imag()))
            {
                osstream<<"warning. NAN is detected when getting bus current mismatch after adding hvdcs when calling DYN_SIMULATOR::"<<__FUNCTION__<<"():"<<endl;
                for(size_t j = 0; j<n; ++j)
                {
                    if(std::isnan(I_mismatch[j].real()) or std::isnan(I_mismatch[j].imag()))
                    {
                        size_t ibus = net.get_physical_bus_number_of_internal_bus(j);
                        osstream<<"Physical bus: "<<ibus<<", internal bus: "<<j<<", "<<I_mismatch[j]<<endl;
                    }
                }
                toolkit->show_information(osstream);
                break;
            }
        }
    }
    add_vsc_hvdcs_to_bus_current_mismatch();
    if(not detailed_log_enabled)
        ;
    else
    {
        for(size_t i = 0; i<n; ++i)
        {
            if(std::isnan(I_mismatch[i].real()) or std::isnan(I_mismatch[i].imag()))
            {
                osstream<<"warning. NAN is detected when getting bus current mismatch after adding vsc hvdcs when calling DYN_SIMULATOR::"<<__FUNCTION__<<"():"<<endl;
                for(size_t j = 0; j<n; ++j)
                {
                    if(std::isnan(I_mismatch[j].real()) or std::isnan(I_mismatch[j].imag()))
                    {
                        size_t ibus = net.get_physical_bus_number_of_internal_bus(j);
                        osstream<<"Physical bus: "<<ibus<<", internal bus: "<<j<<", "<<I_mismatch[j]<<endl;
                    }
                }
                toolkit->show_information(osstream);
                break;
            }
        }
    }
    add_equivalent_devices_to_bus_current_mismatch();
    if(not detailed_log_enabled)
        ;
    else
    {
        for(size_t i = 0; i<n; ++i)
        {
            if(std::isnan(I_mismatch[i].real()) or std::isnan(I_mismatch[i].imag()))
            {
                osstream<<"warning. NAN is detected when getting bus current mismatch after adding equivalent devices when calling DYN_SIMULATOR::"<<__FUNCTION__<<"():"<<endl;
                for(size_t j = 0; j<n; ++j)
                {
                    if(std::isnan(I_mismatch[j].real()) or std::isnan(I_mismatch[j].imag()))
                    {
                        size_t ibus = net.get_physical_bus_number_of_internal_bus(j);
                        osstream<<"Physical bus: "<<ibus<<", internal bus: "<<j<<", "<<I_mismatch[i].real()<<","<<I_mismatch[i].imag()<<endl;
                    }
                }
                toolkit->show_information(osstream);
                break;
            }
        }
    }
    /*
    osstream<<"bus current mismatch:"<<endl;
    toolkit->show_information(osstream);
    for(size_t i=0; i<n; ++i)
    {
        size_t ibus = net.get_physical_bus_number_of_internal_bus(i);
        osstream<<ibus<<", "<<I_mismatch[i]<<endl;
        toolkit->show_information(osstream);
    }
    */

    /*
    double maxmismatch = 0.0;
    complex<double> cmaxmismatch = 0.0;
    size_t busmax = 0;
    for(size_t i=0; i<n; ++i)
    {
        if(maxmismatch<abs(I_mismatch[i]))
        {
            maxmismatch = abs(I_mismatch[i]);
            cmaxmismatch = I_mismatch[i];
            busmax = net.get_physical_bus_number_of_internal_bus(i);
        }
    }
    osstream<<"max mismatch @ bus "<<busmax<<", "<<maxmismatch<<" or complex "<<cmaxmismatch<<", complex power = "<<psdb.get_bus_positive_sequence_complex_voltage_in_pu(busmax)*conj(cmaxmismatch)*psdb.get_system_base_power_in_MVA()<<setprecision(10)<<", v="<<psdb.get_bus_positive_sequence_voltage_in_pu(busmax)<<endl;
    toolkit->show_information(osstream);

    for(size_t i=0; i<n; ++i)
    {
        if(abs(I_mismatch[i])>0.0001)
        {
            osstream<<"mismatch @ bus "<<net.get_physical_bus_number_of_internal_bus(i)<<", "<<I_mismatch[i]<<">0.0001"<<endl;
            toolkit->show_information(osstream);
        }
        if(std::isnan(I_mismatch[i].real()) or std::isnan(I_mismatch[i].imag()))
        {
            osstream<<"NAN is detected in I_mismatch @ bus "<<net.get_physical_bus_number_of_internal_bus(i)<<", "<<I_mismatch[i]<<endl;
            toolkit->show_information(osstream);
        }
    }
    */
}

void DYN_SIMULATOR::get_bus_currnet_into_network()
{
    ostringstream osstream;
    PF_DATA& psdb = toolkit->get_database();
    NET_MATRIX& network_matrix = get_network_matrix();
    const STEPS_COMPLEX_SPARSE_MATRIX& Y = network_matrix.get_dynamic_network_Y_matrix();
    size_t nbus = psdb.get_in_service_bus_count();

	if(I_mismatch.size()!=nbus)
		I_mismatch.resize(nbus, 0.0);

    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_bus_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
	for (size_t i = 0; i<nbus; ++i)
		I_mismatch[i] = 0.0;

    int nsize = Y.get_matrix_size();

    // tho following codes can not be parallelized due to code I_mismatch[row] += Y.get_entry_value(k)*voltage;
    int k_start = Y.get_starting_index_of_column(0);
    for(int column=0; column<nsize; ++column)
    {
		//complex<double> voltage = psdb.get_bus_positive_sequence_complex_voltage_in_pu(column_physical_bus);
		complex<double> voltage = get_bus_voltage_in_pu(column);

        int k_end = Y.get_starting_index_of_column(column+1);

        // parallelization of the following codes is time-consuming since the k_end-k_start is usually very small
        for(int k=k_start; k<k_end; ++k)
        {
            int row = Y.get_row_number_of_entry_index(k);
            I_mismatch[row] += Y.get_entry_value(k)*voltage;
            /*
            if(not detailed_log_enabled)
                continue;
            else
            {
                complex<double> current = Y.get_entry_value(k)*voltage;
                if(std::isnan(current.real()) or std::isnan(current.imag()))
                {
                    size_t column_physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(column);
                    osstream<<"NAN is detected in "<<__FUNCTION__<<"() with Y["
                            <<row<<"("<<network_matrix.get_physical_bus_number_of_internal_bus(row)<<", "
                            <<column<<"("<<column_physical_bus<<"] = "<<Y.get_entry_value(k)<<", voltage["
                            <<column<<"("<<column_physical_bus<<"] = "<<voltage;
                    toolkit->show_information(osstream);
                }
            }
            */
        }
        k_start = k_end;
    }
/*
    ostringstream osstream;
    osstream<<"Current and power flowing into network (physical bus).");
    toolkit->show_information(osstream);
    osstream<<"bus     Ireal(pu)    Iimag(pu)    P(pu)   Q(pu)");
    toolkit->show_information(osstream);

    complex<double> s;
    for(size_t i=0; i!=nbus; ++i)Warning. Network solution converged in
    {
        column_physical_bus = network_matrix.get_physical_bus_number_of_internal_bus(i);
        voltage = psdb.get_bus_positive_sequence_complex_voltage_in_pu(column_physical_bus);
        s = voltage*conj(bus_current[i]);

        osstream<<"%-8u %-10f %-10f %-10f %-10f",column_physical_bus, bus_current[i].real(), bus_current[i].imag(), s.real(), s.imag());
        toolkit->show_information(osstream);
    }
*/
}

void DYN_SIMULATOR::add_generators_to_bus_current_mismatch()
{
    NET_MATRIX& network_matrix = get_network_matrix();

    size_t ngen = generators.size();

    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_generator_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<ngen; ++i)
    {
        GENERATOR* generator = generators[i];

        if(generator->get_status() == true)
        {
            size_t physical_bus = generator->get_generator_bus();

            size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

            SYNC_GENERATOR_MODEL* gen_model = generator->get_sync_generator_model();
            if(gen_model!=NULL)
            {
                //I = gen_model->get_terminal_complex_current_in_pu_in_xy_axis_based_on_sbase();
                //complex<double> I = gen_model->get_source_Norton_equivalent_complex_current_in_pu_in_xy_axis_based_on_sbase();
                //I_mismatch[internal_bus] += I;
                I_mismatch[internal_bus] += gen_model->get_source_Norton_equivalent_complex_current_in_pu_in_xy_axis_based_on_sbase();
                /*os<< "Generator %zu source current: %f + j%f",physical_bus, I.real(), I.imag());
                toolkit->show_information(osstream);
                complex<double> Edq = gen_model->get_internal_voltage_in_pu_in_dq_axis();
                complex<double> Exy = gen_model->get_internal_voltage_in_pu_in_xy_axis();
                osstream<<"Generator %zu internal voltage: %f + j%f (dq), %f + j%f (xy)",physical_bus, Edq.real(), Edq.imag(), Exy.real(), Exy.imag());
                toolkit->show_information(osstream);*/
            }
        }
    }
}

void DYN_SIMULATOR::add_wt_generators_to_bus_current_mismatch()
{
    NET_MATRIX& network_matrix = get_network_matrix();

    size_t ngen = wt_generators.size();

    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_wt_generator_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<ngen; ++i)
    {
        complex<double> I;
        WT_GENERATOR* generator = wt_generators[i];

        if(generator->get_status() == true)
        {
            size_t physical_bus = generator->get_generator_bus();

            size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

            WT_GENERATOR_MODEL* gen_model = generator->get_wt_generator_model();
            if(gen_model!=NULL)
            {
                if(gen_model->is_current_source())
                    I = gen_model->get_terminal_complex_current_in_pu_in_xy_axis_based_on_sbase();
                else
                    I = gen_model->get_source_Norton_equivalent_complex_current_in_pu_in_xy_axis_based_on_sbase();
                //cout<<generator->get_compound_device_name()<<" terminal or Norton current is: "<<I<<endl;

                I_mismatch[internal_bus] += I;
                /*os<< "Generator %zu source current: %f + j%f",physical_bus, I.real(), I.imag());
                toolkit->show_information(osstream);
                complex<double> Edq = gen_model->get_internal_voltage_in_pu_in_dq_axis();
                complex<double> Exy = gen_model->get_internal_voltage_in_pu_in_xy_axis();
                osstream<<"Generator %zu internal voltage: %f + j%f (dq), %f + j%f (xy)",physical_bus, Edq.real(), Edq.imag(), Exy.real(), Exy.imag());
                toolkit->show_information(osstream);*/
            }
        }
    }
}

void DYN_SIMULATOR::add_pv_units_to_bus_current_mismatch()
{
    NET_MATRIX& network_matrix = get_network_matrix();

    size_t npv = pv_units.size();

    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_pv_unit_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<npv; ++i)
    {
        complex<double> I;
        PV_UNIT* pv = pv_units[i];

        if(pv->get_status() == true)
        {
            size_t physical_bus = pv->get_unit_bus();

            size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

            PV_CONVERTER_MODEL* conv_model = pv->get_pv_converter_model();
            if(conv_model!=NULL)
            {
                if(conv_model->is_current_source())
                    I = conv_model->get_terminal_complex_current_in_pu_in_xy_axis_based_on_sbase();
                else
                    I = conv_model->get_source_Norton_equivalent_complex_current_in_pu_in_xy_axis_based_on_sbase();
                //cout<<generator->get_compound_device_name()<<" terminal or Norton current is: "<<I<<endl;

                I_mismatch[internal_bus] += I;

                /*os<< "Generator %zu source current: %f + j%f",physical_bus, I.real(), I.imag());
                toolkit->show_information(osstream);
                complex<double> Edq = conv_model->get_internal_voltage_in_pu_in_dq_axis();
                complex<double> Exy = conv_model->get_internal_voltage_in_pu_in_xy_axis();
                osstream<<"Generator %zu internal voltage: %f + j%f (dq), %f + j%f (xy)",physical_bus, Edq.real(), Edq.imag(), Exy.real(), Exy.imag());
                toolkit->show_information(osstream);*/
            }
        }
    }
}

void DYN_SIMULATOR::add_loads_to_bus_current_mismatch()
{
    NET_MATRIX& network_matrix = get_network_matrix();

    size_t nload = loads.size();

    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_load_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<nload; ++i)
    {
        LOAD* load = loads[i];
        if(load->get_status()==true)
        {
            size_t physical_bus = load->get_load_bus();

            size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

            LOAD_MODEL* model = load->get_load_model();
            if(model==NULL or (not model->is_voltage_source()))
            {
                if(model!=NULL) model->synchronize_bus_voltage_and_frequency();
                I_mismatch[internal_bus] -= load->get_dynamics_load_current_in_pu_based_on_system_base_power();
            }
            else
                I_mismatch[internal_bus] += load->get_dynamics_load_norton_current_in_pu_based_on_system_base_power();
        }
    }
}

void DYN_SIMULATOR::add_2t_lcc_hvdcs_to_bus_current_mismatch()
{
    ostringstream osstream;
    PF_DATA& psdb = toolkit->get_database();
    NET_MATRIX& network_matrix = get_network_matrix();

    size_t nhvdc = hvdcs.size();

    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_lcc_hvdc2t_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<nhvdc; ++i)
    {
        complex<double> I;
        LCC_HVDC2T* hvdc = hvdcs[i];
        if(hvdc->get_status()==true)
        {
            size_t physical_bus = hvdcs[i]->get_converter_bus(RECTIFIER);

            size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

            //I = hvdcs[i]->get_converter_dynamic_current_in_pu_based_on_system_base_power(RECTIFIER);

            //I_mismatch[internal_bus] -= I;
            I_mismatch[internal_bus] -= hvdcs[i]->get_converter_dynamic_current_in_pu_based_on_system_base_power(RECTIFIER);

            if(not detailed_log_enabled)
                ;
            else
            {
                I = hvdcs[i]->get_converter_dynamic_current_in_pu_based_on_system_base_power(RECTIFIER);
                I *= (psdb.get_system_base_power_in_MVA()/(SQRT3*psdb.get_bus_base_voltage_in_kV(physical_bus)));
                osstream<<"Current at rectifier side of "<<hvdc->get_compound_device_name()<<": "<<I<<"kA or "<<abs(I)<<"kA"<<endl
                        <<"Complex power = "<<SQRT3*psdb.get_bus_positive_sequence_complex_voltage_in_kV(physical_bus)*conj(I)<<" MVA";
                toolkit->show_information(osstream);
            }

            physical_bus = hvdcs[i]->get_converter_bus(INVERTER);

            internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

            //I = hvdcs[i]->get_converter_dynamic_current_in_pu_based_on_system_base_power(INVERTER);

            //I_mismatch[internal_bus] -= I;
            I_mismatch[internal_bus] -= hvdcs[i]->get_converter_dynamic_current_in_pu_based_on_system_base_power(INVERTER);

            if(not detailed_log_enabled)
                ;
            else
            {
                I = hvdcs[i]->get_converter_dynamic_current_in_pu_based_on_system_base_power(INVERTER);
                I *= (psdb.get_system_base_power_in_MVA()/(SQRT3*psdb.get_bus_base_voltage_in_kV(physical_bus)));
                osstream<<"Current at inverter side of "<<hvdc->get_compound_device_name()<<": "<<I<<"kA or "<<abs(I)<<"kA"<<endl
                        <<"Complex power = "<<SQRT3*psdb.get_bus_positive_sequence_complex_voltage_in_kV(physical_bus)*conj(I)<<" MVA";
                toolkit->show_information(osstream);
            }
        }
    }
}

void DYN_SIMULATOR::add_vsc_hvdcs_to_bus_current_mismatch()
{
    ostringstream osstream;
    PF_DATA& psdb = toolkit->get_database();
    NET_MATRIX& network_matrix = get_network_matrix();

    size_t nvsc = vsc_hvdcs.size();

    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_lcc_hvdc2t_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<nvsc; ++i)
    {
        complex<double> I;
        VSC_HVDC* vsc_hvdc = vsc_hvdcs[i];
        if(vsc_hvdc->get_status()==true)
        {
            size_t ncon = vsc_hvdc->get_converter_count();
            for(size_t j=0; j!=ncon; ++j)
            {
                size_t physical_bus = vsc_hvdc->get_converter_ac_bus(j);
                size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

                if(vsc_hvdc->is_converter_a_dynamic_voltage_source(j))
                    I_mismatch[internal_bus] += vsc_hvdc->get_converter_dynamic_Norton_current_to_ac_bus_in_pu_on_system_base_as_voltage_source(j);
                else
                    I_mismatch[internal_bus] += vsc_hvdc->get_converter_dynamic_current_to_ac_bus_in_pu_on_system_base_as_current_source(j);
                if(not detailed_log_enabled)
                    ;
                else
                {
                    if(vsc_hvdc->is_converter_a_dynamic_voltage_source(j))
                    {
                        I  = vsc_hvdc->get_converter_dynamic_Norton_current_to_ac_bus_in_pu_on_system_base_as_voltage_source(j);
                        I *= (psdb.get_system_base_power_in_MVA()/(SQRT3*psdb.get_bus_base_voltage_in_kV(physical_bus)));
                        osstream<<"Nortron current at vsc-hvdc ac side of "<<vsc_hvdc->get_compound_device_name()<<": "<<I<<"kA or "<<abs(I)<<"kA"<<endl
                                <<"Complex power = "<<SQRT3*psdb.get_bus_positive_sequence_complex_voltage_in_kV(physical_bus)*conj(I)<<" MVA";
                        toolkit->show_information(osstream);
                    }
                    else
                    {
                        I = vsc_hvdc->get_converter_dynamic_current_to_ac_bus_in_pu_on_system_base_as_current_source(j);
                        I *= (psdb.get_system_base_power_in_MVA()/(SQRT3*psdb.get_bus_base_voltage_in_kV(physical_bus)));
                        osstream<<"Current at vsc-hvdc ac side of "<<vsc_hvdc->get_compound_device_name()<<": "<<I<<"kA or "<<abs(I)<<"kA"<<endl
                                <<"Complex power = "<<SQRT3*psdb.get_bus_positive_sequence_complex_voltage_in_kV(physical_bus)*conj(I)<<" MVA";
                        toolkit->show_information(osstream);
                    }
                }
            }
        }
    }
}

void DYN_SIMULATOR::add_equivalent_devices_to_bus_current_mismatch()
{
    NET_MATRIX& network_matrix = get_network_matrix();

    double one_over_sbase = toolkit->get_one_over_system_base_power_in_one_over_MVA();

    size_t nedevice = e_devices.size();

    //The following codes should not be parallelized. If more than one device is connecting to the same bus, unintended sum of I_mismatch[internal_bus] will occur.
    for(size_t i=0; i<nedevice; ++i)
    {
        if(e_devices[i]->get_status() == true)
        {
            size_t physical_bus = e_devices[i]->get_equivalent_device_bus();

            size_t internal_bus = network_matrix.get_internal_bus_number_of_physical_bus(physical_bus);

            complex<double> S = e_devices[i]->get_total_equivalent_power_as_load_in_MVA()*one_over_sbase;

            complex<double> V = get_bus_voltage_in_pu(internal_bus);

            I_mismatch[internal_bus] -= conj(S/V);
        }
    }
}

bool DYN_SIMULATOR::is_converged()
{
    calculate_bus_power_mismatch_in_MVA();

    double smax = get_max_power_mismatch_struct().greatest_power_mismatch_in_MVA;

    double s_allowed = get_allowed_max_power_imbalance_in_MVA();

    return (smax>s_allowed)? false : true;
}

void DYN_SIMULATOR:: calculate_bus_power_mismatch_in_MVA()
{
    PF_DATA& psdb = toolkit->get_database();

    double sbase = psdb.get_system_base_power_in_MVA();

    S_mismatch = I_mismatch;

    size_t n = I_mismatch.size();
    complex<double> V;
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_bus_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i= 0; i<n; ++i)
    {
        //complex<double> V = ;
        S_mismatch[i] = get_bus_voltage_in_pu(i)*conj(S_mismatch[i])*sbase;
    }
}

GREATEST_POWER_CURRENT_MISMATCH_STRUCT DYN_SIMULATOR::get_max_power_mismatch_struct()
{
    NET_MATRIX& network_matrix = get_network_matrix();

    double smax = 0.0;
    size_t smax_bus = 0;
    size_t n = S_mismatch.size();
    for(size_t i=0; i!=n; ++i)
    {
        double s = steps_fast_complex_abs(S_mismatch[i]);
        if(s>smax)
        {
            smax = s;
            smax_bus = network_matrix.get_physical_bus_number_of_internal_bus(i);
        }
    }
    GREATEST_POWER_CURRENT_MISMATCH_STRUCT s_mismatch;
    s_mismatch.greatest_power_mismatch_in_MVA = smax;
    s_mismatch.bus_with_greatest_power_mismatch = smax_bus;

    return s_mismatch;
}

GREATEST_POWER_CURRENT_MISMATCH_STRUCT DYN_SIMULATOR::get_max_current_mismatch_struct()
{
    NET_MATRIX& network_matrix = get_network_matrix();

    double Imax = 0.0;
    size_t Imax_bus = 0, Imax_internal_bus = 0;
    size_t n = I_mismatch.size();
    for(size_t i=0; i<n; ++i)
    {
        double I = steps_fast_complex_abs(I_mismatch[i]);
        /*
        STEPS_SHOW_FILE_FUNCTION_AND_LINE_INFO
        if(I>5e-7)
        {
            cout<<"dynamic current mismatch exceeding threshold @ bus "<<network_matrix.get_physical_bus_number_of_internal_bus(i)
                <<": "<<I<<endl;
        }*/
        if(I>Imax)
        {
            Imax = I;
            Imax_internal_bus = i;
        }
    }
    Imax_bus = network_matrix.get_physical_bus_number_of_internal_bus(Imax_internal_bus);
    GREATEST_POWER_CURRENT_MISMATCH_STRUCT i_mismatch;
    i_mismatch.greatest_current_mismatch_in_pu = Imax;
    i_mismatch.bus_with_greatest_current_mismatch = Imax_bus;

    if(toolkit->is_detailed_log_enabled())
    {
        PF_DATA& psdb = toolkit->get_database();
        ostringstream osstream;
        osstream<<"Maximum current mismatch at bus "<<Imax_bus<<"("<<psdb.bus_number2bus_name(Imax_bus)<<"): "<<Imax;
        toolkit->show_information(osstream);
    }

    return i_mismatch;
}


void DYN_SIMULATOR::build_bus_current_mismatch_vector()
{
    size_t n = I_mismatch.size();
    I_vec.resize(n*2, 0.0);

    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_bus_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        I_vec[i] = I_mismatch[i].imag();
        I_vec[i+n] = I_mismatch[i].real();
    }
}


void DYN_SIMULATOR::update_bus_voltage()
{
    size_t n = delta_V.size();

    NET_MATRIX& network = toolkit->get_network_matrix();
    #ifdef ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
        set_openmp_number_of_threads(toolkit->get_thread_number());
        #pragma omp parallel for schedule(static)
        //#pragma omp parallel for num_threads(2)
    #endif // ENABLE_OPENMP_FOR_DYNAMIC_SIMULATOR
    for(size_t i=0; i<n; ++i)
    {
        BUS* bus = in_service_buses[i];

        size_t ex_bus = bus->get_bus_number();
        size_t in_bus = network.get_internal_bus_number_of_physical_bus(ex_bus);

        complex<double> V0 = bus->get_positive_sequence_complex_voltage_in_pu();
        complex<double> V = V0+delta_V[in_bus]*alpha;
        internal_bus_complex_voltage_in_pu[in_bus] = V;

		double vmag_new = steps_fast_complex_abs(V);
        //vmag_new = (vmag_new<3.0)?vmag_new:3.0;
        bus->set_positive_sequence_voltage_in_pu(vmag_new);

		double x = V.real(), y = V.imag();
		double x0 = V0.real(), y0 = V0.imag();
		complex<double> z(x*x0 + y*y0, x0*y - y0*x);
        double delta_vang = steps_fast_complex_arg(z);
        double vang0 = bus->get_positive_sequence_angle_in_rad();
        double vang_new = vang0+delta_vang;

        bus->set_positive_sequence_angle_in_rad(vang_new, V/vmag_new);
    }
    /*
    for(size_t i=0; i<n; ++i)
    {
        BUS* bus = internal_bus_pointers[i];
        double vang0 = bus->get_positive_sequence_angle_in_rad();
        complex<double> V0 = bus->get_positive_sequence_complex_voltage_in_pu();

        //complex<double> delta_v = complex<double>(delta_V[i], delta_V[i+n]); previous version for real matrix
        //complex<double> V = V0-delta_v*alpha;  previous version for real matrix

        complex<double> delta_v = delta_V[i];
        complex<double> V = V0+delta_v*alpha;

		double vmag_new = steps_fast_complex_abs(V);

		double x = V.real(), y = V.imag();
		double x0 = V0.real(), y0 = V0.imag();
		complex<double> z(x*x0 + y * y0, x0*y - y0 * x);
        double delta_vang = steps_fast_complex_arg(z);
        double vang_new = vang0+delta_vang;

        if(vmag_new<0.0)
            vmag_new = 0.0;

        if(vmag_new>3.0)
            vmag_new = 3.0;


        bus->set_positive_sequence_voltage_in_pu(vmag_new);
        bus->set_positive_sequence_angle_in_rad(vang_new);

        internal_bus_complex_voltage_in_pu[i] = bus->get_positive_sequence_complex_voltage_in_pu();
    }
    */
}


void DYN_SIMULATOR::build_jacobian()
{
    NET_MATRIX& network_matrix = get_network_matrix();

    jacobian.clear();
    STEPS_COMPLEX_SPARSE_MATRIX& Y = network_matrix.get_dynamic_network_Y_matrix();

    jacobian = Y;
    jacobian.compress_and_merge_duplicate_entries();
    return;

    size_t nbus = Y.get_matrix_size();
    complex<double> y;
    double g, b;
    size_t row, col;
    size_t starting_index=0, ending_index;
    for(size_t i=0; i!=nbus; ++i)
    {
        ending_index = Y.get_starting_index_of_column(i+1);
        col = i;
        for(size_t k=starting_index; k!=ending_index; ++k)
        {
            y = Y.get_entry_value(k);
            g = y.real();
            b = y.imag();
            row = Y.get_row_number_of_entry_index(k);
            jacobian.add_entry(row,col,-b);
            jacobian.add_entry(row,col+nbus,-g);
            jacobian.add_entry(row+nbus,col,-g);
            jacobian.add_entry(row+nbus,col+nbus,b);
        }
        starting_index = ending_index;
    }
    jacobian.compress_and_merge_duplicate_entries();
}


void DYN_SIMULATOR::change_dynamic_simulator_time_step_in_s(double delt)
{
    ostringstream osstream;
    osstream<<"System dynamic simulation time step is changed from "<<DELT<<" s to "<<delt<<" s.";
    toolkit->show_information(osstream);

    set_dynamic_simulation_time_step_in_s(delt);

    update_with_event(TIME_STEP_CHANGE_EVENT);
}


void DYN_SIMULATOR::guess_bus_voltage_with_bus_fault_set(size_t bus, const FAULT& fault)
{
    PF_DATA& psdb = toolkit->get_database();
    BUS* busptr = psdb.get_bus(bus);
    if(busptr->get_bus_type()==OUT_OF_SERVICE)
        return;

    double fault_b = fault.get_fault_shunt_in_pu().imag();
    double current_voltage = busptr->get_positive_sequence_voltage_in_pu();
    double vbase = busptr->get_base_voltage_in_kV();
    double one_over_sbase = toolkit->get_one_over_system_base_power_in_one_over_MVA();
    double zbase = vbase*vbase*one_over_sbase;
    double fault_x = -zbase/fault_b;

    if(fault_x<1)
        busptr->set_positive_sequence_voltage_in_pu(0.05);
    else
    {
        if(fault_x<5)
            busptr->set_positive_sequence_voltage_in_pu(0.1);
        else
        {
            if(fault_x<10)
                busptr->set_positive_sequence_voltage_in_pu(0.2);
            else
            {
                if(fault_x<100)
                    busptr->set_positive_sequence_voltage_in_pu(0.5);
                else
                    busptr->set_positive_sequence_voltage_in_pu(current_voltage-0.1);
            }
        }
    }

    NET_MATRIX& network = get_network_matrix();
    size_t internal_bus = network.get_internal_bus_number_of_physical_bus(bus);
    internal_bus_complex_voltage_in_pu[internal_bus] = busptr->get_positive_sequence_complex_voltage_in_pu();
}

void DYN_SIMULATOR::guess_bus_voltage_with_bus_fault_cleared(size_t bus, const FAULT& fault)
{
    PF_DATA& psdb = toolkit->get_database();
    BUS* busptr = psdb.get_bus(bus);
    double fault_b = fault.get_fault_shunt_in_pu().imag();
    double current_voltage = busptr->get_positive_sequence_voltage_in_pu();
    busptr->set_positive_sequence_voltage_in_pu(0.8);;
    return;

    if(fault_b<-2e8)
        busptr->set_positive_sequence_voltage_in_pu(current_voltage+0.95);
    else
    {
        if(fault_b<-2e7)
            busptr->set_positive_sequence_voltage_in_pu(current_voltage+0.9);
        else
        {
            if(fault_b<-2e6)
                busptr->set_positive_sequence_voltage_in_pu(current_voltage+0.8);
            else
            {
                if(fault_b<-2e5)
                    busptr->set_positive_sequence_voltage_in_pu(current_voltage+0.7);
                else
                {
                    if(fault_b<-2e4)
                        busptr->set_positive_sequence_voltage_in_pu(current_voltage+0.6);
                    else
                        busptr->set_positive_sequence_voltage_in_pu(current_voltage+0.1);
                }
            }
        }
    }
    NET_MATRIX& network = get_network_matrix();
    size_t internal_bus = network.get_internal_bus_number_of_physical_bus(bus);
    internal_bus_complex_voltage_in_pu[internal_bus] = busptr->get_positive_sequence_complex_voltage_in_pu();
}


void DYN_SIMULATOR::guess_bus_voltage_with_ac_line_fault_set(const DEVICE_ID& did, size_t side_bus, double location, const FAULT& fault)
{
    if(location >=0.0 and location <= 1.0)
    {
        PF_DATA& psdb = toolkit->get_database();
        AC_LINE* line = psdb.get_ac_line(did);
        if(line!=NULL and line->is_connected_to_bus(side_bus))
        {
            if(location<=0.5)
            {
                size_t ibus = line->get_sending_side_bus();
                size_t jbus = line->get_receiving_side_bus();

                size_t this_bus = side_bus;
                size_t other_bus = (ibus==side_bus? jbus:ibus);

                double this_location = location;
                double other_location = 1.0-location;

                FAULT this_fault = fault;
                this_fault.set_fault_shunt_in_pu(fault.get_fault_shunt_in_pu()/exp(this_location*2.0));
                FAULT other_fault = fault;
                other_fault.set_fault_shunt_in_pu(fault.get_fault_shunt_in_pu()/exp(other_location*2.0));
                guess_bus_voltage_with_bus_fault_set(this_bus, this_fault);
                guess_bus_voltage_with_bus_fault_set(other_bus, other_fault);
            }
            else
            {
                size_t ibus = line->get_sending_side_bus();
                size_t jbus = line->get_receiving_side_bus();

                location = 1.0-location;

                side_bus = (ibus==side_bus? ibus:jbus);

                guess_bus_voltage_with_ac_line_fault_set(did, side_bus, location, fault);
            }
        }
    }
}

void DYN_SIMULATOR::guess_bus_voltage_with_ac_line_fault_cleared(const DEVICE_ID& did, size_t side_bus, double location, const FAULT& fault)
{
    if(location >= 0.0 and location <= 1.0)
    {
        PF_DATA& psdb = toolkit->get_database();
        AC_LINE* line = psdb.get_ac_line(did);
        if(line!=NULL and line->is_connected_to_bus(side_bus))
        {
            if(location<=0.5)
            {
                size_t ibus = line->get_sending_side_bus();
                size_t jbus = line->get_receiving_side_bus();

                size_t this_bus = side_bus;
                size_t other_bus = (ibus==side_bus? jbus:ibus);

                double this_location = location;
                double other_location = 1.0-location;

                FAULT this_fault = fault;
                this_fault.set_fault_shunt_in_pu(fault.get_fault_shunt_in_pu()/exp(this_location*2.0));
                FAULT other_fault = fault;
                other_fault.set_fault_shunt_in_pu(fault.get_fault_shunt_in_pu()/exp(other_location*2.0));
                guess_bus_voltage_with_bus_fault_cleared(this_bus, this_fault);
                guess_bus_voltage_with_bus_fault_cleared(other_bus, other_fault);
            }
            else
            {
                size_t ibus = line->get_sending_side_bus();
                size_t jbus = line->get_receiving_side_bus();

                location = 1.0-location;

                side_bus = (ibus==side_bus? ibus:jbus);

                guess_bus_voltage_with_ac_line_fault_cleared(did, side_bus, location, fault);
            }
        }
    }
}

void DYN_SIMULATOR::update_generators_in_islands()
{
    ostringstream osstream;
    PF_DATA& psdb = toolkit->get_database();
    NET_MATRIX& network_matrix = get_network_matrix();

    vector< vector<size_t> > islands = network_matrix.get_islands_with_physical_bus_number();
    if(islands.size()==generators_in_islands.size()) // won't update if islands doesn't change
        return;

    generators_in_islands.clear();

    size_t nislands = islands.size();
    for(size_t i=0; i!=nislands; ++i)
    {
        vector<size_t> island = islands[i];

        vector<GENERATOR*> generators_in_island;
        vector<GENERATOR*> generators_at_bus;

        size_t nisland = island.size();
        for(size_t j=0; j!=nisland; ++j)
        {
            size_t bus = island[j];
            generators_at_bus = psdb.get_generators_connecting_to_bus(bus);
            size_t n = generators_at_bus.size();
            for(size_t k=0; k!=n; ++k)
            {
                GENERATOR* generator = generators_at_bus[k];
                generators_in_island.push_back(generator);
            }
        }

        generators_in_islands.push_back(generators_in_island);
    }
}

bool DYN_SIMULATOR::is_system_angular_stable() const
{
    ostringstream osstream;
//    double TIME = TIME;
    bool system_is_stable = true;
    size_t nislands = generators_in_islands.size();
    vector<double> angles;
    for(size_t island=0; island!=nislands; island++)
    {
        vector<GENERATOR*> generators_in_island = generators_in_islands[island];
        size_t n = generators_in_island.size();
        angles.resize(n, 0.0);
        angles.clear();

        for(size_t i=0; i!=n; ++i)
        {
            GENERATOR* generator = generators_in_island[i];
            if(generator->get_status()==true)
            {
                SYNC_GENERATOR_MODEL* genmodel = generator->get_sync_generator_model();
                angles.push_back(genmodel->get_rotor_angle_in_deg());
            }
        }
        double angle_max = 0.0, angle_min = 0.0;
        size_t nangle = angles.size();
        if(nangle>0)
        {
            angle_max = angles[0];
            angle_min = angles[0];
            for(size_t i=1; i!=nangle; ++i)
            {
                if(angles[i]>angle_max)
                    angle_max = angles[i];
            }
            for(size_t i=1; i!=nangle; ++i)
            {
                if(angles[i]<angle_min)
                    angle_min = angles[i];
            }
        }
        double angle_difference = angle_max - angle_min;
        double scaled_angle_difference = rad2deg(round_angle_in_rad_to_PI(deg2rad(angle_difference)));

        if(not detailed_log_enabled)
            ;
        else
        {
            osstream<<"angle difference in island "<<island<<" is "<<scaled_angle_difference<<" deg at time "<<TIME<<"s.";
            toolkit->show_information(osstream);
        }
        if(angle_difference>get_rotor_angle_stability_threshold_in_deg())
        {
            system_is_stable = false;
            osstream<<"The following island is detected to be unstable at time "<<TIME<<" s. Maximum angle difference is :"<<angle_difference<<" deg (a.k.a. "<<scaled_angle_difference<<" deg)"<<endl
                   <<"Generator          Rotor angle in deg"<<endl;

            size_t n = generators_in_island.size();
            for(size_t i=0; i!=n; ++i)
            {
                GENERATOR* generator = generators_in_island[i];
                if(generator->get_status()==true)
                {
                    SYNC_GENERATOR_MODEL* genmodel = generator->get_sync_generator_model();
                    osstream<<generator->get_compound_device_name()<<"  "<<genmodel->get_rotor_angle_in_deg()<<endl;
                }
            }
            toolkit->show_information(osstream);
            break;
        }
    }
    return system_is_stable;
}

void DYN_SIMULATOR::set_bus_fault(size_t bus, const complex<double>& fault_shunt)
{
    ostringstream osstream;
    PF_DATA& psdb = toolkit->get_database();

    BUS* busptr = psdb.get_bus(bus);
    if(busptr!=NULL)
    {
        string busname = busptr->get_bus_name();
        if(steps_fast_complex_abs(fault_shunt)>DOUBLE_EPSILON)
        {
            FAULT fault;
            fault.set_fault_type(THREE_PHASES_FAULT);
            fault.set_fault_shunt_in_pu(fault_shunt);

            if(busptr->get_bus_type()==OUT_OF_SERVICE)
            {
                osstream<<"No "<<fault.get_fault_type_string()<<" will be set for bus "<<bus<<"["<<busname<<"] at time "<<TIME<<" s since it is out of service."<<endl;
                toolkit->show_information(osstream);
                return;
            }
            osstream<<fault.get_fault_type_string()<<" will be set for bus "<<bus<<"["<<busname<<"] at time "<<TIME<<" s."<<endl
                   <<"Fault shunt is"<<fault_shunt<<" pu.";
            toolkit->show_information(osstream);

            busptr->set_fault(fault);
            guess_bus_voltage_with_bus_fault_set(bus, fault);

            //network_matrix.build_dynamic_network_Y_matrix();
            //build_jacobian();
        }
        else
        {
            osstream<<"Zero fault shunt is given for bus "<<bus<<"["<<busname<<"]."<<endl
                   <<"No fault will be set for bus "<<bus<<"["<<busname<<"] at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"Bus "<<bus<<" does not exist in power system database."<<endl
               <<"No bus fault will be set at time "<<TIME<<" s.";
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::clear_bus_fault(size_t bus)
{
    PF_DATA& psdb = toolkit->get_database();

    BUS* busptr = psdb.get_bus(bus);
    if(busptr!=NULL)
    {
        string busname = busptr->get_bus_name();
        ostringstream osstream;

        if(busptr->get_bus_type()==OUT_OF_SERVICE)
        {
            osstream<<"No fault will be cleared for bus "<<bus<<"["<<busname<<"] at time "<<TIME<<" s since it is out of service."<<endl;
            toolkit->show_information(osstream);
            return;
        }

        osstream<<"Fault at bus "<<bus<<"["<<busname<<"] will be cleared at time "<<TIME<<" s.";
        toolkit->show_information(osstream);

        FAULT fault = busptr->get_fault();
        busptr->clear_fault();
        guess_bus_voltage_with_bus_fault_cleared(bus, fault);

        //network_matrix.build_dynamic_network_Y_matrix();
        //build_jacobian();
    }
}

void DYN_SIMULATOR::trip_bus(size_t bus)
{
    PF_DATA& psdb = toolkit->get_database();
    ostringstream osstream;

    BUS* busptr = psdb.get_bus(bus);
    if(busptr!=NULL)
    {
        string busname = busptr->get_bus_name();
        if(busptr->get_bus_type()!=OUT_OF_SERVICE)
        {

            osstream<<"Bus "<<bus<<"["<<busname<<"] will be tripped at time "<<TIME<<" s. Devices connecting to bus "<<bus<<" will also be tripped.";
            toolkit->show_information(osstream);

            psdb.trip_bus(bus);

            in_service_buses = psdb.get_all_in_service_buses();

            optimize_network_ordering();
            //network_matrix.build_dynamic_network_Y_matrix();
            //build_jacobian();
        }
        else
        {
            osstream<<"Bus "<<bus<<"["<<busname<<"] will not be tripped at time "<<TIME<<" s since it is out of service."<<endl;
            toolkit->show_information(osstream);
        }
    }
}

void DYN_SIMULATOR::trip_buses(const vector<size_t>& buses)
{
    size_t n = buses.size();
    for(size_t i=0; i!=n; ++i)
        trip_bus(buses[i]);
}

void DYN_SIMULATOR::set_ac_line_fault(const DEVICE_ID& line_id, size_t side_bus, double location, const complex<double>& fault_shunt)
{
    ostringstream osstream;

    if(line_id.get_device_type()==STEPS_AC_LINE)
    {
        PF_DATA& psdb = toolkit->get_database();
        AC_LINE* lineptr = psdb.get_ac_line(line_id);
        if(lineptr!=NULL)
        {
            string ibusname = psdb.bus_number2bus_name(lineptr->get_sending_side_bus());
            string jbusname = psdb.bus_number2bus_name(lineptr->get_receiving_side_bus());
            if(lineptr->is_connected_to_bus(side_bus))
            {
                if(lineptr->get_sending_side_breaker_status()==false and lineptr->get_receiving_side_breaker_status()==false)
                {
                    osstream<<"No fault will be set for "<<lineptr->get_compound_device_name()<<" at time "<<TIME<<" s since it is out of service."<<endl;
                    toolkit->show_information(osstream);
                    return;
                }
                if(location>=0.0 and location<=1.0)
                {
                    if(steps_fast_complex_abs(fault_shunt)>DOUBLE_EPSILON)
                    {
                        FAULT fault;
                        fault.set_fault_type(THREE_PHASES_FAULT);
                        fault.set_fault_shunt_in_pu(fault_shunt);

                        osstream<<fault.get_fault_type_string()<<" will be set for "<<lineptr->get_compound_device_name()<<" at time "<<TIME<<" s."<<endl
                               <<"Fault shunt is "<<fault_shunt<<" pu at location "<<location<<" to bus "<<side_bus;
                        toolkit->show_information(osstream);


                        lineptr->set_fault(side_bus,location, fault);
                        guess_bus_voltage_with_ac_line_fault_set(line_id, side_bus, location, fault);

                        //network_matrix.build_dynamic_network_Y_matrix();
                        //build_jacobian();
                    }
                    else
                    {
                        osstream<<"Zero fault shunt is given for "<<lineptr->get_compound_device_name()<<"."<<endl
                               <<"No fault will be set for "<<lineptr->get_compound_device_name()<<" at time "<<TIME<<" s.";
                        toolkit->show_information(osstream);
                    }
                }
                else
                {
                    osstream<<"Warning. Fault location ("<<location<<") is out of allowed range [0.0, 1.0] for "<<lineptr->get_compound_device_name()<<"."<<endl
                           <<"No fault will be set at time "<<TIME<<" s.";
                    toolkit->show_information(osstream);
                }
            }
            else
            {
                osstream<<lineptr->get_compound_device_name()<<"is not connected to bus "<<side_bus<<endl
                       <<"No fault will be set at location "<<location<<" to bus "<<side_bus<<" at time "<<TIME<<" s.";
                toolkit->show_information(osstream);
            }
        }
        else
        {
            osstream<<"Warning. "<<line_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No fault will be set at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a LINE (it is a "<<line_id.get_device_type()<<") for setting line fault."<<endl
               <<"No line fault will be set at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::clear_ac_line_fault(const DEVICE_ID& line_id, size_t side_bus, double location)
{
    ostringstream osstream;

    if(line_id.get_device_type()==STEPS_AC_LINE)
    {
        PF_DATA& psdb = toolkit->get_database();
        AC_LINE* lineptr = psdb.get_ac_line(line_id);
        if(lineptr!=NULL)
        {
            string ibusname = psdb.bus_number2bus_name(lineptr->get_sending_side_bus());
            string jbusname = psdb.bus_number2bus_name(lineptr->get_receiving_side_bus());
            if(lineptr->is_connected_to_bus(side_bus))
            {
                if(lineptr->get_sending_side_breaker_status()==false and lineptr->get_receiving_side_breaker_status()==false)
                {
                    osstream<<"No fault will be cleared for "<<lineptr->get_compound_device_name()<<" at time "<<TIME<<" s since it is out of service."<<endl;
                    toolkit->show_information(osstream);
                    return;
                }
                if(location>=0.0 and location<=1.0)
                {
                    osstream<<"Fault at location "<<location<<" to bus "<<side_bus<<" will be cleared for "
                           <<lineptr->get_compound_device_name()<<" at time "<<TIME<<" s.";
                    toolkit->show_information(osstream);

                    FAULT fault = lineptr->get_fault_at_location(side_bus, location);
                    lineptr->clear_fault_at_location(side_bus, location);
                    guess_bus_voltage_with_ac_line_fault_cleared(line_id, side_bus, location, fault);

                    //network_matrix.build_dynamic_network_Y_matrix();
                    //build_jacobian();
                }
                else
                {
                    osstream<<"Warning. Fault location ("<<location<<") is out of allowed range [0.0, 1.0] for "<<lineptr->get_compound_device_name()<<"."<<endl
                           <<"No fault will be cleared at time "<<TIME<<" s.";
                    toolkit->show_information(osstream);
                    return;
                }
            }
            else
            {
                osstream<<lineptr->get_compound_device_name()<<"is not connected to bus "<<side_bus<<endl
                       <<"No fault will be cleared at location "<<location<<" to bus "<<side_bus<<" at time "<<TIME<<" s.";
                toolkit->show_information(osstream);
            }
        }
        else
        {
            osstream<<"Warning. "<<line_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No fault will be cleared at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a LINE (it is a "<<line_id.get_device_type()<<") for clearing line fault."<<endl
               <<"No line fault will be cleared at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::trip_ac_line(const DEVICE_ID& line_id)
{
    ostringstream osstream;

    if(line_id.get_device_type()==STEPS_AC_LINE)
    {
        PF_DATA& psdb = toolkit->get_database();
        AC_LINE* lineptr = psdb.get_ac_line(line_id);
        if(lineptr!=NULL)
        {
            string ibusname = psdb.bus_number2bus_name(lineptr->get_sending_side_bus());
            string jbusname = psdb.bus_number2bus_name(lineptr->get_receiving_side_bus());
            if(lineptr->get_sending_side_breaker_status()==true or lineptr->get_receiving_side_breaker_status()==true)
            {
                osstream<<lineptr->get_compound_device_name()<<" is tripped by taking the following actions at time "<<TIME<<" s."<<endl;
                toolkit->show_information(osstream);

                if(lineptr->get_sending_side_breaker_status()==true)
                    trip_ac_line_breaker(line_id, lineptr->get_sending_side_bus());

                if(lineptr->get_receiving_side_breaker_status()==true)
                    trip_ac_line_breaker(line_id, lineptr->get_receiving_side_bus());
            }
            else
            {
                osstream<<lineptr->get_compound_device_name()<<" will not be tripped at time "<<TIME<<" s since it is out of service."<<endl;
                toolkit->show_information(osstream);
                return;
            }
        }
        else
        {
            osstream<<"Warning. "<<line_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No line will be tripped at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a LINE (it is a "<<line_id.get_device_type()<<") for tripping line."<<endl
               <<"No line will be tripped at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::trip_ac_line_breaker(const DEVICE_ID& line_id, size_t side_bus)
{
    ostringstream osstream;

    if(line_id.get_device_type()==STEPS_AC_LINE)
    {
        PF_DATA& psdb = toolkit->get_database();
        AC_LINE* lineptr = psdb.get_ac_line(line_id);
        if(lineptr!=NULL)
        {
            string ibusname = psdb.bus_number2bus_name(lineptr->get_sending_side_bus());
            string jbusname = psdb.bus_number2bus_name(lineptr->get_receiving_side_bus());
            if(lineptr->get_sending_side_bus()==side_bus)
            {
                if(lineptr->get_sending_side_breaker_status()==true)
                {
                    lineptr->set_sending_side_breaker_status(false);
                    //network_matrix.build_dynamic_network_Y_matrix();
                    //build_jacobian();

                    osstream<<lineptr->get_compound_device_name()<<" breaker at sending side (bus "<<lineptr->get_sending_side_bus()<<") is tripped at time "<<TIME<<" s.";
                    toolkit->show_information(osstream);
                }
            }
            else
            {
                if(lineptr->get_receiving_side_bus()==side_bus)
                {
                    if(lineptr->get_receiving_side_breaker_status()==true)
                    {
                        lineptr->set_receiving_side_breaker_status(false);
                        //network_matrix.build_dynamic_network_Y_matrix();
                        //build_jacobian();

                        osstream<<lineptr->get_compound_device_name()<<" breaker at receiving side (bus "<<lineptr->get_receiving_side_bus()<<") is tripped at time "<<TIME<<" s.";
                        toolkit->show_information(osstream);
                    }
                }
            }
        }
        else
        {
            osstream<<"Warning. "<<line_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No line breaker will be tripped at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a LINE (it is a "<<line_id.get_device_type()<<") for tripping line breaker."<<endl
               <<"No line breaker will be tripped at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::close_ac_line(const DEVICE_ID& line_id)
{
    ostringstream osstream;

    if(line_id.get_device_type()==STEPS_AC_LINE)
    {
        PF_DATA& psdb = toolkit->get_database();
        AC_LINE* lineptr = psdb.get_ac_line(line_id);
        if(lineptr!=NULL)
        {
            string ibusname = psdb.bus_number2bus_name(lineptr->get_sending_side_bus());
            string jbusname = psdb.bus_number2bus_name(lineptr->get_receiving_side_bus());
            if(lineptr->get_sending_side_breaker_status()==false or lineptr->get_receiving_side_breaker_status()==false)
            {
                osstream<<lineptr->get_compound_device_name()<<" is closed by taking the following actions at time "<<TIME<<" s."<<endl;
                toolkit->show_information(osstream);

                if(lineptr->get_sending_side_breaker_status()==false)
                    close_ac_line_breaker(line_id, lineptr->get_sending_side_bus());

                if(lineptr->get_receiving_side_breaker_status()==false)
                    close_ac_line_breaker(line_id, lineptr->get_receiving_side_bus());
            }
        }
        else
        {
            osstream<<"Warning. "<<line_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No line will be closed at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a LINE (it is a "<<line_id.get_device_type()<<") for closing line."<<endl
               <<"No line will be closed at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::close_ac_line_breaker(const DEVICE_ID& line_id, size_t side_bus)
{
    ostringstream osstream;

    if(line_id.get_device_type()==STEPS_AC_LINE)
    {
        PF_DATA& psdb = toolkit->get_database();
        AC_LINE* lineptr = psdb.get_ac_line(line_id);
        if(lineptr!=NULL)
        {
            string ibusname = psdb.bus_number2bus_name(lineptr->get_sending_side_bus());
            string jbusname = psdb.bus_number2bus_name(lineptr->get_receiving_side_bus());
            if(lineptr->get_sending_side_bus()==side_bus)
            {
                if(lineptr->get_sending_side_breaker_status()==false)
                {
                    lineptr->set_sending_side_breaker_status(true);
                    //network_matrix.build_dynamic_network_Y_matrix();
                    //build_jacobian();
                    osstream<<lineptr->get_compound_device_name()<<" breaker at sending side (bus "<<lineptr->get_sending_side_bus()<<") is closed at time "<<TIME<<" s.";
                    toolkit->show_information(osstream);
                }
            }
            else
            {
                if(lineptr->get_receiving_side_bus()==side_bus)
                {
                    if(lineptr->get_receiving_side_breaker_status()==false)
                    {
                        lineptr->set_receiving_side_breaker_status(true);
                        //network_matrix.build_dynamic_network_Y_matrix();
                        //build_jacobian();
                        osstream<<lineptr->get_compound_device_name()<<" breaker at receiving side (bus "<<lineptr->get_sending_side_bus()<<") is closed at time "<<TIME<<" s.";
                        toolkit->show_information(osstream);
                    }
                }
            }
        }
        else
        {
            osstream<<"Warning. "<<line_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No line breaker will be closed at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a LINE (it is a "<<line_id.get_device_type()<<") for closing line breaker"<<endl
               <<"No line breaker will be closed at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::trip_transformer(const DEVICE_ID& trans_id)
{
    ostringstream osstream;

    if(trans_id.get_device_type()==STEPS_TRANSFORMER)
    {
        PF_DATA& psdb = toolkit->get_database();
        TRANSFORMER* transptr = psdb.get_transformer(trans_id);
        if(transptr!=NULL)
        {
            string ibusname = psdb.bus_number2bus_name(transptr->get_winding_bus(PRIMARY_SIDE));
            string jbusname = psdb.bus_number2bus_name(transptr->get_winding_bus(SECONDARY_SIDE));
            string kbusname = "";
            if(transptr->is_three_winding_transformer())
                kbusname = psdb.bus_number2bus_name(transptr->get_winding_bus(TERTIARY_SIDE));

            if(transptr->get_winding_breaker_status(PRIMARY_SIDE)==true or transptr->get_winding_breaker_status(SECONDARY_SIDE)==true or
               (transptr->is_three_winding_transformer() and transptr->get_winding_breaker_status(TERTIARY_SIDE)==true) )
            {
                osstream<<transptr->get_compound_device_name()<<" is tripped by taking the following actions at time "<<TIME<<" s."<<endl;
                toolkit->show_information(osstream);

                if(transptr->get_winding_breaker_status(PRIMARY_SIDE)==true)
                    trip_transformer_breaker(trans_id, transptr->get_winding_bus(PRIMARY_SIDE));

                if(transptr->get_winding_breaker_status(SECONDARY_SIDE)==true)
                    trip_transformer_breaker(trans_id, transptr->get_winding_bus(SECONDARY_SIDE));

                if(transptr->is_three_winding_transformer() and transptr->get_winding_breaker_status(TERTIARY_SIDE)==true)
                    trip_transformer_breaker(trans_id, transptr->get_winding_bus(TERTIARY_SIDE));
            }
        }
        else
        {
            osstream<<"Warning. "<<trans_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No transformer will be tripped at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a TRANSFORMER (it is a "<<trans_id.get_device_type()<<") for tripping transformer."<<endl
               <<"No transformer will be tripped at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::trip_transformer_breaker(const DEVICE_ID& trans_id, size_t side_bus)
{
    ostringstream osstream;

    if(trans_id.get_device_type()==STEPS_TRANSFORMER)
    {
        PF_DATA& psdb = toolkit->get_database();
        TRANSFORMER* transptr = psdb.get_transformer(trans_id);
        if(transptr!=NULL)
        {
            string ibusname = psdb.bus_number2bus_name(transptr->get_winding_bus(PRIMARY_SIDE));
            string jbusname = psdb.bus_number2bus_name(transptr->get_winding_bus(SECONDARY_SIDE));
            string kbusname = "";
            if(transptr->is_three_winding_transformer())
                kbusname = psdb.bus_number2bus_name(transptr->get_winding_bus(TERTIARY_SIDE));

            if(transptr->get_winding_bus(PRIMARY_SIDE)==side_bus)
            {
                if(transptr->get_winding_breaker_status(PRIMARY_SIDE)==true)
                {
                    transptr->set_winding_breaker_status(PRIMARY_SIDE, false);
                    //network_matrix.build_dynamic_network_Y_matrix();
                    //build_jacobian();

                    osstream<<transptr->get_compound_device_name()<<" breaker at primary side (bus "<<side_bus<<") is tripped at time "<<TIME<<" s.";
                    toolkit->show_information(osstream);
                }
            }
            else
            {
                if(transptr->get_winding_bus(SECONDARY_SIDE)==side_bus)
                {
                    if(transptr->get_winding_breaker_status(SECONDARY_SIDE)==true)
                    {
                        transptr->set_winding_breaker_status(SECONDARY_SIDE, false);
                        //network_matrix.build_dynamic_network_Y_matrix();
                        //build_jacobian();

                        osstream<<transptr->get_compound_device_name()<<" breaker at secondary side (bus "<<side_bus<<") is tripped at time "<<TIME<<" s.";
                        toolkit->show_information(osstream);
                    }
                }
                else
                {
                    if(transptr->is_three_winding_transformer() and transptr->get_winding_bus(TERTIARY_SIDE)==side_bus)
                    {
                        if(transptr->get_winding_breaker_status(TERTIARY_SIDE)==true)
                        {
                            transptr->set_winding_breaker_status(TERTIARY_SIDE, false);
                            //network_matrix.build_dynamic_network_Y_matrix();
                            //build_jacobian();

                            osstream<<transptr->get_compound_device_name()<<" breaker at tertiary side (bus "<<side_bus<<") is tripped at time "<<TIME<<" s.";
                            toolkit->show_information(osstream);
                        }
                    }

                }
            }
        }
        else
        {
            osstream<<"Warning. "<<trans_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No transformer breaker will be tripped at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a TRANSFORMER (it is a "<<trans_id.get_device_type()<<") for tripping transformer breaker."<<endl
               <<"No transformer breaker will be tripped at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
        return;
    }
}

void DYN_SIMULATOR::close_transformer(const DEVICE_ID& trans_id)
{
    ostringstream osstream;

    if(trans_id.get_device_type()==STEPS_TRANSFORMER)
    {
        PF_DATA& psdb = toolkit->get_database();
        TRANSFORMER* transptr = psdb.get_transformer(trans_id);
        if(transptr!=NULL)
        {
            string ibusname = psdb.bus_number2bus_name(transptr->get_winding_bus(PRIMARY_SIDE));
            string jbusname = psdb.bus_number2bus_name(transptr->get_winding_bus(SECONDARY_SIDE));
            string kbusname = "";
            if(transptr->is_three_winding_transformer())
                kbusname = psdb.bus_number2bus_name(transptr->get_winding_bus(TERTIARY_SIDE));

            if(transptr->get_winding_breaker_status(PRIMARY_SIDE)==false or transptr->get_winding_breaker_status(SECONDARY_SIDE)==false or
               (transptr->is_three_winding_transformer() and transptr->get_winding_breaker_status(TERTIARY_SIDE)==false))
            {
                osstream<<transptr->get_compound_device_name()<<" is closed by taking the following actions at time "<<TIME<<" s."<<endl;
                toolkit->show_information(osstream);

                if(transptr->get_winding_breaker_status(PRIMARY_SIDE)==false)
                    close_transformer_breaker(trans_id, transptr->get_winding_bus(PRIMARY_SIDE));

                if(transptr->get_winding_breaker_status(SECONDARY_SIDE)==false)
                    close_transformer_breaker(trans_id, transptr->get_winding_bus(SECONDARY_SIDE));

                if(transptr->is_three_winding_transformer() and transptr->get_winding_breaker_status(TERTIARY_SIDE)==false)
                    close_transformer_breaker(trans_id, transptr->get_winding_bus(TERTIARY_SIDE));
            }
        }
        else
        {
            osstream<<"Warning. "<<trans_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No transformer will be closed at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a TRANSFORMER (it is a "<<trans_id.get_device_type()<<") for closing transformer."<<endl
               <<"No transformer will be closed at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::close_transformer_breaker(const DEVICE_ID& trans_id, size_t side_bus)
{
    ostringstream osstream;

    if(trans_id.get_device_type()==STEPS_TRANSFORMER)
    {
        PF_DATA& psdb = toolkit->get_database();
        TRANSFORMER* transptr = psdb.get_transformer(trans_id);
        if(transptr!=NULL)
        {
            string ibusname = psdb.bus_number2bus_name(transptr->get_winding_bus(PRIMARY_SIDE));
            string jbusname = psdb.bus_number2bus_name(transptr->get_winding_bus(SECONDARY_SIDE));
            string kbusname = "";
            if(transptr->is_three_winding_transformer())
                kbusname = psdb.bus_number2bus_name(transptr->get_winding_bus(TERTIARY_SIDE));

            if(transptr->get_winding_bus(PRIMARY_SIDE)==side_bus)
            {
                if(transptr->get_winding_breaker_status(PRIMARY_SIDE)==false)
                {
                    transptr->set_winding_breaker_status(PRIMARY_SIDE, true);
                    //network_matrix.build_dynamic_network_Y_matrix();
                    //build_jacobian();
                    osstream<<transptr->get_compound_device_name()<<" breaker at primary side (bus "<<side_bus<<") is closed at time "<<TIME<<" s.";
                    toolkit->show_information(osstream);
                }
            }
            else
            {
                if(transptr->get_winding_bus(SECONDARY_SIDE)==side_bus)
                {
                    if(transptr->get_winding_breaker_status(SECONDARY_SIDE)==false)
                    {
                        transptr->set_winding_breaker_status(SECONDARY_SIDE, true);
                        //network_matrix.build_dynamic_network_Y_matrix();
                        //build_jacobian();
                        osstream<<transptr->get_compound_device_name()<<" breaker at secondary side (bus "<<side_bus<<") is closed at time "<<TIME<<" s.";
                        toolkit->show_information(osstream);
                    }
                }
                else
                {
                    if(transptr->is_three_winding_transformer() and transptr->get_winding_bus(TERTIARY_SIDE)==side_bus)
                    {
                        if(transptr->get_winding_breaker_status(TERTIARY_SIDE)==false)
                        {
                            transptr->set_winding_breaker_status(TERTIARY_SIDE, true);
                            //network_matrix.build_dynamic_network_Y_matrix();
                            //build_jacobian();
                            osstream<<transptr->get_compound_device_name()<<" breaker at secondary side (bus "<<side_bus<<") is closed at time "<<TIME<<" s.";
                            toolkit->show_information(osstream);
                        }
                    }
                }
            }
        }
        else
        {
            osstream<<"Warning. "<<trans_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No transformer breaker will be closed at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a TRANSFORMER (it is a "<<trans_id.get_device_type()<<") for closing transformer breaker"<<endl
               <<"No transformer breaker will be closed at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::trip_generator(const DEVICE_ID& gen_id)
{

    ostringstream osstream;

    if(gen_id.get_device_type()==STEPS_GENERATOR)
    {
        PF_DATA& psdb = toolkit->get_database();
        GENERATOR* generator = psdb.get_generator(gen_id);
        if(generator!=NULL)
        {
            string busname = psdb.bus_number2bus_name(generator->get_generator_bus());

            if(generator->get_status()==true)
            {
                generator->set_status(false);

                //network_matrix.build_dynamic_network_Y_matrix();
                //build_jacobian();

                osstream<<generator->get_compound_device_name()<<" is tripped at time "<<TIME<<" s.";
                toolkit->show_information(osstream);
            }
        }
        else
        {
            osstream<<"Warning. "<<gen_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No generator will be tripped at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a GENERATOR (it is a "<<gen_id.get_device_type()<<") for tripping generator."<<endl
               <<"No generator will be tripped at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::shed_generator(const DEVICE_ID& gen_id,double percent)
{
    ostringstream osstream;

    if(gen_id.get_device_type()==STEPS_GENERATOR)
    {
        PF_DATA& psdb = toolkit->get_database();
        GENERATOR* generator = psdb.get_generator(gen_id);
        if(generator!=NULL)
        {
            string busname = psdb.bus_number2bus_name(generator->get_generator_bus());

            if(fabs(percent)>DOUBLE_EPSILON)
            {
                if(percent==1.0)
                {
                    osstream<<generator->get_compound_device_name()<<" is shed by "<<100.0<<"% at time "<<TIME<<" s."<<endl
                            <<"Generator is to be tripped.";
                    toolkit->show_information(osstream);
                    trip_generator(gen_id);
                }
                else
                {
                    double mbase = generator->get_mbase_in_MVA();
                    osstream<<generator->get_compound_device_name()<<" is shed by "<<percent*100.0<<"% at time "<<TIME<<" s."<<endl
                            <<"MBASE is changed from "<<mbase<<" MVA to ";

                    generator->set_mbase_in_MVA(mbase*(1.0-percent));
                    osstream<<generator->get_mbase_in_MVA()<<" MVA.";
                    toolkit->show_information(osstream);
                }
            }
            else
            {
                osstream<<"Generator shed percent is 0.0 for "<<generator->get_compound_device_name()<<endl
                        <<"No generator will be shed at time "<<TIME<<" s."<<endl;
                toolkit->show_information(osstream);
            }
        }
        else
        {
            osstream<<"Warning. "<<gen_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                    <<"No generator will be shed at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a GENERATOR (it is a "<<gen_id.get_device_type()<<") for shedding generator."<<endl
                <<"No generator will be shed at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}


void DYN_SIMULATOR::trip_wt_generator(const DEVICE_ID& gen_id)
{
    ostringstream osstream;

    if(gen_id.get_device_type()==STEPS_WT_GENERATOR)
    {
        PF_DATA& psdb = toolkit->get_database();
        WT_GENERATOR* generator = psdb.get_wt_generator(gen_id);
        if(generator!=NULL)
        {
            string busname = psdb.bus_number2bus_name(generator->get_generator_bus());

            if(generator->get_status()==true)
            {
                generator->set_status(false);

                //network_matrix.build_dynamic_network_Y_matrix();
                //build_jacobian();

                osstream<<generator->get_compound_device_name()<<" is tripped at time "<<TIME<<" s.";
                toolkit->show_information(osstream);
            }
        }
        else
        {
            osstream<<"Warning. "<<gen_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                    <<"No wt generator will be tripped at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a WT GENERATOR (it is a "<<gen_id.get_device_type()<<") for tripping wt generator."<<endl
                <<"No wt generator will be tripped at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::shed_wt_generator(const DEVICE_ID& gen_id,double percent)
{
    ostringstream osstream;

    if(gen_id.get_device_type()==STEPS_WT_GENERATOR)
    {
        PF_DATA& psdb = toolkit->get_database();
        WT_GENERATOR* generator = psdb.get_wt_generator(gen_id);
        if(generator!=NULL)
        {
            string busname = psdb.bus_number2bus_name(generator->get_generator_bus());

            if(fabs(percent)>DOUBLE_EPSILON)
            {
                if(percent==1.0)
                {
                    osstream<<generator->get_compound_device_name()<<" is shed by "<<100.0<<"% at time "<<TIME<<" s."<<endl
                            <<"WT generator is to be tripped.";
                    toolkit->show_information(osstream);
                    trip_wt_generator(gen_id);
                }
                else
                {
                    double mbase = generator->get_mbase_in_MVA();
                    osstream<<generator->get_compound_device_name()<<" is shed by "<<percent*100.0<<"% at time "<<TIME<<" s."<<endl
                            <<"MBASE is changed from "<<mbase<<" MVA to ";

                    generator->set_mbase_in_MVA(mbase*(1.0-percent));
                    osstream<<generator->get_mbase_in_MVA()<<" MVA.";
                    toolkit->show_information(osstream);
                }
            }
            else
            {
                osstream<<"WT generator shed percent is 0.0 for "<<generator->get_compound_device_name()<<endl
                        <<"No WT generator will be shed at time "<<TIME<<" s."<<endl;
                toolkit->show_information(osstream);
            }
        }
        else
        {
            osstream<<"Warning. "<<gen_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                    <<"No WT generator will be shed at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a WT GENERATOR (it is a "<<gen_id.get_device_type()<<") for shedding WT generator."<<endl
                <<"No WT generator will be shed at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::trip_load(const DEVICE_ID& load_id)
{
    ostringstream osstream;

    if(load_id.get_device_type()==STEPS_LOAD)
    {
        PF_DATA& psdb = toolkit->get_database();
        LOAD* load = psdb.get_load(load_id);
        if(load!=NULL)
        {
            string busname = psdb.bus_number2bus_name(load->get_load_bus());

            if(load->get_status()==true)
            {
                load->set_status(false);

                //network_matrix.build_dynamic_network_Y_matrix();
                //build_jacobian();

                osstream<<load->get_compound_device_name()<<" is tripped at time "<<TIME<<" s.";
                toolkit->show_information(osstream);
            }
        }
        else
        {
            osstream<<"Warning. "<<load_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No load will be tripped at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a LOAD (it is a "<<load_id.get_device_type()<<") for tripping load."<<endl
               <<"No load will be tripped at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::close_load(const DEVICE_ID& load_id)
{
    ostringstream osstream;

    if(load_id.get_device_type()==STEPS_LOAD)
    {
        PF_DATA& psdb = toolkit->get_database();
        LOAD* load = psdb.get_load(load_id);
        if(load!=NULL)
        {
            string busname = psdb.bus_number2bus_name(load->get_load_bus());

            if(load->get_status()==false)
            {
                load->set_status(true);
                load->get_load_model()->initialize_to_start();

                //network_matrix.build_dynamic_network_Y_matrix();
                //build_jacobian();

                osstream<<load->get_compound_device_name()<<" is closed at time "<<TIME<<" s.";
                toolkit->show_information(osstream);
            }
        }
        else
        {
            osstream<<"Warning. "<<load_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No load will be closed at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a LOAD (it is a "<<load_id.get_device_type()<<") for closing load."<<endl
               <<"No load will be closed at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::scale_load(const DEVICE_ID& load_id, double percent)
{
    ostringstream osstream;

    if(load_id.get_device_type()==STEPS_LOAD)
    {
        PF_DATA& psdb = toolkit->get_database();
        LOAD* load = psdb.get_load(load_id);
        if(load!=NULL)
        {
            string busname = psdb.bus_number2bus_name(load->get_load_bus());

            if(fabs(percent)>DOUBLE_EPSILON)
            {
                if(load->get_status()==true)
                {
                    osstream<<load->get_compound_device_name()<<" is manually scaled "<<(percent>0.0?"up":"down")<<" by "<<percent*100.0<<"% at time "<<TIME<<" s."<<endl;
                    double scale = load->get_load_manually_scale_factor_in_pu();
                    load->set_load_manually_scale_factor_in_pu(scale+percent);
                    osstream<<"Load manual scale is changed from "<<scale<<" to "<<scale+percent<<".";
                    toolkit->show_information(osstream);
                }
                else
                {
                    osstream<<load->get_compound_device_name()<<" is out of service. No load scale action can be applied at time "<<TIME<<" s.";
                    toolkit->show_information(osstream);
                }
            }
            else
            {
                osstream<<"Load scaling percent is 0.0 for "<<load->get_compound_device_name()<<endl
                       <<"No load will be scaled at time "<<TIME<<" s."<<endl;
                toolkit->show_information(osstream);
                return;
            }
        }
        else
        {
            osstream<<"Warning. "<<load_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No load will be scaled at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a LOAD (it is a "<<load_id.get_device_type()<<") for scaling load."<<endl
               <<"No single load will be scaled at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::scale_all_load(double percent)
{
    ostringstream osstream;
    if(fabs(percent)>DOUBLE_EPSILON)
    {
        size_t n = loads.size();
        for(size_t i=0; i!=n; ++i)
        {
            if(loads[i]->get_status()==true)
                scale_load(loads[i]->get_device_id(), percent);
        }
    }
    else
    {
        osstream<<"Load scaling percent is 0.0."<<endl
               <<"No all load will be scaled at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}


void DYN_SIMULATOR::trip_fixed_shunt(const DEVICE_ID& shunt_id)
{
    ostringstream osstream;

    if(shunt_id.get_device_type()==STEPS_FIXED_SHUNT)
    {
        PF_DATA& psdb = toolkit->get_database();
        FIXED_SHUNT* shunt = psdb.get_fixed_shunt(shunt_id);
        if(shunt!=NULL)
        {
            string busname = psdb.bus_number2bus_name(shunt->get_shunt_bus());

            if(shunt->get_status()==true)
            {
                shunt->set_status(false);

                //network_matrix.build_dynamic_network_Y_matrix();
                //build_jacobian();

                osstream<<shunt->get_compound_device_name()<<" is tripped at time "<<TIME<<" s.";
                toolkit->show_information(osstream);
            }
        }
        else
        {
            osstream<<"Warning. "<<shunt_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No fixed shunt will be tripped at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a FIXED SHUNT (it is a "<<shunt_id.get_device_type()<<") for tripping fixed shunt."<<endl
               <<"No fixed shunt will be tripped at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::close_fixed_shunt(const DEVICE_ID& shunt_id)
{
    ostringstream osstream;

    if(shunt_id.get_device_type()==STEPS_FIXED_SHUNT)
    {
        PF_DATA& psdb = toolkit->get_database();
        FIXED_SHUNT* shunt = psdb.get_fixed_shunt(shunt_id);
        if(shunt!=NULL)
        {
            string busname = psdb.bus_number2bus_name(shunt->get_shunt_bus());

            if(shunt->get_status()==false)
            {
                shunt->set_status(true);

                //network_matrix.build_dynamic_network_Y_matrix();
                //build_jacobian();

                osstream<<shunt->get_compound_device_name()<<" is closed at time "<<TIME<<" s.";
                toolkit->show_information(osstream);
            }
        }
        else
        {
            osstream<<"Warning. "<<shunt_id.get_compound_device_name()<<" does not exist in power system database."<<endl
                   <<"No fixed shunt will be closed at time "<<TIME<<" s.";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        osstream<<"The given device is not a FIXED SHUNT (it is a "<<shunt_id.get_device_type()<<") for closing fixed shunt."<<endl
               <<"No fixed shunt will be closed at time "<<TIME<<" s."<<endl;
        toolkit->show_information(osstream);
    }
}


void DYN_SIMULATOR::manual_bypass_2t_lcc_hvdc(const DEVICE_ID& hvdc_id)
{
    ostringstream osstream;

    PF_DATA& psdb = toolkit->get_database();
    LCC_HVDC2T* hvdc = psdb.get_2t_lcc_hvdc(hvdc_id);
    if(hvdc!=NULL)
    {
        LCC_HVDC2T_MODEL* model = hvdc->get_2t_lcc_hvdc_model();
        if(model!=NULL and (not model->is_blocked() and not model->is_bypassed()) )
            model->manual_bypass_2t_lcc_hvdc();
    }
}

void DYN_SIMULATOR::manual_unbypass_2t_lcc_hvdc(const DEVICE_ID& hvdc_id)
{
    ostringstream osstream;
    PF_DATA& psdb = toolkit->get_database();
    LCC_HVDC2T* hvdc = psdb.get_2t_lcc_hvdc(hvdc_id);
    if(hvdc!=NULL)
    {
        LCC_HVDC2T_MODEL* model = hvdc->get_2t_lcc_hvdc_model();
        if(model!=NULL and (not model->is_blocked() and model->is_manual_bypassed()))
            model->manual_unbypass_2t_lcc_hvdc();
    }
}

void DYN_SIMULATOR::manual_block_2t_lcc_hvdc(const DEVICE_ID& hvdc_id)
{
    ostringstream osstream;
    PF_DATA& psdb = toolkit->get_database();
    LCC_HVDC2T* hvdc = psdb.get_2t_lcc_hvdc(hvdc_id);

    if(hvdc!=NULL)
    {
        LCC_HVDC2T_MODEL* model = hvdc->get_2t_lcc_hvdc_model();
        if(model!=NULL and (not model->is_blocked()))
            model->manual_block_2t_lcc_hvdc();
    }
}

void DYN_SIMULATOR::manual_unblock_2t_lcc_hvdc(const DEVICE_ID& hvdc_id)
{
    ostringstream osstream;
    PF_DATA& psdb = toolkit->get_database();
    LCC_HVDC2T* hvdc = psdb.get_2t_lcc_hvdc(hvdc_id);
    if(hvdc!=NULL)
    {
        LCC_HVDC2T_MODEL* model = hvdc->get_2t_lcc_hvdc_model();
        if(model!=NULL and model->is_manual_blocked())
            model->manual_unblock_2t_lcc_hvdc();
    }
}


double DYN_SIMULATOR::get_generator_voltage_reference_in_pu(const DEVICE_ID& gen_id)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        EXCITER_MODEL* exciter = generator->get_exciter_model();
        if(exciter!=NULL)
            return exciter->get_voltage_reference_in_pu();
        else
            return 0.0;
    }
    else
        return 0.0;
}

double DYN_SIMULATOR::get_generator_mechanical_power_reference_in_pu_based_on_mbase(const DEVICE_ID& gen_id)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        TURBINE_GOVERNOR_MODEL* tg = generator->get_turbine_governor_model();
        if(tg!=NULL)
            return tg->get_mechanical_power_reference_in_pu_based_on_mbase();
        else
            return 0.0;
    }
    else
        return 0.0;
}

double DYN_SIMULATOR::get_generator_mechanical_power_reference_in_MW(const DEVICE_ID& gen_id)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        TURBINE_GOVERNOR_MODEL* tg = generator->get_turbine_governor_model();
        if(tg!=NULL)
            return tg->get_mechanical_power_reference_in_pu_based_on_mbase()*generator->get_mbase_in_MVA();
        else
            return 0.0;
    }
    else
        return 0.0;
}

double DYN_SIMULATOR::get_generator_excitation_voltage_in_pu(const DEVICE_ID& gen_id)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        EXCITER_MODEL* exciter = generator->get_exciter_model();
        if(exciter!=NULL)
            return exciter->get_excitation_voltage_in_pu();
        else
        {
            SYNC_GENERATOR_MODEL* sg = generator->get_sync_generator_model();
            if(sg!=NULL)
                return sg->get_initial_excitation_voltage_in_pu();
            else
                return 0.0;
        }
    }
    else
        return 0.0;
}

double DYN_SIMULATOR::get_generator_mechanical_power_in_pu_based_on_mbase(const DEVICE_ID& gen_id)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        TURBINE_GOVERNOR_MODEL* tg = generator->get_turbine_governor_model();
        if(tg!=NULL)
            return tg->get_mechanical_power_in_pu_based_on_mbase();
        else
        {
            SYNC_GENERATOR_MODEL* sg = generator->get_sync_generator_model();
            if(sg!=NULL)
                return sg->get_initial_mechanical_power_in_pu_based_on_mbase();
            else
                return 0.0;
        }
    }
    else
        return 0.0;
}
double DYN_SIMULATOR::get_generator_mechanical_power_in_MW(const DEVICE_ID& gen_id)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        TURBINE_GOVERNOR_MODEL* tg = generator->get_turbine_governor_model();
        if(tg!=NULL)
            return tg->get_mechanical_power_in_pu_based_on_mbase()*generator->get_mbase_in_MVA();
        else
        {
            SYNC_GENERATOR_MODEL* sg = generator->get_sync_generator_model();
            if(sg!=NULL)
                return sg->get_initial_mechanical_power_in_pu_based_on_mbase()*generator->get_mbase_in_MVA();
            else
                return 0.0;
        }
    }
    else
        return 0.0;
}


void DYN_SIMULATOR::change_generator_voltage_reference_in_pu(const DEVICE_ID& gen_id, double vref)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        string busname = psdb.bus_number2bus_name(generator->get_generator_bus());

        EXCITER_MODEL* exciter = generator->get_exciter_model();
        if(exciter!=NULL)
        {
            exciter->set_voltage_reference_in_pu(vref);
            ostringstream osstream;
            osstream<<"Voltage reference of "<<gen_id.get_compound_device_name()<<" does not exist when trying to change excitation voltage in pu. No change is made";
            toolkit->show_information(osstream);
        }
    }
    else
    {
        ostringstream osstream;
        osstream<<"Warning. "<<gen_id.get_compound_device_name()<<" does not exist when trying to change excitation voltage in pu. No change is made";
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::change_generator_mechanical_power_reference_in_pu_based_on_mbase(const DEVICE_ID& gen_id, double Pref)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        TURBINE_GOVERNOR_MODEL* tg = generator->get_turbine_governor_model();
        if(tg!=NULL)
            tg->set_initial_mechanical_power_reference_in_pu_based_on_mbase(Pref);
    }
    else
    {
        ostringstream osstream;
        osstream<<"Warning. "<<gen_id.get_compound_device_name()<<" does not exist when trying to change mechanical power reference in pu based on MBASE. No change is made";
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::change_generator_mechanical_power_reference_in_MW(const DEVICE_ID& gen_id, double Pref)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        double one_over_mbase = generator->get_one_over_mbase_in_one_over_MVA();
        change_generator_mechanical_power_reference_in_pu_based_on_mbase(gen_id, Pref*one_over_mbase);
    }
    else
    {
        ostringstream osstream;
        osstream<<"Warning. "<<gen_id.get_compound_device_name()<<" does not exist when trying to change mechanical power reference in MW. No change is made";
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::change_generator_excitation_voltage_in_pu(const DEVICE_ID& gen_id, double efd)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        EXCITER_MODEL* ex = generator->get_exciter_model();
        if(ex!=NULL)
        {
            ostringstream osstream;
            osstream<<"Warning. Exciter model exists for "<<generator->get_compound_device_name()<<". No manual change of excitation voltage is allowed.";
            toolkit->show_information(osstream);
        }
        else
        {
            SYNC_GENERATOR_MODEL* sg = generator->get_sync_generator_model();
            if(sg!=NULL)
                sg->set_initial_excitation_voltage_in_pu(efd);
        }
    }
    else
    {
        ostringstream osstream;
        osstream<<"Warning. "<<gen_id.get_compound_device_name()<<" does not exist when trying to change excitation voltage. No change is made";
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::change_generator_mechanical_power_in_pu_based_on_mbase(const DEVICE_ID& gen_id, double pmech)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        TURBINE_GOVERNOR_MODEL* tg = generator->get_turbine_governor_model();
        if(tg!=NULL)
        {
            ostringstream osstream;
            osstream<<"Warning. Turbine governor model exists for "<<generator->get_compound_device_name()<<". No manual change of mechanical power is allowed.";
            toolkit->show_information(osstream);
        }
        else
        {
            SYNC_GENERATOR_MODEL* sg = generator->get_sync_generator_model();
            if(sg!=NULL)
                sg->set_initial_mechanical_power_in_pu_based_on_mbase(pmech);
        }
    }
    else
    {
        ostringstream osstream;
        osstream<<"Warning. "<<gen_id.get_compound_device_name()<<" does not exist when trying to change mechanical power in pu based on MBASE. No change is made";
        toolkit->show_information(osstream);
    }
}

void DYN_SIMULATOR::change_generator_mechanical_power_in_MW(const DEVICE_ID& gen_id, double pmech)
{
    PF_DATA& psdb = toolkit->get_database();
    GENERATOR* generator = psdb.get_generator(gen_id);
    if(generator != NULL)
    {
        double one_over_mbase = generator->get_one_over_mbase_in_one_over_MVA();
        change_generator_mechanical_power_in_pu_based_on_mbase(gen_id, pmech*one_over_mbase);
    }
    else
    {
        ostringstream osstream;
        osstream<<"Warning. "<<gen_id.get_compound_device_name()<<" does not exist when trying to change mechanical power in MW. No change is made";
        toolkit->show_information(osstream);
    }
}

double DYN_SIMULATOR::get_2t_lcc_hvdc_power_order_in_MW(const DEVICE_ID& hvdc_id)
{
    PF_DATA& psdb = toolkit->get_database();
    LCC_HVDC2T* hvdc = psdb.get_2t_lcc_hvdc(hvdc_id);
    if(hvdc != NULL)
        return hvdc->get_nominal_dc_power_in_MW();
    else
        return 0.0;
}

void DYN_SIMULATOR::change_2t_lcc_hvdc_power_order_in_MW(const DEVICE_ID& hvdc_id, double porder)
{
    PF_DATA& psdb = toolkit->get_database();
    LCC_HVDC2T* hvdc = psdb.get_2t_lcc_hvdc(hvdc_id);
    if(hvdc != NULL)
    {
        hvdc->set_nominal_dc_power_in_MW(porder);
    }
    else
    {
        ostringstream osstream;
        osstream<<"Warning. "<<hvdc_id.get_compound_device_name()<<" does not exist when trying to change hvdc power order in MW. No change is made";
        toolkit->show_information(osstream);
    }
}


void DYN_SIMULATOR::set_vsc_hvdc_line_fault(string vsc_name, DC_DEVICE_ID line_did, size_t side_bus, double location, double fault_r)
{
    PF_DATA& psdb = toolkit->get_database();
    VSC_HVDC* vsc_hvdc = psdb.get_vsc_hvdc(vsc_name);
    size_t line_index = vsc_hvdc->get_dc_line_index(line_did);
    if(line_index!=INDEX_NOT_EXIST)
    {
        vsc_hvdc->set_dc_line_fault_location(line_index, side_bus, location);
        vsc_hvdc->set_dc_line_fault_r_in_ohm(line_index, fault_r);
    }
}

void DYN_SIMULATOR::clear_vsc_hvdc_line_fault(string vsc_name, DC_DEVICE_ID line_did)
{
    PF_DATA& psdb = toolkit->get_database();
    VSC_HVDC* vsc_hvdc = psdb.get_vsc_hvdc(vsc_name);
    size_t line_index = vsc_hvdc->get_dc_line_index(line_did);
    if(line_index!=INDEX_NOT_EXIST)
        vsc_hvdc->clear_dc_line_fault(line_index);
}

void DYN_SIMULATOR::trip_vsc_hvdc_line(string vsc_name, DC_DEVICE_ID line_did)
{
    PF_DATA& psdb = toolkit->get_database();
    VSC_HVDC* vsc_hvdc = psdb.get_vsc_hvdc(vsc_name);
    size_t line_index = vsc_hvdc->get_dc_line_index(line_did);
    if(line_index!=INDEX_NOT_EXIST)
        vsc_hvdc->set_dc_line_status(line_index, false);
}

void DYN_SIMULATOR::close_vsc_hvdc_line(string vsc_name, DC_DEVICE_ID line_did)
{
    PF_DATA& psdb = toolkit->get_database();
    VSC_HVDC* vsc_hvdc = psdb.get_vsc_hvdc(vsc_name);
    size_t line_index = vsc_hvdc->get_dc_line_index(line_did);
    if(line_index!=INDEX_NOT_EXIST)
        vsc_hvdc->set_dc_line_status(line_index, true);
}

void DYN_SIMULATOR::switch_on_equivalent_device()
{
    size_t n = e_devices.size();
    ostringstream osstream;
    osstream<<"There are "<<n<<" equivalent devices to switch on";
    toolkit->show_information(osstream);
    for(size_t i=0; i!=n; ++i)
        e_devices[i]->switch_on();
}

