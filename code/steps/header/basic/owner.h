#ifndef OWNER_H
#define OWNER_H

#include <string>
using namespace std;

class iSTEPS;

class OWNER
{
    public:
        OWNER(iSTEPS& toolkit);
        virtual ~OWNER();
        void set_toolkit(iSTEPS& toolkit);
        iSTEPS& get_toolkit() const;

        void set_owner_number(size_t owner_number);
        void set_owner_name(string owner_name);

        size_t get_owner_number() const;
        string get_owner_name() const;

        bool is_valid() const;
        void check();
        void clear();
        void report() const;
        virtual OWNER& operator=(const OWNER& owner);
    private:
        iSTEPS* toolkit;

        size_t owner_number;
        string owner_name;
};
#endif // OWNER_H
