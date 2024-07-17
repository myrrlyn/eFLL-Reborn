#pragma once

#include <cstddef>
#include <type_traits>

#include "eFLL/FuzzyRuleAntecedent.hpp"
#include "eFLL/FuzzyRuleConsequent.hpp"

namespace efll
{
    template <typename T>
    class FuzzyRule
    {
        FuzzyRule() = default;
        FuzzyRule(std::size_t idx, RuleAntecedent<T> before, RuleConsequent<T> after) :
            m_index{idx},
            m_before{before},
            m_after{after}
        {
        }

        std::size_t get_index() const { return m_index; }

        bool evaluate()
        {
            T power = m_before.evaluate();
            m_fired = power > 0.0;
            m_after.evaluate(power);
        }

        bool has_fired() const { return m_fired; }

      private:
        RuleAntecedent<T> m_before;
        RuleConsequent<T> m_after;
        std::size_t m_index = 0;
        bool m_fired        = false;
    };
}  // namespace efll
