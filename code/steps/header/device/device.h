#ifndef DEVICE_H
#define DEVICE_H

#include "header/basic/ownership.h"
#include "header/basic/device_id.h"
#include "header/block/block.h"
#include "header/model/model.h"
#include <cstddef>  // declearation of size_t
#include <sstream>
#include <iomanip>

class iSTEPS;

class DEVICE
{
    public:
        DEVICE(iSTEPS& toolkit);
        virtual ~DEVICE();
        void set_toolkit(iSTEPS& toolkit);
        iSTEPS& get_toolkit() const;

        virtual bool is_in_area(size_t area) const = 0;
        virtual bool is_in_zone(size_t zone) const = 0;

        virtual bool is_valid() const = 0;
        virtual void check() {};
        virtual void clear() {};

        virtual void report() const{};

        virtual DEVICE_ID get_device_id() const{};
        string get_compound_device_name() const;
    private:
        iSTEPS* toolkit;

};
#endif // DEVICE_H
