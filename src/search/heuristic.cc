#include "heuristic.h"
#include "operator.h"
#include "option_parser.h"
#include "operator_cost.h"

#include <cassert>
#include <cstdlib>

using namespace std;

Heuristic::Heuristic(const HeuristicOptions &options) {
    heuristic = NOT_INITIALIZED;

    if (options.cost_type < 0 || options.cost_type >= MAX_OPERATOR_COST) {
        cerr << "error: unknown operator cost type: " << options.cost_type << endl;
        exit(2);
    }

    cost_type = static_cast<OperatorCost>(options.cost_type);
}

Heuristic::~Heuristic() {
}

void Heuristic::set_preferred(const Operator *op) {
    if (!op->is_marked()) {
        op->mark();
        preferred_operators.push_back(op);
    }
}

void Heuristic::evaluate(const State &state) {
    if (heuristic == NOT_INITIALIZED)
        initialize();
    preferred_operators.clear();
    heuristic = compute_heuristic(state);
    for (int i = 0; i < preferred_operators.size(); i++)
        preferred_operators[i]->unmark();
    assert(heuristic == DEAD_END || heuristic >= 0);

    if (heuristic == DEAD_END) {
        // It is ok to have preferred operators in dead-end states.
        // This allows a heuristic to mark preferred operators on-the-fly,
        // selecting the first ones before it is clear that all goals
        // can be reached.
        preferred_operators.clear();
    }

#ifndef NDEBUG
    if (heuristic != DEAD_END) {
        for (int i = 0; i < preferred_operators.size(); i++)
            assert(preferred_operators[i]->is_applicable(state));
    }
#endif
    evaluator_value = heuristic;
}

bool Heuristic::is_dead_end() const {
    return evaluator_value == DEAD_END;
}

int Heuristic::get_heuristic() {
    // The -1 value for dead ends is an implementation detail which is
    // not supposed to leak. Thus, calling this for dead ends is an
    // error. Call "is_dead_end()" first.
    assert(heuristic >= 0);
    return heuristic;
}

void Heuristic::get_preferred_operators(std::vector<const Operator *> &result) {
    assert(heuristic >= 0);
    result.insert(result.end(),
                  preferred_operators.begin(),
                  preferred_operators.end());
}

bool Heuristic::reach_state(const State &/*parent_state*/,
                            const Operator &/*op*/, const State &/*state*/) {
    return false;
}

int Heuristic::get_value() const {
    return evaluator_value;
}

void Heuristic::evaluate(int, bool) {
    return;
    // if this is called, evaluate(const State &state) or set_evaluator_value(int val)
    // should already have been called
}

bool Heuristic::dead_end_is_reliable() const {
    return dead_ends_are_reliable();
}

void Heuristic::set_evaluator_value(int val) {
    evaluator_value = val;
}

int Heuristic::get_adjusted_cost(const Operator &op) const {
    return get_adjusted_action_cost(op, cost_type);
}


HeuristicOptions::HeuristicOptions()
    : cost_type(NORMAL) {
}

void HeuristicOptions::add_option_to_parser(NamedOptionParser &option_parser) {
    option_parser.add_int_option("cost_type",
                                 &cost_type,
                                 "operator cost adjustment type");
}