#include "header/apis/steps_api.h"
#include "header/basic/utility.h"
#include "header/steps_namespace.h"

#include <iostream>
using namespace std;

void api_build_sequence_network_Y_matrix(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();
    NET_MATRIX& network_matrix = solver.get_network_matrix();
    network_matrix.build_sequence_network_Y_matrix();
}

void api_save_positive_sequence_network_Y_matrix(char* file, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();
    NET_MATRIX& network_matrix = solver.get_network_matrix();
    network_matrix.save_positive_sequence_Y_matrix_to_file(file);
}

void api_save_negative_sequence_network_Y_matrix(char* file, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();
    NET_MATRIX& network_matrix = solver.get_network_matrix();
    network_matrix.save_negative_sequence_Y_matrix_to_file(file);
}

void api_save_zero_sequence_network_Y_matrix(char* file, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();
    NET_MATRIX& network_matrix = solver.get_network_matrix();
    network_matrix.save_zero_sequence_Y_matrix_to_file(file);
}

void api_set_short_circuit_solver_integer_parameter(char* parameter_name, int value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME=="GENERATOR REACTANCE")
    {
        if(value==0)
        {
            solver.set_generator_reactance_option(SUBTRANSIENT_REACTANCE);
            return;
        }
        if(value==1)
        {
            solver.set_generator_reactance_option(TRANSIENT_REACTANCE);
            return;
        }
        if(value==2)
        {
            solver.set_generator_reactance_option(SYNCHRONOUS_REACTANCE);
            return;
        }
    }
    else if(PARAMETER_NAME=="UNITS")
    {
        if(value==0)
        {
            solver.set_units_of_currents_and_voltages(PU);
            return;
        }
        if(value==1)
        {
            solver.set_units_of_currents_and_voltages(PHYSICAL);
            return;
        }
    }
    else if(PARAMETER_NAME=="COORDINATES")
    {
        if(value==0)
        {
            solver.set_coordinates_of_currents_and_voltages(RECTANGULAR);
            return;
        }
        if(value==1)
        {
            solver.set_coordinates_of_currents_and_voltages(POLAR);
            return;
        }
    }
    else if(PARAMETER_NAME=="OPTION OF DC LINES")
    {
        if(value==0)
        {
            solver.set_option_of_DC_lines(BLOCK_AND_IGNORE);
            return;
        }
        if(value==1)
        {
            solver.set_option_of_DC_lines(CONVERT_TO_CONSTANT_ADMITTANCE_LOAD);
            return;
        }
    }
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for short circuit solver with api %s.\n",
             PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
    return;
}

size_t api_get_short_circuit_solver_integer_parameter(char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME=="GENERATOR REACTANCE")
        return solver.get_generator_reactance_option();

    if(PARAMETER_NAME=="UNITS")
        return solver.get_units_of_currents_and_voltages();

    if(PARAMETER_NAME=="COORDINATES")
        return solver.get_coordinates_of_currents_and_voltages();

    if(PARAMETER_NAME=="FAULT TYPE")
        return solver.get_fault_type();

    if(PARAMETER_NAME=="OPTION OF DC LINES")
        return solver.get_option_of_DC_lines();

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for short circuit solver with api %s.\n"
             "0 will be returned.", PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
    return 0;
}

void api_set_short_circuit_solver_float_parameter(char* parameter_name, double value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
//    SC_SOLVER& solver = toolkit.get_short_circuit_solver();

    string PARAMETER_NAME = string2upper(parameter_name);

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for short circuit solver with api %s.\n",
             PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
    return;
}

double api_get_short_circuit_solver_float_parameter(char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
//    SC_SOLVER& solver = toolkit.get_short_circuit_solver();

    string PARAMETER_NAME = string2upper(parameter_name);

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for short circuit solver with api %s.\n"
             "0.0 will be returned.", PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);

    return 0.0;
}

void api_set_short_circuit_solver_boolean_parameter(char* parameter_name, bool value, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();

    string PARAMETER_NAME = string2upper(parameter_name);

    if(PARAMETER_NAME=="CONSIDER LOAD LOGIC")
    {
        solver.set_consider_load_logic(value);
        return;
    }
    if(PARAMETER_NAME=="CONSIDER MOTOR LOAD LOGIC")
    {
        solver.set_consider_motor_load_logic(value);
        return;
    }

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for short circuit solver with api %s.\n",
             PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
    return;
}
bool api_get_short_circuit_solver_boolean_parameter(char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();

    string PARAMETER_NAME = string2upper(parameter_name);
    if(PARAMETER_NAME=="CONSIDER LOAD LOGIC")
        return solver.get_consider_load_logic();
    if(PARAMETER_NAME=="CONSIDER MOTOR LOAD LOGIC")
        return solver.get_consider_motor_load_logic();

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for short circuit solver with api %s.\n"
             "0.0 will be returned.", PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
    return 0.0;
}

void api_short_circuit_set_bus_fault(size_t bus, char* fault_type, double fault_G, double fault_B, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& sc_solver = toolkit.get_short_circuit_solver();
    string string_fault_type = string2upper(fault_type);
    if(string_fault_type=="THREE PHASES FAULT")
    {
        sc_solver.set_bus_fault(bus, THREE_PHASES_FAULT, complex<double>(fault_G, fault_B));
        return;
    }
    if(string_fault_type=="SINGLE PHASE GROUNDED FAULT")
    {
        sc_solver.set_bus_fault(bus, SINGLE_PHASE_GROUNDED_FAULT, complex<double>(fault_G, fault_B));
        return;
    }
    if(string_fault_type=="DOUBLE PHASES FAULT")
    {
        sc_solver.set_bus_fault(bus, DOUBLE_PHASES_FAULT, complex<double>(fault_G, fault_B));
        return;
    }
    if(string_fault_type=="DOUBLE PHASES GROUNDED FAULT")
    {
        sc_solver.set_bus_fault(bus, DOUBLE_PHASES_GROUNDED_FAULT, complex<double>(fault_G, fault_B));
        return;
    }
    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Fault type '%s' is not supported for short circuit solver with api %s.",
             string_fault_type.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
}

void api_short_circuit_set_line_fault(size_t ibus, size_t jbus, char* identifier, char* fault_type, double fault_location, double fault_G, double fault_B, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& sc_solver = toolkit.get_short_circuit_solver();
    PF_DATA& psdb = toolkit.get_database();

    DEVICE_ID did = get_ac_line_device_id(ibus, jbus, identifier);

    if(not psdb.is_ac_line_exist(did))
    {
        char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
        snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "%s does not exist in database for short circuit with api %s.",
                 (did.get_compound_device_name()).c_str(), __FUNCTION__);
        toolkit.show_information(buffer);
        return;
    }

    string string_fault_type = string2upper(fault_type);
    if(string_fault_type=="THREE PHASES FAULT")
    {
        sc_solver.set_ac_line_fault(did, ibus, fault_location, THREE_PHASES_FAULT, complex<double>(fault_G, fault_B));
        return;
    }
    if(string_fault_type=="SINGLE PHASE GROUNDED FAULT")
    {
        sc_solver.set_ac_line_fault(did, ibus, fault_location, SINGLE_PHASE_GROUNDED_FAULT, complex<double>(fault_G, fault_B));
        return;
    }
    if(string_fault_type=="DOUBLE PHASES FAULT")
    {
        sc_solver.set_ac_line_fault(did, ibus, fault_location, DOUBLE_PHASES_FAULT, complex<double>(fault_G, fault_B));
        return;
    }
    if(string_fault_type=="DOUBLE PHASES GROUNDED FAULT")
    {
        sc_solver.set_ac_line_fault(did, ibus, fault_location, DOUBLE_PHASES_GROUNDED_FAULT, complex<double>(fault_G, fault_B));
        return;
    }

    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Fault type '%s' is not supported for short circuit solver with api %s.",
             string_fault_type.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
}

void api_short_circuit_clear_fault(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();
    solver.clear_fault();
}

void api_solve_short_circuit(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();
    solver.solve();
}

double api_get_short_circuit_result_float_data(char* parameter_name, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();

    string PARAMETER_NAME = string2upper(parameter_name);

    if(PARAMETER_NAME=="IF1_PU" or PARAMETER_NAME=="POSITIVE SEQUENCE CURRENT IN PU")
        return abs(solver.get_positive_sequence_fault_current_in_pu());
    if(PARAMETER_NAME=="IF2_PU" or PARAMETER_NAME=="NEGATIVE SEQUENCE CURRENT IN PU")
        return abs(solver.get_negative_sequence_fault_current_in_pu());
    if(PARAMETER_NAME=="IF0_PU" or PARAMETER_NAME=="ZERO SEQUENCE CURRENT IN PU")
        return abs(solver.get_zero_sequence_fault_current_in_pu());

    if(PARAMETER_NAME=="IF1_KA" or PARAMETER_NAME=="POSITIVE SEQUENCE CURRENT IN KA")
        return abs(solver.get_positive_sequence_fault_current_in_kA());
    if(PARAMETER_NAME=="IF2_KA" or PARAMETER_NAME=="NEGATIVE SEQUENCE CURRENT IN KA")
        return abs(solver.get_negative_sequence_fault_current_in_kA());
    if(PARAMETER_NAME=="IF0_KA" or PARAMETER_NAME=="ZERO SEQUENCE CURRENT IN KA")
        return abs(solver.get_zero_sequence_fault_current_in_kA());

    if(PARAMETER_NAME=="IF1_DEG" or PARAMETER_NAME=="POSITIVE SEQUENCE CURRENT IN DEG")
        return rad2deg(arg(solver.get_positive_sequence_fault_current_in_pu()));
    if(PARAMETER_NAME=="IF2_DEG" or PARAMETER_NAME=="NEGATIVE SEQUENCE CURRENT IN DEG")
        return rad2deg(arg(solver.get_negative_sequence_fault_current_in_pu()));
    if(PARAMETER_NAME=="IF0_DEG" or PARAMETER_NAME=="ZERO SEQUENCE CURRENT IN DEG")
        return rad2deg(arg(solver.get_zero_sequence_fault_current_in_pu()));


    if(PARAMETER_NAME=="VF1_PU" or PARAMETER_NAME=="POSITIVE SEQUENCE VOLTAGE AT FAULT PLACE IN PU")
        return abs(solver.get_positive_sequence_voltage_of_fault_point_in_pu());
    if(PARAMETER_NAME=="VF2_PU" or PARAMETER_NAME=="NEGATIVE SEQUENCE VOLTAGE AT FAULT PLACE IN PU")
        return abs(solver.get_negative_sequence_voltage_of_fault_point_in_pu());
    if(PARAMETER_NAME=="VF0_PU" or PARAMETER_NAME=="ZERO SEQUENCE VOLTAGE AT FAULT PLACE IN PU")
        return abs(solver.get_zero_sequence_voltage_of_fault_point_in_pu());

    if(PARAMETER_NAME=="VF1_KA" or PARAMETER_NAME=="POSITIVE SEQUENCE VOLTAGE AT FAULT PLACE IN KA")
        return abs(solver.get_positive_sequence_voltage_of_fault_point_in_kV());
    if(PARAMETER_NAME=="VF2_KA" or PARAMETER_NAME=="NEGATIVE SEQUENCE VOLTAGE AT FAULT PLACE IN KA")
        return abs(solver.get_negative_sequence_voltage_of_fault_point_in_kV());
    if(PARAMETER_NAME=="VF0_KA" or PARAMETER_NAME=="ZERO SEQUENCE VOLTAGE AT FAULT PLACE IN KA")
        return abs(solver.get_zero_sequence_voltage_of_fault_point_in_kV());

    if(PARAMETER_NAME=="VF1_DEG" or PARAMETER_NAME=="POSITIVE SEQUENCE VOLTAGE AT FAULT IN DEG")
        return rad2deg(arg(solver.get_positive_sequence_voltage_of_fault_point_in_pu()));
    if(PARAMETER_NAME=="VF2_DEG" or PARAMETER_NAME=="NEGATIVE SEQUENCE VOLTAGE AT FAULT IN DEG")
        return rad2deg(arg(solver.get_negative_sequence_voltage_of_fault_point_in_pu()));
    if(PARAMETER_NAME=="VF0_DEG" or PARAMETER_NAME=="ZERO SEQUENCE VOLTAGE AT FAULT IN DEG")
        return rad2deg(arg(solver.get_zero_sequence_voltage_of_fault_point_in_pu()));


    if(PARAMETER_NAME=="R1_PU" or PARAMETER_NAME=="POSITIVE SEQUENCE EQUIVALENT R IN PU")
        return solver.get_positive_sequence_thevenin_impedance_at_fault_in_pu().real();
    if(PARAMETER_NAME=="X1_PU" or PARAMETER_NAME=="POSITIVE SEQUENCE EQUIVALENT X IN PU")
        return solver.get_positive_sequence_thevenin_impedance_at_fault_in_pu().imag();
    if(PARAMETER_NAME=="R2_PU" or PARAMETER_NAME=="NEGATIVE SEQUENCE EQUIVALENT R IN PU")
        return solver.get_negative_sequence_thevenin_impedance_at_fault_in_pu().real();
    if(PARAMETER_NAME=="X2_PU" or PARAMETER_NAME=="NEGATIVE SEQUENCE EQUIVALENT X IN PU")
        return solver.get_negative_sequence_thevenin_impedance_at_fault_in_pu().imag();
    if(PARAMETER_NAME=="R0_PU" or PARAMETER_NAME=="ZERO SEQUENCE EQUIVALENT R IN PU")
        return solver.get_zero_sequence_thevenin_impedance_at_fault_in_pu().real();
    if(PARAMETER_NAME=="X0_PU" or PARAMETER_NAME=="ZERO SEQUENCE EQUIVALENT X IN PU")
        return solver.get_zero_sequence_thevenin_impedance_at_fault_in_pu().imag();


    char buffer[STEPS_MAX_TEMP_CHAR_BUFFER_SIZE];
    snprintf(buffer, STEPS_MAX_TEMP_CHAR_BUFFER_SIZE, "Parameter '%s' is not supported for short circuit result with api %s.\n"
             "0.0 will be returned.", PARAMETER_NAME.c_str(), __FUNCTION__);
    toolkit.show_information(buffer);
    return 0.0;
}


void api_show_short_circuit_result(size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();
    solver.show_short_circuit_result();
}

void api_save_short_circuit_result_to_file(char* file, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();
    solver.save_short_circuit_result_to_file(file);
}

void api_save_extended_short_circuit_result_to_file(char* file, size_t toolkit_index)
{
    iSTEPS& toolkit = get_toolkit(toolkit_index);
    SC_SOLVER& solver = toolkit.get_short_circuit_solver();
    solver.save_extended_short_circuit_result_to_file(file);
}

