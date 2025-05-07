#include "header/basic/terminal.h"
#include "header/basic/utility.h"
#include "header/basic/exception.h"

#include <istream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdio>

using namespace std;

TERMINAL::TERMINAL()
{
    buses.clear();
}

TERMINAL::~TERMINAL()
{
}

void TERMINAL::append_bus(size_t bus)
{
    if(bus!=0 and (not has_bus(bus)))
        append_and_sort_buses(bus);
    else
    {
        if(has_bus(bus))
        {
            ostringstream osstream;
            osstream<<"Duplicate bus is not allowed when appending bus "<<bus<<" to terminal.";
            show_information_with_leading_time_stamp_with_default_toolkit(osstream);
        }
    }
}

bool TERMINAL::has_bus(size_t bus)
{
    bool exist_flag = false;
    size_t n = get_bus_count();
    for(size_t i=0; i!=n; ++i)
    {
        if(buses[i]==bus)
        {
            exist_flag = true;
            break;
        }
    }
    return exist_flag;
}

void TERMINAL::append_and_sort_buses(size_t bus)
{
    buses.push_back(bus);
    sort_bus_in_ascending_order();
}

void TERMINAL::sort_bus_in_ascending_order()
{
    sort(buses.begin(), buses.end());
}

size_t TERMINAL::get_bus_count() const
{
    return buses.size();
}

vector<size_t> TERMINAL::get_buses()
{
    return buses;
}

void TERMINAL::clear()
{
    buses.clear();
}

string TERMINAL::convert2string() const
{
    size_t n = get_bus_count();
    if(n==0)
        return "()";
    else
    {
        string terminal_string = "("+num2str(buses[0]);
        for(size_t i=1; i<n; ++i)
            terminal_string += ","+num2str(buses[i]);
        terminal_string +=")";
        return terminal_string;
    }
}

TERMINAL& TERMINAL::operator= (const TERMINAL& terminal)
{
    if(this==(&terminal)) return *this;

    this->clear();

    size_t n = terminal.get_bus_count();
    for(size_t i=0; i!=n; ++i)
    {
        append_bus(terminal[i]);
    }
    return *this;
}

size_t TERMINAL::operator[] (size_t index) const
{
    if(index<get_bus_count())
        return buses[index];
    else
        return 0;
}
