#ifndef PHOTON_MAPPING_FREQUENCY_METER_HPP
#define PHOTON_MAPPING_FREQUENCY_METER_HPP

#include <deque>

#include <QObject>
#include <QElapsedTimer>

namespace photon_mapping {
    class FrequencyMeter
        : public QObject
    {
        Q_OBJECT
        public:
            FrequencyMeter(QObject* parent = 0);

            int sampleCount() const;
            void setSampleCount(int sampleCount);

            void operator()();

            double frequency() const;
            void setFrequency(double frequency);
    
        signals:
            void frequencyChanged(double frequency);

        private:
            QElapsedTimer m_elapsedTimer;
            int m_sampleCount;
            std::deque<double> m_sampleTimes;
            double m_frequency;
    };
}

#endif
