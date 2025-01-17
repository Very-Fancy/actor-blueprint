//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
// Copyright (c) 2022 Ilia Shirobokov <i.shirobokov@nil.foundation>
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

#ifndef ACTOR_ZK_BLUEPRINT_PLONK_KIMCHI_BINDING_HPP
#define ACTOR_ZK_BLUEPRINT_PLONK_KIMCHI_BINDING_HPP

#include <nil/marshalling/algorithms/pack.hpp>

#include <nil/actor/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/actor/zk/blueprint/plonk.hpp>
#include <nil/actor/zk/component.hpp>

#include <nil/actor/zk/components/systems/snark/plonk/kimchi/detail/inner_constants.hpp>

namespace nil {
    namespace actor {
        namespace zk {
            namespace components {

                template<typename ArithmetizationType, typename BlueprintFieldType, typename KimchiParamsType>
                struct binding {
                    using var = snark::plonk_variable<BlueprintFieldType>;
                    using commitment_parms_type = typename KimchiParamsType::commitment_params_type;
                    using kimchi_constants = zk::components::kimchi_inner_constants<KimchiParamsType>;

                    template<typename VarType, std::size_t BatchSize>
                    struct fr_data {
                    private:
                        constexpr static const std::size_t f_comm_msm_size = kimchi_constants::f_comm_msm_size;
                        constexpr static const std::size_t lookup_columns = KimchiParamsType::circuit_params::lookup_columns;

                    public:
                        std::array<VarType, kimchi_constants::final_msm_size(BatchSize)> scalars;
                        std::array<std::array<VarType, f_comm_msm_size>, BatchSize> f_comm_scalars;
                        std::array<VarType, BatchSize> cip_shifted;

                        std::array<var, KimchiParamsType::public_input_size> neg_pub;
                        std::array<var, BatchSize> zeta_to_srs_len;
                        var zeta_to_domain_size_minus_1;

                        std::array<var, lookup_columns> joint_combiner_powers_prepared;
                    };

                    template<typename VarType>
                    struct fq_data { };

                    struct fq_sponge_output {
                        var joint_combiner;
                        var beta;    // beta and gamma can be combined from limbs in the base circuit
                        var gamma;
                        var alpha;
                        var zeta;
                        var fq_digest;    // TODO overflow check
                        std::array<var, commitment_parms_type::eval_rounds> challenges;
                        var c;
                    };
                };
            }    // namespace components
        }        // namespace zk
    }            // namespace actor
}    // namespace nil

#endif    // ACTOR_ZK_BLUEPRINT_PLONK_KIMCHI_BINDING_HPP