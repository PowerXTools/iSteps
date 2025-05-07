#ifndef AERD0_H
#define AERD0_H

#include "header/model/wtg_models/wt_aerodynamic_model/wt_aerodynamic_model.h"
#include <cstdlib>

class AERD0 : public WT_AERODYNAMIC_MODEL
{
    public:
        AERD0(iSTEPS& toolkit);
        AERD0(const AERD0& model);
        virtual ~AERD0();
        virtual AERD0& operator=(const AERD0& model);
    public:
        void set_C1(double c);
        void set_C2(double c);
        void set_C3(double c);
        void set_C4(double c);
        void set_C5(double c);
        void set_C6(double c);
        void set_C7(double c);
        void set_C8(double c);

        double get_C1() const;
        double get_C2() const;
        double get_C3() const;
        double get_C4() const;
        double get_C5() const;
        double get_C6() const;
        double get_C7() const;
        double get_C8() const;
    public:
        virtual double get_Cp(double lambda, double pitch_deg) const;
        virtual double get_derivative_of_Cp_over_lambda(double lambda, double pitch_deg) const;
    public:
        virtual string get_model_name() const;

        virtual bool setup_model_with_steps_string_vector(vector<string>& data);
        virtual bool setup_model_with_psse_string(string data);
        virtual bool setup_model_with_bpa_string(string data);

        virtual void prepare_model_data_table();
        virtual void prepare_model_internal_variable_table();

        virtual void check();
        virtual void clear();
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
        void copy_from_const_model(const AERD0& model);
        // Cp function parameters
        double Cp_Coefficients[8];
};

#endif // AERD0_H
