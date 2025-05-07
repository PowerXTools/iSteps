#ifndef DYNAMIC_MODEL_DATABASE_H
#define DYNAMIC_MODEL_DATABASE_H

#include "header/model/model.h"
#include <vector>
using namespace std;

class MODEL;
class SOURCE;

class DYN_DATA
{
    public:
        DYN_DATA(iSTEPS& toolkit);
        ~DYN_DATA();

        iSTEPS& get_toolkit() const;

        void check();
        void clear_all_data();
    public:
        void add_model(MODEL* model);
        void remove_the_last_model();

        void check_device_model_minimum_time_constants();

        size_t get_memory_usage();
    private:
        size_t get_model_size(MODEL* model) const;
        void common_set_model(MODEL* model, size_t model_size);
        void shrink_model_starting_position_table_at_position(void *pos);
        bool load_related_model_is_to_update(MODEL* old_model, MODEL* new_model);

        iSTEPS* toolkit;

        char *model_warehouse;
        size_t warehouse_capacity;

        bool is_full;
        size_t occupied_warehouse_capacity;
        vector<size_t> model_starting_position_table;
};
#endif // DYNAMIC_MODEL_DATABASE_H
