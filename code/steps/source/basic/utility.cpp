#include "header/basic/utility.h"
#include "header/basic/constants.h"
#include "header/steps_namespace.h"

#include <cstdio>
#include <algorithm>
//#include <cmath>
#include <istream>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <omp.h>

using namespace std;

string num2str(int number)
{
    char str[1000];
    snprintf(str,1000, "%d",number);
    return string(str);
}

string num2str(size_t number)
{
    return num2str(int(number));
}

string num2str(double number)
{
    char str[1000];
    snprintf(str,100, "%G",number);
    return string(str);
}

string num2hex_str(int number)
{
    char str[64];
    sprintf(str, "%X", number);
    return string(str);
}

string num2hex_str(size_t number)
{
    return num2hex_str(int(number));
}

//string num2hex_str(size_t number)
//{
//    return num2hex_str(int(number));
//}

string num2hex_str(double number)
{
    char str[64];
    sprintf(str, "%A", number);
    return string(str);
}

int str2int(const string& str)
{
    return int(round(str2double(str)));
}

double str2double(const string& str)
{
    double num = 0.0;
    sscanf(str.c_str(),"%lf", &num);
    return num;
}

int get_integer_data(const string& strval, const string& strdefault)
{
    if(strval.size()!=0)
        return str2int(strval);
    else
        return str2int(strdefault);
}

double get_double_data(const string& strval, const string& strdefault)
{
    if(strval.size()!=0)
        return str2double(strval);
    else
        return str2double(strdefault);
}

string get_string_data(string strval, const string& strdefault)
{
    if(strval.size()==0) strval=strdefault;
    while(true)
    {
        size_t found = strval.find_first_of("\'\"");
        if(found==string::npos) break;
        strval.erase(found,1);
    }
    strval =  trim_string(strval);
    if(strval!="")
    {
        return strval;
    }
    else
    {
        return strdefault;
    }
}

size_t get_sparse_matrix_identity(const SPARSE_MATRIX& matrix)
{
    int n = matrix.get_matrix_entry_count();
    int identity = 0;
    for(int i=0; i<n; ++i)
    {
        int row = matrix.get_row_number_of_entry_index(i);
        int col = matrix.get_column_number_of_entry_index(i);
        complex<double> value = matrix.get_entry_value(i);
        int rvalue = int(value.real()*1000.0)<<10;
        int ivalue = int(value.imag()*1000.0)<<10;
        row = row^STEPS_MAGIC1;
        col = col^STEPS_MAGIC2;
        rvalue = rvalue^STEPS_MAGIC3;
        ivalue = ivalue^STEPS_MAGIC4;

        identity = identity^((row^col)^(rvalue^ivalue));
    }
    return (size_t)(identity);
}

size_t get_vector_identity(const vector<double>& vec)
{
    int n = vec.size();
    int identity = 0;
    for(int i=0; i<n; ++i)
    {
        int index = i;
        int value = int(vec[i]*10000.0)<<10;
        index = index^STEPS_MAGIC1;
        value = value^STEPS_MAGIC2;

        identity = identity^(index^value);
    }
    return (size_t)(identity);
}


string string2upper(string str)
{
    transform(str.begin(),str.end(),str.begin(),::toupper);
    return str;
}

double rad2deg(double angle)
{
    return angle*ONE_EIGHTY_OVER_PI;
}

double deg2rad(double angle)
{
    return angle*PI_OVER_180;
}

double radps2hz(double w)
{
    return w*ONE_OVER_DOUBLE_PI;
}

double hz2radps(double f)
{
    return DOUBLE_PI*f;
}

bool is_nan(double& x)
{
    return x!=x;
}

double round_angle_in_rad_to_PI(double angle)
{
    double abs_angle = fabs(angle);
    if(abs_angle>PI)
        return (angle>0.0?1.0:-1.0)*(abs_angle - ceil((abs_angle-PI)*ONE_OVER_DOUBLE_PI)*DOUBLE_PI);
    else
        return angle;
}

double round_angle_in_rad_to_HALF_PI(double angle)
{
    double abs_angle = fabs(angle);
    if(abs_angle>HALF_PI)
        return (angle>0.0?1.0:-1.0)*(abs_angle - ceil((abs_angle-HALF_PI)*ONE_OVER_PI)*PI);
    else
        return angle;
}

double steps_fast_complex_abs(const complex<double>& z)
{
    double x = z.real();
    double y = z.imag();
    return steps_sqrt(x*x+y*y);
}

double steps_fast_complex_arg(const complex<double>& z)
{
    double x = z.real();
    double y = z.imag();

    if(x != 0.0 and y != 0.0)
    {
        double angle = steps_atan(y / x);
        if (x < 0.0)
        {
            if (y > 0.0)
                angle += PI;
            else
                angle -= PI;
        }
        return angle;
    }
    else
    {
        if (x == 0.0 and y != 0.0)
        {
            if (y > 0)
                return HALF_PI;
            else
            {
                if (y < 0.0)
                    return -HALF_PI;
                else
                    return 0.0;
            }
        }
        else
        {
            if (y == 0.0 and x != 0.0)
            {
                if (x > 0.0)
                    return 0.0;
                else
                {
                    if (x < 0.0)
                        return PI;
                    else
                        return 0.0;
                }
            }
            else//x == 0.0 and y == 0.0
                return 0.0;
        }
    }
}

double steps_pow(double base, double exp)
{
    //return pow(base, exp);
    if(exp==0.0)
        return 1.0;
    else
    {
        if(exp==1.0)
            return base;
        else
        {
            if(exp==2.0)
                return base*base;
            else
            {
                if(exp==3.0)
                    return base*base*base;
                else
                    return pow(base, exp);
            }
        }
    }
}

double steps_sin(double angle_in_rad)
{
    if(use_fast_math==true)
        return steps_fast_sin(angle_in_rad);
    else
        return sin(angle_in_rad);
}

double steps_fast_sin(double angle_in_rad)
{
    double x = angle_in_rad;
    x = round_angle_in_rad_to_PI(x);
    if(x>HALF_PI)
        x = PI-x;
    else if(x<-HALF_PI)
        x = -PI - x;

    double x2 = x*x;
    double x3 = x*x2;
    double x5 = x3*x2;
    double x7 = x5*x2;
    double x9 = x7*x2;
    double x11 = x9*x2;
    double x13 = x11*x2;
    double x15 = x13*x2;

    return x-0.1666666666666666666666666666667*x3
           +8.3333333333333333333333333333333e-3*x5
           -1.984126984126984126984126984127e-4*x7
           +2.7557319223985890652557319223986e-6*x9
           -2.5052108385441718775052108385442e-8*x11
           +1.6059043836821614599392377170155e-10*x13
           -7.6471637318198164759011319857881e-13*x15;
}

double steps_cos(double angle_in_rad)
{
    if(use_fast_math==true)
        return steps_fast_cos(angle_in_rad);
    else
        return cos(angle_in_rad);
}

double steps_fast_cos(double angle_in_rad)
{
    angle_in_rad += HALF_PI;
    return steps_fast_sin(angle_in_rad);
}

double steps_tan(double angle_in_rad)
{
    if(use_fast_math==true)
        return steps_fast_tan(angle_in_rad);
    else
        return tan(angle_in_rad);
}

double steps_fast_tan(double angle_in_rad)
{
    return tan(angle_in_rad);
    angle_in_rad = round_angle_in_rad_to_PI(angle_in_rad);
    double sin = steps_fast_sin(angle_in_rad);
    double cos = steps_fast_cos(angle_in_rad);
    if(cos!=0.0)
        return sin/cos;
    else
    {
        cout<<"Fatal error of steps_fast_tan(). Inf found.\n";
        return INFINITE_THRESHOLD;
    }
}

double steps_asin(double x)
{
    if(use_fast_math==true)
        return steps_fast_asin(x);
    else
        return asin(x);
}

double steps_fast_asin(double x)
{
    return asin(x);

    double y = fabs(x);
    if(y<1.0)
    {
        // the following atan function is built based on:
        // Approximation Eq. 4.4.46 on P81 of "Handbook of Mathematical Functions", by Milton Abramowitz and Irene A. Stegun, 1970
        double y2 = y*y;
        double y3 = y2*y;
        double y4 = y3*y;
        double y5 = y4*y;
        double y6 = y5*y;
        double y7 = y6*y;

        double z = HALF_PI - steps_sqrt(1.0-y)*(1.5707963050+
                                                -0.2145988016*y
                                                +0.0889789874*y2
                                                -0.0501743046*y3
                                                +0.0308918810*y4
                                                -0.0170881256*y5
                                                +0.0066700901*y6
                                                -0.0012624911*y7
                                               );
        return (x>0?1:-1)*z;
    }
    else
    {
        if(x>0.0)
            return HALF_PI;
        else
            return -HALF_PI;
    }
}

double steps_acos(double x)
{
    if(use_fast_math==true)
        return steps_fast_acos(x);
    else
        return acos(x);
}

double steps_fast_acos(double x)
{
    return acos(x);
    //return HALF_PI-steps_fast_asin(x);
}

double steps_atan(double x)
{
    if(use_fast_math==true)
        return steps_fast_atan(x);
    else
        return atan(x);
}

double steps_fast_atan(double x)
{
    if(x>-0.25 and x<0.25)
    {
        //user polynomial with maximum error <6.284e-13
        double x2 = x*x;
        double x3 = x2*x;
        double x5 = x3*x2;
        double x7 = x5*x2;
        double x9 = x7*x2;
        double x11 = x9*x2;

        return 0.9999999999909259*x
               -0.33333332897282747*x3
               +0.19999940506845645*x5
               -0.14282246141361374*x7
               +0.11012658805577266*x9
               -0.07731634628276322*x11;
    }
    else
        return atan(x);

    /*
    //user polynomial with maximum error <0.4e-11
    double y = fabs(x);
    if(y>1.0) y = 1.0/y;

    double y2 = y*y;
    double y3 = y2*y;
    double y5 = y3*y2;
    double y7 = y5*y2;
    double y9 = y7*y2;
    double y11 = y9*y2;
    double y13 = y11*y2;
    double y15 = y13*y2;
    double y17 = y15*y2;

    double z =   0.9999999999018119*y
                -0.3333333167717403*y3
                +0.1999991076162263*y5
                -0.14283152496616747*y7
                +0.11056848230911817*y9
                -0.07513116523199578*y11
                -0.07802246093181216*y7*y5 //y12
                +0.2699603659514348*y13
                -0.27810373457929133*y7*y7 //y14
                +0.15078314291241518*y15
                -0.04398393524366868*y9*y7 //y16
                +0.005493202428206681*y17;

    if(fabs(x)>1.0) z = HALF_PI-z;
    return (x>=0?z:-z);
    */

    /*
    // the following atan function is built based on:
    // 1. Approximation Eq. 4.4.49 on P81 of "Handbook of Mathematical Functions", by Milton Abramowitz and Irene A. Stegun, 1970
    // 2. Identity Eq. 4.45.8 arctan(x)=2*arctan(y) if y=x/(1+sqrt(1+x*x)), on https://dlmf.nist.gov/4.45, accessed on Feb. 9, 2020

    double y = fabs(x);
    if(y>1.0) y = 1.0/y;

    bool scaled = false;
    if(y>0.5)
    {
        y = y/(1.0+steps_sqrt(1.0+y*y));
        scaled = true;
    }

    double y2 = y*y;
    double y4 = y2*y2;
    double y6 = y4*y2;
    double y8 = y6*y2;
    double y10 = y8*y2;
    double y12 = y10*y2;
    double y14 = y12*y2;
    double y16 = y14*y2;

    double z = 1.0-0.3333314528*y2
                  +0.1999355085*y4
                  -0.1420889944*y6
                  +0.1065626393*y8
                  -0.0752896400*y10
                  +0.0429096138*y12
                  -0.0161657367*y14
                  +0.0028662257*y16;
    z *= y;

    if(scaled==true)
        z += z;

    if(fabs(x)>1) z = HALF_PI-z;
    return (x>=0?z:-z);
    */

}

double steps_sqrt(double x)
{
    return sqrt(x);
    if(use_fast_math==true)
        return steps_fast_sqrt(x);
    else
        return sqrt(x);
}

double steps_fast_sqrt(double x)
{
    return sqrt(x);
    //return x*quick_inv_sqrt_Lomont(x);
}

double steps_inv_sqrt(double x)
{
    if(use_fast_math==true)
        return steps_fast_inv_sqrt(x);
    else
        return 1.0/sqrt(x);
}

double steps_fast_inv_sqrt(double x)
{
    return quick_inv_sqrt_Lomont(x);
}

float quick_inv_sqrt_Quake3(float x)
{
    float xhalf = 0.5f*x;
    union
    {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = 0x5f3759df- (u.i>>1); // gives initial guess y0
    for(size_t i=0; i<5; ++i)
        u.x *= (1.5f-xhalf*u.x*u.x); // Newton step, repeating increases accuracy
    return u.x;
}

float quick_inv_sqrt_Lomont(float x)
{
    float xhalf = 0.5f*x;
    union
    {
        float x;
        int i;
    } u;
    u.x = x;
    u.i = 0x5f375a86- (u.i>>1); // gives initial guess y0
    for(size_t i=0; i<10; ++i)
        u.x *= (1.5f-xhalf*u.x*u.x); // Newton step, repeating increases accuracy
    return u.x;
}

double quick_double_inv_sqrt_Lomont(double x)
{
    double xhalf = 0.5F*x;
    union
    {
        double x;
        long i;
    } u;
    u.x = x;
    u.i = 0x5fe6ec85e7de30da- (u.i>>1); // gives initial guess y0
    for(size_t i=0; i<10; ++i)
        u.x *= (1.5F-xhalf*u.x*u.x); // Newton step, repeating increases accuracy
    return u.x;
}

string trim_string(string str, const string& garbage)
{
    if(not str.empty())
    {
        if(garbage=="")
        {
            str.erase(0,str.find_first_not_of(" \t\n\r"));
            str.erase(str.find_last_not_of(" \t\n\r")+1);
        }
        else
        {
            str.erase(0,str.find_first_not_of(garbage));
            str.erase(str.find_last_not_of(garbage)+1);
        }
    }
    return str;
}

string replace_string_contents(string str, const string& source, const string& destination)
{
    size_t index = destination.find(source);
    if(index==string::npos)
    {
        while(true)
        {
            size_t ind = str.find(source);
            if(ind!=string::npos)
                str = str.replace(ind, source.size(), destination);
            else
                break;
        }
    }
    return str;
}

string shrink_sucessive_blanks(string str)
{
    str = replace_string_contents(str, "\t", " ");
    str = replace_string_contents(str,"  ", " ");
    return str;
}

string string2csv(string str)
{
    str = trim_string(str);
    str = shrink_sucessive_blanks(str);
    str = replace_string_contents(str, "'", "\"");
    str = replace_string_contents(str, ", ", ",");
    str = replace_string_contents(str, " ,", ",");

    size_t n = str.size();
    size_t n2 = n<<1;

    char* csv = (char*) malloc(sizeof(char)*n2);
    if(csv!=NULL)
    {
        for(size_t i=0; i!=n2; ++i)
            csv[i]='\0';

        size_t ncsv = 0;
        for(size_t i=0; i!=n; ++i)
        {
            char source = str[i];
            if(source=='"')
            {
                csv[ncsv] = source;
                ++ncsv;
                for(++i; i!=n; ++i)
                {
                    source=str[i];
                    if(source=='"')
                    {
                        csv[ncsv] = source;
                        ++ncsv;
                        break;
                    }
                    else
                    {
                        csv[ncsv] = source;
                        ++ncsv;
                    }
                }
                continue;
            }
            else
            {
                if(source==',' or source==' ')
                {
                    csv[ncsv]=',';
                    ++ncsv;
                    continue;
                }
                else
                {
                    csv[ncsv] = source;
                    ++ncsv;
                    continue;
                }
            }
        }

        string newstr(csv);
        free(csv);
        return newstr;
    }
    else
    {
        cout<<"Warning. Failed to allocate memory for "<<__FUNCTION__<<"().\n";
        return "";
    }
}

vector<string> split_string(string str, const string& sep)
{
    vector<string> splitted_str;
    str = trim_string(str);
    size_t newline_index = 0;
    while(not str.empty())
    {
        newline_index = str.find_first_of(sep);
        if(newline_index != string::npos)
        {
            splitted_str.push_back(str.substr(0, newline_index));
            str.erase(0, newline_index+1);
        }
        else
        {
            splitted_str.push_back(str);
            str.clear();
        }
    }

    size_t n_size = splitted_str.size();
    for(size_t i = 0; i!=n_size; ++i)
        splitted_str[i] = trim_string(splitted_str[i]);

    return splitted_str;
}

string string_vector2csv(const vector<string>& vec)
{
    string str="";
    size_t n = vec.size();
    if(n!=0)
    {
        str = vec[0];
        for(size_t i=1; i<n; ++i)
            str = str+", "+vec[i];
    }
    return str;
}

string swap_data_in_csv_string(const string& data, size_t i, size_t j)
{
    vector<string> record = split_string(data,",");
    size_t n = record.size();
    if(i<=n and j<=n)
    {
        string temp = record[i];
        record[i]=record[j];
        record[j]=temp;
        return string_vector2csv(record);
    }
    else
        return data;
}

complex<double> xy2dq_with_angle_in_deg(const complex<double>& V, double angle)
{
    angle = deg2rad(angle);
    return xy2dq_with_angle_in_rad(V, angle);
}

complex<double> xy2dq_with_angle_in_rad(const complex<double>& V, double angle)
{
    return V*get_xy2dq_rotation_with_angle_in_rad(angle);
}

complex<double> dq2xy_with_angle_in_deg(const complex<double>& V, double angle)
{
    angle = deg2rad(angle);
    return dq2xy_with_angle_in_rad(V, angle);
}

complex<double> dq2xy_with_angle_in_rad(const complex<double>& V, double angle)
{
    return V*get_dq2xy_rotation_with_angle_in_rad(angle);
}

complex<double> get_xy2dq_rotation_with_angle_in_deg(double angle)
{
    // (Vx+jVy)*(sin+jcos) =(Vx*sin-Vy*cos)+j(Vx*cos+Vy*sin)
    return get_xy2dq_rotation_with_angle_in_rad(deg2rad(angle));
}

complex<double> get_xy2dq_rotation_with_angle_in_rad(double angle)
{
    // (Vx+jVy)*(sin+jcos) =(Vx*sin-Vy*cos)+j(Vx*cos+Vy*sin)
    return complex<double> (steps_sin(angle), steps_cos(angle));
}

complex<double> get_dq2xy_rotation_with_angle_in_deg(double angle)
{
    // (Vd+jVq)*(sin-jcos) =(Vd*sin+Vq*cos)+j(Vq*sin-Vd*cos)
    return get_dq2xy_rotation_with_angle_in_rad(deg2rad(angle));
}

complex<double> get_dq2xy_rotation_with_angle_in_rad(double angle)
{
    // (Vd+jVq)*(sin-jcos) =(Vd*sin+Vq*cos)+j(Vq*sin-Vd*cos)
    return complex<double> (steps_sin(angle), -steps_cos(angle));
}

complex<double> ab2dq_with_angle_in_deg(const complex<double>& V, double angle)
{
    angle = deg2rad(angle);
    return ab2dq_with_angle_in_rad(V, angle);
}

complex<double> ab2dq_with_angle_in_rad(const complex<double>& V, double angle)
{
    return V*get_ab2dq_rotation_with_angle_in_rad(angle);
}

complex<double> dq2ab_with_angle_in_deg(const complex<double>& V, double angle)
{
    angle = deg2rad(angle);
    return dq2ab_with_angle_in_rad(V, angle);
}

complex<double> dq2ab_with_angle_in_rad(const complex<double>& V, double angle)
{
    return V*get_dq2ab_rotation_with_angle_in_rad(angle);
}

complex<double> get_ab2dq_rotation_with_angle_in_deg(double angle)
{
    // (Va+jVb)*(cos-jsin) =(Va*cos+Vb*sin)+j(-Va*sin+Vb*cos)
    return get_ab2dq_rotation_with_angle_in_rad(deg2rad(angle));
}

complex<double> get_ab2dq_rotation_with_angle_in_rad(double angle)
{
    // (Va+jVb)*(cos-jsin) =(Va*cos+Vb*sin)+j(-Va*sin+Vb*cos)
    return complex<double> (steps_cos(angle), -steps_sin(angle));
}

complex<double> get_dq2ab_rotation_with_angle_in_deg(double angle)
{
    // (Vd+jVq)*(cos+jsin) =(Vd*cos-Vq*sin)+j(Vd*sin+Vq*cos)
    return get_dq2ab_rotation_with_angle_in_rad(deg2rad(angle));
}

complex<double> get_dq2ab_rotation_with_angle_in_rad(double angle)
{
    // (Vd+jVq)*(cos+jsin) =(Vd*cos-Vq*sin)+j(Vd*sin+Vq*cos)
    return complex<double> (steps_cos(angle), steps_sin(angle));
}

COMPLEX3 steps_abc2seq(const COMPLEX3& Vabc)
{
    COMPLEX3 Vseq;
    return Vseq;
}
COMPLEX3 steps_seq2abc(const COMPLEX3& V012)
{
    COMPLEX3 Vabc;
    complex<double> F0 = V012[0];
    complex<double> F1 = V012[1];
    complex<double> F2 = V012[2];

    Vabc.set_complex_value_0(F1 + F2 + F0);
    Vabc.set_complex_value_1(complex<double>(-0.5, -SQRT3/2)*F1 + complex<double>(-0.5,  SQRT3/2)*F2 + F0);
    Vabc.set_complex_value_2(complex<double>(-0.5,  SQRT3/2)*F1 + complex<double>(-0.5, -SQRT3/2)*F2 + F0);

    return Vabc;
}

bool is_file_exist(const string& file)
{
    // check if the file exist
    // date: Sep. 1, 2016
    FILE* fid = fopen(file.c_str(),"r");
    if(fid!=NULL)
    {
        fclose(fid);
        return true;
    }
    else
    {
        return false;
    }
}


void show_information_with_leading_time_stamp_with_default_toolkit(const string& info)
{
    iSTEPS& toolkit = get_default_toolkit();
    toolkit.show_information(info);
}

void show_information_with_leading_time_stamp_with_default_toolkit(ostringstream& stream)
{
    iSTEPS& toolkit = get_default_toolkit();
    toolkit.show_information(stream);
}

void show_test_information_for_function_of_class(const string& func, const string& cls)
{
    ostringstream osstream;
    osstream<<"--s--t--a--r--t-----------------------------------------------------\n"
            <<"Run testing of "<<cls<<"::"<<func;
    show_information_with_leading_time_stamp_with_default_toolkit(osstream);
}

void show_test_end_information()
{
    ostringstream osstream;
    osstream<<"--d--o--n--e-------------------------------------------------------";
    show_information_with_leading_time_stamp_with_default_toolkit(osstream);
}

vector<string> psse_dyr_string2steps_string_vector(const string& data)
{
    vector<string> record = split_string(data,",");
    string temp = record[0];
    record[0]=record[1];
    record[1]=temp;
    return record;
}

string psse_dyr_string2steps_string(const string& data)
{
    return swap_data_in_csv_string(data, 1, 2);
}


void initialize_package()
{
    return;
    default_toolkit.set_name("TK DFLT");
    for(size_t i=0; i<MAX_TOOLKIT_SIZE; ++i)
        toolkits[i] = NULL;
}

size_t generate_new_toolkit(string log_file)
{
    mtx.lock();
    /*while(get_toolkit_count()>=MAX_TOOLKIT_SIZE)
    {
        ostringstream osstream;
        osstream<<"Warning. Toolkit table is full when calling "<<__FUNCTION__<<"().\nTry to generate new toolkit in 5 sec.\n";

        show_information_with_leading_time_stamp_with_default_toolkit(osstream);

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }*/

    ostringstream osstream;
    size_t index=INDEX_NOT_EXIST;
    while(true)
    {
        bool toolkit_index_is_set = false;
        for(size_t i=0; i!=MAX_TOOLKIT_SIZE; ++i)
        {
            if(toolkits[i]==NULL)
            {
                index = i;
                //osstream<<"NULL toolkit is found at "<<i<<endl;
                //show_information_with_leading_time_stamp_with_default_toolkit(osstream);
                osstream<<"TK "<<setfill('0')<<setw(4)<<num2str(index);
                toolkits[index] = new iSTEPS(osstream.str(), log_file);
                toolkit_index_is_set = true;
                break;
            }

        }

        if(toolkit_index_is_set==true)
            break;
    }
    mtx.unlock();
    return index;
}

void delete_toolkit(size_t toolkit_index)
{
    mtx.lock();
    if(toolkit_index<MAX_TOOLKIT_SIZE)
    {
        if(toolkits[toolkit_index]!=NULL)
        {
            delete toolkits[toolkit_index];
            toolkits[toolkit_index] = NULL;
        }
    }
    mtx.unlock();
}

size_t get_toolkit_count()
{
    size_t count = 0;
    for(size_t i=0; i!=MAX_TOOLKIT_SIZE; ++i)
    {
        if(toolkits[i]!=NULL)
            ++count;
    }
    return count;
}

iSTEPS& get_default_toolkit()
{
    return default_toolkit;
}

iSTEPS& get_toolkit(size_t toolkit_index)
{
    if(toolkit_index==INDEX_NOT_EXIST)
        return default_toolkit;

    if(toolkit_index<MAX_TOOLKIT_SIZE and toolkits[toolkit_index]!=NULL)
    {

        return *(toolkits[toolkit_index]);
    }
    else
    {
        ostringstream osstream;
        osstream<<"Fatal Error. The toolkit_index is either invalid or toolkit is NULL when calling "<<__FUNCTION__<<"(). toolkit_index is: "<<toolkit_index<<"\n"
                <<"Default toolkit will be returned. However, no further simulation is guaranteed.";
        show_information_with_leading_time_stamp_with_default_toolkit(osstream);
        return default_toolkit;
    }
}

void add_string_to_str_int_map(const string& str)
{
    toolkit_str_int_map.add_new_string(str);
}

size_t get_index_of_string(const string& str)
{
    return toolkit_str_int_map[str];
}

string get_string_of_index(size_t index)
{
    return toolkit_str_int_map[index];
}

string device_type2string(STEPS_DEVICE_TYPE device_type)
{
    switch(device_type)
    {
    case STEPS_BUS:
        return "BUS";
    case STEPS_GENERATOR:
        return "GENERATOR";
    case STEPS_WT_GENERATOR:
        return "WT GENERATOR";
    case STEPS_PV_UNIT:
        return "PV UNIT";
    case STEPS_ENERGY_STORAGE:
        return "ENERGY STORAGE";
    case STEPS_LOAD:
        return "LOAD";
    case STEPS_FIXED_SHUNT:
        return "FIXED SHUNT";
    case STEPS_SWITCHED_SHUNT:
        return "SWITCHED SHUNT";
    case STEPS_EQUIVALENT_DEVICE:
        return "EQUIVALENT DEVICE";
    case STEPS_AC_LINE:
        return "AC LINE";
    case STEPS_TRANSFORMER:
        return "TRANSFORMER";
    case STEPS_LCC_HVDC2T:
        return "2T LCC HVDC";
    case STEPS_VSC_HVDC:
        return "VSC HVDC";
    case STEPS_FACTS:
        return "FACTS";
    case STEPS_LCC_HVDC:
        return "LCC HVDC";
    case STEPS_HYBRID_DC:
        return "HYBRID DC";
    case STEPS_GENERAL_DEVICE:
        return "GENERAL DEVICE";
    case STEPS_INVALID_DEVICE:
        return "INVALID DEVICE TYPE";
    default:
        return "UNNAMED DEVICE TYPE OF NUMBER "+num2str(device_type);
    }
}

string dc_device_type2string(STEPS_DC_DEVICE_TYPE device_type)
{
    switch(device_type)
    {
    case STEPS_DC_BUS:
        return "DC BUS";
    case STEPS_DC_LINE:
        return "DC LINE";
    case STEPS_GENERAL_DC_DEVICE:
        return "GENERAL DC DEVICE";
    case STEPS_INVALID_DC_DEVICE:
        return "INVALID DC DEVICE TYPE";
    default:
        return "UNNAMED DC DEVICE TYPE OF NUMBER "+num2str(device_type);
    }
}

void set_openmp_number_of_threads(size_t n)
{
    omp_set_num_threads(n);
}

float* roots_quadratic_equation(float a, float b, float c)
{
    //the first element is the number of the real roots, and other elements are the real roots.
    float *roots = new float[3];
    if (a == 0.0)
    {
        if (b == 0.0)
        {
            roots[0] = 0.0;
        }
        else
        {
            roots[1] = -c/b;
            roots[0] = 1.0;
        }
    }
    else
    {
        float d = b*b - 4*a*c;
        if (d < 0.0)
        {
            roots[0] = 0.0;
        }
        else
        {
            roots[1] = (-b + sqrt(d)) / (2*a);
            roots[2] = (-b - sqrt(d)) / (2*a);
            roots[0] = 2.0;
        }
    }
    return roots;
}

float* roots_cubic_equation(float a, float b, float c, float d)
{
    //the first element is the number of the real roots, and other elements are the real roots.
    //Shengjin's formula
    float *roots = new float[4];
    if (a == 0)
    {
        roots = roots_quadratic_equation(b, c, d);
    }
    else
    {
        float A = b*b - 3*a*c;
        float B = b*c - 9*a*d;
        float C = c*c - 3*b*d;
        float deita = B*B - 4*A*C;

        if ((A == B) && (A == 0))
        {
            //the three roots are the same
            if (a != 0)
            {
                roots[1] = -b/(3*a);
            }
            else
            {
                if (b != 0)
                {
                    roots[1] = -c/b;
                }
                else
                {
                    if (c != 0)
                    {
                        roots[1] = -3*d/c;
                    }
                }
            }
            roots[2] = roots[1];
            roots[3] = roots[1];
            roots[0] = 3;
        }
        else if (deita > 0)
        {
            //only one real root
            float y1 = A*b + (3*a/2)*(-B + sqrt(deita));
            float y2 = A*b + (3*a/2)*(-B - sqrt(deita));
            float pow_y1, pow_y2;
            if (y1 < 0)
            {
                //for pow(a,b), when b is not int, a should not be negative.
                pow_y1 = - pow(-y1, 1.0/3.0);
            }
            else
            {
                pow_y1 = pow(y1, 1.0/3.0);
            }
            if (y2 < 0)
            {
                pow_y2 = - pow(-y2, 1.0/3.0);
            }
            else
            {
                pow_y2 = pow(y2, 1.0/3.0);
            }
            roots[1] = (-b - pow_y1 - pow_y2) / (3*a);
            roots[0] = 1;
        }
        else if (deita == 0)
        {
            //three real roots and two of them are the same
            float K = B/A;
            roots[1] = -b/a + K;
            roots[2] = -K/2;
            roots[3] = -K/2;
            roots[0] = 3;
        }
        else if (deita < 0)
        {
            //three different real roots
            float theta = acos((2*A*b-3*a*B) / (2*pow(A, 1.5)));
            roots[1] = (-b - 2*sqrt(A)*cos(theta/3)) / (3*a);
            roots[2] = (-b + sqrt(A) * (cos(theta/3) + sqrt(3)*sin(theta/3))) / (3*a);
            roots[3] = (-b + sqrt(A) * (cos(theta/3) - sqrt(3)*sin(theta/3))) / (3*a);
            roots[0] = 3;
        }
    }
    return roots;
}

float* roots_quartic_equation(float a, float b, float c, float d, float e)
{
    //the first element is the number of the real roots, and other elements are the real roots.
    //Ferrari's solution.
    float *roots = new float[5];
    if (a == 0)
    {
        roots = roots_cubic_equation(b, c, d, e);
    }
    else
    {
        float b1 = b/a;
        float c1 = c/a;
        float d1 = d/a;
        float e1 = e/a;
        if ((b1 == 0) && (c1 == 0) && (d1 == 0))
        {
            //in this special case, such as a=1, b=c=d=0, e=-1, the roots should be +1 and -1
            if (e1 > 0)
            {
                roots[0] = 0.0;
            }
            else
            {
                roots[1] = sqrt(sqrt(-e1));
                roots[2] = -sqrt(sqrt(-e1));
                roots[0] = 2.0;
            }
        }
        else
        {
            float *roots_y = new float[4];
            roots_y = roots_cubic_equation(-1.0, c1, 4*e1-b1*d1, d1*d1+e1*b1*b1-4*e1*c1);
            float y = roots_y[1];
            float B1, B2, C1, C2;
            if (b1*b1-4*c1+4*y == 0)
            {
                B1 = b/2;
                B2 = b/2;
                C1 = y/2;
                C2 = y/2;
            }
            else
            {
                B1 = b1/2 - sqrt(b1*b1-4*c1+4*y)/2;
                B2 = b1/2 + sqrt(b1*b1-4*c1+4*y)/2;
                C1 = y/2 - (b1*y -2*d1)/(2*sqrt(b1*b1-4*c1+4*y));
                C2 = y/2 + (b1*y -2*d1)/(2*sqrt(b1*b1-4*c1+4*y));
            }
            float *roots_x1 = new float[3];
            float *roots_x2 = new float[3];
            roots_x1 = roots_quadratic_equation(1.0, B1, C1);
            roots_x2 = roots_quadratic_equation(1.0, B2, C2);
            if (roots_x1[0] != 0)
            {
                for (int i=1; i<roots_x1[0]+1; i++)
                {
                    roots[i] = roots_x1[i];
                }
            }
            if (roots_x2[0] != 0)
            {
                int roots_x1_number = int(roots_x1[0]);
                for (int j=1; j<roots_x2[0]+1; j++)
                {
                    roots[roots_x1_number+j] =roots_x2[j];
                }
            }
            roots[0] = roots_x1[0] + roots_x2[0];
        }
    }
    return roots;
}

vector<complex<double> > get_column_of_inverse_matrix(STEPS_COMPLEX_SPARSE_MATRIX &matrix, size_t col)
{
    STEPS_SPARSE_MATRIX jacobian_matrix;
    STEPS_COMPLEX_SPARSE_MATRIX* matrix_pointer = (&matrix);
    int n = matrix_pointer->get_matrix_size();
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            complex<double> y = matrix_pointer->get_entry_value(i,j);
            double g = y.real(), b=y.imag();

            if(g!=0.0)
            {
                jacobian_matrix.add_entry(i,j,g);
                jacobian_matrix.add_entry(i+n,j+n,g);
            }
            if(b!=0.0)
            {
                jacobian_matrix.add_entry(i,j+n, -b);
                jacobian_matrix.add_entry(i+n,j, b);
            }
        }
    }
    jacobian_matrix.compress_and_merge_duplicate_entries();
    jacobian_matrix.LU_factorization(1, 1e-13);

    int n2 = n+n;
    vector<double> I;
    I.reserve(n2);
    for(int i=0; i<n2; i++)
        I.push_back(0.0);
    I[col]=1.0;
    vector<double> Zreal = I/jacobian_matrix;
    vector<complex<double> > Z;
    Z.reserve(n);
    for(int i=0; i<n; i++)
        Z.push_back(complex<double>(Zreal[i], Zreal[i+n]));
    return Z;
}

vector<vector<complex<double> > > get_all_columns_of_inverse_matrix(STEPS_COMPLEX_SPARSE_MATRIX &matrix)
{
    STEPS_SPARSE_MATRIX jacobian_matrix;
    STEPS_COMPLEX_SPARSE_MATRIX* matrix_pointer = (&matrix);
    int n = matrix_pointer->get_matrix_size();
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            complex<double> y = matrix_pointer->get_entry_value(i,j);
            double g = y.real(), b=y.imag();

            if(g!=0.0)
            {
                jacobian_matrix.add_entry(i,j,g);
                jacobian_matrix.add_entry(i+n,j+n,g);
            }
            if(b!=0.0)
            {
                jacobian_matrix.add_entry(i,j+n, -b);
                jacobian_matrix.add_entry(i+n,j, b);
            }
        }
    }
    jacobian_matrix.compress_and_merge_duplicate_entries();
    jacobian_matrix.LU_factorization(1, 1e-13);

    int n2 = n+n;
    vector<vector<complex<double> > > Z;
    for(int j=0; j<n; j++)
    {
        vector<double> I;
        I.reserve(n2);
        for(int i=0; i<n2; i++)
            I.push_back(0.0);
        I[j]=1.0;
        vector<double> Zreal = I/jacobian_matrix;
        vector<complex<double> > Zcol;
        Zcol.reserve(n);
        for(int i=0; i<n; i++)
            Zcol.push_back(complex<double>(Zreal[i], Zreal[i+n]));
        Z.push_back(Zcol);
    }
    return Z;
}
