#ifndef IEEEG1SB_H
#define IEEEG1SB_H

#include "header/model/sg_models/turbine_governor_model/turbine_governor_model.h"
#include "header/block/integral_block.h"
#include "header/block/first_order_block.h"
#include "header/block/lead_lag_block.h"
#include "header/block/pi_block.h"

class IEEEG1SB : public TURBINE_GOVERNOR_MODEL
{
    public:
        IEEEG1SB(iSTEPS& toolkit);
        IEEEG1SB(const IEEEG1SB&model);
        virtual ~IEEEG1SB();
        virtual IEEEG1SB& operator=(const IEEEG1SB& model);
    public: // specific model level
        virtual string get_model_name() const;

        void set_K(double K);
        void set_T1_in_s(double T);
        void set_T2_in_s(double T);
        void set_T3_in_s(double T);
        void set_Uo_in_pu(double U);
        void set_Uc_in_pu(double U);
        void set_Pmax_in_pu(double P);
        void set_Pmin_in_pu(double P);
        void set_T4_in_s(double T);
        void set_T5_in_s(double T);
        void set_T6_in_s(double T);
        void set_T7_in_s(double T);
        void set_K1(double K);
        void set_K2(double K);
        void set_K3(double K);
        void set_K4(double K);
        void set_K5(double K);
        void set_K6(double K);
        void set_K7(double K);
        void set_K8(double K);
        void set_KP(double K);
        void set_KI(double K);
        void set_Tfuel_in_s(double T);
        void set_TFD1_in_s(double T);
        void set_TFD2_in_s(double T);
        void set_Cb_in_s(double T);
        void set_Kb(double K);

        double get_K() const;
        double get_T1_in_s() const;
        double get_T2_in_s() const;
        double get_T3_in_s() const;
        double get_Uo_in_pu() const;
        double get_Uc_in_pu() const;
        double get_Pmax_in_pu() const;
        double get_Pmin_in_pu() const;
        double get_T4_in_s() const;
        double get_T5_in_s() const;
        double get_T6_in_s() const;
        double get_T7_in_s() const;
        double get_K1() const;
        double get_K2() const;
        double get_K3() const;
        double get_K4() const;
        double get_K5() const;
        double get_K6() const;
        double get_K7() const;
        double get_K8() const;
        double get_KP() const;
        double get_KI() const;
        double get_Tfuel_in_s() const;
        double get_TFD1_in_s() const;
        double get_TFD2_in_s() const;
        double get_Cb_in_s() const;
        double get_Kb() const;

    public:
        virtual bool setup_model_with_steps_string_vector(vector<string>& data);
        virtual bool setup_model_with_psse_string(string data);
        virtual bool setup_model_with_bpa_string(string data);

        virtual void prepare_model_data_table();
        virtual void prepare_model_internal_variable_table();

        virtual void setup_block_toolkit_and_parameters();

        virtual void initialize();
        virtual void run(DYNAMIC_MODE mode);
        virtual double get_mechanical_power_in_pu_based_on_mbase() const;
        virtual double get_mechanical_power_upper_limit_in_pu_based_on_mbase() const;
        virtual double get_mechanical_power_lower_limit_in_pu_based_on_mbase() const;
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

        virtual void linearize();
        virtual void build_linearized_matrix_ABCD();
    private:
        void copy_from_const_model(const IEEEG1SB& model);

        LEAD_LAG_BLOCK droop;
        INTEGRAL_BLOCK servo_motor;
        FIRST_ORDER_BLOCK delayer1;
        FIRST_ORDER_BLOCK delayer2;
        FIRST_ORDER_BLOCK delayer3;
        FIRST_ORDER_BLOCK delayer4;
        PI_BLOCK boiler_PI;
        FIRST_ORDER_BLOCK fuel_producer;
        FIRST_ORDER_BLOCK fuel_delayer1;
        FIRST_ORDER_BLOCK fuel_delayer2;
        INTEGRAL_BLOCK drum;
        double Tservo;
        double Uo, Uc;
        double K1, K2;
        double K3, K4;
        double K5, K6;
        double K7, K8;
        double Kb;

};

#endif // IEEEG1SB_H
