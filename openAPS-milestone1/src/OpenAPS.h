#ifndef OPENAPS_H
#define OPENAPS_H

#include <Arduino.h>
#include <vector>

// Structure for each insulin treatment (bolus or basal)
struct InsulinTreatment {
    long time;       // timestamp (e.g., simulation time in minutes)
    float dose;      // insulin dose in units
    int duration;    // duration of insulin action in minutes

    InsulinTreatment(long t, float d, int dur)
      : time(t), dose(d), duration(dur) {}
};

// Main OpenAPS class definition
class OpenAPS {
public:
    OpenAPS();

    void noteNewBG(float bg, long t_min);

    // Add a new insulin treatment to the list
    void addInsulinTreatment(InsulinTreatment t);

    // Compute total insulin activity and IOB at time t
    std::pair<float, float> insulin_calculations(long t);

    // Predict future BG (naive and eventual)
    std::pair<float, float> get_BG_forecast(float current_BG,
                                            float activity,
                                            float IOB
                                        );

    // Determine basal rate based on BG thresholds and forecast
    float get_basal_rate(long t, float current_BG);
    bool patient_diabetic = true;

private:
    std::vector<InsulinTreatment> treatments;

    // Constants (will be tuned or read from patient profile)
    float ISF = 3.0f;   // Insulin Sensitivity Factor (mg/dL per unit) - reduced sensitivity
    float DIA = 90.0f;    // Duration of Insulin Action (min)
    float target_BG = 120.0f;  // Higher target glucose
    float threshold_BG = 70.0f;  // Higher safety threshold
    //
    float prev_BG = NAN;
    float last_BG = NAN;        // recent BG
    long  last_BG_time = -1;    // recent BG time in min

    float last_basal_rate = 0.0f;
    bool  has_last_rate   = false;
};

#endif

