/**
 * StateManager.h - Finite State Machine for WiBLE
 * 
 * Manages provisioning states with predictable transitions,
 * event handling, and state persistence.
 */

#ifndef WIBLE_STATE_MANAGER_H
#define WIBLE_STATE_MANAGER_H

#include <Arduino.h>
#include <functional>
#include <map>
#include <vector>

#include "WiBLE_Defs.h"

namespace WiBLE {

// Forward declarations
// enum class ProvisioningState; // Now in Defs
// enum class ErrorCode;         // Now in Defs

// ============================================================================
// STATE EVENTS
// ============================================================================

enum class StateEvent {
    // Lifecycle events
    INIT_REQUESTED,
    RESET_REQUESTED,
    
    // BLE events
    START_ADVERTISING,
    STOP_ADVERTISING,
    BLE_CLIENT_CONNECTED,
    BLE_CLIENT_DISCONNECTED,
    
    // Authentication events
    AUTH_STARTED,
    AUTH_SUCCESS,
    AUTH_FAILED,
    AUTH_TIMEOUT,
    
    // Provisioning events
    CREDENTIALS_RECEIVED,
    CREDENTIALS_INVALID,
    
    // WiFi events
    WIFI_CONNECT_STARTED,
    WIFI_CONNECTED,
    WIFI_CONNECTION_FAILED,
    WIFI_DISCONNECTED,
    
    // Validation events
    VALIDATION_STARTED,
    VALIDATION_SUCCESS,
    VALIDATION_FAILED,
    
    // Error events
    ERROR_OCCURRED,
    ERROR_RECOVERED,
    
    // Timeout events
    CONNECTION_TIMEOUT,
    PROVISIONING_TIMEOUT
};

// ============================================================================
// STATE MACHINE CONTEXT
// ============================================================================

struct StateMachineContext {
    String clientAddress;
    String ssid;
    String ipAddress;
    ErrorCode lastError;
    String lastErrorMessage;
    uint32_t stateEntryTime;
    uint32_t totalStateTime;
    uint8_t retryCount;
    uint8_t maxRetries;
    bool isSecureConnection;
    
    void reset() {
        clientAddress = "";
        ssid = "";
        ipAddress = "";
        lastError = ErrorCode::NONE;
        lastErrorMessage = "";
        stateEntryTime = millis();
        totalStateTime = 0;
        retryCount = 0;
        isSecureConnection = false;
    }
};

// ============================================================================
// STATE CALLBACKS
// ============================================================================

using StateEntryCallback = std::function<void(ProvisioningState state, const StateMachineContext& context)>;
using StateExitCallback = std::function<void(ProvisioningState state, const StateMachineContext& context)>;
using StateTransitionCallback = std::function<void(ProvisioningState from, ProvisioningState to, StateEvent event)>;
using StateTimeoutCallback = std::function<void(ProvisioningState state, uint32_t duration)>;

// ============================================================================
// STATE TRANSITION
// ============================================================================

struct StateTransition {
    ProvisioningState fromState;
    StateEvent event;
    ProvisioningState toState;
    std::function<bool()> guard;  // Optional guard condition
    std::function<void()> action; // Optional transition action
    
    StateTransition(ProvisioningState from, StateEvent evt, ProvisioningState to)
        : fromState(from), event(evt), toState(to), guard(nullptr), action(nullptr) {}
    
    StateTransition& withGuard(std::function<bool()> guardFn) {
        guard = guardFn;
        return *this;
    }
    
    StateTransition& withAction(std::function<void()> actionFn) {
        action = actionFn;
        return *this;
    }
    
    bool canTransition() const {
        return !guard || guard();
    }
    
    void executeAction() const {
        if (action) action();
    }
};

// ============================================================================
// STATE MANAGER CLASS
// ============================================================================

class StateManager {
public:
    StateManager();
    ~StateManager();
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    /**
     * Initialize state machine with default transitions
     */
    void initialize();
    
    /**
     * Reset state machine to IDLE
     */
    void reset();
    
    // ========================================================================
    // STATE QUERIES
    // ========================================================================
    
    ProvisioningState getCurrentState() const { return currentState; }
    ProvisioningState getPreviousState() const { return previousState; }
    const StateMachineContext& getContext() const { return context; }
    
    /**
     * Check if in a specific state
     */
    bool isInState(ProvisioningState state) const;
    
    /**
     * Check if transitioning between states
     */
    bool isTransitioning() const { return isInTransition; }
    
    /**
     * Get time spent in current state (ms)
     */
    uint32_t getTimeInCurrentState() const;
    
    /**
     * Get state history (last N states)
     */
    std::vector<ProvisioningState> getStateHistory(size_t maxCount = 10) const;
    
    // ========================================================================
    // EVENT HANDLING
    // ========================================================================
    
    /**
     * Process an event (triggers state transitions)
     * @return true if transition occurred
     */
    bool handleEvent(StateEvent event);
    
    /**
     * Process an event with context data
     */
    bool handleEvent(StateEvent event, const String& data);
    
    /**
     * Check if event is valid in current state
     */
    bool isEventValid(StateEvent event) const;
    
    // ========================================================================
    // TRANSITION MANAGEMENT
    // ========================================================================
    
    /**
     * Add custom state transition
     */
    void addTransition(const StateTransition& transition);
    
    /**
     * Remove transition
     */
    void removeTransition(ProvisioningState from, StateEvent event);
    
    /**
     * Get all valid transitions from current state
     */
    std::vector<StateEvent> getValidEvents() const;
    
    // ========================================================================
    // CALLBACK REGISTRATION
    // ========================================================================
    
    void onStateEntry(StateEntryCallback callback);
    void onStateExit(StateExitCallback callback);
    void onStateTransition(StateTransitionCallback callback);
    void onStateTimeout(StateTimeoutCallback callback);
    
    // ========================================================================
    // TIMEOUT MANAGEMENT
    // ========================================================================
    
    /**
     * Set timeout for specific state
     */
    void setStateTimeout(ProvisioningState state, uint32_t timeoutMs);
    
    /**
     * Check for state timeouts (call in loop)
     */
    void checkTimeouts();
    
    /**
     * Clear timeout for state
     */
    void clearStateTimeout(ProvisioningState state);
    
    // ========================================================================
    // CONTEXT MANAGEMENT
    // ========================================================================
    
    void setContextData(const String& key, const String& value);
    String getContextData(const String& key) const;
    void updateRetryCount(uint8_t count);
    void resetRetryCount();
    
    // ========================================================================
    // STATE PERSISTENCE
    // ========================================================================
    
    /**
     * Save current state to persistent storage
     */
    bool saveState();
    
    /**
     * Restore state from persistent storage
     */
    bool restoreState();
    
    // ========================================================================
    // DEBUGGING
    // ========================================================================
    
    String getCurrentStateName() const;
    String getEventName(StateEvent event) const;
    void dumpStateMachine() const;
    void dumpContext() const;

private:
    // Current state
    ProvisioningState currentState;
    ProvisioningState previousState;
    bool isInTransition;
    
    // Context
    StateMachineContext context;
    std::map<String, String> customContextData;
    
    // Transitions
    using TransitionKey = std::pair<ProvisioningState, StateEvent>;
    std::map<TransitionKey, StateTransition> transitions;
    
    // State history (circular buffer)
    std::vector<ProvisioningState> stateHistory;
    size_t maxHistorySize;
    
    // Timeouts
    std::map<ProvisioningState, uint32_t> stateTimeouts;
    
    // Callbacks
    StateEntryCallback entryCallback;
    StateExitCallback exitCallback;
    StateTransitionCallback transitionCallback;
    StateTimeoutCallback timeoutCallback;
    
    // Internal methods
    void defineDefaultTransitions();
    bool executeTransition(const StateTransition& transition, StateEvent event);
    void enterState(ProvisioningState newState);
    void exitState(ProvisioningState oldState);
    void recordStateInHistory(ProvisioningState state);
    void notifyTransition(ProvisioningState from, ProvisioningState to, StateEvent event);
    void notifyTimeout(ProvisioningState state, uint32_t duration);
    
    // Transition guards (example conditions)
    bool canStartProvisioning() const;
    bool canConnectWiFi() const;
    bool hasValidCredentials() const;
    
    // Transition actions (example actions)
    void onEnterIdle();
    void onEnterAdvertising();
    void onEnterConnected();
    void onEnterProvisioning();
    void onEnterError();
};

// ============================================================================
// STATE UTILITIES
// ============================================================================

class StateUtils {
public:
    static String stateToString(ProvisioningState state);
    static String eventToString(StateEvent event);
    static bool isTerminalState(ProvisioningState state);
    static bool isErrorState(ProvisioningState state);
    static bool requiresBLE(ProvisioningState state);
    static bool requiresWiFi(ProvisioningState state);
};

} // namespace WiBLE

#endif // WIBLE_STATE_MANAGER_H
