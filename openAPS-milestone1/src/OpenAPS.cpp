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

    // TODO (Milestone 2): compute activity and IOB using time, dose, duration
    // Example outline:
    // for each treatment:
    //     if (t - treat.time < treat.duration)
    //         activity += f(t - treat.time)
    //         IOB += g(t - treat.time)
    return {total_activity, total_IOB};
}

// Predict future BG using Eventual BG algorithm
// (Placeholder for Milestone 1; full logic in Milestone 2)
std::pair<float, float> OpenAPS::get_BG_forecast(float current_BG,
                                                 float activity,
                                                 float IOB) {
    // Example placeholder computation
    float naive_eventual_BG = current_BG - activity * ISF;
    float eventual_BG       = naive_eventual_BG - IOB * ISF;
    return {naive_eventual_BG, eventual_BG};
}

// Determine basal insulin rate
// (Placeholder for Milestone 1; control logic in Milestone 2)
float OpenAPS::get_basal_rate(long t, float current_BG) {
    float basal_rate = 0.0f;

    // TODO (Milestone 2): implement control logic based on BG levels
    // Example structure:
    // if (current_BG > target_BG) → increase basal
    // else if (current_BG < threshold_BG) → decrease basal
    // add new treatment to list
    return basal_rate;
}
