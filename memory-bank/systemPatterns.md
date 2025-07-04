# System Patterns - ddotlp

## Architecture Overview

### High-Level Design
```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   ActivityMonitor   │────│   Main Daemon    │────│   TLPManager    │
│  (CPU Monitoring)   │    │  (Coordination)  │    │ (Power Control) │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                        │                        │
         │                        │                        │
    ┌─────────┐              ┌─────────┐              ┌─────────┐
    │/proc/stat│              │ Logger  │              │   TLP   │
    │         │              │         │              │Commands │
    └─────────┘              └─────────┘              └─────────┘
```

### Component Responsibilities

#### ActivityMonitor
- **Purpose**: Monitor system activity through CPU usage
- **Data Source**: `/proc/stat` filesystem
- **Decision Logic**: CPU threshold-based activity detection
- **Callback Pattern**: Notifies main daemon of state changes
- **Threading**: Runs monitoring loop in separate thread

#### TLPManager
- **Purpose**: Interface with TLP power management system
- **Command Execution**: Executes `tlp start` and `tlp bat` commands
- **Output Capture**: Captures and processes TLP command output
- **State Tracking**: Maintains current power mode state
- **Error Handling**: Robust command failure handling

#### Logger
- **Purpose**: Centralized logging with precise timestamps
- **Format**: `[YYYY-MM-DD HH:MM:SS.mmm] [LEVEL] message`
- **Outputs**: File logging with stderr for errors/warnings
- **Rotation**: Integrated with system logrotate
- **Thread Safety**: Safe for concurrent access

#### Daemon
- **Purpose**: System service lifecycle management
- **Daemonization**: Proper double-fork daemon process
- **Signal Handling**: Graceful shutdown on SIGTERM, SIGINT
- **PID Management**: Creates and manages PID file
- **Security**: Drops unnecessary privileges where possible

## Key Design Patterns

### 1. Observer Pattern (Activity Monitoring)
```cpp
// ActivityMonitor notifies daemon of state changes
activityMonitor.setActivityCallback([&tlpManager](bool isActive) {
    if (isActive) {
        tlpManager.setPerformanceMode();
    } else {
        tlpManager.setBatteryMode();
    }
});
```

**Rationale**: Decouples activity detection from power management actions
**Benefits**: Easy to extend with additional observers, clean separation of concerns

### 2. Command Pattern (TLP Execution)
```cpp
// TLPManager encapsulates command execution
bool TLPManager::setPerformanceMode() {
    std::string output = executeCommandWithOutput("sudo tlp start 2>&1");
    // Process output and handle results
}
```

**Rationale**: Encapsulates TLP command execution with proper error handling
**Benefits**: Consistent command execution, output capture, and error handling

### 3. State Machine (Power Mode Management)
```
[Unknown] → [Performance] ⟷ [Battery]
     ↓            ↑              ↓
  TLP Check   CPU Active    CPU Idle (5min)
```

**Rationale**: Clear state transitions based on activity detection
**Benefits**: Prevents unnecessary mode switches, clear behavior definition

### 4. Strategy Pattern (Activity Detection)
Current implementation uses CPU monitoring, but architecture supports different strategies:
```cpp
class ActivityMonitor {
    virtual bool isSystemActive() = 0;  // Could be CPU, network, disk, etc.
};
```

**Rationale**: Future extensibility for different activity detection methods
**Benefits**: Easy to switch or combine detection strategies

## Critical Implementation Decisions

### 1. CPU-Based Activity Detection
**Decision**: Use `/proc/stat` CPU usage instead of X11 events
**Rationale**:
- Universal compatibility (works without GUI)
- More representative of actual system activity
- No external dependencies
- Works on servers and containers

**Trade-offs**:
- May not detect user presence without CPU activity
- Requires tuning of CPU threshold for different systems

### 2. Polling vs Event-Driven Monitoring
**Decision**: 5-second polling interval for CPU checks
**Rationale**:
- Balance between responsiveness and system overhead
- CPU usage calculation requires time intervals
- Simpler implementation than complex event systems

**Trade-offs**:
- Not instantaneous response to activity changes
- Constant low-level CPU usage for monitoring

### 3. Synchronous TLP Command Execution
**Decision**: Block on TLP command completion
**Rationale**:
- TLP commands are fast (typically <1 second)
- Need to capture output for logging
- Ensures mode changes complete before continuing

**Trade-offs**:
- Brief blocking during mode switches
- Simpler error handling and state management

### 4. Millisecond Timestamp Precision
**Decision**: Include milliseconds in all log timestamps
**Rationale**:
- Better correlation with system events
- Improved debugging capability
- Minimal performance overhead

**Benefits**:
- Clear sequence of rapid events
- Better troubleshooting of timing issues

## Error Handling Patterns

### 1. Graceful Degradation
```cpp
if (!tlpManager.isTLPAvailable()) {
    Logger::warning("TLP not available, continuing without power management");
    // Continue running for monitoring/logging
}
```

### 2. Retry Logic
```cpp
// TLP commands may fail temporarily
if (!setPerformanceMode()) {
    Logger::warning("TLP command failed, will retry on next activity");
    // Don't exit, just log and continue
}
```

### 3. Resource Cleanup
```cpp
// Proper cleanup in destructors and signal handlers
~ActivityMonitor() {
    stop();  // Ensure monitoring thread stops
}
```

## Performance Considerations

### 1. CPU Monitoring Efficiency
- 5-second intervals to minimize overhead
- Simple calculation avoiding complex parsing
- Minimal memory allocation in monitoring loop

### 2. Logging Performance
- Buffered file I/O for log writes
- Avoid logging in tight loops
- Debug level logging only when needed

### 3. Thread Management
- Single monitoring thread, not per-check threads
- Detached thread for fire-and-forget monitoring
- Clean shutdown coordination

## Security Patterns

### 1. Privilege Management
- Runs as root (required for TLP commands)
- No privilege escalation beyond initial requirements
- Minimal file system access beyond required paths

### 2. Input Validation
- Command-line argument validation
- Configuration file parsing with bounds checking
- Safe string handling throughout

### 3. Process Isolation
- Systemd security features enabled
- Private temporary directories
- Protected system directories

## Extension Points

### 1. Additional Activity Sources
- Network activity monitoring
- Disk I/O monitoring
- Process-specific monitoring
- User session detection

### 2. Power Management Backends
- Alternative to TLP
- Custom power profiles
- Hardware-specific optimizations

### 3. Notification Systems
- Desktop notifications
- Log aggregation systems
- Monitoring system integration

## Testing Strategy Patterns

### 1. Component Testing
- Mock TLP commands for testing
- Configurable CPU thresholds for testing
- Separate test builds with debug logging

### 2. Integration Testing
- Test full daemon lifecycle
- Verify systemd integration
- Test log rotation and cleanup

### 3. Performance Testing
- Long-running stability tests
- Resource usage monitoring
- Activity detection accuracy testing
