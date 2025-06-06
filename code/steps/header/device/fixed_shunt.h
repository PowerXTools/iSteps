#ifndef FIXED_SHUNT_H
#define FIXED_SHUNT_H

#include "header/device/nonbus_device.h"
#include <string>
#include <complex>

using namespace std;

class BUS;
class FIXED_SHUNT : public NONBUS_DEVICE
{
    public:
        FIXED_SHUNT(iSTEPS& toolkit);
        virtual ~FIXED_SHUNT();

        void set_shunt_bus(size_t bus);
        void set_identifier(string identifier);
        void set_name(string name);
        void set_status(bool status);
        void set_nominal_positive_sequence_impedance_shunt_in_MVA(complex<double> s);
        void set_nominal_zero_sequence_impedance_shunt_in_MVA(complex<double> s);

        size_t get_shunt_bus() const;
        BUS* get_bus_pointer() const;
        string get_identifier() const;
        string get_name() const;
        size_t get_identifier_index() const;
        size_t get_name_index() const;
        bool get_status() const;
        complex<double> get_nominal_positive_sequence_impedance_shunt_in_MVA() const;
        complex<double> get_nominal_impedance_shunt_in_pu() const;
        complex<double> get_nominal_admittance_shunt_in_pu() const;
        complex<double> get_nominal_zero_sequence_impedance_shunt_in_MVA() const;
        complex<double> get_nominal_zero_sequence_impedance_shunt_in_pu() const;
        complex<double> get_nominal_zero_sequence_admittance_shunt_in_pu() const;

        virtual bool is_valid() const;
        virtual void check()const;
        virtual void clear();
        virtual bool is_connected_to_bus(size_t bus) const;
        virtual bool is_in_area(size_t area) const;
        virtual bool is_in_zone(size_t zone) const;
        virtual void report() const;
        virtual void save() const;
        virtual void set_model(MODEL* model);
        virtual MODEL* get_model_of_type(string model_type, size_t index=0);

        virtual FIXED_SHUNT& operator=(const FIXED_SHUNT& load);

        virtual DEVICE_ID get_device_id() const;
        //virtual string get_compound_device_name() const;

        complex<double> get_actual_impedance_shunt_in_MVA() const;

        complex<double> get_positive_sequence_complex_current_in_pu();
        complex<double> get_negative_sequence_complex_current_in_pu();
        complex<double> get_zero_sequence_complex_current_in_pu();
        complex<double> get_positive_sequence_complex_current_in_kA();
        complex<double> get_negative_sequence_complex_current_in_kA();
        complex<double> get_zero_sequence_complex_current_in_kA();

        void set_sequence_parameter_import_flag(bool flag);
        bool get_sequence_parameter_import_flag() const;
    private:
        size_t bus;
        BUS* busptr;
        size_t identifier_index;
        size_t name_index;
        bool status;
        complex<double> nominal_positive_sequence_impedance_shunt_in_MVA;
        complex<double> nominal_zero_squence_impedance_shunt_in_MVA;

        bool sequence_parameter_import_flag;
};
#endif // FIXED_SHUNT_H
