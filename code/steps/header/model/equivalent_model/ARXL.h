#ifndef ARXL_H
#define ARXL_H

#include "header/model/equivalent_model/equivalent_model.h"
#include "header/meter/meter.h"
#include "header/basic/continuous_buffer.h"
#include <complex>

class ARXL : public EQUIVALENT_MODEL
{
    public:
        ARXL(iSTEPS& toolkit);
        ARXL(const ARXL& model);
        virtual ARXL& operator=(const ARXL& model);
        virtual ~ARXL();

        virtual string get_model_name() const;

        void set_output_line(DEVICE_ID did, size_t meter_side);

        void add_P_input_item(METER meter, size_t delay, double coef);
        void add_Q_input_item(METER meter, size_t delay, double coef);

        vector<METER> get_P_meters() const;
        vector< vector<size_t> > get_P_delays() const;
        vector< vector<double> > get_P_coefficients() const;

        vector<METER> get_Q_meters() const;
        vector< vector<size_t> > get_Q_delays() const;
        vector< vector<double> > get_Q_coefficients() const;

        virtual void switch_output_to_equivalent_device();
    private:
        size_t get_P_meter_index(METER meter);
        size_t get_Q_meter_index(METER meter);
        bool is_P_meter_exist(METER meter);
        bool is_Q_meter_exist(METER meter);
        void add_P_meter(METER meter);
        void add_Q_meter(METER meter);
    public:
        virtual bool setup_model_with_steps_string_vector(vector<string>& data);
        virtual bool setup_model_with_psse_string(string data);
        virtual bool setup_model_with_bpa_string(string data);

        virtual void prepare_model_data_table();
        virtual void prepare_model_internal_variable_table();

        virtual void setup_block_toolkit_and_parameters();

        virtual void initialize();
        virtual void run(DYNAMIC_MODE mode);
        virtual void update_meters_buffer();
        virtual void update_equivalent_outputs();
        void update_equivalent_constant_power_load();
        virtual complex<double> get_equivalent_voltage_source_voltage_in_pu() const;
        virtual complex<double> get_equivalent_voltage_source_impedance_in_pu() const;
        virtual complex<double> get_equivalent_nominal_constant_power_load_in_MVA() const;
        virtual complex<double> get_equivalent_nominal_constant_current_load_in_MVA() const;
        virtual complex<double> get_equivalent_nominal_constant_impedance_load_in_MVA() const;
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
        void clear();
        complex<double> get_total_load_power_in_MVA() const;
        double get_P_load_power_in_MW() const;
        double get_Q_load_power_in_MVar() const;
        string get_meter_string(const METER& meter) const;
        string get_line_meter_string(const METER& meter) const;
        string get_transformer_meter_string(const METER& meter) const;
        string get_bus_meter_string(const METER& meter) const;
        string get_generator_meter_string(const METER& meter) const;
        string get_load_meter_string(const METER& meter) const;
    private:
        void copy_from_constant_model(const ARXL& model);

        vector<METER> p_meters, q_meters;
        vector<CONTINUOUS_BUFFER> p_meter_buffers, q_meter_buffers;
        vector< vector<size_t> > p_delays, q_delays;
        vector< vector<double> > p_coefficients, q_coefficients;
    private:
        complex<double> equivalent_voltage_source_V, equivalent_voltage_source_Z;
        complex<double> equivalent_load_constant_power, equivalent_load_constant_current, equivalent_load_constant_impedance;
};
#endif // ARXL_H
