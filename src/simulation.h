#ifndef AIRPORT_SIMULATION_H
#define AIRPORT_SIMULATION_H

#include <algorithm>
#include <cmath>

namespace airport {
constexpr int WIDTH = 1200;
constexpr int HEIGHT = 700;
constexpr double PI = 3.14159265358979323846;
constexpr double WHEEL_Y = 538.0;
constexpr double GEAR_BOTTOM = 29.0;
constexpr double TOUCH_X = 525.0;
constexpr double STOP_X = 985.0;

struct Point { double x, y; };

// Local coordinates -> scale -> rotation -> translation (screen y points down).
inline Point transform(Point p, Point translation, double scale, double degrees) {
    const double a = degrees * PI / 180.0;
    return {translation.x + scale * (p.x * std::cos(a) - p.y * std::sin(a)),
            translation.y + scale * (p.x * std::sin(a) + p.y * std::cos(a))};
}
inline double lerp(double a, double b, double t) { return a + (b - a) * t; }
inline double smooth(double t) { return t * t * (3.0 - 2.0 * t); }

enum class LandingState { WAITING, APPROACH, DESCENT, FINAL_APPROACH, TOUCHDOWN, BRAKING, LANDED };
inline const char* stateName(LandingState s) {
    switch (s) {
    case LandingState::WAITING: return "AWAITING CLEARANCE";
    case LandingState::APPROACH: return "APPROACHING";
    case LandingState::DESCENT: return "DESCENDING";
    case LandingState::FINAL_APPROACH: return "FINAL APPROACH";
    case LandingState::TOUCHDOWN: return "TOUCHDOWN";
    case LandingState::BRAKING: return "BRAKING";
    case LandingState::LANDED: return "LANDED";
    }
    return "UNKNOWN";
}

struct Simulation {
    LandingState state = LandingState::WAITING;
    bool paused = false;
    bool night = false;
    double rate = 1.0;
    double clock = 0.0;
    double flightTime = 0.0;
    double stageTime = 0.0;
    double x = 100.0, y = 195.0, scale = 0.72, angle = 3.0;
    double velocity = 0.0; // screen pixels per simulated second
    double gear = 0.0;
    double smokeAge = -1.0;

    void restart() {
        const bool mode = night;
        const double speedSetting = rate;
        *this = Simulation{};
        night = mode;
        rate = speedSetting;
    }
    void start() {
        if (state == LandingState::WAITING) { state = LandingState::APPROACH; velocity = 38.0; }
    }
    void changeRate(double factor) { rate = std::max(0.25, std::min(4.0, rate * factor)); }
    double wheelHeight() const {
        // Contact uses the lowest of both transformed wheels, including radius.
        return std::max(transform({-42, 25}, {x,y}, scale, angle).y,
                        transform({65, 25}, {x,y}, scale, angle).y) + 4.0 * scale;
    }
    double altitude() const { return std::max(0.0, (WHEEL_Y - wheelHeight()) * 3.0); }
    double distance() const { return std::max(0.0, (TOUCH_X - x) * 4.0); }
    double speedKnots() const { return velocity * 2.4; }

    void step(double dt) {
        clock += dt;
        if (state == LandingState::WAITING || state == LandingState::LANDED) return;
        stageTime += dt;
        if (smokeAge >= 0.0) smokeAge += dt;
        if (state == LandingState::APPROACH || state == LandingState::DESCENT ||
            state == LandingState::FINAL_APPROACH) {
            flightTime = std::min(12.0, flightTime + dt);
            const double t = flightTime / 12.0;
            x = lerp(100.0, TOUCH_X, t);
            // A smoothstep descent flattens at contact, so no vertical snap occurs.
            y = lerp(195.0, WHEEL_Y - GEAR_BOTTOM, smooth(t));
            scale = lerp(0.72, 1.0, smooth(t));
            angle = t < 0.65 ? lerp(3.0, 1.0, t / 0.65)
                                 : lerp(1.0, 0.0, smooth((t - 0.65) / 0.35));
            gear = std::max(0.0, std::min(1.0, (t - 0.30) / 0.28));
            velocity = (TOUCH_X - 100.0) / 12.0;
            state = t < 0.34 ? LandingState::APPROACH :
                    t < 0.72 ? LandingState::DESCENT : LandingState::FINAL_APPROACH;
            if (flightTime >= 12.0) {
                state = LandingState::TOUCHDOWN;
                stageTime = 0.0;
                smokeAge = 0.0;
                angle = 0.0; scale = 1.0; gear = 1.0;
                y = WHEEL_Y - GEAR_BOTTOM;
            }
        } else if (state == LandingState::TOUCHDOWN) {
            x += velocity * dt;
            if (stageTime >= 0.7) { state = LandingState::BRAKING; stageTime = 0.0; }
        } else if (state == LandingState::BRAKING) {
            // v^2 = 2*a*remainingDistance: constant deceleration with an exact stop.
            const double remaining = std::max(0.0, STOP_X - x);
            const double acceleration = remaining > 0.0 ? velocity * velocity / (2.0 * remaining) : 0.0;
            if (remaining <= 0.001 || velocity <= acceleration * dt) {
                x = STOP_X; velocity = 0.0; state = LandingState::LANDED;
            } else {
                x += velocity * dt - 0.5 * acceleration * dt * dt;
                velocity = std::max(0.0, velocity - acceleration * dt);
            }
        }
    }
    void update(double realSeconds) {
        if (paused || realSeconds <= 0.0) return;
        // Small substeps keep event boundaries and wheel contact stable at 4x speed.
        double remaining = realSeconds * rate;
        while (remaining > 1e-9) {
            const double dt = std::min(remaining, 1.0 / 120.0);
            step(dt);
            remaining -= dt;
        }
    }
};
} // namespace airport
#endif
