//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
// Copyright (c) 2022 Alisa Cherniaeva <a.cherniaeva@nil.foundation>
// Copyright (c) 2022 Ekaterina Chukavina <kate@nil.foundation>
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
// @file Declaration of interfaces for auxiliary components for the SHA512 component.
//---------------------------------------------------------------------------//

#ifndef ACTOR_ZK_BLUEPRINT_PLONK_SHA512_HPP
#define ACTOR_ZK_BLUEPRINT_PLONK_SHA512_HPP

#include <nil/actor/zk/blueprint/plonk.hpp>
#include <nil/actor/zk/assignment/plonk.hpp>
#include <nil/actor/zk/components/hashes/sha256/plonk/sha512_process.hpp>
// #include <nil/actor/zk/components/hashes/sha256/plonk/decomposition.hpp>

namespace nil {
    namespace actor {
        namespace zk {
            namespace components {

                template<typename ArithmetizationType, typename CurveType, std::size_t... WireIndexes>
                class sha512;

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
                         std::size_t W8>
                class sha512<snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>,
                             CurveType,
                             W0,
                             W1,
                             W2,
                             W3,
                             W4,
                             W5,
                             W6,
                             W7,
                             W8> {

                    typedef snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>
                        ArithmetizationType;

                    using var = snark::plonk_variable<BlueprintFieldType>;

                    using sha512_process_component =
                        sha512_process<ArithmetizationType, CurveType, W0, W1, W2, W3, W4, W5, W6, W7, W8>;
                    //                   using decomposition_component =
                    //                       decomposition<ArithmetizationType, BlueprintFieldType, W0, W1, W2, W3, W4,
                    //                       W5, W6, W7, W8>;

                public:
                    constexpr static const std::size_t gates_amount = 5;
                    constexpr static const std::size_t selector_seed = 0x0f14;
                    constexpr static const std::size_t rows_amount_creating_input_words_component = 15;
                    //
                    constexpr static const std::size_t rows_amount =
                        rows_amount_creating_input_words_component +
                        sha512_process_component::rows_amount * 2 /* + 2 */;

                    struct var_ec_point {
                        std::array<var, 4> x;
                        std::array<var, 4> y;
                    };

                    struct params_type {
                        var_ec_point R;
                        var_ec_point A;
                        std::array<var, 4> M;
                    };

                    struct result_type {
                        std::array<var, 8> output_state;

                        result_type(const std::size_t &start_row_index) {
                            output_state = {var(W0, start_row_index + rows_amount - 3, false),
                                            var(W1, start_row_index + rows_amount - 3, false),
                                            var(W2, start_row_index + rows_amount - 3, false),
                                            var(W3, start_row_index + rows_amount - 3, false),
                                            var(W0, start_row_index + rows_amount - 1, false),
                                            var(W1, start_row_index + rows_amount - 1, false),
                                            var(W2, start_row_index + rows_amount - 1, false),
                                            var(W3, start_row_index + rows_amount - 1, false)};
                        }
                    };
                    static result_type
                        generate_circuit(blueprint<ArithmetizationType> &bp,
                                         blueprint_public_assignment_table<ArithmetizationType> &assignment,
                                         const params_type &params,
                                         const std::size_t start_row_index) {

                        auto selector_iterator = assignment.find_selector(selector_seed);
                        std::size_t first_selector_index;

                        if (selector_iterator == assignment.selectors_end()) {
                            first_selector_index = assignment.allocate_selector(selector_seed, gates_amount);
                            generate_gates(bp, assignment, first_selector_index);
                        } else {
                            first_selector_index = selector_iterator->second;
                        }

                        std::size_t j = start_row_index;

                        assignment.enable_selector(first_selector_index, j + 1);
                        assignment.enable_selector(first_selector_index + 1, j + 4);
                        assignment.enable_selector(first_selector_index + 2, j + 7);
                        assignment.enable_selector(first_selector_index + 3, j + 10);
                        assignment.enable_selector(first_selector_index + 4, j + 13);
                        /*
                                                std::array<var, 16> input_words_vars_1;

                                                for(std::size_t k = 0; k < 4; k++) {
                                                    for(std::size_t i = 0; i < 4; i++) {
                                                        input_words_vars_1[4*k + i] = var(2*i, start_row_index + 1 +
                           3*k, false);
                                                    }
                                                }

                                                std::array<var, 8> constants_var = {var(0, start_row_index, false,
                           var::column_type::constant), var(0, start_row_index + 1, false, var::column_type::constant),
                                                                                    var(0, start_row_index + 2, false,
                           var::column_type::constant), var(0, start_row_index + 3, false, var::column_type::constant),
                                                                                    var(0, start_row_index + 4, false,
                           var::column_type::constant), var(0, start_row_index + 5, false, var::column_type::constant),
                                                                                    var(0, start_row_index + 6, false,
                           var::column_type::constant), var(0, start_row_index + 7, false, var::column_type::constant)};
                                                typename sha512_process_component::params_type sha_params =
                           {constants_var, input_words_vars_1}; j = j + 15; auto sha_output =
                           sha512_process_component::generate_circuit(bp, assignment, sha_params, j).output_state; j +=
                           sha512_process_component::rows_amount;

                                                // second chunk
                                                std::array<var, 16> input_words_vars_2;

                                                for(std::size_t i = 0; i < 4; i++) {
                                                    input_words_vars_2[i] = var(2*i, start_row_index + 1 + 12, false);
                                                }

                                                for (std::size_t i = 4; i < 15; i++) {
                                                    input_words_vars_2[i] = var(0, start_row_index + 8, false,
                           var::column_type::constant);
                                                }
                                                input_words_vars_2[15] = var(0, start_row_index + 9, false,
                           var::column_type::constant);


                                                sha_params = {sha_output, input_words_vars_2};
                                                sha512_process_component::generate_circuit(bp, assignment, sha_params,
                           j);

                        */

                        generate_copy_constraints(bp, assignment, params, start_row_index);
                        return result_type(start_row_index);
                    }

                    static result_type generate_assignments(blueprint_assignment_table<ArithmetizationType> &assignment,
                                                            const params_type &params,
                                                            std::size_t component_start_row) {
                        std::size_t row = component_start_row;

                        std::array<typename ArithmetizationType::field_type::integral_type, 20> RAM = {
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.R.x[0]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.R.x[1]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.R.x[2]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.R.x[3]).data),

                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.R.y[0]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.R.y[1]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.R.y[2]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.R.y[3]).data),

                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.A.x[0]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.A.x[1]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.A.x[2]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.A.x[3]).data),

                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.A.y[0]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.A.y[1]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.A.y[2]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.A.y[3]).data),

                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.M[0]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.M[1]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.M[2]).data),
                            typename ArithmetizationType::field_type::integral_type(
                                assignment.var_value(params.M[3]).data)};

                        std::array<typename ArithmetizationType::field_type::integral_type, 32> input_words_values;
                        typename ArithmetizationType::field_type::integral_type integral_one = 1;
                        typename ArithmetizationType::field_type::integral_type mask = ((integral_one << 64) - 1);
                        input_words_values[0] = (RAM[0]) & mask;
                        input_words_values[1] = ((RAM[0] >> 64) + (RAM[1] << 2)) & mask;
                        input_words_values[2] = ((RAM[1] >> 62) + (RAM[2] << 4)) & mask;
                        input_words_values[3] = ((RAM[2] >> 60) + (RAM[3] << 6) + (RAM[4] << 63)) & mask;
                        input_words_values[4] = ((RAM[4] >> 1)) & mask;
                        input_words_values[5] = ((RAM[4] >> 65) + (RAM[5] << 1)) & mask;
                        input_words_values[6] = ((RAM[5] >> 63) + (RAM[6] << 3)) & mask;
                        input_words_values[7] = ((RAM[6] >> 61) + (RAM[7] << 5) + (RAM[8] << 62)) & mask;
                        input_words_values[8] = ((RAM[8] >> 2)) & mask;
                        input_words_values[9] = ((RAM[9])) & mask;
                        input_words_values[10] = ((RAM[9] >> 64) + (RAM[10] << 2)) & mask;
                        input_words_values[11] = ((RAM[10] >> 62) + (RAM[11] << 4) + (RAM[12] << 61)) & mask;
                        input_words_values[12] = ((RAM[12] >> 3) + (RAM[13] << 63)) & mask;
                        input_words_values[13] = ((RAM[13] >> 1)) & mask;
                        input_words_values[14] = ((RAM[13] >> 65) + (RAM[14] << 1)) & mask;
                        input_words_values[15] = ((RAM[14] >> 63) + (RAM[15] << 3) + (RAM[16] << 60)) & mask;
                        input_words_values[16] = ((RAM[16] >> 4) + (RAM[17] << 62)) & mask;
                        input_words_values[17] = ((RAM[17] >> 2)) & mask;
                        input_words_values[18] = ((RAM[18])) & mask;
                        input_words_values[19] = ((RAM[18] >> 64) + (RAM[19] << 2) + (integral_one << 60));

                        for (std::size_t i = 20; i < 31; ++i) {
                            input_words_values[i] = 0;
                        }
                        input_words_values[31] = 1024 + 252;

                        std::array<typename ArithmetizationType::field_type::integral_type, 77> range_chunks;

                        typename ArithmetizationType::field_type::integral_type mask22 = ((integral_one << 22) - 1);
                        typename ArithmetizationType::field_type::integral_type mask21 = ((integral_one << 21) - 1);
                        typename ArithmetizationType::field_type::integral_type mask20 = ((integral_one << 20) - 1);
                        typename ArithmetizationType::field_type::integral_type mask19 = ((integral_one << 19) - 1);
                        typename ArithmetizationType::field_type::integral_type mask18 = ((integral_one << 18) - 1);
                        typename ArithmetizationType::field_type::integral_type mask17 = ((integral_one << 17) - 1);
                        typename ArithmetizationType::field_type::integral_type mask16 = ((integral_one << 16) - 1);
                        typename ArithmetizationType::field_type::integral_type mask15 = ((integral_one << 15) - 1);
                        typename ArithmetizationType::field_type::integral_type mask14 = ((integral_one << 14) - 1);
                        typename ArithmetizationType::field_type::integral_type mask13 = ((integral_one << 13) - 1);
                        typename ArithmetizationType::field_type::integral_type mask12 = ((integral_one << 12) - 1);
                        typename ArithmetizationType::field_type::integral_type mask11 = ((integral_one << 11) - 1);
                        typename ArithmetizationType::field_type::integral_type mask10 = ((integral_one << 10) - 1);
                        typename ArithmetizationType::field_type::integral_type mask9 = ((integral_one << 9) - 1);
                        typename ArithmetizationType::field_type::integral_type mask8 = ((integral_one << 8) - 1);

                        auto row_witness = row + 1;

                        // W0,1     W1,1                    W1,0                 W1, -1
                        // 12|34567890123456789012.3456789012345678901234.5678901234567890123456
                        range_chunks[0] = RAM[0] & mask22;
                        range_chunks[1] = (RAM[0] >> 22) & mask22;
                        range_chunks[2] = (RAM[0] >> 44) & mask20;
                        range_chunks[3] = (RAM[0] >> 64) & 0b11;

                        assignment.witness(W0)[row_witness - 1] = RAM[0];
                        assignment.witness(W0)[row_witness - 0] = input_words_values[0];
                        assignment.witness(W1)[row_witness - 1] = range_chunks[0];
                        assignment.witness(W1)[row_witness - 0] = range_chunks[1];
                        assignment.witness(W1)[row_witness + 1] = range_chunks[2];
                        assignment.witness(W0)[row_witness + 1] = range_chunks[3];

                        // W2,1       W3,1                  W3,0                  W3, -1
                        // 1234|567890123456789012.3456789012345678901234.5678901234567890123456
                        range_chunks[4] = (RAM[1]) & mask22;
                        range_chunks[5] = (RAM[1] >> 22) & mask22;
                        range_chunks[6] = (RAM[1] >> 44) & mask18;
                        range_chunks[7] = (RAM[1] >> 62) & 15;

                        assignment.witness(W2)[row_witness - 1] = RAM[1];
                        assignment.witness(W2)[row_witness - 0] = input_words_values[1];
                        assignment.witness(W3)[row_witness - 1] = range_chunks[4];
                        assignment.witness(W3)[row_witness - 0] = range_chunks[5];
                        assignment.witness(W3)[row_witness + 1] = range_chunks[6];
                        assignment.witness(W2)[row_witness + 1] = range_chunks[7];

                        //  W4,1        W5,1                W5,0                  W5, -1
                        // 123456|7890123456789012.3456789012345678901234.5678901234567890123456
                        range_chunks[8] = (RAM[2]) & mask22;
                        range_chunks[9] = (RAM[2] >> 22) & mask22;
                        range_chunks[10] = (RAM[2] >> 44) & mask16;
                        range_chunks[11] = (RAM[2] >> 60) & 0b111111;

                        assignment.witness(W4)[row_witness - 1] = RAM[2];
                        assignment.witness(W4)[row_witness - 0] = input_words_values[2];
                        assignment.witness(W5)[row_witness - 1] = range_chunks[8];
                        assignment.witness(W5)[row_witness - 0] = range_chunks[9];
                        assignment.witness(W5)[row_witness + 1] = range_chunks[10];
                        assignment.witness(W4)[row_witness + 1] = range_chunks[11];

                        //     W7, 1              W7, 0                 W7, -1
                        // 1234567890123.4567890123456789012345.6789012345678901234567
                        range_chunks[12] = (RAM[3]) & mask22;
                        range_chunks[13] = (RAM[3] >> 22) & mask22;
                        range_chunks[14] = (RAM[3] >> 44) & mask13;

                        assignment.witness(W6)[row_witness - 1] = RAM[3];
                        assignment.witness(W6)[row_witness - 0] = input_words_values[3];
                        assignment.witness(W7)[row_witness - 1] = range_chunks[12];
                        assignment.witness(W7)[row_witness - 0] = range_chunks[13];
                        assignment.witness(W7)[row_witness + 1] = range_chunks[14];

                        row_witness += 3;

                        // W0,1      W1,1                  W1,0               W1,-1             W8,-1
                        // 1|234567890123456789012.3456789012345678901234.567890123456789012345|6
                        range_chunks[15] = (RAM[4]) & 1;
                        range_chunks[16] = (RAM[4] >> 1) & mask21;
                        range_chunks[17] = (RAM[4] >> 22) & mask22;
                        range_chunks[18] = (RAM[4] >> 44) & mask21;
                        range_chunks[19] = (RAM[4] >> 65) & 1;

                        assignment.witness(W6)[row_witness - 3 + 1] = range_chunks[15];
                        assignment.witness(W8)[row_witness - 1] = range_chunks[15];

                        assignment.witness(W0)[row_witness - 1] = RAM[4];
                        assignment.witness(W0)[row_witness - 0] = input_words_values[4];
                        assignment.witness(W1)[row_witness - 1] = range_chunks[16];
                        assignment.witness(W1)[row_witness - 0] = range_chunks[17];
                        assignment.witness(W1)[row_witness + 1] = range_chunks[18];
                        assignment.witness(W0)[row_witness + 1] = range_chunks[19];

                        // W2,1       W3,1                  W3,0                  W3, -1
                        // 123|4567890123456789012.3456789012345678901234.5678901234567890123456
                        range_chunks[20] = (RAM[5]) & mask22;
                        range_chunks[21] = (RAM[5] >> 22) & mask22;
                        range_chunks[22] = (RAM[5] >> 44) & mask19;
                        range_chunks[23] = (RAM[5] >> 63) & 0b111;

                        assignment.witness(W2)[row_witness - 1] = RAM[5];
                        assignment.witness(W2)[row_witness - 0] = input_words_values[5];
                        assignment.witness(W3)[row_witness - 1] = range_chunks[20];
                        assignment.witness(W3)[row_witness - 0] = range_chunks[21];
                        assignment.witness(W3)[row_witness + 1] = range_chunks[22];
                        assignment.witness(W2)[row_witness + 1] = range_chunks[23];

                        //  W4,1        W5,1                W5,0                  W5, -1
                        // 12345|67890123456789012.3456789012345678901234.5678901234567890123456
                        range_chunks[24] = (RAM[6]) & mask22;
                        range_chunks[25] = (RAM[6] >> 22) & mask22;
                        range_chunks[26] = (RAM[6] >> 44) & mask17;
                        range_chunks[27] = (RAM[6] >> 61) & 0b11111;

                        assignment.witness(W4)[row_witness - 1] = RAM[6];
                        assignment.witness(W4)[row_witness - 0] = input_words_values[6];
                        assignment.witness(W5)[row_witness - 1] = range_chunks[24];
                        assignment.witness(W5)[row_witness - 0] = range_chunks[25];
                        assignment.witness(W5)[row_witness + 1] = range_chunks[26];
                        assignment.witness(W4)[row_witness + 1] = range_chunks[27];

                        //     W7, 1              W7, 0                 W7, -1
                        // 1234567890123.4567890123456789012345.6789012345678901234567
                        range_chunks[28] = (RAM[7]) & mask22;
                        range_chunks[29] = (RAM[7] >> 22) & mask22;
                        range_chunks[30] = (RAM[7] >> 44) & mask13;

                        assignment.witness(W6)[row_witness - 1] = RAM[7];
                        assignment.witness(W6)[row_witness - 0] = input_words_values[7];
                        assignment.witness(W7)[row_witness - 1] = range_chunks[28];
                        assignment.witness(W7)[row_witness - 0] = range_chunks[29];
                        assignment.witness(W7)[row_witness + 1] = range_chunks[30];

                        row_witness += 3;

                        //          W0,1                    W1,1                  W1,0        W1,-1
                        // |1234567890123456789012.3456789012345678901234.56789012345678901234|56
                        range_chunks[31] = RAM[8] & 0b11;
                        range_chunks[32] = (RAM[8] >> 2) & mask20;
                        range_chunks[33] = (RAM[8] >> 22) & mask22;
                        range_chunks[34] = (RAM[8] >> 44) & mask22;

                        assignment.witness(W6)[row_witness - 3 + 1] = range_chunks[31];

                        assignment.witness(W0)[row_witness - 1] = RAM[8];
                        assignment.witness(W0)[row_witness - 0] = input_words_values[8];
                        assignment.witness(W1)[row_witness - 1] = range_chunks[31];
                        assignment.witness(W1)[row_witness - 0] = range_chunks[32];
                        assignment.witness(W1)[row_witness + 1] = range_chunks[33];
                        assignment.witness(W0)[row_witness + 1] = range_chunks[34];

                        // W2,1       W3,1                  W3,0                  W3, -1
                        // 12|34567890123456789012.3456789012345678901234.5678901234567890123456
                        range_chunks[35] = (RAM[9]) & mask22;
                        range_chunks[36] = (RAM[9] >> 22) & mask22;
                        range_chunks[37] = (RAM[9] >> 44) & mask20;
                        range_chunks[38] = (RAM[9] >> 64) & 0b11;

                        assignment.witness(W2)[row_witness - 1] = RAM[9];
                        assignment.witness(W2)[row_witness - 0] = input_words_values[9];
                        assignment.witness(W3)[row_witness - 1] = range_chunks[35];
                        assignment.witness(W3)[row_witness - 0] = range_chunks[36];
                        assignment.witness(W3)[row_witness + 1] = range_chunks[37];
                        assignment.witness(W2)[row_witness + 1] = range_chunks[38];

                        // W4,1        W5,1                 W5,0                  W5, -1
                        // 1234|567890123456789012.3456789012345678901234.5678901234567890123456
                        range_chunks[39] = (RAM[10]) & mask22;
                        range_chunks[40] = (RAM[10] >> 22) & mask22;
                        range_chunks[41] = (RAM[10] >> 44) & mask18;
                        range_chunks[42] = (RAM[10] >> 62) & 0b1111;

                        assignment.witness(W4)[row_witness - 1] = RAM[10];
                        assignment.witness(W4)[row_witness - 0] = input_words_values[10];
                        assignment.witness(W5)[row_witness - 1] = range_chunks[39];
                        assignment.witness(W5)[row_witness - 0] = range_chunks[40];
                        assignment.witness(W5)[row_witness + 1] = range_chunks[41];
                        assignment.witness(W4)[row_witness + 1] = range_chunks[42];

                        //     W7, 1              W7, 0                 W7, -1
                        // 1234567890123.4567890123456789012345.6789012345678901234567
                        range_chunks[43] = (RAM[11]) & mask22;
                        range_chunks[44] = (RAM[11] >> 22) & mask22;
                        range_chunks[45] = (RAM[11] >> 44) & mask13;

                        assignment.witness(W6)[row_witness - 1] = RAM[11];
                        assignment.witness(W6)[row_witness - 0] = input_words_values[11];
                        assignment.witness(W7)[row_witness - 1] = range_chunks[43];
                        assignment.witness(W7)[row_witness - 0] = range_chunks[44];
                        assignment.witness(W7)[row_witness + 1] = range_chunks[45];

                        row_witness += 3;

                        //         W0,1                    W1,1                  W1,0        (W1,-1  &  W6,1-3)
                        // 1234567890123456789012.3456789012345678901234.5678901234567890123|456
                        range_chunks[46] = (RAM[12]) & 0b111;
                        range_chunks[47] = (RAM[12] >> 3) & mask19;
                        range_chunks[48] = (RAM[12] >> 22) & mask22;
                        range_chunks[49] = (RAM[12] >> 44) & mask22;

                        assignment.witness(W6)[row_witness - 3 + 1] = range_chunks[46];

                        assignment.witness(W0)[row_witness - 1] = RAM[12];
                        assignment.witness(W0)[row_witness - 0] = input_words_values[12];
                        assignment.witness(W1)[row_witness - 1] = range_chunks[46];
                        assignment.witness(W1)[row_witness - 0] = range_chunks[47];
                        assignment.witness(W1)[row_witness + 1] = range_chunks[48];
                        assignment.witness(W0)[row_witness + 1] = range_chunks[49];

                        // W2,1       W3,1                  W3,0                  W3, -1        W8, -1
                        // 1|234567890123456789012.3456789012345678901234.567890123456789012345|6
                        range_chunks[50] = (RAM[13]) & 1;
                        range_chunks[51] = (RAM[13] >> 1) & mask21;
                        range_chunks[52] = (RAM[13] >> 22) & mask22;
                        range_chunks[53] = (RAM[13] >> 44) & mask21;
                        range_chunks[54] = (RAM[13] >> 65) & 1;

                        assignment.witness(W2)[row_witness - 1] = RAM[13];
                        assignment.witness(W2)[row_witness - 0] = input_words_values[13];
                        assignment.witness(W8)[row_witness - 1] = range_chunks[50];
                        assignment.witness(W3)[row_witness - 1] = range_chunks[51];
                        assignment.witness(W3)[row_witness - 0] = range_chunks[52];
                        assignment.witness(W3)[row_witness + 1] = range_chunks[53];
                        assignment.witness(W2)[row_witness + 1] = range_chunks[54];

                        // W4,1        W5,1                 W5,0                  W5, -1
                        // 123|4567890123456789012.3456789012345678901234.5678901234567890123456
                        range_chunks[55] = (RAM[14]) & mask22;
                        range_chunks[56] = (RAM[14] >> 22) & mask22;
                        range_chunks[57] = (RAM[14] >> 44) & mask19;
                        range_chunks[58] = (RAM[14] >> 63) & 0b111;

                        assignment.witness(W4)[row_witness - 1] = RAM[14];
                        assignment.witness(W4)[row_witness - 0] = input_words_values[14];
                        assignment.witness(W5)[row_witness - 1] = range_chunks[55];
                        assignment.witness(W5)[row_witness - 0] = range_chunks[56];
                        assignment.witness(W5)[row_witness + 1] = range_chunks[57];
                        assignment.witness(W4)[row_witness + 1] = range_chunks[58];

                        //     W7, 1              W7, 0                 W7, -1
                        // 1234567890123.4567890123456789012345.6789012345678901234567
                        range_chunks[59] = (RAM[15]) & mask22;
                        range_chunks[60] = (RAM[15] >> 22) & mask22;
                        range_chunks[61] = (RAM[15] >> 44) & mask13;

                        assignment.witness(W6)[row_witness - 1] = RAM[15];
                        assignment.witness(W6)[row_witness - 0] = input_words_values[15];
                        assignment.witness(W7)[row_witness - 1] = range_chunks[59];
                        assignment.witness(W7)[row_witness - 0] = range_chunks[60];
                        assignment.witness(W7)[row_witness + 1] = range_chunks[61];

                        row_witness += 3;

                        //         W0,1                    W1,1                  W1,0        (W1,-1  &  W6,1-3)
                        // 1234567890123456789012.3456789012345678901234.567890123456789012|3456
                        range_chunks[62] = (RAM[16]) & 0b1111;
                        range_chunks[63] = (RAM[16] >> 4) & mask18;
                        range_chunks[64] = (RAM[16] >> 22) & mask22;
                        range_chunks[65] = (RAM[16] >> 44) & mask22;

                        assignment.witness(W6)[row_witness - 3 + 1] = range_chunks[62];

                        assignment.witness(W0)[row_witness - 1] = RAM[16];
                        assignment.witness(W0)[row_witness - 0] = input_words_values[16];
                        assignment.witness(W1)[row_witness - 1] = range_chunks[62];
                        assignment.witness(W1)[row_witness - 0] = range_chunks[63];
                        assignment.witness(W1)[row_witness + 1] = range_chunks[64];
                        assignment.witness(W0)[row_witness + 1] = range_chunks[65];

                        //           W2,1                    W3,1                  W3,0      W3, -1
                        // |1234567890123456789012.3456789012345678901234.56789012345678901234|56
                        range_chunks[66] = (RAM[17]) & 3;
                        range_chunks[67] = (RAM[17] >> 2) & mask20;
                        range_chunks[68] = (RAM[17] >> 22) & mask22;
                        range_chunks[69] = (RAM[17] >> 44) & mask22;
                        assignment.witness(W2)[row_witness - 1] = RAM[17];
                        assignment.witness(W2)[row_witness - 0] = input_words_values[17];
                        assignment.witness(W3)[row_witness - 1] = range_chunks[66];
                        assignment.witness(W3)[row_witness - 0] = range_chunks[67];
                        assignment.witness(W3)[row_witness + 1] = range_chunks[68];
                        assignment.witness(W2)[row_witness + 1] = range_chunks[69];

                        // W4,1        W5,1                 W5,0                  W5, -1
                        // 12|34567890123456789012.3456789012345678901234.5678901234567890123456
                        range_chunks[70] = (RAM[18]) & mask22;
                        range_chunks[71] = (RAM[18] >> 22) & mask22;
                        range_chunks[72] = (RAM[18] >> 44) & mask20;
                        range_chunks[73] = (RAM[18] >> 64) & 0b11;

                        assignment.witness(W4)[row_witness - 1] = RAM[18];
                        assignment.witness(W4)[row_witness - 0] = input_words_values[18];
                        assignment.witness(W5)[row_witness - 1] = range_chunks[70];
                        assignment.witness(W5)[row_witness - 0] = range_chunks[71];
                        assignment.witness(W5)[row_witness + 1] = range_chunks[72];
                        assignment.witness(W4)[row_witness + 1] = range_chunks[73];

                        //     W7, 1              W7, 0                 W7, -1
                        // 12345678901234.5678901234567890123456.7890123456789012345678
                        range_chunks[74] = (RAM[19]) & mask22;
                        range_chunks[75] = (RAM[19] >> 22) & mask22;
                        range_chunks[76] = (RAM[19] >> 44) & mask14;

                        assignment.witness(W6)[row_witness - 1] = RAM[19];
                        assignment.witness(W6)[row_witness - 0] = input_words_values[19];
                        assignment.witness(W7)[row_witness - 1] = range_chunks[74];
                        assignment.witness(W7)[row_witness - 0] = range_chunks[75];
                        assignment.witness(W7)[row_witness + 1] = range_chunks[76];
                        assignment.witness(W8)[row_witness + 1] = 1;

                        std::array<var, 16> input_words_vars_1;
                        std::array<var, 16> input_words_vars_2;

                        for (std::size_t j = 0; j < 4; j++) {
                            for (std::size_t i = 0; i < 4; i++) {
                                input_words_vars_1[4 * j + i] = var(2 * i, row + 1 + 3 * j, false);
                            }
                        }

                        for (std::size_t i = 0; i < 4; i++) {
                            input_words_vars_2[i] = var(2 * i, row + 1 + 12, false);
                        }

                        assignment.constant(0)[component_start_row + 8] = 0;
                        assignment.constant(0)[component_start_row + 9] = 252 + 1024;

                        for (std::size_t i = 4; i < 15; i++) {
                            input_words_vars_2[i] = var(0, component_start_row + 8, false, var::column_type::constant);
                        }
                        input_words_vars_2[15] = var(0, component_start_row + 9, false, var::column_type::constant);

                        row = component_start_row + rows_amount_creating_input_words_component;

                        std::array<typename ArithmetizationType::field_type::value_type, 8> constants = {
                            0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
                            0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
                        for (int i = 0; i < 8; i++) {
                            assignment.constant(0)[component_start_row + i] = constants[i];
                        }

                        std::array<var, 8> constants_var = {
                            var(0, component_start_row, false, var::column_type::constant),
                            var(0, component_start_row + 1, false, var::column_type::constant),
                            var(0, component_start_row + 2, false, var::column_type::constant),
                            var(0, component_start_row + 3, false, var::column_type::constant),
                            var(0, component_start_row + 4, false, var::column_type::constant),
                            var(0, component_start_row + 5, false, var::column_type::constant),
                            var(0, component_start_row + 6, false, var::column_type::constant),
                            var(0, component_start_row + 7, false, var::column_type::constant)};
                        typename sha512_process_component::params_type sha_params = {constants_var, input_words_vars_1};
                        auto sha_output =
                            sha512_process_component::generate_assignments(assignment, sha_params, row).output_state;
                        row += sha512_process_component::rows_amount;

                        // TODO

                        /*for (std::size_t i = 0; i < 8; i++) {
                            assignment.witness(i)[row] = input_words_values[16 + i];
                            assignment.witness(i)[row+1] = input_words_values[16 + i+8];
                            input_words_vars_2[i] = var(i, row, false);
                            input_words_vars_2[i+8] = var(i, row+1, false);
                        }*/

                        // row = row + 2;
                        sha_params = {sha_output, input_words_vars_2};

                        /*std::array<typename ArithmetizationType::field_type::value_type, 16> input_words2 = {
                             1 << 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 << 9};
                         for (int i = 0; i < 16; i++) {
                             assignment.constant(0)[component_start_row + 8 + i] = input_words2[i];
                         }
                         std::vector<var> input_words2_var = {var(0, row + 8, false, var::column_type::constant),
                                                              var(0, row + 9, false, var::column_type::constant),
                                                              var(0, row + 10, false, var::column_type::constant),
                                                              var(0, row + 11, false, var::column_type::constant),
                                                              var(0, row + 12, false, var::column_type::constant),
                                                              var(0, row + 13, false, var::column_type::constant),
                                                              var(0, row + 14, false, var::column_type::constant),
                                                              var(0, row + 15, false, var::column_type::constant),
                                                              var(0, row + 16, false, var::column_type::constant),
                                                              var(0, row + 17, false, var::column_type::constant),
                                                              var(0, row + 18, false, var::column_type::constant),
                                                              var(0, row + 19, false, var::column_type::constant),
                                                              var(0, row + 20, false, var::column_type::constant),
                                                              var(0, row + 21, false, var::column_type::constant),
                                                              var(0, row + 22, false, var::column_type::constant),
                                                              var(0, row + 23, false, var::column_type::constant)};
                         typename sha512_process_component::params_type sha_params2 = {sha_output.output_state,
                                                                                       input_words2_var}; */

                        sha512_process_component::generate_assignments(assignment, sha_params, row);
                        return result_type(component_start_row);
                    }

                private:
                    static void generate_gates(blueprint<ArithmetizationType> &bp,
                                               blueprint_public_assignment_table<ArithmetizationType> &assignment,
                                               const std::size_t &first_selector_index) {

                        typename ArithmetizationType::field_type::integral_type one = 1;

                        auto constraint_ram_0 =
                            bp.add_constraint(var(W0, -1) - (var(W1, -1) + var(W1, 0) * (one << 22) +
                                                             var(W1, 1) * (one << 44) + var(W0, 1) * (one << 64)));
                        auto constraint_word_0 = bp.add_constraint(
                            var(W0, 0) - (var(W1, -1) + var(W1, 0) * (one << 22) + var(W1, 1) * (one << 44)));

                        // W2,1       W3,1                  W3,0                  W3, -1
                        // 1234|567890123456789012.3456789012345678901234.5678901234567890123456

                        auto constraint_ram_1 =
                            bp.add_constraint(var(W2, -1) - (var(W3, -1) + var(W3, 0) * (one << 22) +
                                                             var(W3, 1) * (one << 44) + var(W2, 1) * (one << 62)));
                        auto constraint_word_1 =
                            bp.add_constraint(var(W2, 0) - (var(W0, 1) + var(W3, -1) * (one << 2) +
                                                            var(W3, 0) * (one << 24) + var(W3, 1) * (one << 46)));

                        //  W4,1        W5,1                W5,0                  W5, -1
                        // 123456|7890123456789012.3456789012345678901234.5678901234567890123456
                        auto constraint_ram_2 =
                            bp.add_constraint(var(W4, -1) - (var(W5, -1) + var(W5, 0) * (one << 22) +
                                                             var(W5, 1) * (one << 44) + var(W4, 1) * (one << 60)));
                        auto constraint_word_2 = bp.add_constraint(var(W4, 0) - (var(W2, 1) + var(W5, -1) * (one << 4) +
                                                                                 var(W5, 0) * (one << (4 + 22)) +
                                                                                 var(W5, 1) * (one << (4 + 44))));

                        //     W7, 1              W7, 0                 W7, -1
                        // 1234567890123.4567890123456789012345.6789012345678901234567

                        auto constraint_ram_3 = bp.add_constraint(
                            var(W6, -1) - (var(W7, -1) + var(W7, 0) * (one << 22) + var(W7, 1) * (one << 44)));
                        auto constraint_word_3 = bp.add_constraint(
                            var(W6, 0) - (var(W4, 1) + var(W7, -1) * (one << 6) + var(W7, 0) * (one << (6 + 22)) +
                                          var(W7, 1) * (one << (6 + 44)) + var(W6, 1) * (one << 63)));

                        bp.add_gate(first_selector_index,
                                    {constraint_ram_0, constraint_ram_1, constraint_ram_2, constraint_ram_3,
                                     constraint_word_0, constraint_word_1, constraint_word_2, constraint_word_3});

                        // W0,1      W1,1                  W1,0               W1,-1             W8,-1
                        // 1|234567890123456789012.3456789012345678901234.567890123456789012345|6
                        auto constraint_ram_4 = bp.add_constraint(
                            var(W0, -1) - (var(W8, -1) + var(W1, -1) * (1 << 1) + var(W1, 0) * (one << 22) +
                                           var(W1, 1) * (one << 44) + var(W0, 1) * (one << 65)));
                        auto constraint_word_4 =
                            bp.add_constraint(var(W0, 0) - (var(W1, -1) + var(W1, 0) * (one << (22 - 1)) +
                                                            var(W1, 1) * (one << (22 + 22 - 1))));

                        // W2,1       W3,1                  W3,0                  W3, -1
                        // 123|4567890123456789012.3456789012345678901234.5678901234567890123456
                        auto constraint_ram_5 =
                            bp.add_constraint(var(W2, -1) - (var(W3, -1) + var(W3, 0) * (one << 22) +
                                                             var(W3, 1) * (one << 44) + var(W2, 1) * (one << 63)));
                        auto constraint_word_5 = bp.add_constraint(var(W2, 0) - (var(W0, 1) + var(W3, -1) * (1 << 1) +
                                                                                 var(W3, 0) * (one << (22 + 1)) +
                                                                                 var(W3, 1) * (one << (44 + 1))));

                        //  W4,1        W5,1                W5,0                  W5, -1
                        // 12345|67890123456789012.3456789012345678901234.5678901234567890123456
                        auto constraint_ram_6 =
                            bp.add_constraint(var(W4, -1) - (var(W5, -1) + var(W5, 0) * (one << 22) +
                                                             var(W5, 1) * (one << 44) + var(W4, 1) * (one << 61)));
                        auto constraint_word_6 = bp.add_constraint(var(W4, 0) - (var(W2, 1) + var(W5, -1) * (one << 3) +
                                                                                 var(W5, 0) * (one << (3 + 22)) +
                                                                                 var(W5, 1) * (one << (3 + 44))));

                        //     W7, 1              W7, 0                 W7, -1
                        // 1234567890123.4567890123456789012345.6789012345678901234567
                        auto constraint_ram_7 = bp.add_constraint(
                            var(W6, -1) - (var(W7, -1) + var(W7, 0) * (one << 22) + var(W7, 1) * (one << 44)));
                        auto constraint_word_7 = bp.add_constraint(
                            var(W6, 0) - (var(4, 1) + var(W7, -1) * (one << 5) + var(W7, 0) * (one << (5 + 22)) +
                                          var(W7, 1) * (one << (5 + 44)) + var(W6, 1) * (one << 62)));

                        bp.add_gate(first_selector_index + 1,
                                    {constraint_ram_4, constraint_ram_5, constraint_ram_6, constraint_ram_7,
                                     constraint_word_4, constraint_word_5, constraint_word_6, constraint_word_7});

                        //          W0,1                    W1,1                  W1,0        W1,-1
                        // |1234567890123456789012.3456789012345678901234.56789012345678901234|56
                        auto constraint_ram_8 =
                            bp.add_constraint(var(W0, -1) - (var(W1, -1) + var(W1, 0) * (1 << 2) +
                                                             var(W1, 1) * (one << 22) + var(W0, 1) * (one << 44)));
                        auto constraint_word_8 = bp.add_constraint(
                            var(W0, 0) - (var(W1, 0) + var(W1, 1) * (one << 20) + var(W0, 1) * (one << 42)));

                        // W2,1       W3,1                  W3,0                  W3, -1
                        // 12|34567890123456789012.3456789012345678901234.5678901234567890123456
                        auto constraint_ram_9 =
                            bp.add_constraint(var(W2, -1) - (var(W3, -1) + var(W3, 0) * (one << 22) +
                                                             var(W3, 1) * (one << 44) + var(W2, 1) * (one << 64)));
                        auto constraint_word_9 = bp.add_constraint(
                            var(W2, 0) - (var(W3, -1) + var(W3, 0) * (one << 22) + var(W3, 1) * (one << 44)));

                        // W4,1        W5,1                 W5,0                  W5, -1
                        // 1234|567890123456789012.3456789012345678901234.5678901234567890123456
                        auto constraint_ram_10 =
                            bp.add_constraint(var(W4, -1) - (var(W5, -1) + var(W5, 0) * (one << 22) +
                                                             var(W5, 1) * (one << 44) + var(W4, 1) * (one << 62)));
                        auto constraint_word_10 =
                            bp.add_constraint(var(W4, 0) - (var(W2, 1) + var(W5, -1) * (one << 2) +
                                                            var(W5, 0) * (one << 24) + var(W5, 1) * (one << 46)));

                        //     W7, 1              W7, 0                 W7, -1
                        // 1234567890123.4567890123456789012345.6789012345678901234567
                        auto constraint_ram_11 = bp.add_constraint(
                            var(W6, -1) - (var(W7, -1) + var(W7, 0) * (one << 22) + var(W7, 1) * (one << 44)));
                        auto constraint_word_11 = bp.add_constraint(
                            var(W6, 0) - (var(W4, 1) + var(W7, -1) * (one << 4) + var(W7, 0) * (one << (4 + 22)) +
                                          var(W7, 1) * (one << (4 + 44)) + var(W6, 1) * (one << 61)));

                        bp.add_gate(first_selector_index + 2,
                                    {constraint_ram_8, constraint_ram_9, constraint_ram_10, constraint_ram_11,
                                     constraint_word_8, constraint_word_9, constraint_word_10, constraint_word_11});

                        //         W0,1                    W1,1                  W1,0        (W1,-1  &  W6,1-3)
                        // 1234567890123456789012.3456789012345678901234.5678901234567890123|456
                        auto constraint_ram_12 =
                            bp.add_constraint(var(W0, -1) - (var(W1, -1) + var(W1, 0) * (one << 3) +
                                                             var(W1, 1) * (one << 22) + var(W0, 1) * (one << 44)));
                        auto constraint_word_12 = bp.add_constraint(
                            var(W0, 0) - (var(W1, 0) + var(W1, 1) * (one << 19) + var(W0, 1) * (one << (19 + 22)) +
                                          var(W8, -1) * (one << 63)));

                        // W2,1       W3,1                  W3,0                  W3, -1        W8, -1
                        // 1|234567890123456789012.3456789012345678901234.567890123456789012345|6
                        auto constraint_ram_13 = bp.add_constraint(
                            var(W2, -1) - (var(W8, -1) + var(W3, -1) * (1 << 1) + var(W3, 0) * (one << 22) +
                                           var(W3, 1) * (one << 44) + var(W2, 1) * (one << 65)));
                        auto constraint_word_13 =
                            bp.add_constraint(var(W2, 0) - (var(W3, -1) + var(W3, 0) * (one << (22 - 1)) +
                                                            var(W3, 1) * (one << (22 + 22 - 1))));

                        // W4,1        W5,1                 W5,0                  W5, -1
                        // 123|4567890123456789012.3456789012345678901234.5678901234567890123456
                        auto constraint_ram_14 =
                            bp.add_constraint(var(W4, -1) - (var(W5, -1) + var(W5, 0) * (one << 22) +
                                                             var(W5, 1) * (one << 44) + var(W4, 1) * (one << 63)));
                        auto constraint_word_14 = bp.add_constraint(var(W4, 0) - (var(W2, 1) + var(W5, -1) * (1 << 1) +
                                                                                  var(W5, 0) * (one << (22 + 1)) +
                                                                                  var(W5, 1) * (one << (44 + 1))));

                        //     W7, 1              W7, 0                 W7, -1
                        // 1234567890123.4567890123456789012345.6789012345678901234567
                        auto constraint_ram_15 = bp.add_constraint(
                            var(W6, -1) - (var(W7, -1) + var(W7, 0) * (one << 22) + var(W7, 1) * (one << 44)));
                        auto constraint_word_15 = bp.add_constraint(
                            var(W6, 0) - (var(W4, 1) + var(W7, -1) * (one << 3) + var(W7, 0) * (one << (3 + 22)) +
                                          var(W7, 1) * (one << (3 + 44)) + var(W6, 1) * (one << 60)));

                        bp.add_gate(first_selector_index + 3,
                                    {constraint_ram_12, constraint_ram_13, constraint_ram_14, constraint_ram_15,
                                     constraint_word_12, constraint_word_13, constraint_word_14, constraint_word_15});

                        //         W0,1                    W1,1                  W1,0        (W1,-1  &  W6,1-3)
                        // 1234567890123456789012.3456789012345678901234.567890123456789012|3456
                        auto constraint_ram_16 =
                            bp.add_constraint(var(W0, -1) - (var(W1, -1) + var(W1, 0) * (one << 4) +
                                                             var(W1, 1) * (one << 22) + var(W0, 1) * (one << 44)));
                        auto constraint_word_16 = bp.add_constraint(
                            var(W0, 0) - (var(W1, 0) + var(W1, 1) * (one << 18) + var(W0, 1) * (one << (18 + 22)) +
                                          var(W3, -1) * (one << 62)));

                        //           W2,1                    W3,1                  W3,0      W3, -1
                        // |1234567890123456789012.3456789012345678901234.56789012345678901234|56
                        auto constraint_ram_17 =
                            bp.add_constraint(var(W2, -1) - (var(W3, -1) + var(W3, 0) * (one << 2) +
                                                             var(W3, 1) * (one << 22) + var(W2, 1) * (one << 44)));
                        auto constraint_word_17 = bp.add_constraint(
                            var(W2, 0) - (var(W3, 0) + var(W3, 1) * (one << 20) + var(W2, 1) * (one << 42)));

                        // W4,1        W5,1                 W5,0                  W5, -1
                        // 12|34567890123456789012.3456789012345678901234.5678901234567890123456
                        auto constraint_ram_18 =
                            bp.add_constraint(var(W4, -1) - (var(W5, -1) + var(W5, 0) * (one << 22) +
                                                             var(W5, 1) * (one << 44) + var(W4, 1) * (one << 64)));
                        auto constraint_word_18 = bp.add_constraint(
                            var(W4, 0) - (var(W5, -1) + var(W5, 0) * (one << 22) + var(W5, 1) * (one << 44)));

                        //     W7, 1              W7, 0                 W7, -1
                        // 12345678901234.5678901234567890123456.7890123456789012345678
                        auto constraint_ram_19 = bp.add_constraint(
                            var(W6, -1) - (var(W7, -1) + var(W7, 0) * (one << 22) + var(W7, 1) * (one << 44)));
                        auto constraint_word_19 = bp.add_constraint(
                            var(W6, 0) - (var(W4, 1) + var(W7, -1) * (one << 2) + var(W7, 0) * (one << (2 + 22)) +
                                          var(W7, 1) * (one << (2 + 44)) + var(W8, 1) * (one << 60)));

                        bp.add_gate(first_selector_index + 4,
                                    {constraint_ram_16, constraint_ram_17, constraint_ram_18, constraint_ram_19,
                                     constraint_word_16, constraint_word_17, constraint_word_18, constraint_word_19});
                    }

                    static void
                        generate_copy_constraints(blueprint<ArithmetizationType> &bp,
                                                  blueprint_public_assignment_table<ArithmetizationType> &assignment,
                                                  const params_type &params,
                                                  const std::size_t &component_start_row) {
                        std::size_t row = component_start_row;

                        for (std::size_t i = 0; i < 4; i++) {
                            bp.add_copy_constraint({var(2 * i, row + 0, false), params.R.x[i]});
                            bp.add_copy_constraint({var(2 * i, row + 3, false), params.R.y[i]});
                            bp.add_copy_constraint({var(2 * i, row + 6, false), params.A.x[i]});
                            bp.add_copy_constraint({var(2 * i, row + 9, false), params.A.y[i]});
                            bp.add_copy_constraint({var(2 * i, row + 12, false), params.M[i]});
                        }

                        bp.add_copy_constraint({var(W6, (row + 4) - 3 + 1, false), var(W8, (row + 4) - 1, false)});
                        for (std::size_t i = 0; i < 3; i++) {
                            std::size_t current_row = row + 1 + 6 + 3 * i;
                            bp.add_copy_constraint(
                                {var(W6, (current_row - 3) + 1, false), var(W1, current_row - 1, false)});
                        }
                    }
                };
            }    // namespace components
        }        // namespace zk
    }            // namespace actor
}    // namespace nil

#endif    // ACTOR_ZK_BLUEPRINT_PLONK_SHA512_HPP