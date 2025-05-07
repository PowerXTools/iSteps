#ifndef LCC_STATION_H
#define LCC_STATION_H

#include "header/device/nonbus_device.h"
#include "header/device/converter_station.h"
#include "header/device/vsc.h"
#include "header/basic/device_id.h"
#include "header/basic/steps_enum.h"

#include <complex>
#include <string>

using namespace std;

class BUS;

class LCC_STATION : public CONVERTER_STATION
{
    public:
        LCC_STATION();
        virtual ~LCC_STATION();
        void clear();

        virtual LCC_STATION& operator=(const LCC_STATION& station);

        void set_number_of_converters(size_t n);
        void set_converter(size_t converter_index, LCC& converter);

        size_t get_number_of_converters() const;
        const LCC* get_converter(size_t converter_index) const;
        LCC* get_mutable_converter(size_t converter_index);
        size_t get_total_power_percent_of_converter_station() const;

        void solve_with_desired_dc_voltage_and_current(double Vdc, double Idc);
        //void solve_transformer_tap_and_angle(double Vdc, double Idc);

        double get_no_load_dc_voltage_in_kV_with_solved_transformer_tap_and_angle() const;
        double get_dc_voltage_drop_multiple_in_kV_per_A() const;

        void report() const;

        bool is_connected_to_bus(size_t bus) const;
        bool is_in_area(size_t area) const;
        bool is_in_zone(size_t zone) const;
    private:
        LCC* converters;
        size_t nconverters;
};
#endif // LCC_STATION_H
