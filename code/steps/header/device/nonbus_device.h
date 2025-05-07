#ifndef NONBUS_DEVICE_H
#define NONBUS_DEVICE_H

#include "header/basic/ownership.h"
#include "header/device/device.h"
#include <cstddef>  // declearation of size_t
#include <sstream>
#include <iomanip>

class NONBUS_DEVICE : public DEVICE
{
    public:
        NONBUS_DEVICE(iSTEPS& toolkit);
        virtual ~NONBUS_DEVICE();

        //double get_dynamic_simulator_time_in_s() const;

        void set_ownership(const OWNERSHIP& ownership);
        OWNERSHIP get_ownership() const;
        size_t get_owner_count() const;
        size_t get_owner_of_index(size_t index) const;
        double get_fraction_of_owner_of_index(size_t index) const;

        virtual bool is_connected_to_bus(size_t bus) const = 0;
        virtual bool is_in_area(size_t area) const = 0;
        virtual bool is_in_zone(size_t zone) const = 0;

        virtual bool is_valid() const = 0;
        virtual void check() {};
        virtual void clear()  {};

        virtual void report() const {};
        virtual void save() const {};

        virtual DEVICE_ID get_device_id() const {};

        virtual void set_model(MODEL* model) = 0;
        virtual MODEL* get_model_of_type(string model_type, size_t index=0) = 0;
    private:
        OWNERSHIP ownership;
};
#endif // NONBUS_DEVICE_H
