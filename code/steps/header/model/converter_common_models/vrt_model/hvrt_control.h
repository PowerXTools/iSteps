#ifndef HVRT_CONTROL_H
#define HVRT_CONTROL_H

#include "header/model/converter_common_models/vrt_model/vrt_control_model.h"
#include "header/model/converter_common_models/lvpl_model/lvpl.h"

class HVRT_CONTROL : public VRT_CONTROL_MODEL
{
    public:
        HVRT_CONTROL(iSTEPS& toolkit);
        ~HVRT_CONTROL();
        HVRT_CONTROL(const HVRT_CONTROL& model);
        virtual HVRT_CONTROL& operator=(const HVRT_CONTROL& model);
        void clear();
        void copy_from_const_model(const HVRT_CONTROL& model);

    public:
        void set_P_vrt_during_mode(VRT_DURING_CONTROL_MODE m);
        void set_kP_vrt_during(double k);
        void set_P_set_vrt_during_in_pu(double p);
        void set_k1_Ip_vrt_during(double k);
        void set_k2_Ip_vrt_during(double k);
        void set_Ip_set_vrt_during_in_pu(double I);

        void set_P_vrt_prepare2recover_mode(VRT_PREPARE2RECOVER_CONTROL_MODE m);
        void set_k_Ip_prepare2recover(double k);
        void set_Ip_set_prepare2recover(double I);

        void set_P_vrt_recover_mode(VRT_RECOVER_CONTROL_MODE m);
        void set_P_recover_slope_in_pu_per_s(double s);
        void set_P_recover_inertia_in_s(double T);

        void set_Q_vrt_during_mode(VRT_DURING_CONTROL_MODE m);
        void set_kQ_vrt_during(double k);
        void set_Q_set_vrt_during_in_pu(double q);
        void set_k1_Iq_vrt_during(double k);
        void set_k2_Iq_vrt_during(double k);
        void set_Iq_set_vrt_during_in_pu(double I);

        void set_Q_vrt_prepare2recover_mode(VRT_PREPARE2RECOVER_CONTROL_MODE m);
        void set_k_Iq_prepare2recover(double k);
        void set_Iq_set_prepare2recover(double I);

        void set_Q_vrt_recover_mode(VRT_RECOVER_CONTROL_MODE m);
        void set_Q_recover_slope_in_pu_per_s(double s);
        void set_Q_recover_inertia_in_s(double T);

        VRT_DURING_CONTROL_MODE get_P_vrt_during_mode() const;
        double get_kP_vrt_during() const;
        double get_P_set_vrt_during_in_pu() const;
        double get_k1_Ip_vrt_during() const;
        double get_k2_Ip_vrt_during() const;
        double get_Ip_set_vrt_during_in_pu() const;

        VRT_PREPARE2RECOVER_CONTROL_MODE get_P_vrt_prepare2recover_mode() const;
        double get_k_Ip_prepare2recover() const;
        double get_Ip_set_prepare2recover() const;

        VRT_RECOVER_CONTROL_MODE get_P_vrt_recover_mode() const;
        double get_P_recover_slope_in_pu_per_s() const;
        double get_P_recover_inertia_in_s() const;

        VRT_DURING_CONTROL_MODE get_Q_vrt_during_mode() const;
        double get_kQ_vrt_during() const;
        double get_Q_set_vrt_during_in_pu() const;
        double get_k1_Iq_vrt_during() const;
        double get_k2_Iq_vrt_during() const;
        double get_Iq_set_vrt_during_in_pu() const;

        VRT_PREPARE2RECOVER_CONTROL_MODE get_Q_vrt_prepare2recover_mode() const;
        double get_k_Iq_prepare2recover() const;
        double get_Iq_set_prepare2recover() const;

        VRT_RECOVER_CONTROL_MODE get_Q_vrt_recover_mode() const;
        double get_Q_recover_slope_in_pu_per_s() const;
        double get_Q_recover_inertia_in_s() const;

    public:
        virtual void check_vrt_status();
        virtual double get_vrt_P_command();
        virtual double get_vrt_Q_command();
    private:
        void check_when_vrt_in_normal_status();
        void check_when_vrt_in_during_status();
        void check_when_vrt_in_prepare2recover_status();
        void check_when_vrt_in_recover_status();
        bool is_active_power_recovered() const;
        bool is_reactive_power_recovered() const;
    private:
        VRT_DURING_CONTROL_MODE P_vrt_during_mode, Q_vrt_during_mode;
        VRT_PREPARE2RECOVER_CONTROL_MODE P_vrt_prepare2recover_mode, Q_vrt_prepare2recover_mode;
        VRT_RECOVER_CONTROL_MODE P_vrt_recover_mode, Q_vrt_recover_mode;

        double kP_during, P_set_during; // P power control
        double k1_Ip_during, k2_Ip_during, Ip_set_during; // P current control
        double kQ_during, Q_set_during; // Q power control
        double k1_Iq_during, k2_Iq_during, Iq_set_during; // Q current control

        double k_Ip_prepare2recover, Ip_set_prepare2recover;  // prepare to recover
        double k_Iq_prepare2recover, Iq_set_prepare2recover;

        double kP_recover, TP_recover; // recover
        double kQ_recover, TQ_recover;

        bool P_recovered, Q_recovered;
};

#endif // HVRT_CONTROL_H
