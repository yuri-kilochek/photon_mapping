namespace photon_mapping {
    inline int WorkerSet::workerCount() const {
        return m_workerCount;
    }

    inline void WorkerSet::checkInterrupts() const {
        if (m_abortRequested.load()) {
            throw AbortInterrupt();
        }
        if (m_exitRequested.load()) {
            throw ExitInterrupt();
        }
    }
}