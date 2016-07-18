#ifndef PHOTON_MAPPING_WORKER_SET_HPP
#define PHOTON_MAPPING_WORKER_SET_HPP

#include <vector>

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QAtomicInt>

namespace photon_mapping {
    class WorkerSet;

    class Worker
        : public QThread
    {
        Q_OBJECT
        public:
            Worker(WorkerSet* set, int index);

        protected:
            virtual void run();
        
        private:
            WorkerSet* m_set;
            int m_index;
    };

    class WorkerSet
        : public QObject
    {
        Q_OBJECT
        friend Worker;
        public:
            WorkerSet();
            virtual ~WorkerSet();
        
            void start();
            void abort();

        protected:
            int workerCount() const;
            void checkInterrupts() const;
            virtual bool nextTask() = 0;
            virtual void doTaskPart(int workerIndex) = 0;
            virtual void taskDone();
            virtual void taskAborted();

        private:
            struct AbortInterrupt {};
            struct ExitInterrupt {};

            int const m_workerCount;
            std::vector<Worker*> m_workers;

            QMutex m_syncronizer;
            QWaitCondition m_taskStarted;
            QWaitCondition m_allAborted;
            int m_workerDoneCount;
            int m_workerAbortedCount;
            QAtomicInt m_abortRequested;
            QAtomicInt m_exitRequested;

            void workerLoop(int workerIndex);
    };
}

#include "WorkerSet.inl"

#endif
