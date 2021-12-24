//---------------------------------------------------------------------------//
// Copyright (c) 2018-2022 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020-2022 Nikita Kaskov <nbering@nil.foundation>
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

#define BOOST_TEST_MODULE fixed_base_scalar_mul_5_wires_test

#include <boost/test/unit_test.hpp>

#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/bls12.hpp>
#include <nil/crypto3/algebra/curves/params/multiexp/bls12.hpp>
#include <nil/crypto3/algebra/curves/params/wnaf/bls12.hpp>

#include <nil/crypto3/zk/components/blueprint.hpp>
#include <nil/crypto3/zk/components/algebra/curves/plonk/fixed_base_scalar_mul_5_wires.hpp>

using namespace nil::crypto3;

BOOST_AUTO_TEST_SUITE(fixed_base_scalar_mul_5_wires_test_suite)

BOOST_AUTO_TEST_CASE(fixed_base_scalar_mul_5_wires_test_case) {

	using curve_type = algebra::curves::bls12<381>;
	using TBlueprintField = typename curve_type::base_field_type;
	constexpr std::size_t WiresAmount = 5;
	constexpr typename curve_type::template g1_type<>::value_type B = curve_type::template g1_type<>::value_type::one();
	using TArithmetization = zk::snark::plonk_constraint_system<TBlueprintField, WiresAmount>;

	zk::components::blueprint<TArithmetization> bp;

	using component_type = zk::components::element_g1_fixed_base_scalar_mul<curve_type, TArithmetization>;

	component_type scalar_mul_component(bp, B);

}

BOOST_AUTO_TEST_SUITE_END()