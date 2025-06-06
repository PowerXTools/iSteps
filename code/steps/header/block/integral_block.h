#ifndef INTEGRAL_BLOCK_H
#define INTEGRAL_BLOCK_H

#include "header/block/block.h"

class INTEGRAL_BLOCK : public BLOCK
{
    // 1/(sT)
    public:
        INTEGRAL_BLOCK(iSTEPS& toolkit);
        ~INTEGRAL_BLOCK();

        void set_T_in_s(double T);
        double get_T_in_s() const;

        void initialize();
        void run(DYNAMIC_MODE);

        STEPS_SPARSE_MATRIX get_linearized_matrix_variable(char var) const;
        STEPS_SPARSE_MATRIX get_linearized_matrix_A() const;
        STEPS_SPARSE_MATRIX get_linearized_matrix_B() const;
        STEPS_SPARSE_MATRIX get_linearized_matrix_C() const;
        STEPS_SPARSE_MATRIX get_linearized_matrix_D() const;

        virtual void check();
    private:
        void integrate();
        void update();
        void update_simulation_time_step();

        double T;

        double one_over_t, h_over_t;
};
#endif // INTEGRAL_BLOCK_H
