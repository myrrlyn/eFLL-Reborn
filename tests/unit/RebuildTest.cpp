#include <gtest/gtest.h>

#include "eFLL/Fuzzy.h"
#include "eFLL/Fuzzy.hpp"

TEST(Rebuild, compare_pertinence)
{
    FuzzySet fs1                 = FuzzySet(0, 10, 10, 20);
    efll::FuzzyGroup<double> fg1 = efll::FuzzyGroup<double>(0, 10, 10, 20);

    fs1.calculatePertinence(-5);
    fg1.compute_pertinence(-5);
    ASSERT_FLOAT_EQ(fs1.getPertinence(), fg1.get_pertinence());

    fs1.calculatePertinence(5);
    fg1.compute_pertinence(5);
    ASSERT_FLOAT_EQ(fs1.getPertinence(), fg1.get_pertinence());
    ASSERT_FLOAT_EQ(fg1.get_pertinence(), 0.5);

    fs1.calculatePertinence(10);
    fg1.compute_pertinence(10);
    ASSERT_FLOAT_EQ(1.0, fs1.getPertinence());
    ASSERT_FLOAT_EQ(fs1.getPertinence(), fg1.get_pertinence());

    fs1.calculatePertinence(15);
    fg1.compute_pertinence(15);
    ASSERT_FLOAT_EQ(0.5, fs1.getPertinence());
    ASSERT_FLOAT_EQ(fs1.getPertinence(), fg1.get_pertinence());

    fs1.calculatePertinence(25);
    fg1.compute_pertinence(25);
    ASSERT_FLOAT_EQ(0.0, fs1.getPertinence());
    ASSERT_FLOAT_EQ(fs1.getPertinence(), fg1.get_pertinence());

    FuzzySet fs2                 = FuzzySet(0, 0, 20, 30);
    efll::FuzzyGroup<double> fg2 = efll::FuzzyGroup<double>(0, 0, 20, 30);

    fs2.calculatePertinence(-5);
    fg2.compute_pertinence(-5);
    ASSERT_FLOAT_EQ(1.0, fs2.getPertinence());
    ASSERT_FLOAT_EQ(fs2.getPertinence(), fg2.get_pertinence());

    FuzzySet fs3                 = FuzzySet(0, 10, 20, 20);
    efll::FuzzyGroup<double> fg3 = efll::FuzzyGroup<double>(0, 10, 20, 20);

    fs3.calculatePertinence(25);
    fg3.compute_pertinence(25);
    ASSERT_FLOAT_EQ(1.0, fs3.getPertinence());
    ASSERT_FLOAT_EQ(fs3.getPertinence(), fg3.get_pertinence());
}

TEST(RebuildInput, add_group)
{
    FuzzyInput inp1 = FuzzyInput(1);
    FuzzySet set1   = FuzzySet(0, 10, 10, 20);
    FuzzySet set2   = FuzzySet(10, 20, 20, 30);

    efll::FuzzyInput<double> inp2 = efll::FuzzyInput<double>(1);
    efll::FuzzyGroup<double> grp1 = efll::FuzzyGroup<double>(0, 10, 10, 20);
    efll::FuzzyGroup<double> grp2 = efll::FuzzyGroup<double>(10, 20, 20, 30);

    inp1.setCrispInput(10.190);
    inp2.set_crispness(10.190);

    inp1.addFuzzySet(&set1);
    // inp1.addFuzzySet(&set2);
    inp2.push_group(grp1);
    // inp2.push_group(grp2);

    inp1.setCrispInput(5);
    inp2.set_crispness(5);

    inp1.calculateFuzzySetPertinences();
    inp2.calculate_group_pertinences();
    ASSERT_FLOAT_EQ(0.5, set1.getPertinence());
    ASSERT_FLOAT_EQ(0.5, grp1.get_pertinence());
    // ASSERT_FLOAT_EQ(0.0, set2.getPertinence());
    ASSERT_FLOAT_EQ(set1.getPertinence(), grp1.get_pertinence());
    // ASSERT_FLOAT_EQ(set2.getPertinence(), grp2.get_pertinence());
}
