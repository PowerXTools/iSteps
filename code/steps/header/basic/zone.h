#ifndef ZONE_H
#define ZONE_H

#include <string>
using namespace std;

class iSTEPS;

class ZONE
{
    public:
        ZONE(iSTEPS& toolkit);
        virtual ~ZONE();
        void set_toolkit(iSTEPS& toolkit);
        iSTEPS& get_toolkit() const;

        void set_zone_number(size_t zone_number);
        void set_zone_name(string zone_name);

        size_t get_zone_number() const;
        string get_zone_name() const;

        bool is_valid() const;
        void check();
        void clear();
        void report() const;
        virtual ZONE& operator=(const ZONE& zone);
    private:
        iSTEPS* toolkit;

        size_t zone_number;
        string zone_name;
};
#endif // ZONE_H
