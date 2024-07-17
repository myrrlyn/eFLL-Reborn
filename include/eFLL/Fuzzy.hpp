#pragma once

#include <deque>
#include <type_traits>

#include "eFLL/FuzzyInput.hpp"
#include "eFLL/FuzzyOutput.hpp"
#include "eFLL/FuzzyRule.hpp"

namespace efll
{
    template <typename T>
    class Fuzzy
    {
        Fuzzy()  = default;
        ~Fuzzy() = default;

        void add_input(FuzzyInput<T> inp) { m_inputs.push_back(inp); }
        void add_output(FuzzyOutput<T> out) { m_outputs.push_back(out); }
        void add_rule(FuzzyRule<T> rule) { m_rules.push_back(rule); }

        bool set_input(std::size_t index, T crispness)
        {
            for (auto& inp : m_inputs)
            {
                if (inp.get_index() == index)
                {
                    inp.set_crispness(crispness);
                    return true;
                }
            }
            return false;
        }

        void fuzz()
        {
            for (auto& inp : m_inputs)
            {
                inp.reset_all_groups();
            }
            for (auto& out : m_outputs)
            {
                out.reset_all_groups();
            }
            for (auto& inp : m_inputs)
            {
                inp.calculate_group_pertinences();
            }
            for (auto& rule : m_rules)
            {
                rule.evaluate();
            }
            for (auto& out : m_outputs)
            {
                out.truncate();
            }
        }

        bool rule_has_fired(std::size_t index)
        {
            for (auto const& rule : m_rules)
            {
                if (rule.get_index() == index)
                {
                    return rule.has_fired();
                }
            }
            return false;
        }

        T unfuzz(std::size_t index)
        {
            for (auto& out : m_outputs)
            {
                if (out.get_index() == index)
                {
                    return out.get_crispness_output();
                }
            }
            return 0.0;
        }

      private:
        std::deque<FuzzyInput<T>> m_inputs;
        std::deque<FuzzyOutput<T>> m_outputs;
        std::deque<FuzzyRule<T>> m_rules;
    };
}  // namespace efll
