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

#ifndef ACTOR_ZK_BLUEPRINT_PLONK_BIT_CHECK_CHIP_HPP
#define ACTOR_ZK_BLUEPRINT_PLONK_BIT_CHECK_CHIP_HPP

#include <nil/actor/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/actor/zk/blueprint/plonk.hpp>
#include <nil/actor/zk/assignment/plonk.hpp>

namespace nil {
    namespace actor {
        namespace zk {
            namespace chips {

                template<typename ArithmetizationType>
                class bit_check;

                template<typename BlueprintFieldType,
                         typename ArithmetizationParams>
                class bit_check<
                    snark::plonk_constraint_system<BlueprintFieldType,
                        ArithmetizationParams>>{

                    typedef snark::plonk_constraint_system<BlueprintFieldType,
                        ArithmetizationParams> ArithmetizationType;

                    using var = snark::plonk_variable<BlueprintFieldType>;
                public:
                    constexpr static const std::size_t rows_amount = 0;

                    static snark::plonk_constraint<BlueprintFieldType> generate(
                        blueprint<ArithmetizationType> &bp,
                        const var& X1) {

                        return bp.add_constraint(X1 * (X1 - 1));
                    }
                };
            }    // namespace chips
        }        // namespace zk
    }            // namespace actor
}    // namespace nil

#endif    // ACTOR_ZK_BLUEPRINT_PLONK_BIT_CHECK_CHIP_HPP
