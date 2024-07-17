#pragma once

#include <cstddef>
#include <type_traits>

#include "eFLL/FuzzyGroup.hpp"
#include "eFLL/FuzzyIo.hpp"

namespace efll
{
    template <typename T>
    class FuzzyInput : public FuzzyIo<T>
    {
      public:
        FuzzyInput() :
            FuzzyIo<T>()
        {
        }

        FuzzyInput(std::size_t index) :
            FuzzyIo<T>(index)
        {
        }

        virtual ~FuzzyInput() = default;

        void calculate_group_pertinences()
        {
            for (FuzzyGroup<T>& grp : this->m_groups)
            {
                grp.compute_pertinence(this->m_crispness);
            }
        }
    };
}  // namespace efll
