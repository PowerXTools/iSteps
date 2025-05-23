#ifndef BUS_FREQUENCY_MODEL_H
#define BUS_FREQUENCY_MODEL_H

#include "header/block/differential_block.h"

class BUS;

class BUS_FREQUENCY_MODEL
{
    public:
        BUS_FREQUENCY_MODEL(iSTEPS& toolkit);
        ~BUS_FREQUENCY_MODEL();
        void set_toolkit(iSTEPS& toolkit);
        iSTEPS& get_toolkit() const;

        void set_bus_pointer(BUS* bus);
        BUS* get_bus_pointer() const;
        size_t get_bus() const;

        void initialize();
        void run(DYNAMIC_MODE mode);
        void update_for_applying_event(DYNAMIC_EVENT_TYPE type);
    public:
        void set_frequency_deviation_in_pu(double f);
        double get_frequency_deviation_in_pu() const;
        double get_frequency_deviation_in_Hz() const;
        double get_frequency_in_pu() const;
        double get_frequency_in_Hz() const;
    private:
        iSTEPS* toolkit;
        DIFFERENTIAL_BLOCK frequency_block;
        BUS* bus_ptr;
        double fbase_Hz, tbase_s;
        double frequency_block_output_old;
        bool is_model_updated;
};

#endif // BUS_FREQUENCY_MODEL_H
