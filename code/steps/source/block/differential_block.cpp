#include "header/block/differential_block.h"
#include "header/basic/utility.h"
#include "header/steps_namespace.h"
#include <istream>
#include <iostream>

using namespace std;

DIFFERENTIAL_BLOCK::DIFFERENTIAL_BLOCK(iSTEPS& toolkit) : BLOCK(toolkit)
{
    set_K(1.0);
    set_T_in_s(999.0);
}

DIFFERENTIAL_BLOCK::~DIFFERENTIAL_BLOCK()
{
    ;
}

void DIFFERENTIAL_BLOCK::set_K(double k)
{
    this->K = k;
}

double DIFFERENTIAL_BLOCK::get_K() const
{
    return K;
}

void DIFFERENTIAL_BLOCK::set_T_in_s(double t)
{
    if(t==0.0)
    {
        ostringstream osstream;
        osstream<<"Error. Zero time constant T is not allowed for DIFFERENTIAL_BLOCK.";
        iSTEPS& toolkit = get_toolkit();
        toolkit.show_information(osstream);
        return;
    }
    this->T = t;
}

double DIFFERENTIAL_BLOCK::get_T_in_s() const
{
    return T;
}

void DIFFERENTIAL_BLOCK::initialize()
{
    determine_block_integration_time_step_mode();
    determine_block_integration_time_step();
    determine_block_temp_variables();

    switch(get_integration_time_step_mode())
    {
        case NORMAL_INTEGRATION_TIME_STEP_MODE:
            initialize_normal_time_step_mode();
            break;
        case SMALL_INTEGRATION_TIME_STEP_MODE:
            initialize_small_time_step_mode();
            break;
        case LARGE_INTEGRATION_TIME_STEP_MODE:
        default:
            initialize_large_time_step_mode();
            break;
    }
}

void DIFFERENTIAL_BLOCK::determine_block_integration_time_step_mode()
{
    iSTEPS& toolkit = get_toolkit();
    double global_h = toolkit.get_dynamic_simulation_time_step_in_s();
    BLOCK_INTEGRATION_TIME_STEP_MODE mode = NORMAL_INTEGRATION_TIME_STEP_MODE;

    bool is_automatic_large_step_logic_enabled = is_automatic_large_time_step_logic_enabled();
    if(is_automatic_large_step_logic_enabled)
    {
        double t = get_T_in_s();
        if(global_h<7.6*t) // magic number is NOT allowed. must be removed.
        {
            if(global_h>t/4)
                mode = SMALL_INTEGRATION_TIME_STEP_MODE;
        }
        else
            mode = LARGE_INTEGRATION_TIME_STEP_MODE;
    }
    set_integration_time_step_mode(mode);
}

void DIFFERENTIAL_BLOCK::determine_block_integration_time_step()
{
    iSTEPS& toolkit = get_toolkit();
    h = toolkit.get_dynamic_simulation_time_step_in_s();

    BLOCK_INTEGRATION_TIME_STEP_MODE mode = get_integration_time_step_mode();
    if(mode == SMALL_INTEGRATION_TIME_STEP_MODE)
    {
        double t = get_T_in_s();
        count_of_time_slice_when_in_small_integration_time_step_mode = ceil(4.0*h/t);
        h = h/count_of_time_slice_when_in_small_integration_time_step_mode;
    }
}

void DIFFERENTIAL_BLOCK::determine_block_temp_variables()
{
    double k = get_K();
    if(k!=0.0)
    {
        double t = get_T_in_s();
        one_over_t = 1.0/t;
        k_over_t = k*one_over_t;
        t_over_h = t/h;
    }
}

void DIFFERENTIAL_BLOCK::initialize_normal_time_step_mode()
{
    double k = get_K();
    if(k!=0.0)
    {
        double x = get_input();

        double y = 0.0;

        double s, z;

        //s = x*k/t;
        //z = k/t*x-(1.0-2.0*t/h)*s;
        s = x*k_over_t;
        z = k_over_t*x-(1.0-2.0*t_over_h)*s;

        set_state(s);
        set_store(z);
        set_output(y);
    }
    else
    {
        set_state(0.0);
        set_store(0.0);
        set_output(0.0);
    }
}

void DIFFERENTIAL_BLOCK::initialize_small_time_step_mode()
{
    initialize_normal_time_step_mode();
    copy_current_input_to_old_input_in_last_time_step();
}

void DIFFERENTIAL_BLOCK::initialize_large_time_step_mode()
{
    initialize_normal_time_step_mode();
}

void DIFFERENTIAL_BLOCK::run(DYNAMIC_MODE mode)
{
    if(get_K()!=0.0)
    {
        if(mode==DYNAMIC_INTEGRATE_MODE)
            integrate();
        if(mode==DYNAMIC_UPDATE_MODE)
            update();
        if(mode==DYNAMIC_UPDATE_TIME_STEP_MODE)
            update_simulation_time_step();
    }
}

void DIFFERENTIAL_BLOCK::integrate()
{
    switch(get_integration_time_step_mode())
    {
        case NORMAL_INTEGRATION_TIME_STEP_MODE:
            integrate_normal_time_step_mode();
            break;
        case SMALL_INTEGRATION_TIME_STEP_MODE:
            integrate_small_time_step_mode();
            break;
        case LARGE_INTEGRATION_TIME_STEP_MODE:
        default:
            integrate_large_time_step_mode();
            break;
    }
}

void DIFFERENTIAL_BLOCK::integrate_normal_time_step_mode()
{
    double x = get_input();

    double s, z, y;
    z = get_store();

    //s = (z+k/t*x)/(1.0+2.0*t/h);
    //y = k/t*x-s;
    //double ds = (k/t*x-s)/t;
    s = (z+k_over_t*x)/(1.0+2.0*t_over_h);
    y = k_over_t*x-s;
    //double ds = (k_over_t*x-s)*one_over_t;

    //if(fabs(ds)>DOUBLE_EPSILON)
    //{
        set_state(s);
        set_output(y);
    //}
    //if(fabs(ds)>DSTATE_THRESHOLD)
    //    cout<<"Derivative of state is changed dramatically in DIFFERENTIAL_BLOCK\n";

}

void DIFFERENTIAL_BLOCK::integrate_small_time_step_mode()
{
    double x = get_input();
    double x0 = get_old_input_in_last_time_step();
    double xi = x0;
    double deltaX = (x-x0)/count_of_time_slice_when_in_small_integration_time_step_mode;
    double s=0, z=0, y=0;
    z=get_store();
    for(size_t i=0; i<count_of_time_slice_when_in_small_integration_time_step_mode; ++i)
    {
        xi += deltaX;
        s = (z+k_over_t*xi)/(1.0+2.0*t_over_h);
        y = k_over_t*xi - s;
        z = k_over_t*xi-(1.0-2.0*t_over_h)*s;
    }
    set_state(s);
    set_output(y);
}

void DIFFERENTIAL_BLOCK::integrate_large_time_step_mode()
{
    double x = get_input();
    double y = 0;
    double s = k_over_t*x;
    set_state(s);
    set_output(y);
}

void DIFFERENTIAL_BLOCK::update()
{
    switch(get_integration_time_step_mode())
    {
        case NORMAL_INTEGRATION_TIME_STEP_MODE:
            update_normal_time_step_mode();
            break;
        case SMALL_INTEGRATION_TIME_STEP_MODE:
            update_small_time_step_mode();
            break;
        case LARGE_INTEGRATION_TIME_STEP_MODE:
        default:
            update_large_time_step_mode();
            break;
    }
}

void DIFFERENTIAL_BLOCK::update_normal_time_step_mode()
{
    double x = get_input();

    double s, z, y;

    s = get_state();
    //y = k/t*x-s;
    //z = k/t*x-(1.0-2.0*t/h)*s;
    //ds = y/t;
    y = k_over_t*x-s;
    z = k_over_t*x-(1.0-2.0*t_over_h)*s;

    set_store(z);
    set_output(y);
}

void DIFFERENTIAL_BLOCK::update_small_time_step_mode()
{
    update_normal_time_step_mode();
    copy_current_input_to_old_input_in_last_time_step();
}

void DIFFERENTIAL_BLOCK::update_large_time_step_mode()
{
    update_normal_time_step_mode();
}

void DIFFERENTIAL_BLOCK::update_simulation_time_step()
{
    determine_block_integration_time_step_mode();
    determine_block_integration_time_step();
    determine_block_temp_variables();

    double x = get_input();
    double y = get_output();
    double s = k_over_t*x-y;
    set_state(s);
    double z = k_over_t*x-(1.0-2.0*t_over_h)*s;
    set_store(z);

    copy_current_input_to_old_input_in_last_time_step();
}

STEPS_SPARSE_MATRIX DIFFERENTIAL_BLOCK::get_linearized_matrix_variable(char var) const
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

STEPS_SPARSE_MATRIX DIFFERENTIAL_BLOCK::get_linearized_matrix_A() const
{
    /*
    x: state; u: input; y: output
    dx/dt = A*x + B*u
    y     = C*x + D*u
    */
    STEPS_SPARSE_MATRIX matrix;

    double a = -1/get_T_in_s();

    matrix.add_entry(0,0, a);
    matrix.compress_and_merge_duplicate_entries();

    return matrix;
}

STEPS_SPARSE_MATRIX DIFFERENTIAL_BLOCK::get_linearized_matrix_B() const
{
    STEPS_SPARSE_MATRIX matrix;

    double b = get_K()/(get_T_in_s()*get_T_in_s());

    matrix.add_entry(0,0, b);
    matrix.compress_and_merge_duplicate_entries();

    return matrix;
}

STEPS_SPARSE_MATRIX DIFFERENTIAL_BLOCK::get_linearized_matrix_C() const
{
    STEPS_SPARSE_MATRIX matrix;

    double c = -1.0;

    matrix.add_entry(0,0, c);
    matrix.compress_and_merge_duplicate_entries();

    return matrix;
}

STEPS_SPARSE_MATRIX DIFFERENTIAL_BLOCK::get_linearized_matrix_D() const
{
    STEPS_SPARSE_MATRIX matrix;

    double d = get_K()/get_T_in_s();

    matrix.add_entry(0,0, d);
    matrix.compress_and_merge_duplicate_entries();

    return matrix;
}

void DIFFERENTIAL_BLOCK::check()
{
    if(get_limiter_type() != NO_LIMITER)
    {
        ostringstream osstream;
        osstream<<"Warning. Limiter is not allowed for DIFFERENTIAL_BLOCK, and will be disabled.";
        iSTEPS& toolkit = get_toolkit();
        toolkit.show_information(osstream);;
    }
}
