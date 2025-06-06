#ifndef JACOBIAN_BUILDER_H
#define JACOBIAN_BUILDER_H

#include "header/device/ac_line.h"
#include "header/device/transformer.h"
#include "header/device/fixed_shunt.h"

#include "header/network/network_matrix.h"
#include "header/basic/sparse_matrix_define.h"

#include <ctime>

class JACOBIAN_BUILDER
{
    public:
        JACOBIAN_BUILDER(iSTEPS& toolkit);
        ~JACOBIAN_BUILDER();
        iSTEPS& get_toolkit() const;

        void set_network_matrix(NET_MATRIX& network_matrix);
        NET_MATRIX* get_network_Y_matrix() const;

        bool is_network_matrix_set() const;

        void build_seprate_jacobians();
        void update_seprate_jacobians();

        double get_jacobian_delta_p_over_angle_of_internal_bus(size_t ibus, size_t jbus);
        double get_jacobian_delta_p_over_voltage_of_internal_bus(size_t ibus, size_t jbus);
        double get_jacobian_delta_q_over_angle_of_internal_bus(size_t ibus, size_t jbus);
        double get_jacobian_delta_q_over_voltage_of_internal_bus(size_t ibus, size_t jbus);

        double get_jacobian_delta_p_over_angle_of_physical_bus(size_t ibus, size_t jbus);
        double get_jacobian_delta_p_over_voltage_of_physical_bus(size_t ibus, size_t jbus);
        double get_jacobian_delta_q_over_angle_of_physical_bus(size_t ibus, size_t jbus);
        double get_jacobian_delta_q_over_voltage_of_physical_bus(size_t ibus, size_t jbus);

        STEPS_SPARSE_MATRIX get_full_coupled_jacobian_with_P_and_Q_equation_internal_buses(const vector<size_t>& internal_P_equation_buses,
                                                                                    const vector<size_t>& internal_Q_equation_buses);

        STEPS_SPARSE_MATRIX get_decoupled_B_jacobian_with_P_equation_internal_buses(const vector<size_t>& internal_P_equation_buses);

        STEPS_SPARSE_MATRIX get_decoupled_B_jacobian_with_Q_equation_internal_buses(const vector<size_t>& internal_Q_equation_buses);


        void show_seprate_jacobians();
        void save_jacobian_matrix(const string& filename) const;
    private:
        iSTEPS* toolkit;
        NET_MATRIX* network_matrix;
        void update_jacobian_delta_p_over_angle();
        void update_jacobian_delta_p_over_voltage();
        void update_jacobian_delta_q_over_angle();
        void update_jacobian_delta_q_over_voltage();

        STEPS_SPARSE_MATRIX jacobian_delta_p_over_angle,
                      jacobian_delta_p_over_voltage,
                      jacobian_delta_q_over_angle,
                      jacobian_delta_q_over_voltage;
};

#endif // JACOBIAN_BUILDER_H
