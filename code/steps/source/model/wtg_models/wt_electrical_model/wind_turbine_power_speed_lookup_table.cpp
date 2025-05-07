#include "header/model/wtg_models/wt_electrical_model/wind_turbine_power_speed_lookup_table.h"
#include "header/basic/utility.h"
WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE()
{
    clear();
}

WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::~WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE()
{
    ;
}

WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE(const WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE& table)
{
    copy_from_const_table(table);
}

WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE& WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::operator=(const WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE& table)
{
    if(this==(&table)) return *this;

    copy_from_const_table(table);

    return *this;
}

void WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::copy_from_const_table(const WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE& table)
{
    clear();
    vector<vector<double> > old_table = table.get_wind_turbine_power_speed_table();
    for(size_t i=0; i<STEPS_MAX_WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE_SIZE; ++i)
    {
        power_speed_table[i][0] = old_table[i][0];
        power_speed_table[i][1] = old_table[i][1];
    }
}

void WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::clear()
{
    for(size_t i=0; i<STEPS_MAX_WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE_SIZE; ++i)
    {
        power_speed_table[i][0] = 0.0;
        power_speed_table[i][1] = 0.0;
    }
}

void WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::add_wind_turbine_power_and_speed_pair_in_pu(double power, double speed)
{
    size_t n = get_valid_record_size();
    if(n>=STEPS_MAX_WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE_SIZE)
    {
        ostringstream osstream;
        osstream<<"Warning. Wind turbine power-speed lookup table is full (size "<<STEPS_MAX_WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE_SIZE<<").\n"
                <<"No more power-speed record will be added.";
        show_information_with_leading_time_stamp_with_default_toolkit(osstream);
        return;
    }

    if(is_power_record_exist(power))
    {
        ostringstream osstream;
        osstream<<"Warning. Duplicate wind power-speed record is identified with the same power "<<power<<" MW.\n"
                <<"Input power-speed record ("<<power<<" MW, "<<speed<<") will not be added.";
        show_information_with_leading_time_stamp_with_default_toolkit(osstream);
        return;
    }

    power_speed_table[n][0] = power;
    power_speed_table[n][1] = speed;

    reorder_records();
}

size_t WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::get_valid_record_size() const
{
    size_t valid_record_count = STEPS_MAX_WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE_SIZE;
    for(size_t i=0; i<STEPS_MAX_WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE_SIZE; ++i)
    {
        if(power_speed_table[i][0]==0.0)
        {
            valid_record_count = i;
            break;
        }
    }
    return valid_record_count;
}


bool WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::is_power_record_exist(double power) const
{
    size_t n = get_valid_record_size();
    bool record_exist = false;
    for(size_t i=0; i<n; ++i)
    {
        if(fabs(power_speed_table[i][0]-power)<DOUBLE_EPSILON)
        {
            record_exist = true;
            break;
        }
    }
    return record_exist;
}

void WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::reorder_records()
{
    size_t n = get_valid_record_size();
    if(n<=1)
        return;
    else
    {
        for(size_t k=0; k<n-1; ++k)
        {
            bool swapped = false;
            for(size_t i=0; i<n-1; ++i)
            {
                if(power_speed_table[i][0]>power_speed_table[i+1][0])
                {
                    double temp_power = power_speed_table[i][0];
                    double temp_speed = power_speed_table[i][1];
                    power_speed_table[i][0] = power_speed_table[i+1][0];
                    power_speed_table[i][1] = power_speed_table[i+1][1];
                    power_speed_table[i+1][0] = temp_power;
                    power_speed_table[i+1][1] = temp_speed;
                    swapped = true;
                }
            }
            if(swapped==false)
                break;
        }
    }
}

double WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::get_reference_speed_with_power_in_pu(double power)
{
    size_t n = get_valid_record_size();
    if(n<2)
        return 0.0;

    size_t i_less = get_the_last_record_with_power_less_than_or_equal_to(power);
    if(i_less==INDEX_NOT_EXIST)
        return get_speed_of_record(0);
    if(i_less == n-1)
        return get_speed_of_record(n-1);

    size_t i_greater = i_less + 1;

    double power_less = get_power_of_record(i_less);
    double speed_less = get_speed_of_record(i_less);
    double power_greater = get_power_of_record(i_greater);
    double speed_greater = get_speed_of_record(i_greater);

    double slope = (speed_greater-speed_less)/(power_greater-power_less);
    return speed_less + slope*(power-power_less);
}


size_t WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::get_the_last_record_with_power_less_than_or_equal_to(double power)
{
    size_t n = get_valid_record_size();
    size_t index = INDEX_NOT_EXIST;
    for(size_t i=0; i<n; ++i)
    {
        if(get_power_of_record(i)<=power)
            index = i;
        else
            break;
    }
    return index;
}

const vector<vector<double> > WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::get_wind_turbine_power_speed_table() const
{
    vector< vector<double> > table;
    table.reserve(STEPS_MAX_WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE_SIZE);
    for(size_t i=0; i<STEPS_MAX_WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE_SIZE; ++i)
    {
        vector<double> record;
        record.push_back(power_speed_table[i][0]);
        record.push_back(power_speed_table[i][1]);
        table.push_back(record);
    }
    return table;
}

double WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::get_power_of_record(size_t i) const
{
    if(i<STEPS_MAX_WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE_SIZE)
        return power_speed_table[i][0];
    else
        return 0.0;
}
double WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE::get_speed_of_record(size_t i) const
{
    if(i<STEPS_MAX_WIND_TURBINE_POWER_SPEED_LOOKUP_TABLE_SIZE)
        return power_speed_table[i][1];
    else
        return 0.0;
}
