#include "header/model/pvu_models/pv_converter_model/pvcvx.h"
#include "header/basic/utility.h"
#include "header/steps_namespace.h"
#include <cstdio>
#include <istream>
#include <iostream>
using namespace std;

PVCVX::PVCVX(iSTEPS& toolkit) : PV_CONVERTER_MODEL(toolkit),
                               active_current_commander(toolkit),
                               reactive_voltage_commander(toolkit),
                               PLL_frequency_integrator(toolkit),
                               PLL_angle_integrator(toolkit)
{
    clear();
}

PVCVX::~PVCVX()
{
}

void PVCVX::clear()
{
    set_model_float_parameter_count(5);

    set_current_source_flag(false);

    set_Xeq_in_pu(0.0);

    active_current_commander.set_limiter_type(NO_LIMITER);
    active_current_commander.set_K(1.0);
    active_current_commander.set_T_in_s(0.02);

    reactive_voltage_commander.set_limiter_type(NO_LIMITER);
    reactive_voltage_commander.set_K(1.0);
    reactive_voltage_commander.set_T_in_s(0.02);

    PLL_frequency_integrator.set_limiter_type(NON_WINDUP_LIMITER);

    PLL_angle_integrator.set_limiter_type(NO_LIMITER);
}

void PVCVX::copy_from_const_model(const PVCVX& model)
{
    iSTEPS& toolkit = model.get_toolkit();
    set_toolkit(toolkit);
    active_current_commander.set_toolkit(toolkit);
    reactive_voltage_commander.set_toolkit(toolkit);
    PLL_frequency_integrator.set_toolkit(toolkit);
    PLL_angle_integrator.set_toolkit(toolkit);

    clear();
    set_Xeq_in_pu(model.get_Xeq_in_pu());
    set_KPLL(model.get_KPLL());
    set_KIPLL(model.get_KIPLL());
    set_PLLmax(model.get_PLLmax());
}

PVCVX::PVCVX(const PVCVX& model):PV_CONVERTER_MODEL(model.get_toolkit()),
                                 active_current_commander(model.get_toolkit()),
                                 reactive_voltage_commander(model.get_toolkit()),
                                 PLL_frequency_integrator(model.get_toolkit()),
                                 PLL_angle_integrator(model.get_toolkit())
{
    copy_from_const_model(model);
}

PVCVX& PVCVX::operator=(const PVCVX& model)
{
    if(this==&model)
        return *this;

    copy_from_const_model(model);

    return (*this);
}

void PVCVX::set_Xeq_in_pu(double xeq)
{
    Xeq = xeq;
}

void PVCVX::set_KPLL(double K)
{
    KPLL = K;
}

void PVCVX::set_KIPLL(double K)
{
    PLL_frequency_integrator.set_T_in_s(1.0/K);
}

void PVCVX::set_PLLmax(double pmax)
{
    PLL_frequency_integrator.set_upper_limit(pmax);
}

double PVCVX::get_Xeq_in_pu() const
{
    return Xeq;
}

double PVCVX::get_KPLL() const
{
    return KPLL;
}

double PVCVX::get_KIPLL() const
{
    return 1.0/PLL_frequency_integrator.get_T_in_s();
}

double PVCVX::get_PLLmax() const
{
    return PLL_frequency_integrator.get_upper_limit();
}

string PVCVX::get_model_name() const
{
    return "PVCVX";
}

bool PVCVX::setup_model_with_steps_string_vector(vector<string>& data)
{
    ostringstream osstream;

    bool is_successful = false;
    if(data.size()>=8)
    {
        string model_name = get_string_data(data[0],"");
        if(model_name==get_model_name())
        {
            size_t ibus;
            string id;
            double xeq, kpll, kipll, pllmax, prate;

            ibus = (size_t)(get_integer_data(data[1],"0"));
            id = get_string_data(data[2],"");

            size_t i=3;
            xeq = get_double_data(data[i],"0.0"); i++;
            kpll = get_double_data(data[i],"0.0"); i++;
            kipll = get_double_data(data[i],"0.0"); i++;
            pllmax = get_double_data(data[i],"0.0"); i++;
            prate = get_double_data(data[i],"0.0");

            DEVICE_ID did = get_pv_unit_device_id(ibus, id);
            iSTEPS& toolkit = get_toolkit();
            PF_DATA& psdb = toolkit.get_database();
            PV_UNIT* pv_unit = psdb.get_pv_unit(did);
            if(pv_unit==NULL)
            {
                osstream<<"Error when loading data to build "<<get_model_name()<<" model for "<<did.get_compound_device_name()<<endl
                       <<"No such pv unit exists in the power system database.";
                toolkit.show_information(osstream);
                return is_successful;
            }

            double mbase = pv_unit->get_mbase_in_MVA();
            double n_lumped_turbine;
            n_lumped_turbine = mbase/prate;
            if(fabs(n_lumped_turbine-round(n_lumped_turbine))>0.1)
            {
                osstream<<"Warning. The MBASE of "<<did.get_compound_device_name()<<" deviates from n(integer) times of the Prate in "<<get_model_name()<<" model."<<endl
                        <<"MBASE = "<<mbase<<" MVA, Prate = "<<prate<<" MW, and N(float) = "<<n_lumped_turbine<<"."<<endl
                        <<"User should confirm if MBASE or Prate needs to be corrected.";
                toolkit.show_information(osstream);
            }
            pv_unit->set_number_of_lumped_pv_units(n_lumped_turbine);
            pv_unit->set_rated_power_per_pv_unit_in_MW(prate);


            set_Xeq_in_pu(xeq);
            set_KPLL(kpll);
            set_KIPLL(kipll);
            set_PLLmax(pllmax);

            complex<double> Z = pv_unit->get_source_impedance_in_pu();
            if(fabs(Z.imag()-xeq)>DOUBLE_EPSILON)
            {
                osstream<<"Warning. The Xeq of "<<get_model_name()<<" model is different from imaginary part of ZSOURCE of "<<did.get_compound_device_name()<<endl
                       <<"Xeq = "<<xeq<<" and XSource = "<<Z.imag()<<endl
                       <<"XSource will be updated as "<<xeq;
                toolkit.show_information(osstream);
                Z = complex<double>(Z.real(), xeq);
                pv_unit->set_source_impedance_in_pu(Z);
            }

            is_successful = true;

            return is_successful;
        }
        else
            return is_successful;
    }
    else
        return is_successful;
}

bool PVCVX::setup_model_with_psse_string(string data)
{
    vector<string> record = psse_dyr_string2steps_string_vector(data);
    return setup_model_with_steps_string_vector(record);
}

bool PVCVX::setup_model_with_bpa_string(string data)
{
    ostringstream osstream;
    osstream<<get_model_name()<<"::"<<__FUNCTION__<<"() is not fully supported to set up model with following data:"<<endl
            <<data;
    iSTEPS& toolkit = get_toolkit();
    toolkit.show_information(osstream);
    return false;
}

void PVCVX::setup_block_toolkit_and_parameters()
{
}

void PVCVX::initialize()
{
    iSTEPS& toolkit = get_toolkit();
    ostringstream osstream;
    if(not is_model_initialized())
    {
        PV_UNIT* pv_unit = get_pv_unit_pointer();
        setup_block_toolkit_and_parameters();

        size_t n_lumped = get_number_of_lumped_pv_units();
        double fbase = get_bus_base_frequency_in_Hz();
        double wbase = DOUBLE_PI*fbase;

        double kipll = get_KIPLL();
        if(kipll!=0.0)
        {
            PLL_frequency_integrator.set_T_in_s(1.0/kipll);
            double pllmax = get_PLLmax();
            PLL_frequency_integrator.set_upper_limit(pllmax);
            PLL_frequency_integrator.set_lower_limit(-pllmax);
        }

        PLL_angle_integrator.set_T_in_s(1.0/wbase);

        double mbase = get_mbase_in_MVA();
        mbase /= n_lumped;

        double xeq = get_Xeq_in_pu();
        complex<double>Zsource(0.0, xeq);

        double P = pv_unit->get_p_generation_in_MW()/n_lumped;
        double Q = pv_unit->get_q_generation_in_MVar()/n_lumped;
        complex<double> S(P/mbase,Q/mbase);


        complex<double> Vxy = get_terminal_complex_voltage_in_pu();
        double angle_in_rad = get_terminal_voltage_angle_in_rad();
        // ignore voltage angle
        complex<double> Ixy = conj(S/Vxy);
        complex<double> Isource = Ixy + Vxy/Zsource;

        double Ix = Isource.real();
        double Iy = Isource.imag();

        double sine = steps_sin(angle_in_rad), cosine = steps_cos(angle_in_rad);
        double IP = Ix*cosine + Iy*sine;
        double IQ =-Ix*sine + Iy*cosine;

        double EQ = IQ*(-xeq);

        active_current_commander.set_output(IP);
        active_current_commander.initialize();

        reactive_voltage_commander.set_output(EQ);
        reactive_voltage_commander.initialize();

        if(kipll!=0.0)
        {
            PLL_frequency_integrator.set_output(0.0);
            PLL_frequency_integrator.initialize();
        }

        PLL_angle_integrator.set_output(angle_in_rad);
        PLL_angle_integrator.initialize();

        set_initial_active_current_command_in_pu_based_on_mbase(IP);
        set_initial_reactive_current_command_in_pu_based_on_mbase(IQ);
        set_initial_reactive_voltage_command_in_pu(EQ);

        set_flag_model_initialized_as_true();

        if(toolkit.is_detailed_log_enabled())
        {
            osstream<<get_model_name()<<" model of "<<get_compound_device_name()<<" is initialized."<<endl
                    <<"(1) Initial active current command = "<<get_initial_active_current_command_in_pu_based_on_mbase()<<endl
                    <<"(2) Initial reactive current command = "<<get_initial_reactive_current_command_in_pu_based_on_mbase()<<endl
                    <<"(3) States of blocks"<<endl
                    <<"    active_current_commander block state: "<<active_current_commander.get_state()<<endl
                    <<"    reactive_voltage_commander block state: "<<reactive_voltage_commander.get_state()<<endl
                    <<"    PLL_frequency_integrator block state: "<<PLL_frequency_integrator.get_state()<<endl
                    <<"    PLL_angle_integrator block state: "<<PLL_angle_integrator.get_state()<<endl
                    <<"(4) active power generation :"<<get_terminal_active_power_in_MW()<<"MW"<<endl
                    <<"(5) reactive power generation :"<<get_terminal_reactive_power_in_MVar()<<"MVar"<<endl
                    <<"(6) terminal current :"<<get_terminal_current_in_pu_based_on_mbase()<<"pu";
            toolkit.show_information(osstream);
        }
    }
}

void PVCVX::run(DYNAMIC_MODE mode)
{
    double fbase = get_bus_base_frequency_in_Hz();
    double wbase = DOUBLE_PI*fbase;

    complex<double> Vxy = get_terminal_complex_voltage_in_pu();
    double angle_in_rad = get_terminal_voltage_angle_in_rad();
    double angle_in_deg = rad2deg(angle_in_rad);

    double IP = get_active_current_command_in_pu_based_on_mbase();

    active_current_commander.set_input(IP);
    active_current_commander.run(mode);

    double EQ = get_reactive_voltage_command_in_pu();

    reactive_voltage_commander.set_input(EQ);
    reactive_voltage_commander.run(mode);

    double kpll = get_KPLL();
    double kipll = get_KIPLL();
    if(kpll!=0.0 or kipll!=0.0)
    {
        double Vr = Vxy.real();
        double Vi = Vxy.imag();

        double angle = get_pll_angle_in_rad();
        double Vy = -Vr*steps_sin(angle)+Vi*steps_cos(angle);

        double input = Vy*kpll/wbase;
        PLL_frequency_integrator.set_input(input);
        PLL_frequency_integrator.run(mode);

        double output = PLL_frequency_integrator.get_output();
        input += output;

        double pllmax = get_PLLmax();
        if(input>=-pllmax and input<=pllmax)
            ;
        else
        {
            if(input>pllmax)
                input = pllmax;
            else
                input = -pllmax;
        }

        PLL_angle_integrator.set_input(input);
        PLL_angle_integrator.run(mode);
    }
    else
    {
        set_pll_angle_in_deg(angle_in_deg);
    }
    if(mode==DYNAMIC_UPDATE_MODE)
        set_flag_model_updated_as_true();
}

complex<double> PVCVX::get_source_Norton_equivalent_complex_current_in_pu_in_xy_axis_based_on_sbase()
{
    iSTEPS& toolkit = get_toolkit();
    double one_over_sbase = toolkit.get_one_over_system_base_power_in_one_over_MVA();
    double mbase = get_mbase_in_MVA();

    double Xeq = get_Xeq_in_pu();

    double Ip = active_current_commander.get_output();
    double Iq = -reactive_voltage_commander.get_output()/Xeq;

    double pll_angle = get_pll_angle_in_rad();

    double sine = steps_sin(pll_angle), cosine = steps_cos(pll_angle);
    double Ix = Ip*cosine - Iq*sine;
    double Iy = Ip*sine + Iq*cosine;

    complex<double> Ixy(Ix, Iy);
    //cout<<"Norton Ixy based on mbase = "<<Ixy<<endl;

    return Ixy*(mbase*one_over_sbase);
}

complex<double> PVCVX::get_terminal_complex_current_in_pu_in_xy_axis_based_on_mbase()
{
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    double sbase = psdb.get_system_base_power_in_MVA();
    double one_over_mbase = get_one_over_mbase_in_one_over_MVA();
    complex<double> Ixy = get_terminal_complex_current_in_pu_in_xy_axis_based_on_sbase();
    return Ixy*(sbase*one_over_mbase);
}

complex<double> PVCVX::get_terminal_complex_current_in_pu_in_xy_axis_based_on_sbase()
{
    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    double sbase = psdb.get_system_base_power_in_MVA();
    double one_over_mbase = get_one_over_mbase_in_one_over_MVA();

    complex<double> Zsource (0.0, get_Xeq_in_pu());
    Zsource *= one_over_mbase;
    Zsource *= sbase;

    complex<double> Ixy_norton = get_source_Norton_equivalent_complex_current_in_pu_in_xy_axis_based_on_sbase();
    complex<double> Vxy = get_terminal_complex_voltage_in_pu();
    complex<double> Ixy_term = Ixy_norton - Vxy/Zsource;
    return Ixy_term;
}

double PVCVX::get_terminal_current_in_pu_based_on_mbase()
{
    return steps_fast_complex_abs(get_terminal_complex_current_in_pu_in_xy_axis_based_on_mbase());
}

double PVCVX::get_terminal_current_in_pu_based_on_sbase()
{
    return steps_fast_complex_abs(get_terminal_complex_current_in_pu_in_xy_axis_based_on_sbase());
}


void PVCVX::check()
{
    ;
}

void PVCVX::report()
{
    ostringstream osstream;
    osstream<<get_standard_psse_string(false);
    iSTEPS& toolkit = get_toolkit();
    toolkit.show_information(osstream);
}

void PVCVX::save()
{
    ;
}

string PVCVX::get_standard_psse_string(bool export_internal_bus_number) const
{
    ostringstream osstream;
    DEVICE_ID did = get_device_id();
    size_t bus = did.get_device_terminal().get_buses()[0];
    string identifier = "'"+did.get_device_identifier()+"'";

    string model_name = "'"+get_model_name()+"'";

    iSTEPS& toolkit = get_toolkit();
    NET_MATRIX& network = toolkit.get_network_matrix();
    if(export_internal_bus_number==true)
        bus = network.get_internal_bus_number_of_physical_bus(bus)+1;

    osstream<<setw(8)<<bus<<", "
            <<setw(10)<<model_name<<", "
            <<setw(6)<<identifier<<", "
            <<setw(8)<<setprecision(6)<<get_Xeq_in_pu()<<", "
            <<setw(8)<<setprecision(6)<<get_KPLL()<<", "
            <<setw(8)<<setprecision(6)<<get_KIPLL()<<", "
            <<setw(8)<<setprecision(6)<<get_PLLmax()<<", "
            <<setw(8)<<setprecision(6)<<get_rated_power_per_pv_unit_in_MW()<<" /";

    return osstream.str();
}

void PVCVX::prepare_model_data_table()
{
    clear_model_data_table();
    size_t i=0;
    add_model_data_name_and_index_pair("XEQ", i); i++;
    add_model_data_name_and_index_pair("KPLL", i); i++;
    add_model_data_name_and_index_pair("KIPLL", i); i++;
    add_model_data_name_and_index_pair("PLLMAX", i); i++;
    add_model_data_name_and_index_pair("PN", i); i++;
}

double PVCVX::get_model_data_with_name(string par_name) const
{
    par_name = string2upper(par_name);
    if(par_name=="XEQ")
        return get_Xeq_in_pu();
    if(par_name=="KPLL")
        return get_KPLL();
    if(par_name=="KIPLL")
        return get_KIPLL();
    if(par_name=="PLLMAX")
        return get_PLLmax();
    if(par_name=="PN")
        return get_rated_power_per_pv_unit_in_MW();

    return 0.0;
}

void PVCVX::set_model_data_with_name(string par_name, double value)
{
    par_name = string2upper(par_name);
    if(par_name=="N")
    {
        PV_UNIT* pv_unit= get_pv_unit_pointer();
        return pv_unit->set_number_of_lumped_pv_units((size_t)(value));
    }
    if(par_name=="KPLL")
        return set_KPLL(value);
    if(par_name=="KIPLL")
        return set_KIPLL(value);
    if(par_name=="PLLMAX")
        return set_PLLmax(value);
    if(par_name=="PN")
    {
        PV_UNIT* pv_unit= get_pv_unit_pointer();
        return pv_unit->set_rated_power_per_pv_unit_in_MW(value);
    }

    return;
}

double PVCVX::get_minimum_nonzero_time_constant_in_s()
{
    return INFINITE_THRESHOLD;
}

void PVCVX::prepare_model_internal_variable_table()
{
    clear_model_internal_variable_table();
    size_t i=0;
    add_model_internal_variable_name_and_index_pair("PLL ANGLE IN DEG", i); i++;
    add_model_internal_variable_name_and_index_pair("STATE@ACTIVE CURRENT COMMAND BLOCK", i); i++;
    add_model_internal_variable_name_and_index_pair("STATE@REACTIVE VOLTAGE COMMAND BLOCK", i); i++;
    add_model_internal_variable_name_and_index_pair("STATE@PLL FREQUENCY BLOCK", i); i++;
    add_model_internal_variable_name_and_index_pair("STATE@PLL ANGLE BLOCK", i); i++;
}

double PVCVX::get_model_internal_variable_with_name(string var_name)
{
    var_name = string2upper(var_name);
    if(var_name == "PLL ANGLE IN DEG")
        return get_pll_angle_in_deg();
    if(var_name == "STATE@ACTIVE CURRENT COMMAND BLOCK")
        return active_current_commander.get_state();
    if(var_name == "STATE@REACTIVE VOLTAGE COMMAND BLOCK")
        return reactive_voltage_commander.get_state();
    if(var_name == "STATE@PLL FREQUENCY BLOCK")
        return PLL_frequency_integrator.get_state();
    if(var_name == "STATE@PLL ANGLE BLOCK")
        return PLL_angle_integrator.get_state();

    return 0.0;
}

complex<double> PVCVX::get_terminal_complex_power_in_pu_based_on_mbase()
{
    complex<double> Vxy = get_terminal_complex_voltage_in_pu();
    complex<double> Ixy = get_terminal_complex_current_in_pu_in_xy_axis_based_on_mbase();

    complex<double> S = Vxy*conj(Ixy);
    return S;
}

complex<double> PVCVX::get_terminal_complex_power_in_MVA()
{
    return get_terminal_complex_power_in_pu_based_on_mbase()*get_mbase_in_MVA();
}

double PVCVX::get_terminal_active_power_in_pu_based_on_mbase()
{
    return get_terminal_complex_power_in_pu_based_on_mbase().real();
}

double PVCVX::get_terminal_active_power_in_MW()
{
    return get_terminal_complex_power_in_MVA().real();
}

double PVCVX::get_terminal_reactive_power_in_pu_based_on_mbase()
{
    return get_terminal_complex_power_in_pu_based_on_mbase().imag();
}

double PVCVX::get_terminal_reactive_power_in_MVar()
{
    return get_terminal_complex_power_in_MVA().imag();
}

double PVCVX::get_active_power_generation_including_stator_loss_in_pu_based_on_mbase()
{
    return get_active_power_generation_including_stator_loss_in_MW()/get_mbase_in_MVA();
}

double PVCVX::get_active_power_generation_including_stator_loss_in_MW()
{
    double pterm = get_terminal_active_power_in_MW();
    return pterm;

    /*double rsource = get_source_impedance_in_pu_based_on_mbase().real();
    double iterm = steps_fast_complex_abs(get_terminal_complex_current_in_pu_in_xy_axis_based_on_mbase());
    double mbase = get_mbase_in_MVA();

    return pterm+rsource*iterm*iterm*mbase;*/
}

double PVCVX::get_pll_angle_in_rad()
{
    double kpll = get_KPLL();
    double kipll = get_KIPLL();
    if(kpll!=0.0 or kipll!=0.0)
        return PLL_angle_integrator.get_output();
    else
        return get_terminal_voltage_angle_in_rad();
}

double PVCVX::get_pll_angle_in_deg()
{
    return rad2deg(get_pll_angle_in_rad());
}

double PVCVX::get_pll_frequency_deviation_in_pu()
{
    double fbase = get_bus_base_frequency_in_Hz();
    double wbase = DOUBLE_PI*fbase;

    complex<double> Vxy = get_terminal_complex_voltage_in_pu();

    double kpll = get_KPLL();
    double kipll = get_KIPLL();
    if(kpll!=0.0 or kipll!=0.0)
    {
        double Vr = Vxy.real();
        double Vi = Vxy.imag();

        double angle = get_pll_angle_in_rad();
        double Vy = -Vr*steps_sin(angle)+Vi*steps_cos(angle);

        double input = Vy*kpll/wbase;

        double output = PLL_frequency_integrator.get_output();

        return input+output;
    }
    else
        return 0.0;
}

double PVCVX::get_pll_frequency_deviation_in_Hz()
{
    double fbase = get_bus_base_frequency_in_Hz();

    return fbase*get_pll_frequency_deviation_in_pu();
}

double PVCVX::get_pll_frequency_in_pu()
{
    return 1.0+get_pll_frequency_deviation_in_pu();
}

double PVCVX::get_pll_frequency_in_Hz()
{
    double fbase = get_bus_base_frequency_in_Hz();

    return fbase*get_pll_frequency_in_pu();
}

complex<double> PVCVX::get_internal_voltage_in_pu_in_xy_axis()
{
    complex<double> Ixy = get_source_Norton_equivalent_complex_current_in_pu_in_xy_axis_based_on_sbase();
    complex<double> Z(0.0, get_Xeq_in_pu());

    iSTEPS& toolkit = get_toolkit();
    PF_DATA& psdb = toolkit.get_database();
    double sbase = psdb.get_system_base_power_in_MVA();
    double one_over_mbase = get_one_over_mbase_in_one_over_MVA();

    Z *= (one_over_mbase*sbase);

    return Ixy*Z;
}



void PVCVX::set_pll_angle_in_deg(double angle)
{
    PLL_angle_integrator.set_output(deg2rad(angle));
    PLL_angle_integrator.initialize();// the initialize function is used to update STORE
}


string PVCVX::get_dynamic_data_in_psse_format() const
{
    return "";
}

string PVCVX::get_dynamic_data_in_bpa_format() const
{
    return get_dynamic_data_in_psse_format();
}

string PVCVX::get_dynamic_data_in_steps_format() const
{
    return get_dynamic_data_in_psse_format();
}
