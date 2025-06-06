#include "header/model/vsc_hvdc_model/vsc_hvdc_converter_model/vsc_hvdc_converter_model.h"
#include "header/device/vsc_hvdc.h"
#include "header/basic/utility.h"
#include "header/iSTEPS.h"
#include<iostream>

using namespace std;

VSC_HVDC_CONVERTER_MODEL::VSC_HVDC_CONVERTER_MODEL(iSTEPS& toolkit):VSC_HVDC_MODEL(toolkit)
{
    converter_index = INDEX_NOT_EXIST;
    converter_name_index = INDEX_NOT_EXIST;
    converter_ac_busptr = NULL;
    set_as_current_source();
}

VSC_HVDC_CONVERTER_MODEL::~VSC_HVDC_CONVERTER_MODEL()
{
    ;
}

string VSC_HVDC_CONVERTER_MODEL::get_model_type() const
{
    return "VSC LCC_HVDC2T CONVERTER";
}

void VSC_HVDC_CONVERTER_MODEL::set_converter_name(string name)
{
    iSTEPS& toolkit = get_toolkit();
    VSC_HVDC* vsc = get_vsc_hvdc_pointer();
    if(vsc!=NULL)
    {
        size_t index = vsc->get_converter_index_with_converter_name(name);
        if(index!=INDEX_NOT_EXIST)
        {
            converter_index = index;
            add_string_to_str_int_map(name);
            converter_name_index = get_index_of_string(name);

            size_t bus = vsc->get_converter_ac_bus(converter_index);
            PF_DATA& psdb = toolkit.get_database();
            converter_ac_busptr = psdb.get_bus(bus);
        }
        else
        {
            ostringstream osstream;
            osstream<<"Fatal error. Converter with name ("<<name<<") cannot be found when building "
                    <<get_model_name()<<" model for VSC_HVDC "<<vsc->get_name();
            cout<<osstream.str()<<endl;
            toolkit.show_information(osstream);
        }
    }
}

string VSC_HVDC_CONVERTER_MODEL::get_converter_name() const
{
    return get_string_of_index(get_converter_name_index());
}

size_t VSC_HVDC_CONVERTER_MODEL::get_converter_name_index() const
{
    return converter_name_index;
}

size_t VSC_HVDC_CONVERTER_MODEL::get_converter_index() const
{
    return converter_index;
}

size_t VSC_HVDC_CONVERTER_MODEL::get_converter_bus() const
{
    return get_converter_ac_bus_pointer()->get_bus_number();
}

void VSC_HVDC_CONVERTER_MODEL::set_as_voltage_source()
{
    voltage_source_flag = true;
}

void VSC_HVDC_CONVERTER_MODEL::set_as_current_source()
{
    voltage_source_flag = false;
}

bool VSC_HVDC_CONVERTER_MODEL::is_voltage_source() const
{
    return voltage_source_flag;
}

void VSC_HVDC_CONVERTER_MODEL::set_converter_active_control_mode(VSC_HVDC_CONVERTER_ACTIVE_POWER_DYNAMIC_CONTROL_MODE active_control_mode)
{
    active_power_control_mode = active_control_mode;
}

void VSC_HVDC_CONVERTER_MODEL::set_converter_reactive_control_mode(VSC_HVDC_CONVERTER_REACTIVE_POWER_DYNAMIC_CONTROL_MODE reactive_control_mode)
{
    reactive_power_control_mode = reactive_control_mode;
}

VSC_HVDC_CONVERTER_ACTIVE_POWER_DYNAMIC_CONTROL_MODE VSC_HVDC_CONVERTER_MODEL::get_converter_active_control_mode() const
{
    return active_power_control_mode;
}

VSC_HVDC_CONVERTER_REACTIVE_POWER_DYNAMIC_CONTROL_MODE VSC_HVDC_CONVERTER_MODEL::get_converter_reactive_control_mode() const
{
    return reactive_power_control_mode;
}

BUS* VSC_HVDC_CONVERTER_MODEL::get_converter_ac_bus_pointer() const
{
    return converter_ac_busptr;
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_dc_base_voltage_in_kV() const
{
    VSC_HVDC* vsc_hvdc = get_vsc_hvdc_pointer();
    if(vsc_hvdc!=NULL)
        return vsc_hvdc->get_dc_network_base_voltage_in_kV();
    else
        return 0.0;
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_ac_bus_base_voltage_in_kV() const
{
    BUS* bus = get_converter_ac_bus_pointer();
    if(bus!=NULL)
        return bus->get_base_voltage_in_kV();
    else
        return 0.0;
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_ac_bus_angle_in_deg() const
{
    BUS* bus = get_converter_ac_bus_pointer();
    if(bus!=NULL)
        return bus->get_positive_sequence_angle_in_deg();
    else
        return 0.0;
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_ac_bus_angle_in_rad() const
{
    BUS* bus = get_converter_ac_bus_pointer();
    if(bus!=NULL)
        return bus->get_positive_sequence_angle_in_rad();
    else
        return 0.0;
}


double VSC_HVDC_CONVERTER_MODEL::get_converter_ac_bus_voltage_in_pu() const
{
    BUS* bus = get_converter_ac_bus_pointer();
    if(bus!=NULL)
        return bus->get_positive_sequence_voltage_in_pu();
    else
        return 0.0;
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_ac_bus_voltage_in_kV() const
{
    BUS* bus = get_converter_ac_bus_pointer();
    if(bus!=NULL)
        return bus->get_positive_sequence_voltage_in_kV();
    else
        return 0.0;
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_ac_bus_complex_voltage_in_pu() const
{
    BUS* bus = get_converter_ac_bus_pointer();
    if(bus!=NULL)
        return bus->get_positive_sequence_complex_voltage_in_pu();
    else
        return 0.0;
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_ac_bus_complex_voltage_in_kV() const
{
    BUS* bus = get_converter_ac_bus_pointer();
    if(bus!=NULL)
        return bus->get_positive_sequence_complex_voltage_in_kV();
    else
        return 0.0;
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_ac_bus_frequency_deviation_in_pu() const
{
    BUS* bus = get_converter_ac_bus_pointer();
    if(bus!=NULL)
        return bus->get_frequency_deviation_in_pu();
    else
        return 0.0;
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_commutating_impedance_in_pu_on_converter_base() const
{
    VSC_HVDC* vsc_hvdc = get_vsc_hvdc_pointer();
    size_t index = get_converter_index();
    return vsc_hvdc->get_converter_commutating_impedance_in_pu_on_converter_base(index);
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_commutating_impedance_in_pu_on_system_base() const
{
    VSC_HVDC* vsc_hvdc = get_vsc_hvdc_pointer();
    size_t index = get_converter_index();
    return vsc_hvdc->get_converter_commutating_impedance_in_pu_on_system_base(index);
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_commutating_impedance_in_ohm() const
{
    VSC_HVDC* vsc_hvdc = get_vsc_hvdc_pointer();
    size_t index = get_converter_index();
    return vsc_hvdc->get_converter_commutating_impedance_in_ohm(index);
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_initial_S_from_converter_to_AC_bus_in_MVA() const
{
    double P = get_converter_initial_P_to_AC_bus_in_MW();
    double Q = get_converter_initial_Q_to_AC_bus_in_MVar();
    return complex<double>(P,Q);
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_initial_P_to_AC_bus_in_MW() const
{
    VSC_HVDC* vsc_hvdc = get_vsc_hvdc_pointer();
    size_t converter_index = get_converter_index();
    return vsc_hvdc->get_converter_P_to_AC_bus_in_MW(converter_index);
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_initial_Q_to_AC_bus_in_MVar() const
{
    VSC_HVDC* vsc_hvdc = get_vsc_hvdc_pointer();
    size_t converter_index = get_converter_index();
    return vsc_hvdc->get_converter_Q_to_AC_bus_in_MVar(converter_index);
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_initial_ac_angle_at_converter_side_in_rad() const
{
    VSC_HVDC* vsc_hvdc = get_vsc_hvdc_pointer();
    size_t converter_index = get_converter_index();
    double P = get_converter_initial_P_to_AC_bus_in_MW();
    double Q = get_converter_initial_Q_to_AC_bus_in_MVar();
    complex<double> S(P, Q);
    complex<double> Vac = vsc_hvdc->get_converter_ac_bus_positive_sequency_complex_voltage_in_kV(converter_index);
    complex<double> It = conj(S/Vac/SQRT3);

    complex<double> Zc_in_ohm = vsc_hvdc->get_converter_commutating_impedance_in_ohm(converter_index);
    complex<double> Vac_converter = Vac+It*Zc_in_ohm*SQRT3;

    return arg(Vac_converter);
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_initial_ac_angle_at_converter_side_in_deg() const
{
    return rad2deg(get_converter_initial_ac_angle_at_converter_side_in_rad());
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_initial_ac_angle_at_ac_bus_side_in_rad() const
{
    return get_converter_dynamic_ac_angle_at_ac_bus_side_in_rad();
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_initial_ac_angle_at_ac_bus_side_in_deg() const
{
    return get_converter_dynamic_ac_angle_at_ac_bus_side_in_deg();
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_S_from_converter_to_AC_bus_in_MVA() const
{
    complex<double> V_converter = get_converter_source_voltage_in_xy_axis_in_pu();
    complex<double> I_converter = get_converter_dynamic_current_from_converter_to_ac_bus_in_dq_axis_in_pu_on_system_base();
    double S_base = get_toolkit().get_system_base_power_in_MVA();
    return V_converter*conj(I_converter)*S_base;
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_P_from_converter_to_AC_bus_in_MW() const
{
    return get_converter_dynamic_S_from_converter_to_AC_bus_in_MVA().real();
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_Q_from_converter_to_AC_bus_in_MVar() const
{
    return get_converter_dynamic_S_from_converter_to_AC_bus_in_MVA().imag();
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_ac_angle_at_converter_side_in_rad() const
{
    complex<double> It = get_converter_dynamic_current_from_converter_to_ac_bus_in_xy_axis_in_kA();
    complex<double> Vac = get_converter_ac_bus_complex_voltage_in_kV();

    VSC_HVDC* vsc_hvdc = get_vsc_hvdc_pointer();
    complex<double> Zc_in_ohm = vsc_hvdc->get_converter_commutating_impedance_in_ohm(converter_index);
    complex<double> Vac_converter = Vac+It*Zc_in_ohm*SQRT3;

    return arg(Vac_converter);

}

double VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_ac_angle_at_converter_side_in_deg() const
{
    return rad2deg(get_converter_dynamic_ac_angle_at_converter_side_in_rad());
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_ac_angle_at_ac_bus_side_in_rad() const
{
    BUS* bus = get_converter_ac_bus_pointer();
    return bus->get_positive_sequence_angle_in_rad();
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_ac_angle_at_ac_bus_side_in_deg() const
{
    return rad2deg(get_converter_dynamic_ac_angle_at_ac_bus_side_in_rad());
}

double VSC_HVDC_CONVERTER_MODEL::get_initial_angle_at_pll_in_deg() const
{
    return rad2deg(get_initial_angle_at_pll_in_rad());
}

double VSC_HVDC_CONVERTER_MODEL::get_dynamic_angle_at_pll_in_deg() const
{
    return rad2deg(get_dynamic_angle_at_pll_in_rad());
}

double VSC_HVDC_CONVERTER_MODEL::get_converter_capacity_in_MVA() const
{
    VSC_HVDC* vsc_hvdc = get_vsc_hvdc_pointer();
    return vsc_hvdc->get_converter_rated_capacity_in_MVA(get_converter_index());
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_initial_current_from_converter_to_ac_bus_in_xy_axis_in_kA() const
{
    VSC_HVDC* vsc_hvdc = get_vsc_hvdc_pointer();
    size_t converter_index = get_converter_index();

    double P = get_converter_initial_P_to_AC_bus_in_MW();
    double Q = get_converter_initial_Q_to_AC_bus_in_MVar();
    complex<double> S(P, Q);
    complex<double> Vac = vsc_hvdc->get_converter_ac_bus_positive_sequency_complex_voltage_in_kV(converter_index);
    complex<double> It = conj(S/Vac/SQRT3);

    return It;
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_initial_current_from_converter_to_ac_bus_in_xy_axis_in_pu_on_system_base() const
{
    complex<double> I = get_converter_initial_current_from_converter_to_ac_bus_in_xy_axis_in_kA();
    double Vbase = get_converter_ac_bus_base_voltage_in_kV();
    double Sbase = get_toolkit().get_system_base_power_in_MVA();

    return I*Vbase*SQRT3/Sbase;
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_initial_current_from_converter_to_ac_bus_in_xy_axis_in_pu_on_converter_base() const
{
    complex<double> I = get_converter_initial_current_from_converter_to_ac_bus_in_xy_axis_in_kA();
    double Vbase = get_converter_ac_bus_base_voltage_in_kV();
    double Sbase = get_vsc_hvdc_pointer()->get_converter_rated_capacity_in_MVA(get_converter_index());
    return I*Vbase*SQRT3/Sbase;
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_initial_current_from_converter_to_ac_bus_in_dq_axis_in_pu_on_converter_base() const
{
    complex<double> I = get_converter_initial_current_from_converter_to_ac_bus_in_xy_axis_in_pu_on_converter_base();
    double angle = get_initial_angle_at_pll_in_rad();
    return ab2dq_with_angle_in_rad(I, angle);
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_current_from_converter_to_ac_bus_in_xy_axis_in_kA() const
{
    complex<double> I = get_converter_dynamic_current_from_converter_to_ac_bus_in_xy_axis_in_pu_on_converter_base();
    double Sbase = get_converter_capacity_in_MVA();
    double Vbase = get_converter_ac_bus_base_voltage_in_kV();

    return I*Sbase/Vbase/SQRT3;
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_current_from_converter_to_ac_bus_in_xy_axis_in_pu_on_converter_base() const
{
    if(is_voltage_source())
    {
        complex<double> E = get_converter_source_voltage_in_xy_axis_in_pu();
        complex<double> V = get_converter_ac_bus_complex_voltage_in_pu();
        complex<double> Z = get_converter_commutating_impedance_in_pu_on_converter_base();
        complex<double> I = (E-V)/Z;
        return I;
    }
    else
    {
        complex<double> Idq = get_converter_dynamic_current_from_converter_to_ac_bus_in_dq_axis_in_pu_on_converter_base_as_current_source();
        double angle = get_dynamic_angle_at_pll_in_rad();
        return dq2ab_with_angle_in_rad(Idq, angle);
    }
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_current_from_converter_to_ac_bus_in_xy_axis_in_pu_on_system_base() const
{
    if(is_voltage_source())
    {
        complex<double> I = get_converter_dynamic_current_from_converter_to_ac_bus_in_xy_axis_in_pu_on_converter_base();
        double Sbase_converter = get_converter_capacity_in_MVA();
        double Sbase_system = get_toolkit().get_system_base_power_in_MVA();
        return I*Sbase_converter/Sbase_system;
    }
    else
    {
        complex<double> Idq = get_converter_dynamic_current_from_converter_to_ac_bus_in_dq_axis_in_pu_on_system_base();
        double angle = get_dynamic_angle_at_pll_in_rad();
        return dq2ab_with_angle_in_rad(Idq, angle);
    }
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_dynamic_current_from_converter_to_ac_bus_in_dq_axis_in_pu_on_system_base() const
{
    if(is_voltage_source())
        return 0.0;
    else
    {
        complex<double> Idq = get_converter_dynamic_current_from_converter_to_ac_bus_in_dq_axis_in_pu_on_converter_base_as_current_source();

        double Sbase_converter = get_converter_capacity_in_MVA();
        double Sbase_system = get_toolkit().get_system_base_power_in_MVA();
        return Idq*Sbase_converter/Sbase_system;
    }
}

complex<double> VSC_HVDC_CONVERTER_MODEL::get_converter_source_voltage_in_xy_axis_in_pu() const
{
    if(is_voltage_source())
        return get_converter_voltage_in_xy_axis_in_pu_as_voltage_source();
    else
    {
        complex<double> V = get_converter_ac_bus_complex_voltage_in_pu();
        complex<double> I = get_converter_dynamic_current_from_converter_to_ac_bus_in_xy_axis_in_pu_on_converter_base();
        complex<double> Z = get_converter_commutating_impedance_in_pu_on_converter_base();
        complex<double> E = V+I*Z;
        return E;
    }
}

