#include "FrequencyMeter.hpp"

namespace photon_mapping {
    FrequencyMeter::FrequencyMeter(QObject* parent)
        : QObject(parent)
        , m_sampleCount(10)
        , m_frequency(60.0) // oh, I wish
    {}

    int FrequencyMeter::sampleCount() const {
        return m_sampleCount;
    }
    void FrequencyMeter::setSampleCount(int sampleCount) {
        m_sampleCount = sampleCount;
    }

    void FrequencyMeter::operator()() {
        if (!m_elapsedTimer.isValid()) {
            m_elapsedTimer.start();
            return;
        }
        double sampleTime = m_elapsedTimer.nsecsElapsed() / 1e9;
        m_elapsedTimer.restart();
        m_sampleTimes.push_back(sampleTime);
        if (m_sampleTimes.size() > m_sampleCount) {
            m_sampleTimes.pop_front();
        }
        double totalTime = 0.0;
        for (double sampleTime : m_sampleTimes) {
            totalTime += sampleTime;
        }
        double time = totalTime / m_sampleTimes.size();
        m_frequency = 1.0 / time;
        emit frequencyChanged(m_frequency);
    }

    double FrequencyMeter::frequency() const {
        return m_frequency;
    }

    void FrequencyMeter::setFrequency(double frequency) {
        m_frequency = frequency;
        double time = 1.0 / frequency;
        m_sampleTimes.resize(m_sampleCount);
        for (double& sampleTime : m_sampleTimes) {
            sampleTime = time;
        }
        emit frequencyChanged(m_frequency);
    }
}