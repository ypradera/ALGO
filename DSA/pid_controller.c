/*
 * PID CONTROLLER IMPLEMENTATIONS
 * Use: Motor control, temperature regulation, servo control, process control
 *
 * Features:
 * - Basic PID with anti-windup
 * - Velocity form PID (bumpless transfer)
 * - Cascaded PID
 * - Auto-tuning helpers
 *
 * Time: O(1) per update
 * Space: O(1)
 */

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* ============================================
 * BASIC PID CONTROLLER
 * ============================================ */

typedef struct {
    // Gains
    float kp;           // Proportional gain
    float ki;           // Integral gain
    float kd;           // Derivative gain

    // State
    float integral;     // Integral accumulator
    float prev_error;   // Previous error for derivative
    float prev_input;   // Previous input (for derivative on measurement)

    // Limits
    float output_min;
    float output_max;
    float integral_min;
    float integral_max;

    // Configuration
    float dt;           // Sample time in seconds
    bool use_derivative_on_measurement;
} PIDController;

void pid_init(PIDController *pid, float kp, float ki, float kd, float dt) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->dt = dt;

    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_input = 0.0f;

    pid->output_min = -1000.0f;
    pid->output_max = 1000.0f;
    pid->integral_min = -500.0f;
    pid->integral_max = 500.0f;

    pid->use_derivative_on_measurement = false;
}

void pid_set_limits(PIDController *pid, float out_min, float out_max,
                    float int_min, float int_max) {
    pid->output_min = out_min;
    pid->output_max = out_max;
    pid->integral_min = int_min;
    pid->integral_max = int_max;
}

void pid_set_gains(PIDController *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

void pid_reset(PIDController *pid) {
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_input = 0.0f;
}

static float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float pid_update(PIDController *pid, float setpoint, float measurement) {
    float error = setpoint - measurement;

    // Proportional term
    float p_term = pid->kp * error;

    // Integral term with anti-windup
    pid->integral += error * pid->dt;
    pid->integral = clamp(pid->integral, pid->integral_min, pid->integral_max);
    float i_term = pid->ki * pid->integral;

    // Derivative term
    float d_term;
    if (pid->use_derivative_on_measurement) {
        // Derivative on measurement (avoids derivative kick on setpoint change)
        float d_input = (measurement - pid->prev_input) / pid->dt;
        d_term = -pid->kd * d_input;
        pid->prev_input = measurement;
    } else {
        // Derivative on error
        float d_error = (error - pid->prev_error) / pid->dt;
        d_term = pid->kd * d_error;
        pid->prev_error = error;
    }

    // Calculate output
    float output = p_term + i_term + d_term;
    output = clamp(output, pid->output_min, pid->output_max);

    return output;
}


/* ============================================
 * VELOCITY FORM PID (Incremental)
 * Better for bumpless transfer, no integral windup
 * ============================================ */

typedef struct {
    float kp;
    float ki;
    float kd;
    float dt;

    float prev_error;
    float prev_prev_error;
    float prev_output;

    float output_min;
    float output_max;
} PIDVelocity;

void pid_velocity_init(PIDVelocity *pid, float kp, float ki, float kd, float dt) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->dt = dt;

    pid->prev_error = 0.0f;
    pid->prev_prev_error = 0.0f;
    pid->prev_output = 0.0f;

    pid->output_min = -1000.0f;
    pid->output_max = 1000.0f;
}

void pid_velocity_reset(PIDVelocity *pid) {
    pid->prev_error = 0.0f;
    pid->prev_prev_error = 0.0f;
    pid->prev_output = 0.0f;
}

float pid_velocity_update(PIDVelocity *pid, float setpoint, float measurement) {
    float error = setpoint - measurement;

    // Velocity form: delta_output = Kp*(e - e_prev) + Ki*e*dt + Kd*(e - 2*e_prev + e_prev_prev)/dt
    float delta = pid->kp * (error - pid->prev_error)
                + pid->ki * error * pid->dt
                + pid->kd * (error - 2.0f * pid->prev_error + pid->prev_prev_error) / pid->dt;

    float output = pid->prev_output + delta;
    output = clamp(output, pid->output_min, pid->output_max);

    // Update state
    pid->prev_prev_error = pid->prev_error;
    pid->prev_error = error;
    pid->prev_output = output;

    return output;
}


/* ============================================
 * PID WITH FEED-FORWARD
 * ============================================ */

typedef struct {
    PIDController pid;
    float kf;           // Feed-forward gain
} PIDFeedForward;

void pid_ff_init(PIDFeedForward *pidff, float kp, float ki, float kd,
                 float kf, float dt) {
    pid_init(&pidff->pid, kp, ki, kd, dt);
    pidff->kf = kf;
}

float pid_ff_update(PIDFeedForward *pidff, float setpoint, float measurement,
                    float feedforward_value) {
    float fb_output = pid_update(&pidff->pid, setpoint, measurement);
    float ff_output = pidff->kf * feedforward_value;

    float output = fb_output + ff_output;
    return clamp(output, pidff->pid.output_min, pidff->pid.output_max);
}


/* ============================================
 * CASCADED PID (Inner/Outer Loop)
 * Use: Position control with velocity inner loop
 * ============================================ */

typedef struct {
    PIDController outer;
    PIDController inner;
    float inner_min;
    float inner_max;
} PIDCascade;

void pid_cascade_init(PIDCascade *cascade,
                      float outer_kp, float outer_ki, float outer_kd,
                      float inner_kp, float inner_ki, float inner_kd,
                      float dt) {
    pid_init(&cascade->outer, outer_kp, outer_ki, outer_kd, dt);
    pid_init(&cascade->inner, inner_kp, inner_ki, inner_kd, dt);
    cascade->inner_min = -100.0f;
    cascade->inner_max = 100.0f;
}

float pid_cascade_update(PIDCascade *cascade,
                         float outer_setpoint, float outer_measurement,
                         float inner_measurement) {
    // Outer loop generates setpoint for inner loop
    float inner_setpoint = pid_update(&cascade->outer, outer_setpoint, outer_measurement);
    inner_setpoint = clamp(inner_setpoint, cascade->inner_min, cascade->inner_max);

    // Inner loop generates final output
    return pid_update(&cascade->inner, inner_setpoint, inner_measurement);
}


/* ============================================
 * FIXED-POINT PID (No floating point)
 * Use: Low-power MCUs without FPU
 * ============================================ */

#define PID_FP_SCALE        1024    // 10-bit fractional
#define PID_FP_SCALE_SHIFT  10

typedef struct {
    int32_t kp;         // Scaled by PID_FP_SCALE
    int32_t ki;
    int32_t kd;

    int32_t integral;
    int32_t prev_error;

    int32_t output_min;
    int32_t output_max;
    int32_t integral_max;
} PIDFixedPoint;

void pid_fp_init(PIDFixedPoint *pid, int32_t kp, int32_t ki, int32_t kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->integral = 0;
    pid->prev_error = 0;

    pid->output_min = -32768;
    pid->output_max = 32767;
    pid->integral_max = 32767 * PID_FP_SCALE;
}

static int32_t clamp_i32(int32_t value, int32_t min, int32_t max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int32_t pid_fp_update(PIDFixedPoint *pid, int32_t setpoint, int32_t measurement) {
    int32_t error = setpoint - measurement;

    // Proportional
    int32_t p_term = (pid->kp * error) >> PID_FP_SCALE_SHIFT;

    // Integral with anti-windup
    pid->integral += pid->ki * error;
    pid->integral = clamp_i32(pid->integral, -pid->integral_max, pid->integral_max);
    int32_t i_term = pid->integral >> PID_FP_SCALE_SHIFT;

    // Derivative
    int32_t d_term = (pid->kd * (error - pid->prev_error)) >> PID_FP_SCALE_SHIFT;
    pid->prev_error = error;

    // Output
    int32_t output = p_term + i_term + d_term;
    return clamp_i32(output, pid->output_min, pid->output_max);
}


/* ============================================
 * PI CONTROLLER (Simplified, common in motor control)
 * ============================================ */

typedef struct {
    float kp;
    float ki;
    float integral;
    float integral_max;
    float output_max;
    float dt;
} PIController;

void pi_init(PIController *pi, float kp, float ki, float dt) {
    pi->kp = kp;
    pi->ki = ki;
    pi->dt = dt;
    pi->integral = 0.0f;
    pi->integral_max = 100.0f;
    pi->output_max = 100.0f;
}

float pi_update(PIController *pi, float setpoint, float measurement) {
    float error = setpoint - measurement;

    // PI calculation
    pi->integral += error * pi->dt;
    pi->integral = clamp(pi->integral, -pi->integral_max, pi->integral_max);

    float output = pi->kp * error + pi->ki * pi->integral;
    return clamp(output, -pi->output_max, pi->output_max);
}


/* ============================================
 * RATE LIMITER FOR PID OUTPUT
 * Prevents sudden output changes
 * ============================================ */

typedef struct {
    float prev_output;
    float max_rate;     // Max change per sample
} RateLimiter;

void rate_limiter_init(RateLimiter *rl, float max_rate) {
    rl->prev_output = 0.0f;
    rl->max_rate = max_rate;
}

float rate_limiter_apply(RateLimiter *rl, float input) {
    float delta = input - rl->prev_output;

    if (delta > rl->max_rate) {
        delta = rl->max_rate;
    } else if (delta < -rl->max_rate) {
        delta = -rl->max_rate;
    }

    rl->prev_output += delta;
    return rl->prev_output;
}


/* ============================================
 * SETPOINT RAMPING
 * Smooth setpoint transitions
 * ============================================ */

typedef struct {
    float current;
    float target;
    float ramp_rate;    // Units per sample
} SetpointRamp;

void setpoint_ramp_init(SetpointRamp *sr, float initial, float ramp_rate) {
    sr->current = initial;
    sr->target = initial;
    sr->ramp_rate = ramp_rate;
}

void setpoint_ramp_set_target(SetpointRamp *sr, float target) {
    sr->target = target;
}

float setpoint_ramp_update(SetpointRamp *sr) {
    float error = sr->target - sr->current;

    if (error > sr->ramp_rate) {
        sr->current += sr->ramp_rate;
    } else if (error < -sr->ramp_rate) {
        sr->current -= sr->ramp_rate;
    } else {
        sr->current = sr->target;
    }

    return sr->current;
}

bool setpoint_ramp_is_done(SetpointRamp *sr) {
    return sr->current == sr->target;
}


/* ============================================
 * AUTO-TUNING HELPERS (Ziegler-Nichols)
 * ============================================ */

typedef struct {
    float ku;           // Ultimate gain
    float tu;           // Ultimate period
} ZNParams;

// Calculate PID gains from Ziegler-Nichols ultimate method
void zn_calculate_pid(ZNParams *zn, float *kp, float *ki, float *kd) {
    *kp = 0.6f * zn->ku;
    *ki = 2.0f * (*kp) / zn->tu;
    *kd = (*kp) * zn->tu / 8.0f;
}

// Calculate PI gains (no derivative)
void zn_calculate_pi(ZNParams *zn, float *kp, float *ki) {
    *kp = 0.45f * zn->ku;
    *ki = 1.2f * (*kp) / zn->tu;
}

// Calculate P-only gain
void zn_calculate_p(ZNParams *zn, float *kp) {
    *kp = 0.5f * zn->ku;
}

// Tyreus-Luyben tuning (less aggressive than ZN)
void tl_calculate_pid(ZNParams *zn, float *kp, float *ki, float *kd) {
    *kp = 0.45f * zn->ku;
    *ki = (*kp) / (2.2f * zn->tu);
    *kd = (*kp) * zn->tu / 6.3f;
}


/* ============================================
 * MOTOR SPEED CONTROLLER EXAMPLE
 * ============================================ */

typedef struct {
    PIDController speed_pid;
    RateLimiter output_limiter;
    SetpointRamp speed_ramp;

    float current_speed;
    float pwm_output;
} MotorController;

void motor_controller_init(MotorController *mc, float dt) {
    pid_init(&mc->speed_pid, 1.0f, 0.5f, 0.1f, dt);
    pid_set_limits(&mc->speed_pid, -100.0f, 100.0f, -50.0f, 50.0f);

    rate_limiter_init(&mc->output_limiter, 10.0f);  // Max 10% change per sample
    setpoint_ramp_init(&mc->speed_ramp, 0.0f, 5.0f);  // 5 RPM per sample

    mc->current_speed = 0.0f;
    mc->pwm_output = 0.0f;
}

void motor_controller_set_speed(MotorController *mc, float target_rpm) {
    setpoint_ramp_set_target(&mc->speed_ramp, target_rpm);
}

float motor_controller_update(MotorController *mc, float measured_rpm) {
    mc->current_speed = measured_rpm;

    // Get ramped setpoint
    float setpoint = setpoint_ramp_update(&mc->speed_ramp);

    // PID calculation
    float pid_output = pid_update(&mc->speed_pid, setpoint, measured_rpm);

    // Rate limit the output
    mc->pwm_output = rate_limiter_apply(&mc->output_limiter, pid_output);

    return mc->pwm_output;
}


/* ============================================
 * TEMPERATURE CONTROLLER EXAMPLE
 * ============================================ */

typedef struct {
    PIDController temp_pid;
    float heater_output;    // 0-100%
    float deadband;         // Temperature deadband
} TempController;

void temp_controller_init(TempController *tc, float dt) {
    pid_init(&tc->temp_pid, 10.0f, 0.5f, 2.0f, dt);
    pid_set_limits(&tc->temp_pid, 0.0f, 100.0f, 0.0f, 100.0f);
    tc->temp_pid.use_derivative_on_measurement = true;  // Reduce noise sensitivity

    tc->heater_output = 0.0f;
    tc->deadband = 0.5f;  // 0.5 degree deadband
}

float temp_controller_update(TempController *tc, float setpoint, float current_temp) {
    float error = setpoint - current_temp;

    // Apply deadband
    if (error > -tc->deadband && error < tc->deadband) {
        return tc->heater_output;  // Hold current output
    }

    tc->heater_output = pid_update(&tc->temp_pid, setpoint, current_temp);
    return tc->heater_output;
}

