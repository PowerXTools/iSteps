#ifndef INPHNO_H
#define INPHNO_H

#include <vector>

using namespace std;

class INPHNO
{
    public:
        INPHNO();
        ~INPHNO();
        INPHNO(const INPHNO& inphno);
        INPHNO& operator=(const INPHNO& inphno);

        //size_t get_internal_bus_count() const;

        void set_physical_internal_bus_number_pair(size_t physical_bus, size_t internal_bus);
        void update_with_new_internal_bus_permutation(const vector<size_t>& P);

        bool is_table_full() const;
        bool empty() const;
        void clear();

        size_t get_table_size() const;

        size_t get_internal_bus_number_of_physical_bus_number(size_t bus) const;
        size_t get_physical_bus_number_of_internal_bus_number(size_t bus) const;

        void report() const;

    private:
        void copy_from_const_inphno(const INPHNO& inphno);

        bool is_new_internal_bus_permutation_correct(const vector<size_t>& P);
        vector<size_t> physical_to_internal_lookup_table;
        vector<size_t> internal_to_physical_lookup_table;
};

#endif // INPHNO_H
