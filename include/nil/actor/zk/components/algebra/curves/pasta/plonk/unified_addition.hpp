//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//
// @file Declaration of interfaces for PLONK unified addition component.
//---------------------------------------------------------------------------//

#ifndef ACTOR_ZK_BLUEPRINT_PLONK_UNIFIED_ADDITION_COMPONENT_11_WIRES_HPP
#define ACTOR_ZK_BLUEPRINT_PLONK_UNIFIED_ADDITION_COMPONENT_11_WIRES_HPP

#include <cmath>

#include <nil/actor/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/actor/zk/blueprint/plonk.hpp>
#include <nil/actor/zk/assignment/plonk.hpp>
#include <nil/actor/zk/algorithms/generate_circuit.hpp>
#include <nil/crypto3/math/expressions/expression_tstr.hpp>
#include <nil/crypto3/math/expressions/expression_string.hpp>

namespace nil {
    namespace actor {
        namespace zk {
            namespace components {

                // Input: P, Q - elliptic curve points
                // Output: R = P + Q
                template<typename ArithmetizationType, typename CurveType, std::size_t... WireIndexes>
                class curve_element_unified_addition;

                template<typename BlueprintFieldType,
                         typename ArithmetizationParams,
                         typename CurveType,
                         std::size_t W0,
                         std::size_t W1,
                         std::size_t W2,
                         std::size_t W3,
                         std::size_t W4,
                         std::size_t W5,
                         std::size_t W6,
                         std::size_t W7,
                         std::size_t W8,
                         std::size_t W9,
                         std::size_t W10>
                class curve_element_unified_addition<
                    snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>,
                    CurveType,
                    W0,
                    W1,
                    W2,
                    W3,
                    W4,
                    W5,
                    W6,
                    W7,
                    W8,
                    W9,
                    W10> {

                    typedef snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>
                        ArithmetizationType;

                    using var = snark::plonk_variable<BlueprintFieldType>;

                    constexpr static const std::size_t selector_seed = 0x0f01;

                    template<typename ComponentType, typename ArithmetizationType>
                    friend typename std::enable_if<
                        (!(has_static_member_function_generate_circuit<
                            ComponentType,
                            typename ComponentType::result_type,
                            boost::mpl::vector<blueprint<ArithmetizationType> &,
                                               blueprint_public_assignment_table<ArithmetizationType> &,
                                               const typename ComponentType::params_type &,
                                               const std::size_t>>::value)),
                        typename ComponentType::result_type>::type
                        generate_circuit(blueprint<ArithmetizationType> &bp,
                                         blueprint_public_assignment_table<ArithmetizationType> &assignment,
                                         const typename ComponentType::params_type &params,
                                         const std::size_t start_row_index);

                public:
                    constexpr static const std::size_t rows_amount = 1;
                    constexpr static const std::size_t gates_amount = 1;

                    using constraint1 = ExpressionTstr<X<
                        decltype("W20 * W20 * W100 - W20 * W100 * W00 - W20 * W100 * W00 + W20 * W100 * W00 - W30 + W10"_tstr)>>;
                    using constraint2 = ExpressionTstr<X<
                        decltype("W80 * W10 * W100 + W80 * W10 * W100 - W80 * W00 * W00 - W80 * "
                                 "W00 * W00 - W80* W00 * W00 - W20 * W80 * W10 * W100 - W20 * W80 * "
                                 "W10 * W100 + W20 * W80 * W00 * W00 + W20 * W80 * W00 * W00 + W20 * var(W8, "
                                 "0) * W00 * W00 + W00 * W80 * W10 * W100 + W00 * W80 * W10 * W100 - W00 * "
                                 "W80 * W00 * W00 - W00 * W80 * W00 * W00 - W00 * W80 * W00 * W00"_tstr)>>;

                    struct params_type {
                        struct var_ec_point {
                            var x;
                            var y;
                        };

                        var_ec_point P;
                        var_ec_point Q;
                    };

                    // To obtain the result from outside:
                    // TODO: bind columns in result_type to the one actually used in
                    // circuit generation
                    struct result_type {
                        var X = var(0, 0, false);
                        var Y = var(0, 0, false);
                        result_type(const params_type &params, const std::size_t start_row_index = 0) {
                            X = var(W4, start_row_index, false, var::column_type::witness);
                            Y = var(W5, start_row_index, false, var::column_type::witness);
                        }

                        result_type() {
                        }
                    };

                    static result_type generate_assignments(blueprint_assignment_table<ArithmetizationType> &assignment,
                                                            const params_type params,
                                                            const std::size_t start_row_index) {

                        const std::size_t j = start_row_index;

                        typename BlueprintFieldType::value_type p_x = assignment.var_value(params.P.x);
                        typename BlueprintFieldType::value_type p_y = assignment.var_value(params.P.y);
                        typename CurveType::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type P(p_x,
                                                                                                                 p_y);

                        typename BlueprintFieldType::value_type q_x = assignment.var_value(params.Q.x);
                        typename BlueprintFieldType::value_type q_y = assignment.var_value(params.Q.y);
                        typename CurveType::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type Q(q_x,
                                                                                                                 q_y);

                        const typename CurveType::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type R =
                            P + Q;

                        assignment.witness(W0)[j] = P.X;
                        assignment.witness(W1)[j] = P.Y;
                        assignment.witness(W2)[j] = Q.X;
                        assignment.witness(W3)[j] = Q.Y;
                        typename CurveType::template g1_type<nil::crypto3::algebra::curves::coordinates::affine>::value_type zero = {
                            0, 0};
                        if (P.X == zero.X && P.Y == zero.Y) {
                            assignment.witness(W4)[j] = Q.X;
                            assignment.witness(W5)[j] = Q.Y;
                        } else {
                            if (Q.X == zero.X && Q.Y == zero.Y) {
                                assignment.witness(W4)[j] = P.X;
                                assignment.witness(W5)[j] = P.Y;
                            } else {
                                if (Q.X == P.X && Q.Y == -P.Y) {
                                    assignment.witness(W4)[j] = 0;
                                    assignment.witness(W5)[j] = 0;
                                } else {
                                    assignment.witness(W4)[j] = (P + Q).X;
                                    assignment.witness(W5)[j] = (P + Q).Y;
                                }
                            }
                        }
                        if (P.X != 0) {
                            assignment.witness(W6)[j] = P.X.inversed();
                        } else {
                            assignment.witness(W6)[j] = 0;
                        }

                        if (Q.X != 0) {
                            assignment.witness(W7)[j] = Q.X.inversed();
                        } else {
                            assignment.witness(W7)[j] = 0;
                        }

                        if (P.X != Q.X) {
                            assignment.witness(W10)[j] = (Q.Y - P.Y) / (Q.X - P.X);

                            assignment.witness(W9)[j] = 0;

                            assignment.witness(W8)[j] = (Q.X - P.X).inversed();
                        } else {

                            if (P.Y != -Q.Y) {
                                assignment.witness(W9)[j] = (Q.Y + P.Y).inversed();
                            } else {
                                assignment.witness(W9)[j] = 0;
                            }
                            if (P.Y != 0) {
                                assignment.witness(W10)[j] = (3 * (P.X * P.X)) / (2 * P.Y);
                            } else {
                                assignment.witness(W10)[j] = 0;
                            }
                            assignment.witness(W8)[j] = 0;
                        }

                        return result_type(params, start_row_index);
                    }
                    template<typename VarType, typename ValueType>
                    static std::vector<VarType> evaluate_gates(std::array<VarType, 11> column_polynomials,
                                                               ValueType theta) {

                        ValueType theta_acc = ValueType::one();
                        std::vector<VarType> input {};
                        for (std::size_t i = 0; i < 11; i++) {
                            input.push_back(column_polynomials[i]);
                        }

                        std::vector<const char *> s {"W00", "W10", "W20", "W30", "W40", "W50",
                                                     "W60", "W70", "W80", "W90", "W100"};
                        //     for (std::size_t i = 0; i < gates.size(); i++) {
                        VarType gate_result = evaluate<constraint1>(std::make_pair(s, input)) * theta_acc;

                        gate_result = gate_result + evaluate<constraint2>(std::make_pair(s, input)) * theta_acc;

                        return std::vector<VarType> {gate_result};
                    }

                    static void
                        generate_copy_constraints(blueprint<ArithmetizationType> &bp,
                                                  blueprint_public_assignment_table<ArithmetizationType> &assignment,
                                                  const params_type params,
                                                  const std::size_t start_row_index) {
                        bp.add_copy_constraint({params.P.x, var(W0, start_row_index, false)});
                        bp.add_copy_constraint({params.P.y, var(W1, start_row_index, false)});
                        bp.add_copy_constraint({params.Q.x, var(W2, start_row_index, false)});
                        bp.add_copy_constraint({params.Q.y, var(W3, start_row_index, false)});
                    }
                };
            }    // namespace components
        }        // namespace zk
    }            // namespace actor
}    // namespace nil

#endif    // ACTOR_ZK_BLUEPRINT_PLONK_UNIFIED_ADDITION_COMPONENT_11_WIRES_HPP
