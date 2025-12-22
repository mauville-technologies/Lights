//
// Created by ozzadar on 2025-12-06.
//

#pragma once

#include "lights/util/enums.h"
#include <functional>

namespace OZZ::util {
    struct StateDefs {
        std::function<void()> OnEnter;
        std::function<void()> OnUpdate;
        std::function<void()> OnExit;
    };

    template <typename StateType>
    struct Transition {
        StateType From;
        std::function<bool()> Condition;
        StateType To;
    };

    /**
     * Statemachine. Call Start() after construction to begin
     * @tparam StateType The state type of the state machine. It should have a Count field as its last one
     */
    template <typename StateType>
    class StateMachine {
    public:
        StateMachine(StateType initial,
                     std::array<StateDefs, to_index(StateType::Count)> inDefs,
                     std::vector<Transition<StateType>> inTransitions)
            : currentState(initial), defs(std::move(inDefs)), transitions(std::move(inTransitions)) {};

        const StateType& GetCurrentState() const { return currentState; }

        /**
         * Start the state machine. Separate
         */
        void Start() {
            defs[to_index(currentState)].OnEnter();
            bStarted = true;
        };

        void Update() {
            if (!bStarted)
                return;

            defs[to_index(currentState)].OnUpdate();

            for (const auto transition : transitions) {
                if (transition.From == currentState && transition.Condition()) {
                    defs[to_index(currentState)].OnExit();
                    currentState = transition.To;
                    defs[to_index(currentState)].OnEnter();
                    break;
                }
            }
        }

    private:
        StateType currentState;
        std::array<StateDefs, to_index(StateType::Count)> defs;
        std::vector<Transition<StateType>> transitions;

        bool bStarted{false};
    };
} // namespace OZZ::util