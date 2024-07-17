#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <utility>

#include "eFLL/FuzzyGroup.hpp"

namespace efll
{
    enum class Operator : uint8_t
    {
        Empty,
        Self,
        Min,
        Max,
    };

    enum class Kind : uint8_t
    {
        Group,
        Rule,
    };

    template <typename T>
    class RuleAntecedent;

    template <typename T>
    class Operand
    {
      public:
        Operand() :
            m_tag{Kind::Group},
            m_group{}
        {
        }

        Operand(FuzzyGroup<T> grp_) :
            m_tag{Kind::Group},
            m_group{grp_}
        {
        }

        Operand(RuleAntecedent<T> rule_) :
            m_tag{Kind::Rule}
        {
            m_rule = std::make_unique(rule_);
        }

        Kind which() const { return m_tag; }

        bool try_view_group(FuzzyGroup<T>& out)
        {
            if (m_tag == Kind::Group)
            {
                out = m_group;
            }
            return m_tag == Kind::Group;
        }

        FuzzyGroup<T>& view_group() { return m_group; }

        bool try_view_rule(RuleAntecedent<T>& out)
        {
            if (m_tag == Kind::Rule)
            {
                out = *m_rule;
            }
            return m_tag == Kind::Rule;
        }

        RuleAntecedent<T>& view_rule() { return *m_rule; }

        T get_pertinence() const
        {
            switch (m_tag)
            {
                case Kind::Group:
                    return m_group.get_pertinence();
                case Kind::Rule:
                    return m_rule->evaluate();
            }
        }

      private:
        Kind m_tag;
        union
        {
            FuzzyGroup<T> m_group;
            std::unique_ptr<RuleAntecedent<T>> m_rule;
        };
    };

    template <typename T>
    class RuleAntecedent
    {
      public:
        static_assert(std::is_floating_point<T>::value);

        RuleAntecedent() = default;

        void set_unary(FuzzyGroup<T> group)
        {
            m_op   = Operator::Self;
            m_left = Operand(group);
        }

        template <class Left, class Right>
        void join_and(Left left, Right right)
        {
            m_op    = Operator::Min;
            m_left  = Operand(left);
            m_right = Operand(right);
        }

        template <class Left, class Right>
        void join_or(Left left, Right right)
        {
            m_op    = Operator::Max;
            m_left  = Operand(left);
            m_right = Operand(right);
        }

        T evaluate() const
        {
            switch (m_op)
            {
                case Operator::Empty:
                    return 0.0;
                case Operator::Self:
                    return m_left.get_pertinence();
                case Operator::Min:
                {
                    T left_pert  = m_left.get_pertinence();
                    T right_pert = m_right.get_pertinence();
                    if (left_pert > 0.0 && right_pert > 0.0)
                    {
                        return std::min(left_pert, right_pert);
                    }
                    return 0.0;
                }
                case Operator::Max:
                {
                    T left_pert  = m_left.get_pertinence();
                    T right_pert = m_right.get_pertinence();
                    if (left_pert > 0.0 && right_pert > 0.0)
                    {
                        return std::max(left_pert, right_pert);
                    }
                    return 0.0;
                }
            }
        }

      private:
        Operator m_op = Operator::Empty;
        Operand<T> m_left;
        Operand<T> m_right;
    };
}  // namespace efll
