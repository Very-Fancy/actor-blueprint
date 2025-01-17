//---------------------------------------------------------------------------//
// Copyright (c) 2018-2022 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020-2022 Nikita Kaskov <nbering@nil.foundation>
// Copyright (c) 2022 Alisa Cherniaeva <a.cherniaeva@nil.foundation>
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

#define BOOST_TEST_MODULE decomposed_variable_base_scalar_mul_test

#include <boost/test/unit_test.hpp>

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>
#include <nil/crypto3/algebra/curves/vesta.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/vesta.hpp>
#include <nil/crypto3/algebra/random_element.hpp>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/hash/keccak.hpp>

#include <nil/actor/zk/snark/arithmetization/plonk/params.hpp>

#include <nil/actor/zk/blueprint/plonk.hpp>
#include <nil/actor/zk/assignment/plonk.hpp>
#include <nil/actor/zk/components/algebra/curves/pasta/plonk/decomposed_variable_base_scalar_mul_15_wires.hpp>
#include "test_plonk_component.hpp"

#include "../../../profiling.hpp"

using namespace nil::crypto3;

BOOST_AUTO_TEST_SUITE(blueprint_plonk_test_suite)

BOOST_AUTO_TEST_CASE(blueprint_plonk_decomposed_variable_base_scalar_mul) {
    auto start = std::chrono::high_resolution_clock::now();

    using curve_type = crypto3::algebra::curves::pallas;
    using BlueprintFieldType = typename curve_type::base_field_type;
    using BlueprintScalarType = typename curve_type::scalar_field_type;
    constexpr std::size_t WitnessColumns = 15;
    constexpr std::size_t PublicInputColumns = 1;
    constexpr std::size_t ConstantColumns = 1;
    constexpr std::size_t SelectorColumns = 5;
    using ArithmetizationParams = zk::snark::plonk_arithmetization_params<WitnessColumns,
        PublicInputColumns, ConstantColumns, SelectorColumns>;
    using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType,
                ArithmetizationParams>;
    using AssignmentType = zk::blueprint_assignment_table<ArithmetizationType>;
	using hash_type = nil::crypto3::hashes::keccak_1600<256>;
    constexpr std::size_t Lambda = 1;
    using component_type = zk::components::curve_element_decomposed_variable_base_scalar_mul<ArithmetizationType, curve_type, 
                                                            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14>;
	using var = zk::snark::plonk_variable<BlueprintFieldType>;
    typename BlueprintScalarType::value_type b_scalar = crypto3::algebra::random_element<BlueprintScalarType>();

	typename curve_type::scalar_field_type::value_type shift_base = 2;
    typename curve_type::scalar_field_type::integral_type base = 1;
	auto shift = shift_base.pow(255) + 1;
	typename BlueprintScalarType::value_type x = (b_scalar - shift)/2;
	typename BlueprintScalarType::integral_type integral_x = typename BlueprintScalarType::integral_type(x.data);
	BlueprintFieldType::value_type x_scalar =  integral_x;


    curve_type::template g1_type<crypto3::algebra::curves::coordinates::affine>::value_type T = crypto3::algebra::random_element<curve_type::template g1_type<crypto3::algebra::curves::coordinates::affine>>();
	var scalar_var1 = {0, 2, false, var::column_type::public_input};
	var scalar_var2 = {0, 3, false, var::column_type::public_input};
    var T_X_var = {0, 0, false, var::column_type::public_input};
    var T_Y_var = {0, 1, false, var::column_type::public_input};
    typename component_type::params_type assignment_params = {{T_X_var, T_Y_var},scalar_var1, scalar_var2};
	std::vector<typename BlueprintFieldType::value_type> public_input = {T.X, T.Y, integral_x & ((base << 254) - 1), integral_x >> 254};
	curve_type::template g1_type<crypto3::algebra::curves::coordinates::affine>::value_type expected;
	if (b_scalar != 0) {
	 	expected = b_scalar * T;
	} else {
		expected = {0, 0};
	}
	std::cout<<"Expected result: "<< expected.X.data <<" " << expected.Y.data<<std::endl;
	auto result_check = [&expected, T, shift_base](AssignmentType &assignment, 
        component_type::result_type &real_res) {
			curve_type::template g1_type<crypto3::algebra::curves::coordinates::affine>::value_type R;
			R.X = assignment.var_value(real_res.X);
			R.Y = assignment.var_value(real_res.Y);
			std::cout<<"Component result: "<< assignment.var_value(real_res.X).data <<" " << assignment.var_value(real_res.Y).data<<std::endl;
			assert(expected.X == assignment.var_value(real_res.X));
			assert(expected.Y == assignment.var_value(real_res.Y));
    };
    test_component<component_type, BlueprintFieldType, ArithmetizationParams, hash_type, Lambda> (assignment_params, public_input, result_check);

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "base_scalar_mul: " << duration.count() << "ms" << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()