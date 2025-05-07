#ifndef BASE_H
#define BASE_H
#include <string>

class PF_DATA;
class iSTEPS;

using namespace std;

class BASE
{
    public:
        BASE();
        virtual ~BASE();

        void set_toolkit(iSTEPS& toolkit);
        iSTEPS& get_toolkit(const string& calling_function) const;
        bool is_toolkit_set() const;

        virtual bool is_valid() const = 0;
        virtual void check() = 0;
        virtual void clear()  = 0;
    private:
        iSTEPS* toolkit;
};

#endif // BASE_H
