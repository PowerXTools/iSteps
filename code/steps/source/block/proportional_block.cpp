#include "header/block/proportional_block.h"
#include "header/basic/utility.h"
#include "header/iSTEPS.h"
#include <cstdio>

PROPORTIONAL_BLOCK::PROPORTIONAL_BLOCK(iSTEPS& toolkit) : BLOCK(toolkit)
{
    set_K(1.0);
}

PROPORTIONAL_BLOCK::~PROPORTIONAL_BLOCK()
{
    ;
}

void PROPORTIONAL_BLOCK::set_K(double k)
{
    /*if(k==0.0)
    {
        ostringstream osstream;
        osstream<<"Error. Zero amplifier K is not allowed for PROPORTIONAL_BLOCK.";
        toolkit.show_information(osstream);
        return;
    }*/

    this->K = k;
}

double PROPORTIONAL_BLOCK::get_K() const
{
    return K;
}

void PROPORTIONAL_BLOCK::initialize()
{
    double k = get_K();
    if(k==0.0)
    {
        double y = get_output();
        double x = y/k;

        set_input(x);

        if(get_limiter_type()!=NO_LIMITER)
        {
            double vmax = get_upper_limit();
            double vmin = get_lower_limit();
            if(y>=vmin and y<=vmax)
                return;

            ostringstream osstream;

            if(y>vmax)
            {
                osstream<<"Initialization Error. Proportional output ("<<y<<") exceeds upper limit bound ("<<vmax<<").";
                iSTEPS& toolkit = get_toolkit();
                toolkit.show_information(osstream);
                return;
            }
            if(y<vmin)
            {
                osstream<<"Initialization Error. Proportional output ("<<y<<") exceeds lower limit bound ("<<vmin<<").";
                iSTEPS& toolkit = get_toolkit();
                toolkit.show_information(osstream);
                return;
            }
        }
    }
    else
    {
        set_input(0.0);
    }
}

void PROPORTIONAL_BLOCK::run(DYNAMIC_MODE mode)
{
    double k = get_K();
    if(k!=0.0)
    {
        double y = k*get_input();

        if(get_limiter_type()==NO_LIMITER)
        {
            set_output(y);
        }
        else
        {
            double vmax = get_upper_limit();
            double vmin = get_lower_limit();
            if(y>vmax)
                y = vmax;
            else
            {
                if(y<vmin)
                    y = vmin;
            }
            set_output(y);
        }
    }
}

void PROPORTIONAL_BLOCK::integrate()
{
    ;
}

void PROPORTIONAL_BLOCK::update()
{
    ;
}

STEPS_SPARSE_MATRIX PROPORTIONAL_BLOCK::get_linearized_matrix_variable(char var) const
{
    var = toupper(var);
    switch(var)
    {
        case 'A':
            return get_linearized_matrix_A();
        case 'B':
            return get_linearized_matrix_B();
        case 'C':
            return get_linearized_matrix_C();
        case 'D':
            return get_linearized_matrix_D();
        default:
            STEPS_SPARSE_MATRIX matrix;
            return matrix;
    }
}

STEPS_SPARSE_MATRIX PROPORTIONAL_BLOCK::get_linearized_matrix_A() const
{
    /*
    x: state; u: input; y: output
    dx/dt = A*x + B*u
    y     = C*x + D*u
    */
    STEPS_SPARSE_MATRIX matrix;
    matrix.compress_and_merge_duplicate_entries();
    return matrix;
}

STEPS_SPARSE_MATRIX PROPORTIONAL_BLOCK::get_linearized_matrix_B() const
{
    STEPS_SPARSE_MATRIX matrix;
    matrix.compress_and_merge_duplicate_entries();
    return matrix;
}

STEPS_SPARSE_MATRIX PROPORTIONAL_BLOCK::get_linearized_matrix_C() const
{
    STEPS_SPARSE_MATRIX matrix;
    matrix.compress_and_merge_duplicate_entries();
    return matrix;
}

STEPS_SPARSE_MATRIX PROPORTIONAL_BLOCK::get_linearized_matrix_D() const
{
    STEPS_SPARSE_MATRIX matrix;
    matrix.add_entry(0,0, get_K());
    matrix.compress_and_merge_duplicate_entries();
    return matrix;
}

void PROPORTIONAL_BLOCK::check()
{
    check_limiter();
}

