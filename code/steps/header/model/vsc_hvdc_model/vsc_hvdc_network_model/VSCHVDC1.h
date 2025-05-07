#ifndef VSCHVDC1_H_INCLUDED
#define VSCHVDC1_H_INCLUDED

#include "header/model/vsc_hvdc_model/VSC_HVDC_MODEL.h"
#include "header/model/vsc_hvdc_model/vsc_station.h"
#include "header/block/first_order_block.h"
#include "header/block/differential_block.h"
#include "header/block/integral_block.h"
#include "header/block/saturation_block.h"
#include "header/basic/timer.h"

class VSCHVDC1: public VSC_HVDC_MODEL
{
    public:
        VSCHVDC1(iSTEPS& toolkit);
        VSCHVDC1(const VSCHVDC1& model);
        virtual ~VSCHVDC1();
        virtual VSCHVDC1& operator=(const VSCHVDC1& model);
        void set_vsc_stations_count(size_t n);

    public: // specific exciter
        virtual string get_model_name() const;
        virtual void clear();
        void set_converter_bus(const size_t index, const size_t bus);
        void set_converter_active_control_mode(const size_t index, VSC_HVDC_CONVERTER_ACTIVE_POWER_CONTROL_MODE active_control_mode);
        void set_converter_reactive_control_mode(const size_t index, VSC_HVDC_CONVERTER_REACTIVE_POWER_CONTROL_MODE reactive_control_mode);
        void set_converter_active_power_control_T_in_s(const size_t index, const double t);
        void set_converter_active_power_Pmax_in_pu(const size_t index, const double Pmax);
        void set_converter_active_power_Pmin_in_pu(const size_t index, const double Pmin);
        void set_converter_active_power_kpp(const size_t index, const double k);
        void set_converter_reactive_power_control_T_in_s(const size_t index, const double t);
        void set_converter_reactive_power_Qmax_in_pu(const size_t index, const double Qmax);
        void set_converter_reactive_power_Qmin_in_pu(const size_t index, const double Qmin);
        void set_converter_reactive_power_kpq(const size_t index, const double k);
        void set_converter_dc_voltage_control_T_in_s(const size_t index, const double t);
        void set_converter_dc_voltage_Umax(const size_t index, const double Umax);
        void set_converter_dc_voltage_Umin(const size_t index, const double Umin);
        void set_converter_dc_voltage_kpud(const size_t index, const double k);
        void set_converter_ac_voltage_control_T_in_s(const size_t index, const double t);
        void set_converter_ac_voltage_Umax(const size_t index, const double Umax);
        void set_converter_ac_voltage_Umin(const size_t index, const double Umin);
        vSC_HVDC_CONVERTER_REACTIVE_POWER_CONTROL_MODE get_converter_reactive_control_mode(size_t index) const;
        double get_converter_active_power_control_T_in_s(size_t index) const;
        double get_converter_active_power_Pmax_in_pu(size_t index) const;
        double get_converter_active_power_Pmin_in_pu(size_t index) const;
        double get_converter_active_power_kpp(size_t index) const;
        double get_converter_reactive_power_control_T_in_s(size_t index) const;
        double get_converter_reactive_power_Qmax_in_pu(size_t index) const;
        double get_converter_reactive_power_Qmin_in_pu(size_t index) const;
        double get_converter_reactive_power_kpq(size_t index) const;
        double get_converter_dc_voltage_control_T_in_s(size_t index) const;
        double get_converter_dc_voltage_Umax(size_t index) const;
        double get_converter_dc_voltage_Umin(size_t index) const;
        double get_converter_dc_voltage_kpud(size_t index) const;
        double get_converter_ac_voltage_control_T_in_s(size_t index) const;
        double get_converter_ac_voltage_Umax(size_t index) const;
        double get_converter_ac_voltage_Umin(size_t index) const;
        double get_converter_dc_voltage_kpuc(size_t index)const ;
        double get_dc_voltage_ceq(size_t index) const;oid set_converter_dc_voltage_kpuc(const size_t index, const double k);
        void set_dc_voltage_ceq(const size_t index, const double ceq);

        size_t get_vsc_stations_count() const;
        size_t get_converter_bus(size_t index) const;
        VSC_HVDC_CONVERTER_ACTIVE_POWER_CONTROL_MODE get_converter_active_control_mode(size_t index) const;
        VSC_HVDC_CONVERTER_REACTIVE_POWER_CONTROL_MODE get_converter_reactive_control_mode(size_t index) const;
        double get_converter_active_power_control_T_in_s(size_t index) const;
        double get_converter_active_power_Pmax_in_pu(size_t index) const;
        double get_converter_active_power_Pmin_in_pu(size_t index) const;
        double get_converter_active_power_kpp(size_t index) const;
        double get_converter_reactive_power_control_T_in_s(size_t index) const;
        double get_converter_reactive_power_Qmax_in_pu(size_t index) const;
        double get_converter_reactive_power_Qmin_in_pu(size_t index) const;
        double get_converter_reactive_power_kpq(size_t index) const;
        double get_converter_dc_voltage_control_T_in_s(size_t index) const;
        double get_converter_dc_voltage_Umax(size_t index) const;
        double get_converter_dc_voltage_Umin(size_t index) const;
        double get_converter_dc_voltage_kpud(size_t index) const;
        double get_converter_ac_voltage_control_T_in_s(size_t index) const;
        double get_converter_ac_voltage_Umax(size_t index) const;
        double get_converter_ac_voltage_Umin(size_t index) const;
        double get_converter_dc_voltage_kpuc(size_t index)const ;
        double get_dc_voltage_ceq(size_t index) const;

        virtual complex<double> get_source_Norton_equivalent_complex_current_in_pu_in_xy_axis_based_on_sbase();

    public:
        virtual bool setup_model_with_steps_string_vector(vector<string>& data);
        virtual bool setup_model_with_psse_string(string data);
        virtual bool setup_model_with_bpa_string(string data);

        virtual void prepare_model_data_table();
        virtual void prepare_model_internal_variable_table();

        virtual void setup_block_toolkit_and_parameters();

        virtual void initialize();
        virtual void run(DYNAMIC_MODE mode);
        virtual void check_blocking_logic();
        virtual void check_bypassing_logic();
        virtual void check_mode_switching_logic();
        virtual void solve_vsc_hvdc_model_without_integration();
        virtual void check();
        virtual void report();
        virtual void save();
        virtual string get_standard_psse_string(bool export_internal_bus_number=false) const;

        virtual double get_model_data_with_name(string par_name) const;
        virtual void set_model_data_with_name(string par_name, double value);
        virtual double get_minimum_nonzero_time_constant_in_s();
        virtual double get_model_internal_variable_with_name(string var_name);

        virtual string get_dynamic_data_in_psse_format() const;
        virtual string get_dynamic_data_in_bpa_format() const;
        virtual string get_dynamic_data_in_steps_format() const;

    private:
        void copy_from_const_model(const VSCHVDC1& model);

    private:
        // vsc converter
        vector<VSC_MODEL_VAR> vsc_stations;
        size_t n_converter;
        // blocking related timers

        /*double time_when_rectifier_ac_voltage_below_delayed_blocking_voltage;
        double time_when_rectifier_ac_voltage_above_delayed_unblocking_voltage;
        double time_when_inverter_ac_voltage_below_instataneous_blocking_voltage;
        double time_when_inverter_ac_voltage_above_instataneous_unblocking_voltage;
        double time_when_inverter_ac_unblocking_signal_is_sent;

        double time_when_inverter_ac_voltage_below_delayed_bypassing_voltage;
        double time_when_inverter_ac_voltage_above_delayed_unbypassing_voltage;*/
};

#endif // VSCHVDC1_H_INCLUDED
