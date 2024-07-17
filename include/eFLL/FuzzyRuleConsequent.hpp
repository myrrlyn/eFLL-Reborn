#pragma once

#include <cstdlib>
#include <deque>
#include <type_traits>

#include "eFLL/FuzzyGroup.hpp"

namespace efll
{
    template <typename T>
    class RuleConsequent
    {
      public:
        static_assert(std::is_floating_point<T>::value);
        RuleConsequent()  = default;
        ~RuleConsequent() = default;

        void add_output(FuzzyGroup<T> grp) { m_groups.push_back(grp); }

        void evaluate(T power)
        {
            for (auto& grp : m_groups)
            {
                grp.set_pertinence(power);
            }
        }

      private:
        std::deque<FuzzyGroup<T>> m_groups;
    };
}  // namespace efll
