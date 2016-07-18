#include "WorkerSet.hpp"

#include <QMutexLocker>

namespace photon_mapping {
    Worker::Worker(WorkerSet* set, int index)
        : m_set(set)
        , m_index(index)
    {}

    void Worker::run() {
        m_set->workerLoop(m_index);
    }

    WorkerSet::WorkerSet()
        : m_workerCount(QThread::idealThreadCount())
        , m_workers(m_workerCount)
        , m_workerDoneCount(0)
        , m_workerAbortedCount(0)
        , m_abortRequested(false)
        , m_exitRequested(false)
    {
        for (int workerIndex = 0; workerIndex < m_workerCount; ++workerIndex) {
            m_workers[workerIndex] = new Worker(this, workerIndex);
        }
        QMutexLocker syncronizerLocker(&m_syncronizer);
        m_abortRequested.store(true);
        for (auto& worker : m_workers) {
            worker->start();
        }
        m_allAborted.wait(&m_syncronizer);
        m_abortRequested.store(false);
    }

    WorkerSet::~WorkerSet() {
        m_syncronizer.lock();
        m_exitRequested.store(true);
        m_taskStarted.wakeAll();
        m_syncronizer.unlock();
        for (auto& worker : m_workers) {
            worker->wait();
        }
    }

    void WorkerSet::start() {
        m_taskStarted.wakeAll();
    }

    void WorkerSet::abort() {
        QMutexLocker syncronizerLocker(&m_syncronizer);
        if (m_workerDoneCount + m_workerAbortedCount < m_workerCount) {
            m_abortRequested.store(true);
            m_allAborted.wait(&m_syncronizer);
            m_abortRequested.store(false);
        }
    }

    void WorkerSet::taskDone() {
        // do nothing
    }

    void WorkerSet::taskAborted() {
        // do nothing
    }

    void WorkerSet::workerLoop(int workerIndex) {
        try {
            while (true) {
                bool taskPartAborted;
                try {
                    checkInterrupts();
                    doTaskPart(workerIndex);
                    taskPartAborted = false;
                } catch (AbortInterrupt) {
                    taskPartAborted = true;
                }

                QMutexLocker syncronizerLocker(&m_syncronizer);
                if (taskPartAborted) {
                    ++m_workerAbortedCount;
                } else {
                    ++m_workerDoneCount;
                }
                if (m_workerDoneCount + m_workerAbortedCount < m_workerCount) {
                    m_taskStarted.wait(&m_syncronizer);
                } else {
                    if (m_workerAbortedCount > 0) {
                        taskAborted();
                        m_allAborted.wakeOne();
                        m_taskStarted.wait(&m_syncronizer);
                    } else {
                        taskDone();
                        if (nextTask()) {
                            m_taskStarted.wakeAll();
                        } else {
                            m_taskStarted.wait(&m_syncronizer);
                        }
                    }
                }
                if (taskPartAborted) {
                    --m_workerAbortedCount;
                } else {
                    --m_workerDoneCount;
                }
            }
        } catch (ExitInterrupt) {
             // do nothing
        }
    }
}