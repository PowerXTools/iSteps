#ifndef DC_DEVICE_ID_H
#define DC_DEVICE_ID_H

#include "header/basic/constants.h"
#include "header/basic/steps_enum.h"

#include <vector>
#include <string>
#include <functional>
#include "terminal.h"
using namespace std;

class DC_DEVICE_ID
{
    public:
        DC_DEVICE_ID();
        DC_DEVICE_ID(const DC_DEVICE_ID& did);
        virtual ~DC_DEVICE_ID();
        void set_device_type(STEPS_DC_DEVICE_TYPE device_type);
        void set_device_terminal(const TERMINAL& terminal);
        /*void set_DC_DEVICE_IDentifier(string DC_DEVICE_IDentifier);
        void set_device_name(string device_name);*/
        void set_device_identifier_index(size_t index);
        void set_device_name_index(size_t index);

        STEPS_DC_DEVICE_TYPE get_device_type() const;
        TERMINAL get_device_terminal() const;
        string get_device_identifier() const;
        string get_device_name() const;
        size_t get_device_identifier_index() const;
        size_t get_device_name_index() const;
        size_t get_minimum_allowed_terminal_count() const;
        size_t get_maximum_allowed_terminal_count() const;

        string get_compound_device_name() const;

        bool is_valid() const;
        bool is_name_allowed() const;
        bool is_terminal_allowed() const;
        bool is_identifier_allowed() const;

        void clear();
        virtual DC_DEVICE_ID& operator= (const DC_DEVICE_ID& DC_DEVICE_ID);
        bool operator< (const DC_DEVICE_ID& DC_DEVICE_ID) const;
        bool operator==(const DC_DEVICE_ID& DC_DEVICE_ID) const;
        bool operator!=(const DC_DEVICE_ID& DC_DEVICE_ID) const;
    private:
        void initialize_minimum_maximum_terminal_count();
        void set_minimum_allowed_terminal_count(size_t n);
        void set_maximum_allowed_terminal_count(size_t n);
        void set_device_type_and_allowed_terminal_count(STEPS_DC_DEVICE_TYPE device_type);
        bool is_given_terminal_acceptable(const TERMINAL& terminal);

    private:
        void enable_allow_terminal();
        void enable_allow_name();
        void enable_allow_identifier();
        void disable_allow_identifier();
    private:
        STEPS_DC_DEVICE_TYPE device_type;
        size_t minimum_terminal_count, maximum_terminal_count;
        bool allow_identifier;
        bool allow_name;
        bool allow_terminal;

        TERMINAL terminal;
        size_t device_identifier_index;
        size_t device_name_index;
};


DC_DEVICE_ID get_dc_bus_device_id(size_t bus_number);
DC_DEVICE_ID get_dc_line_device_id(size_t ibus, size_t jbus, const string identifier);


namespace std
{
    template<> class hash<DC_DEVICE_ID>
    {
        public:
        size_t operator()(const DC_DEVICE_ID& did) const
        {
            size_t seed = 0;
            if(did.is_name_allowed())
            {
                size_t name_index = did.get_device_name_index();
                size_t hash_value = name_index;
                seed ^= hash_value + STEPS_MAGIC1 + (seed << 6) + (seed >> 2);
                return seed;
            }
            else
            {
                TERMINAL terminal  = did.get_device_terminal();
                vector<size_t> buses = terminal.get_buses();
                size_t n = buses.size();

                for(size_t i=0; i!=n; ++i)
                {
                    size_t bus = buses[i];
                    if(bus!=0)
                    {
                        size_t hash_value = bus;
                        switch(i)
                        {
                            case 0:
                                seed ^= hash_value + STEPS_MAGIC1 + (seed << 6) + (seed >> 2);
                                break;
                            case 1:
                                seed ^= hash_value + STEPS_MAGIC2 + (seed << 6) + (seed >> 2);
                                break;
                            case 2:
                                seed ^= hash_value + STEPS_MAGIC3 + (seed << 6) + (seed >> 2);
                                break;
                            case 3:
                            default:
                                seed ^= hash_value + STEPS_MAGIC4 + (seed << 6) + (seed >> 2);
                                break;
                        }
                    }
                }
                if(did.is_identifier_allowed())
                {
                    size_t id_index = did.get_device_identifier_index();
                    size_t hash_value = id_index;
                    seed ^= hash_value + STEPS_MAGIC1 + (seed << 6) + (seed >> 2);
                }

                return seed;
            }
        }
    };
}
#endif // DC_DEVICE_ID_H
