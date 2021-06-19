//---------------------------------------------------------------------------//
// Copyright (c) 2018-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020-2021 Nikita Kaskov <nbering@nil.foundation>
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
// @file Declaration of interfaces for Fp6 components.
//
// The components verify field arithmetic in Fp6 = Fp3[Y]/(Y^2-X) where
// Fp3 = Fp[X]/(X^3-non_residue) and non_residue is in Fp.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_ZK_FP6_2OVER3_COMPONENTS_HPP
#define CRYPTO3_ZK_FP6_2OVER3_COMPONENTS_HPP

#include <nil/crypto3/zk/snark/component.hpp>
#include <nil/crypto3/zk/snark/components/fields/element_fp2.hpp>
#include <nil/crypto3/zk/snark/components/fields/element_fp3.hpp>

#include <nil/crypto3/zk/snark/blueprint_variable.hpp>

namespace nil {
    namespace crypto3 {
        namespace zk {
            namespace snark {
                namespace components {

                    /******************************** element_fp6_2over3 ************************************/

                    /**
                     * Component that represents an Fp6 variable.
                     */
                    template<typename Fp6_2over3T>    // Fp6_2over3T
                    struct element_fp6_2over3 : public component<typename Fp6_2over3T::base_field_type> {
                        
                        using field_type = Fp6_2over3T;
                        using base_field_type = typename field_type::base_field_type;
                        using underlying_field_type = typename field_type::underlying_field_type;

                        using underlying_element_type = element_fp3<underlying_field_type>;

                        using data_type = std::array<
                            underlying_element_type, 
                            field_type::arity / underlying_field_type::arity>;

                        data_type data;

                        element_fp6_2over3(blueprint<base_field_type> &bp) :
                            component<base_field_type>(bp), data({underlying_type(bp), underlying_type(bp)}) {
                        }

                        element_fp6_2over3(blueprint<base_field_type> &bp,
                                            const typename Fp6_2over3T::value_type &el) :
                            component<base_field_type>(bp),
                            data({underlying_type(bp, el.data[0]), underlying_type(bp, el.data[1])}) {
                        }

                        element_fp6_2over3(blueprint<base_field_type> &bp, const Fp3_variable<Fp3T> &in_data0,
                                            const Fp3_variable<Fp3T> &in_data1) :
                            component<base_field_type>(bp),
                            data({underlying_type(in_data0), underlying_type(in_data1)}) {
                        }

                        void generate_r1cs_equals_const_constraints(const typename Fp6_2over3T::value_type &el) {
                            data[0].generate_r1cs_equals_const_constraints(el.data[0]);
                            data[1].generate_r1cs_equals_const_constraints(el.data[1]);
                        }

                        void generate_r1cs_witness(const typename Fp6_2over3T::value_type &el) {
                            data[0].generate_r1cs_witness(el.data[0]);
                            data[1].generate_r1cs_witness(el.data[1]);
                        }

                        typename Fp6_2over3T::value_type get_element() {
                            typename Fp6_2over3T::value_type el;
                            el.data[0] = data[0].get_element();
                            el.data[1] = data[1].get_element();
                            return el;
                        }

                        element_fp6_2over3<Fp6_2over3T> Frobenius_map(const std::size_t power) const {
                            blueprint_linear_combination<base_field_type> new_c0c0, new_c0c1, new_c0c2, new_c1c0,
                                new_c1c1, new_c1c2;
                            new_c0c0.assign(this->bp, data[0].data[0]);
                            new_c0c1.assign(this->bp,
                                            data[0].data[1] * Fp3T::value_type::Frobenius_coeffs_c1[power % 3]);
                            new_c0c2.assign(this->bp,
                                            data[0].data[2] * Fp3T::value_type::Frobenius_coeffs_c2[power % 3]);
                            new_c1c0.assign(this->bp,
                                            data[1].data[0] * Fp6_2over3T::value_type::Frobenius_coeffs_c1[power % 6]);
                            new_c1c1.assign(this->bp,
                                            data[1].data[1] * (Fp6_2over3T::value_type::Frobenius_coeffs_c1[power % 6] *
                                                               Fp3T::value_type::Frobenius_coeffs_c1[power % 3]));
                            new_c1c2.assign(this->bp,
                                            data[1].data[2] * (Fp6_2over3T::value_type::Frobenius_coeffs_c1[power % 6] *
                                                               Fp3T::value_type::Frobenius_coeffs_c2[power % 3]));

                            return element_fp6_2over3<Fp6_2over3T>(
                                this->bp,
                                Fp3_variable<Fp3T>(this->bp, new_c0c0, new_c0c1, new_c0c2),
                                Fp3_variable<Fp3T>(this->bp, new_c1c0, new_c1c1, new_c1c2));
                        }

                        void evaluate() const {
                            data[0].evaluate();
                            data[1].evaluate();
                        }
                    };

                    /******************************** Fp6_2over3_mul_component ************************************/

                    /**
                     * Component that creates constraints for Fp6 multiplication.
                     */
                    template<typename Fp6_2over3T>
                    struct Fp6_2over3_mul_component : public component<typename Fp6_2over3T::base_field_type> {
                        using base_field_type = typename Fp6_2over3T::base_field_type;
                        typedef typename Fp6_2over3T::underlying_field_type Fp3T;

                        element_fp6_2over3<Fp6_2over3T> A;
                        element_fp6_2over3<Fp6_2over3T> B;
                        element_fp6_2over3<Fp6_2over3T> result;

                        blueprint_linear_combination<base_field_type> v0_c0;
                        blueprint_linear_combination<base_field_type> v0_c1;
                        blueprint_linear_combination<base_field_type> v0_c2;

                        blueprint_linear_combination<base_field_type> Ac0_plus_Ac1_c0;
                        blueprint_linear_combination<base_field_type> Ac0_plus_Ac1_c1;
                        blueprint_linear_combination<base_field_type> Ac0_plus_Ac1_c2;
                        std::shared_ptr<Fp3_variable<Fp3T>> Ac0_plus_Ac1;

                        std::shared_ptr<Fp3_variable<Fp3T>> v0;
                        std::shared_ptr<Fp3_variable<Fp3T>> v1;

                        blueprint_linear_combination<base_field_type> Bc0_plus_Bc1_c0;
                        blueprint_linear_combination<base_field_type> Bc0_plus_Bc1_c1;
                        blueprint_linear_combination<base_field_type> Bc0_plus_Bc1_c2;
                        std::shared_ptr<Fp3_variable<Fp3T>> Bc0_plus_Bc1;

                        blueprint_linear_combination<base_field_type> result_c1_plus_v0_plus_v1_c0;
                        blueprint_linear_combination<base_field_type> result_c1_plus_v0_plus_v1_c1;
                        blueprint_linear_combination<base_field_type> result_c1_plus_v0_plus_v1_c2;
                        std::shared_ptr<Fp3_variable<Fp3T>> result_c1_plus_v0_plus_v1;

                        std::shared_ptr<Fp3_mul_component<Fp3T>> compute_v0;
                        std::shared_ptr<Fp3_mul_component<Fp3T>> compute_v1;
                        std::shared_ptr<Fp3_mul_component<Fp3T>> compute_result_c1;

                        Fp6_2over3_mul_component(blueprint<base_field_type> &bp,
                                                 const element_fp6_2over3<Fp6_2over3T> &A,
                                                 const element_fp6_2over3<Fp6_2over3T> &B,
                                                 const element_fp6_2over3<Fp6_2over3T> &result) :
                            component<base_field_type>(bp),
                            A(A), B(B), result(result) {
                            /*
                                Karatsuba multiplication for Fp6 as a quadratic extension of Fp3:
                                    v0 = A.data[0] * B.data[0]
                                    v1 = A.data[1] * B.data[1]
                                    result.data[0] = v0 + non_residue * v1
                                    result.data[1] = (A.data[0] + A.data[1]) * (B.data[0] + B.data[1]) - v0 - v1
                                where "non_residue * elem" := (non_residue * elem.data[2], elem.data[0], elem.data[1])

                                Enforced with 3 Fp3_mul_component's that ensure that:
                                    A.data[1] * B.data[1] = v1
                                    A.data[0] * B.data[0] = v0
                                    (A.data[0]+A.data[1])*(B.data[0]+B.data[1]) = result.data[1] + v0 + v1

                                Reference:
                                    "Multiplication and Squaring on Pairing-Friendly Fields"
                                    Devegili, OhEigeartaigh, Scott, Dahab
                            */
                            v1.reset(new Fp3_variable<Fp3T>(bp));

                            compute_v1.reset(new Fp3_mul_component<Fp3T>(bp, A.data[1], B.data[1], *v1));

                            v0_c0.assign(bp, result.data[0].data[0] -
                                                 Fp6_2over3T::value_type::one().non_residue * v1->data[2]);
                            // while constepr is not ready
                            // must be:
                            // v0_c0.assign(bp, result.data[0].data[0] - Fp6_2over3T::value_type::non_residue *
                            // v1->data[2]);

                            v0_c1.assign(bp, result.data[0].data[1] - v1->data[0]);
                            v0_c2.assign(bp, result.data[0].data[2] - v1->data[1]);
                            v0.reset(new Fp3_variable<Fp3T>(bp, v0_c0, v0_c1, v0_c2));

                            compute_v0.reset(new Fp3_mul_component<Fp3T>(bp, A.data[0], B.data[0], *v0));

                            Ac0_plus_Ac1_c0.assign(bp, A.data[0].data[0] + A.data[1].data[0]);
                            Ac0_plus_Ac1_c1.assign(bp, A.data[0].data[1] + A.data[1].data[1]);
                            Ac0_plus_Ac1_c2.assign(bp, A.data[0].data[2] + A.data[1].data[2]);
                            Ac0_plus_Ac1.reset(
                                new Fp3_variable<Fp3T>(bp, Ac0_plus_Ac1_c0, Ac0_plus_Ac1_c1, Ac0_plus_Ac1_c2));

                            Bc0_plus_Bc1_c0.assign(bp, B.data[0].data[0] + B.data[1].data[0]);
                            Bc0_plus_Bc1_c1.assign(bp, B.data[0].data[1] + B.data[1].data[1]);
                            Bc0_plus_Bc1_c2.assign(bp, B.data[0].data[2] + B.data[1].data[2]);
                            Bc0_plus_Bc1.reset(
                                new Fp3_variable<Fp3T>(bp, Bc0_plus_Bc1_c0, Bc0_plus_Bc1_c1, Bc0_plus_Bc1_c2));

                            result_c1_plus_v0_plus_v1_c0.assign(bp, result.data[1].data[0] + v0->data[0] + v1->data[0]);
                            result_c1_plus_v0_plus_v1_c1.assign(bp, result.data[1].data[1] + v0->data[1] + v1->data[1]);
                            result_c1_plus_v0_plus_v1_c2.assign(bp, result.data[1].data[2] + v0->data[2] + v1->data[2]);
                            result_c1_plus_v0_plus_v1.reset(new Fp3_variable<Fp3T>(bp,
                                                                                   result_c1_plus_v0_plus_v1_c0,
                                                                                   result_c1_plus_v0_plus_v1_c1,
                                                                                   result_c1_plus_v0_plus_v1_c2));

                            compute_result_c1.reset(new Fp3_mul_component<Fp3T>(bp, *Ac0_plus_Ac1, *Bc0_plus_Bc1,
                                                                                *result_c1_plus_v0_plus_v1));
                        }

                        void generate_r1cs_constraints() {
                            compute_v0->generate_r1cs_constraints();
                            compute_v1->generate_r1cs_constraints();
                            compute_result_c1->generate_r1cs_constraints();
                        }

                        void generate_r1cs_witness() {
                            compute_v0->generate_r1cs_witness();
                            compute_v1->generate_r1cs_witness();

                            Ac0_plus_Ac1_c0.evaluate(this->bp);
                            Ac0_plus_Ac1_c1.evaluate(this->bp);
                            Ac0_plus_Ac1_c2.evaluate(this->bp);

                            Bc0_plus_Bc1_c0.evaluate(this->bp);
                            Bc0_plus_Bc1_c1.evaluate(this->bp);
                            Bc0_plus_Bc1_c2.evaluate(this->bp);

                            compute_result_c1->generate_r1cs_witness();

                            const typename Fp6_2over3T::value_type Aval = A.get_element();
                            const typename Fp6_2over3T::value_type Bval = B.get_element();
                            const typename Fp6_2over3T::value_type Rval = Aval * Bval;

                            result.generate_r1cs_witness(Rval);

                            result_c1_plus_v0_plus_v1_c0.evaluate(this->bp);
                            result_c1_plus_v0_plus_v1_c1.evaluate(this->bp);
                            result_c1_plus_v0_plus_v1_c2.evaluate(this->bp);

                            compute_result_c1->generate_r1cs_witness();
                        }
                    };

                    /******************************** Fp6_2over3_mul_by_2345_component
                     * ************************************/

                    /**
                     * Component that creates constraints for Fp6 multiplication by a Fp6 element B for which
                     * B.data[0].data[0] = B.data[0].data[1] = 0.
                     */
                    template<typename Fp6_2over3T>
                    struct Fp6_2over3_mul_by_2345_component : public component<typename Fp6_2over3T::base_field_type> {
                        using base_field_type = typename Fp6_2over3T::base_field_type;
                        typedef typename Fp6_2over3T::underlying_field_type Fp3T;

                        element_fp6_2over3<Fp6_2over3T> A;
                        element_fp6_2over3<Fp6_2over3T> B;
                        element_fp6_2over3<Fp6_2over3T> result;

                        blueprint_linear_combination<base_field_type> v0_c0;
                        blueprint_linear_combination<base_field_type> v0_c1;
                        blueprint_linear_combination<base_field_type> v0_c2;

                        blueprint_linear_combination<base_field_type> Ac0_plus_Ac1_c0;
                        blueprint_linear_combination<base_field_type> Ac0_plus_Ac1_c1;
                        blueprint_linear_combination<base_field_type> Ac0_plus_Ac1_c2;
                        std::shared_ptr<Fp3_variable<Fp3T>> Ac0_plus_Ac1;

                        std::shared_ptr<Fp3_variable<Fp3T>> v0;
                        std::shared_ptr<Fp3_variable<Fp3T>> v1;

                        blueprint_linear_combination<base_field_type> Bc0_plus_Bc1_c0;
                        blueprint_linear_combination<base_field_type> Bc0_plus_Bc1_c1;
                        blueprint_linear_combination<base_field_type> Bc0_plus_Bc1_c2;
                        std::shared_ptr<Fp3_variable<Fp3T>> Bc0_plus_Bc1;

                        blueprint_linear_combination<base_field_type> result_c1_plus_v0_plus_v1_c0;
                        blueprint_linear_combination<base_field_type> result_c1_plus_v0_plus_v1_c1;
                        blueprint_linear_combination<base_field_type> result_c1_plus_v0_plus_v1_c2;
                        std::shared_ptr<Fp3_variable<Fp3T>> result_c1_plus_v0_plus_v1;

                        std::shared_ptr<Fp3_mul_component<Fp3T>> compute_v1;
                        std::shared_ptr<Fp3_mul_component<Fp3T>> compute_result_c1;

                        Fp6_2over3_mul_by_2345_component(blueprint<base_field_type> &bp,
                                                         const element_fp6_2over3<Fp6_2over3T> &A,
                                                         const element_fp6_2over3<Fp6_2over3T> &B,
                                                         const element_fp6_2over3<Fp6_2over3T> &result) :
                            component<base_field_type>(bp),
                            A(A), B(B), result(result) {
                            /*
                                Karatsuba multiplication for Fp6 as a quadratic extension of Fp3:
                                    v0 = A.data[0] * B.data[0]
                                    v1 = A.data[1] * B.data[1]
                                    result.data[0] = v0 + non_residue * v1
                                    result.data[1] = (A.data[0] + A.data[1]) * (B.data[0] + B.data[1]) - v0 - v1
                                where "non_residue * elem" := (non_residue * elem.data[2], elem.data[0], elem.data[1])

                                We know that B.data[0].data[0] = B.data[0].data[1] = 0

                                Enforced with 2 Fp3_mul_component's that ensure that:
                                    A.data[1] * B.data[1] = v1
                                    (A.data[0]+A.data[1])*(B.data[0]+B.data[1]) = result.data[1] + v0 + v1

                                And one multiplication (three direct constraints) that enforces A.data[0] * B.data[0]
                                = v0, where B.data[0].data[0] = B.data[0].data[1] = 0.

                                Note that (u + v * X + t * X^2) * (0 + 0 * X + z * X^2) =
                                (v * z * non_residue + t * z * non_residue * X + u * z * X^2)

                                Reference:
                                    "Multiplication and Squaring on Pairing-Friendly Fields"
                                    Devegili, OhEigeartaigh, Scott, Dahab
                            */
                            v1.reset(new Fp3_variable<Fp3T>(bp));
                            compute_v1.reset(new Fp3_mul_component<Fp3T>(bp, A.data[1], B.data[1], *v1));

                            /* we inline result.data[0] in v0 as follows: v0 = (result.data[0].data[0] -
                             * Fp6_2over3T::value_type::non_residue * v1->data[2],
                             * result.data[0].data[1] - v1->data[0], result.data[0].data[2] - v1->data[1]) */
                            v0.reset(new Fp3_variable<Fp3T>(bp));

                            Ac0_plus_Ac1_c0.assign(bp, A.data[0].data[0] + A.data[1].data[0]);
                            Ac0_plus_Ac1_c1.assign(bp, A.data[0].data[1] + A.data[1].data[1]);
                            Ac0_plus_Ac1_c2.assign(bp, A.data[0].data[2] + A.data[1].data[2]);
                            Ac0_plus_Ac1.reset(
                                new Fp3_variable<Fp3T>(bp, Ac0_plus_Ac1_c0, Ac0_plus_Ac1_c1, Ac0_plus_Ac1_c2));

                            Bc0_plus_Bc1_c0.assign(bp, B.data[0].data[0] + B.data[1].data[0]);
                            Bc0_plus_Bc1_c1.assign(bp, B.data[0].data[1] + B.data[1].data[1]);
                            Bc0_plus_Bc1_c2.assign(bp, B.data[0].data[2] + B.data[1].data[2]);
                            Bc0_plus_Bc1.reset(
                                new Fp3_variable<Fp3T>(bp, Bc0_plus_Bc1_c0, Bc0_plus_Bc1_c1, Bc0_plus_Bc1_c2));

                            result_c1_plus_v0_plus_v1_c0.assign(bp, result.data[1].data[0] + v0->data[0] + v1->data[0]);
                            result_c1_plus_v0_plus_v1_c1.assign(bp, result.data[1].data[1] + v0->data[1] + v1->data[1]);
                            result_c1_plus_v0_plus_v1_c2.assign(bp, result.data[1].data[2] + v0->data[2] + v1->data[2]);
                            result_c1_plus_v0_plus_v1.reset(new Fp3_variable<Fp3T>(bp,
                                                                                   result_c1_plus_v0_plus_v1_c0,
                                                                                   result_c1_plus_v0_plus_v1_c1,
                                                                                   result_c1_plus_v0_plus_v1_c2));

                            compute_result_c1.reset(new Fp3_mul_component<Fp3T>(bp, *Ac0_plus_Ac1, *Bc0_plus_Bc1,
                                                                                *result_c1_plus_v0_plus_v1));
                        }

                        void generate_r1cs_constraints() {
                            compute_v1->generate_r1cs_constraints();
                            this->bp.add_r1cs_constraint(r1cs_constraint<base_field_type>(
                                A.data[0].data[1], Fp3T::value_type::non_residue * B.data[0].data[2],
                                result.data[0].data[0] - Fp6_2over3T::value_type::non_residue * v1->data[2]));

                            this->bp.add_r1cs_constraint(r1cs_constraint<base_field_type>(
                                A.data[0].data[2], Fp3T::value_type::non_residue * B.data[0].data[2],
                                result.data[0].data[1] - v1->data[0]));

                            this->bp.add_r1cs_constraint(r1cs_constraint<base_field_type>(
                                A.data[0].data[0], B.data[0].data[2], result.data[0].data[2] - v1->data[1]));
                            compute_result_c1->generate_r1cs_constraints();
                        }

                        void generate_r1cs_witness() {
                            compute_v1->generate_r1cs_witness();

                            const typename Fp3T::value_type A_c0_val = A.data[0].get_element();
                            const typename Fp3T::value_type B_c0_val = B.data[0].get_element();
                            assert(B_c0_val.data[0].is_zero());
                            assert(B_c0_val.data[1].is_zero());

                            const typename Fp3T::value_type v0_val = A_c0_val * B_c0_val;
                            v0->generate_r1cs_witness(v0_val);

                            Ac0_plus_Ac1_c0.evaluate(this->bp);
                            Ac0_plus_Ac1_c1.evaluate(this->bp);
                            Ac0_plus_Ac1_c2.evaluate(this->bp);

                            Bc0_plus_Bc1_c0.evaluate(this->bp);
                            Bc0_plus_Bc1_c1.evaluate(this->bp);
                            Bc0_plus_Bc1_c2.evaluate(this->bp);

                            compute_result_c1->generate_r1cs_witness();

                            const typename Fp6_2over3T::value_type Aval = A.get_element();
                            const typename Fp6_2over3T::value_type Bval = B.get_element();
                            const typename Fp6_2over3T::value_type Rval = Aval * Bval;

                            result.generate_r1cs_witness(Rval);

                            result_c1_plus_v0_plus_v1_c0.evaluate(this->bp);
                            result_c1_plus_v0_plus_v1_c1.evaluate(this->bp);
                            result_c1_plus_v0_plus_v1_c2.evaluate(this->bp);

                            compute_result_c1->generate_r1cs_witness();
                        }
                    };

                    /******************************** Fp6_2over3_sqr_component ************************************/

                    /**
                     * Component that creates constraints for Fp6 squaring.
                     */
                    template<typename Fp6_2over3T>
                    struct Fp6_2over3_sqr_component : public component<typename Fp6_2over3T::base_field_type> {
                        using base_field_type = typename Fp6_2over3T::base_field_type;

                        element_fp6_2over3<Fp6_2over3T> A;
                        element_fp6_2over3<Fp6_2over3T> result;

                        std::shared_ptr<Fp6_2over3_mul_component<Fp6_2over3T>> mul;

                        Fp6_2over3_sqr_component(blueprint<base_field_type> &bp,
                                                 const element_fp6_2over3<Fp6_2over3T> &A,
                                                 const element_fp6_2over3<Fp6_2over3T> &result) :
                            component<base_field_type>(bp),
                            A(A), result(result) {
                            mul.reset(new Fp6_2over3_mul_component<Fp6_2over3T>(bp, A, A, result));
                        }

                        void generate_r1cs_constraints() {
                            mul->generate_r1cs_constraints();
                        }

                        void generate_r1cs_witness() {
                            mul->generate_r1cs_witness();
                        }
                    };

                    /******************************** Fp6_2over3_cyclotomic_sqr_component
                     * ************************************/

                    /**
                     * Component that creates constraints for Fp6 cyclotomic squaring
                     */
                    template<typename Fp6_2over3T>
                    struct Fp6_2over3_cyclotomic_sqr_component
                        : public component<typename Fp6_2over3T::base_field_type> {
                        typedef typename Fp6_2over3T::base_field_type base_field_type;
                        typedef typename Fp6_2over3T::underlying_field_type Fp2T;

                        element_fp6_2over3<Fp6_2over3T> A;
                        element_fp6_2over3<Fp6_2over3T> result;

                        std::shared_ptr<element_fp2<Fp2T>> a;
                        std::shared_ptr<element_fp2<Fp2T>> b;
                        std::shared_ptr<element_fp2<Fp2T>> c;

                        blueprint_linear_combination<base_field_type> asq_c0;
                        blueprint_linear_combination<base_field_type> asq_c1;

                        blueprint_linear_combination<base_field_type> bsq_c0;
                        blueprint_linear_combination<base_field_type> bsq_c1;

                        blueprint_linear_combination<base_field_type> csq_c0;
                        blueprint_linear_combination<base_field_type> csq_c1;

                        std::shared_ptr<element_fp2<Fp2T>> asq;
                        std::shared_ptr<element_fp2<Fp2T>> bsq;
                        std::shared_ptr<element_fp2<Fp2T>> csq;

                        std::shared_ptr<Fp2_sqr_component<Fp2T>> compute_asq;
                        std::shared_ptr<Fp2_sqr_component<Fp2T>> compute_bsq;
                        std::shared_ptr<Fp2_sqr_component<Fp2T>> compute_csq;

                        Fp6_2over3_cyclotomic_sqr_component(blueprint<base_field_type> &bp,
                                                            const element_fp6_2over3<Fp6_2over3T> &A,
                                                            const element_fp6_2over3<Fp6_2over3T> &result) :
                            component<base_field_type>(bp),
                            A(A), result(result) {
                            /*
                                underlying_field_type a = underlying_field_type(data[0].data[0], data[1].data[1]);
                                underlying_field_type b = underlying_field_type(data[1].data[0], data[0].data[2]);
                                underlying_field_type c = underlying_field_type(data[0].data[1], data[1].data[2]);

                                underlying_field_type asq = a.squared();
                                underlying_field_type bsq = b.squared();
                                underlying_field_type csq = c.squared();

                                result.data[0].data[0] = 3 * asq_a - 2 * a_a;
                                result.data[1].data[1] = 3 * asq_b + 2 * a_b;

                                result.data[0].data[1] = 3 * bsq_a - 2 * c_a;
                                result.data[1].data[2] = 3 * bsq_b + 2 * c_b;

                                result.data[0].data[2] = 3 * csq_a - 2 * b_b;
                                result.data[1].data[0] = 3 * my_Fp3::non_residue * csq_b + 2 * b_a;

                                return Fp6_2over3_model<n, mbodulus>(my_Fp3(A_a, C_a, B_b),
                                                                     my_Fp3(B_a, A_b, C_b))
                            */
                            a.reset(new element_fp2<Fp2T>(bp, A.data[0].data[0], A.data[1].data[1]));
                            b.reset(new element_fp2<Fp2T>(bp, A.data[1].data[0], A.data[0].data[2]));
                            c.reset(new element_fp2<Fp2T>(bp, A.data[0].data[1], A.data[1].data[2]));

                            asq_c0.assign(bp, (result.data[0].data[0] + 2 * a->data[0]) *
                                                  typename base_field_type::value_type(3).inversed());
                            asq_c1.assign(bp, (result.data[1].data[1] - 2 * a->data[1]) *
                                                  typename base_field_type::value_type(3).inversed());

                            bsq_c0.assign(bp, (result.data[0].data[1] + 2 * c->data[0]) *
                                                  typename base_field_type::value_type(3).inversed());
                            bsq_c1.assign(bp, (result.data[1].data[2] - 2 * c->data[1]) *
                                                  typename base_field_type::value_type(3).inversed());

                            csq_c0.assign(bp, (result.data[0].data[2] + 2 * b->data[1]) *
                                                  typename base_field_type::value_type(3).inversed());
                            csq_c1.assign(bp,
                                          (result.data[1].data[0] - 2 * b->data[0]) *
                                              (typename base_field_type::value_type(3) * Fp2T::value_type::non_residue)
                                                  .inversed());

                            asq.reset(new element_fp2<Fp2T>(bp, asq_c0, asq_c1));
                            bsq.reset(new element_fp2<Fp2T>(bp, bsq_c0, bsq_c1));
                            csq.reset(new element_fp2<Fp2T>(bp, csq_c0, csq_c1));

                            compute_asq.reset(new Fp2_sqr_component<Fp2T>(bp, *a, *asq));
                            compute_bsq.reset(new Fp2_sqr_component<Fp2T>(bp, *b, *bsq));
                            compute_csq.reset(new Fp2_sqr_component<Fp2T>(bp, *c, *csq));
                        }

                        void generate_r1cs_constraints() {
                            compute_asq->generate_r1cs_constraints();
                            compute_bsq->generate_r1cs_constraints();
                            compute_csq->generate_r1cs_constraints();
                        }

                        void generate_r1cs_witness() {
                            const typename Fp6_2over3T::value_type Aval = A.get_element();
                            const typename Fp6_2over3T::value_type Rval = Aval.cyclotomic_squared();

                            result.generate_r1cs_witness(Rval);

                            asq->evaluate();
                            bsq->evaluate();
                            csq->evaluate();

                            compute_asq->generate_r1cs_witness();
                            compute_bsq->generate_r1cs_witness();
                            compute_csq->generate_r1cs_witness();
                        }
                    };
                }    // namespace components
            }        // namespace snark
        }            // namespace zk
    }                // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_ZK_FP6_2OVER3_COMPONENTS_HPP