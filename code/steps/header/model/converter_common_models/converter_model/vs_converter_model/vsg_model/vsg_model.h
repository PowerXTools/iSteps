#ifndef VSG_MODEL_H
#define VSG_MODEL_H

#include "header/model/converter_common_models/converter_model/vs_converter_model/vs_converter_model.h"
#include "header/iSTEPS.h"

class BUS;

class VSG_MODEL: public VS_CONVERTER_MODEL
{
    public:
        VSG_MODEL(iSTEPS& toolkit);
        ~VSG_MODEL();

    public:
        virtual string get_model_name() const = 0;

        virtual void initialize() = 0;
        virtual void run(DYNAMIC_MODE mode) = 0;

        virtual double get_virtual_frequency_deviation_in_Hz() const = 0;
        virtual double get_virtual_angle_in_rad() const = 0;
        virtual double get_virtual_voltage_in_pu() const = 0;
        virtual string get_standard_psse_string() const = 0;
    private:
};

#endif // VSG_MODEL_H
