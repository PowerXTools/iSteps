#ifndef AREA_H
#define AREA_H

#include <string>
using namespace std;

class iSTEPS;

class AREA
{
    public:
        AREA(iSTEPS& toolkit);
        virtual ~AREA();
        void set_toolkit(iSTEPS& toolkit);
        iSTEPS& get_toolkit() const;

        void set_area_number(size_t area_number);
        void set_area_name(string area_name);
        void set_area_swing_bus(size_t bus);
        void set_expected_power_leaving_area_in_MW(double P);
        void set_area_power_mismatch_tolerance_in_MW(double P);

        size_t get_area_number() const;
        string get_area_name() const;
        size_t get_area_swing_bus() const;
        double get_expected_power_leaving_area_in_MW() const;
        double get_area_power_mismatch_tolerance_in_MW() const;

        bool is_valid() const;
        void check();
        void clear();
        void report() const;

        virtual AREA& operator=(const AREA& area);
        //double get_actual_power_leaving_area_in_MW() const;
    private:
        void set_area_swing_bus_with_zero_input();
        void set_area_swing_bus_with_existing_bus(size_t bus);

        iSTEPS* toolkit;

        size_t area_number;
        size_t area_name_index;
        size_t area_swing_bus;
        double expected_power_leaving_area_in_MW;
        double area_power_mismatch_tolerance_in_MW;
};
#endif // AREA_H
