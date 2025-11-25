/**
 * StateManager.cpp - Finite State Machine implementation for WiBLE
 */

#include "StateManager.h"
#include "WiBLE.h"
#include "utils/LogManager.h"

namespace WiBLE {

// ============================================================================
// STATE UTILITIES
// ============================================================================

String StateUtils::stateToString(ProvisioningState state) {
    switch (state) {
        case ProvisioningState::IDLE: return "IDLE";
        case ProvisioningState::BLE_ADVERTISING: return "BLE_ADVERTISING";
        case ProvisioningState::BLE_CONNECTED: return "BLE_CONNECTED";
        case ProvisioningState::AUTHENTICATING: return "AUTHENTICATING";
        case ProvisioningState::RECEIVING_CREDENTIALS: return "RECEIVING_CREDENTIALS";
        case ProvisioningState::CONNECTING_WIFI: return "CONNECTING_WIFI";
        case ProvisioningState::VALIDATING_CONNECTION: return "VALIDATING_CONNECTION";
        case ProvisioningState::PROVISIONED: return "PROVISIONED";
        case ProvisioningState::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

String StateUtils::eventToString(StateEvent event) {
    switch (event) {
        case StateEvent::INIT_REQUESTED: return "INIT_REQUESTED";
        case StateEvent::RESET_REQUESTED: return "RESET_REQUESTED";
        case StateEvent::START_ADVERTISING: return "START_ADVERTISING";
        case StateEvent::STOP_ADVERTISING: return "STOP_ADVERTISING";
        case StateEvent::BLE_CLIENT_CONNECTED: return "BLE_CLIENT_CONNECTED";
        case StateEvent::BLE_CLIENT_DISCONNECTED: return "BLE_CLIENT_DISCONNECTED";
        case StateEvent::AUTH_STARTED: return "AUTH_STARTED";
        case StateEvent::AUTH_SUCCESS: return "AUTH_SUCCESS";
        case StateEvent::AUTH_FAILED: return "AUTH_FAILED";
        case StateEvent::AUTH_TIMEOUT: return "AUTH_TIMEOUT";
        case StateEvent::CREDENTIALS_RECEIVED: return "CREDENTIALS_RECEIVED";
        case StateEvent::CREDENTIALS_INVALID: return "CREDENTIALS_INVALID";
        case StateEvent::WIFI_CONNECT_STARTED: return "WIFI_CONNECT_STARTED";
        case StateEvent::WIFI_CONNECTED: return "WIFI_CONNECTED";
        case StateEvent::WIFI_CONNECTION_FAILED: return "WIFI_CONNECTION_FAILED";
        case StateEvent::WIFI_DISCONNECTED: return "WIFI_DISCONNECTED";
        case StateEvent::VALIDATION_STARTED: return "VALIDATION_STARTED";
        case StateEvent::VALIDATION_SUCCESS: return "VALIDATION_SUCCESS";
        case StateEvent::VALIDATION_FAILED: return "VALIDATION_FAILED";
        case StateEvent::ERROR_OCCURRED: return "ERROR_OCCURRED";
        case StateEvent::ERROR_RECOVERED: return "ERROR_RECOVERED";
        case StateEvent::CONNECTION_TIMEOUT: return "CONNECTION_TIMEOUT";
        case StateEvent::PROVISIONING_TIMEOUT: return "PROVISIONING_TIMEOUT";
        default: return "UNKNOWN";
    }
}

bool StateUtils::isTerminalState(ProvisioningState state) {
    return state == ProvisioningState::PROVISIONED;
}

bool StateUtils::isErrorState(ProvisioningState state) {
    return state == ProvisioningState::ERROR;
}

// ============================================================================
// STATE MANAGER IMPLEMENTATION
// ============================================================================

StateManager::StateManager() 
    : currentState(ProvisioningState::IDLE),
      previousState(ProvisioningState::IDLE),
      isInTransition(false),
      maxHistorySize(10) {
}

StateManager::~StateManager() {
}

void StateManager::initialize() {
    defineDefaultTransitions();
    context.reset();
    LogManager::info("StateManager initialized");
}

void StateManager::reset() {
    context.reset();
    stateHistory.clear();
    handleEvent(StateEvent::RESET_REQUESTED);
}

bool StateManager::isInState(ProvisioningState state) const {
    return currentState == state;
}

uint32_t StateManager::getTimeInCurrentState() const {
    return millis() - context.stateEntryTime;
}

std::vector<ProvisioningState> StateManager::getStateHistory(size_t maxCount) const {
    // Return copy of history, limited by maxCount
    if (stateHistory.size() <= maxCount) {
        return stateHistory;
    }
    return std::vector<ProvisioningState>(
        stateHistory.end() - maxCount, 
        stateHistory.end()
    );
}

bool StateManager::handleEvent(StateEvent event) {
    return handleEvent(event, "");
}

bool StateManager::handleEvent(StateEvent event, const String& data) {
    LogManager::debug("Event: " + StateUtils::eventToString(event));
    
    // Find valid transition
    TransitionKey key = {currentState, event};
    auto it = transitions.find(key);
    
    if (it != transitions.end()) {
        const StateTransition& transition = it->second;
        return executeTransition(transition, event);
    }
    
    // Check for global transitions (like RESET or ERROR)
    // TODO: Implement global transitions map
    if (event == StateEvent::RESET_REQUESTED) {
        StateTransition resetTransition(currentState, event, ProvisioningState::IDLE);
        return executeTransition(resetTransition, event);
    }
    
    if (event == StateEvent::ERROR_OCCURRED) {
        StateTransition errorTransition(currentState, event, ProvisioningState::ERROR);
        // Store error message in context
        context.lastErrorMessage = data;
        return executeTransition(errorTransition, event);
    }
    
    LogManager::warn("No transition found for event " + StateUtils::eventToString(event) + 
                     " in state " + StateUtils::stateToString(currentState));
    return false;
}

bool StateManager::executeTransition(const StateTransition& transition, StateEvent event) {
    if (!transition.canTransition()) {
        LogManager::warn("Guard condition failed for transition");
        return false;
    }
    
    isInTransition = true;
    
    // 1. Exit current state
    exitState(currentState);
    
    // 2. Execute transition action
    transition.executeAction();
    
    // 3. Notify transition listeners
    notifyTransition(currentState, transition.toState, event);
    
    // 4. Update state
    previousState = currentState;
    currentState = transition.toState;
    
    // 5. Enter new state
    enterState(currentState);
    
    isInTransition = false;
    return true;
}

void StateManager::enterState(ProvisioningState newState) {
    context.stateEntryTime = millis();
    recordStateInHistory(newState);
    
    LogManager::info("Entering State: " + StateUtils::stateToString(newState));
    
    if (entryCallback) {
        entryCallback(newState, context);
    }
    
    // Execute internal entry actions
    switch (newState) {
        case ProvisioningState::IDLE: onEnterIdle(); break;
        case ProvisioningState::BLE_ADVERTISING: onEnterAdvertising(); break;
        case ProvisioningState::BLE_CONNECTED: onEnterConnected(); break;
        case ProvisioningState::PROVISIONED: 
            // Save state if needed
            break;
        default: break;
    }
}

void StateManager::exitState(ProvisioningState oldState) {
    if (exitCallback) {
        exitCallback(oldState, context);
    }
}

void StateManager::recordStateInHistory(ProvisioningState state) {
    stateHistory.push_back(state);
    if (stateHistory.size() > maxHistorySize) {
        stateHistory.erase(stateHistory.begin());
    }
}

void StateManager::defineDefaultTransitions() {
    // IDLE -> ADVERTISING
    addTransition(StateTransition(ProvisioningState::IDLE, 
                                 StateEvent::START_ADVERTISING, 
                                 ProvisioningState::BLE_ADVERTISING));
                                 
    // ADVERTISING -> CONNECTED
    addTransition(StateTransition(ProvisioningState::BLE_ADVERTISING, 
                                 StateEvent::BLE_CLIENT_CONNECTED, 
                                 ProvisioningState::BLE_CONNECTED));
                                 
    // CONNECTED -> AUTHENTICATING
    addTransition(StateTransition(ProvisioningState::BLE_CONNECTED, 
                                 StateEvent::AUTH_STARTED, 
                                 ProvisioningState::AUTHENTICATING));
                                 
    // AUTHENTICATING -> RECEIVING_CREDENTIALS
    addTransition(StateTransition(ProvisioningState::AUTHENTICATING, 
                                 StateEvent::AUTH_SUCCESS, 
                                 ProvisioningState::RECEIVING_CREDENTIALS));
                                 
    // RECEIVING_CREDENTIALS -> CONNECTING_WIFI
    addTransition(StateTransition(ProvisioningState::RECEIVING_CREDENTIALS, 
                                 StateEvent::CREDENTIALS_RECEIVED, 
                                 ProvisioningState::CONNECTING_WIFI));
                                 
    // CONNECTING_WIFI -> PROVISIONED
    addTransition(StateTransition(ProvisioningState::CONNECTING_WIFI, 
                                 StateEvent::WIFI_CONNECTED, 
                                 ProvisioningState::PROVISIONED));
                                 
    // CONNECTING_WIFI -> ERROR
    addTransition(StateTransition(ProvisioningState::CONNECTING_WIFI, 
                                 StateEvent::WIFI_CONNECTION_FAILED, 
                                 ProvisioningState::ERROR));
                                 
    // ERROR -> IDLE (Recovery)
    addTransition(StateTransition(ProvisioningState::ERROR, 
                                 StateEvent::ERROR_RECOVERED, 
                                 ProvisioningState::IDLE));
                                 
    // Disconnection handling (Global-ish)
    addTransition(StateTransition(ProvisioningState::BLE_CONNECTED, 
                                 StateEvent::BLE_CLIENT_DISCONNECTED, 
                                 ProvisioningState::BLE_ADVERTISING));
                                 
    addTransition(StateTransition(ProvisioningState::AUTHENTICATING, 
                                 StateEvent::BLE_CLIENT_DISCONNECTED, 
                                 ProvisioningState::BLE_ADVERTISING));
                                 
    addTransition(StateTransition(ProvisioningState::RECEIVING_CREDENTIALS, 
                                 StateEvent::BLE_CLIENT_DISCONNECTED, 
                                 ProvisioningState::BLE_ADVERTISING));
}

void StateManager::addTransition(const StateTransition& transition) {
    TransitionKey key = {transition.fromState, transition.event};
    transitions.insert({key, transition});
}

// Callback registration
void StateManager::onStateEntry(StateEntryCallback callback) { entryCallback = callback; }
void StateManager::onStateExit(StateExitCallback callback) { exitCallback = callback; }
void StateManager::onStateTransition(StateTransitionCallback callback) { transitionCallback = callback; }

// Internal actions
void StateManager::onEnterIdle() {
    // Stop advertising, disconnect everything
}

void StateManager::onEnterAdvertising() {
    // Trigger BLE advertising start
}

void StateManager::onEnterConnected() {
    // Prepare for auth
}

void StateManager::notifyTransition(ProvisioningState from, ProvisioningState to, StateEvent event) {
    if (transitionCallback) {
        transitionCallback(from, to, event);
    }
}

} // namespace WiBLE
