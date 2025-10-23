#include "OpenAPS.h"

// Constructor
OpenAPS::OpenAPS() {}

// Add insulin treatment
void OpenAPS::addInsulinTreatment(InsulinTreatment t) {
    treatments.push_back(t);
}

// Calculate insulin activity and IOB
// (Placeholder for Milestone 1; implement in Milestone 2)
std::pair<float, float> OpenAPS::insulin_calculations(long t) {
    float total_activity = 0.0f;
    float total_IOB = 0.0f;
    
    const float peak_frac = 75.0f / 180.0f; // based on OPENAPS PPT
    const float DIA_min = DIA * 60.0f;
    // TODO (Milestone 2): compute activity and IOB using time, dose, duration
    // Example outline:
    // for each treatment:
    //     if (t - treat.time < treat.duration)
    //         activity += f(t - treat.time)
    //         IOB += g(t - treat.time)
    for (const auto& tr : treatments) {
        // get time difference btw now and last dose time
        long dt = t - tr.time;
        if (dt < 0){
            total_IOB += tr.dose;   // not effective yet, add all the dose in IOB
        }

        if (dt >= (long)DIA_min) {
            // this dose is done → activity=0, IOB=0
            continue;
        }
        
        // by using this algorithm, no matter how much or less the insulin, time is always DIA_min
        const float t_peak = DIA_min * peak_frac;
        const float peak   = (2.0f * tr.dose) / DIA_min;

        // ---- calculate current activity(t)（U/min）----
        float act_t = 0.0f;
        if (dt <= t_peak) {
            // increase part
            act_t = peak * (static_cast<float>(dt) / t_peak);
        } else {
            // decrease part：from peak to 0
            float down_len = DIA_min - t_peak;
            act_t = peak * ((DIA_min - static_cast<float>(dt)) / down_len);
        }
        if (act_t < 0) act_t = 0;

        // ---- calculate the used to t = area under bi-linear (triangle)----
        float used = 0.0f;
        if (dt <= t_peak) {
            // area =  0.5 * dt (width) * act_t (height)
            // = 0.5 * (peak / t_peak) * dt^2
            used = 0.5f * (peak / t_peak) * (static_cast<float>(dt) * static_cast<float>(dt));
        } else {
            // left area
            float area_left = 0.5f * t_peak * peak;

            // right area (trapezoid)
            float width = static_cast<float>(dt) - t_peak;
            used = area_left + 0.5f * (peak + act_t) * width;
        }

        // IOB: btw 0~dose
        float iob = tr.dose - used;
        if (iob < 0) iob = 0.0f;
        if (iob > tr.dose) iob = tr.dose;

        total_activity += act_t;    // current activity（U/min）
        total_IOB      += iob;      // leftover（U）

    }

    return {total_activity, total_IOB};
}

// Predict future BG using Eventual BG algorithm
// (Placeholder for Milestone 1; full logic in Milestone 2)
// TODO: USE COB FOR EXTRA CREDITS
std::pair<float, float> OpenAPS::get_BG_forecast(float current_BG,
                                                 float activity,
                                                 float IOB
                                                ) {

    float naive_eventual_BG = current_BG - (IOB * ISF);
    // deviation：if no BG_5min，naive = eventual
    float deviation = 0.0f;
    // TODO: everytime onMqttMessage deal with CGM, update this value
    if (!isnan(prev_BG)) {
        float delta    = current_BG - prev_BG;      // actual change in 5 min
        float predBGI  = - activity * ISF * 5.0f;        // predict 5 min change
        deviation      = (30.0f / 5.0f) * (delta - predBGI); // 30 min prediction
    }

    float eventual_BG = naive_eventual_BG + deviation;

    return {naive_eventual_BG, eventual_BG};
}

// Determine basal insulin rate
// (Placeholder for Milestone 1; control logic in Milestone 2)
float OpenAPS::get_basal_rate(long t, float current_BG) {

    // TODO (Milestone 2): implement control logic based on BG levels
    // Example structure:
    // if (current_BG > target_BG) → increase basal
    // else if (current_BG < threshold_BG) → decrease basal
    // add new treatment to list
    // ---- customize variables (SET BY US) ----
    const float basal_default = 0.5f;   // U/hr
    const float basal_min     = 0.0f;   // U/hr
    const float basal_max     = 2.0f;   // U/hr
    const float kWindowMin    = 5.0f;   // control window（min）

    auto clampf = [](float v, float lo, float hi){
        return v < lo ? lo : (v > hi ? hi : v);
    };

    float basal_rate = basal_default;

    // 1) get activity / IOB，and predict BG
    auto [activity, IOB] = insulin_calculations(t);
    auto [naive_eventual_BG, eventual_BG] = get_BG_forecast(current_BG, activity, IOB);

    if (current_BG < threshold_BG || eventual_BG < threshold_BG) {
        // set insulin rate to 0
        basal_rate = 0.0f;

    } else if (eventual_BG < target_BG) {
        if (naive_eventual_BG < 40.0f) {
            basal_rate = 0.0f;
        } else {
            // multiplication by 2 to increase hypo safety (feel free to tune)
            float insulinReq = 2.0f * (eventual_BG - target_BG) / ISF;  // U 
            basal_rate = basal_default + (insulinReq / DIA);             // U/hr
            //set rate to (current basal + insulinReq / DIA) to deliver insulinReq less insulin over DIA mins
        }

    } else if (eventual_BG > target_BG) {
        float insulinReq = (eventual_BG - target_BG) / ISF / 10.0f;      // U
        basal_rate = basal_default + (insulinReq / DIA);                 // U/hr

    } else {
        // maintain
        basal_rate = basal_default;
    }

    // limit in the boundary
    basal_rate = clampf(basal_rate, basal_min, basal_max);

    // 3) record this treatment，calculate further IOB/Activity
    float dose_this_window = basal_rate * (kWindowMin / 60.0f);
    addInsulinTreatment(InsulinTreatment{ t, dose_this_window, static_cast<int>(kWindowMin) });

    // debug print
    Serial.print("[Basal] t="); Serial.print(t);
    Serial.print(" BG="); Serial.print(current_BG, 1);
    Serial.print(" naive="); Serial.print(naive_eventual_BG, 1);
    Serial.print(" eventual="); Serial.print(eventual_BG, 1);
    Serial.print(" rate="); Serial.println(basal_rate, 3);

    return basal_rate;
}


void OpenAPS::noteNewBG(float bg, long t_min) {
    // first time
    if (last_BG_time < 0) {
        last_BG = bg;
        last_BG_time = t_min;
        return;
    }
    long dt = t_min - last_BG_time;   // minute difference
    if (dt >= 5) {
        // if over five min, update prev_bg
        prev_BG = last_BG;
        last_BG = bg;
        last_BG_time = t_min;
    } else {
        // if less the 5 min, don't update prev_BG
        last_BG = bg;
        last_BG_time = t_min;
    }
}