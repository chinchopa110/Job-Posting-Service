#include "JobService.hpp"
#include "utils/ThreadPool.hpp"
#include "filter/JobFilter.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <atomic>
#include <chrono>
#include <mutex>

int main() {
    const size_t numThreads = 4;
    std::cout << "=== JobService: thread pool (" << numThreads << " threads) ===" << std::endl;

    JobService service;
    ThreadPool pool(numThreads);

    std::vector<size_t> workerIds;
    std::vector<size_t> employerIds;
    std::mutex idsMutex;
    std::atomic<size_t> errors{0};
    auto t0 = std::chrono::steady_clock::now();

    auto fWorkers = pool.submit([&]() {
        std::vector<std::string> skills = {"C++", "Python"};
        size_t count = 0;
        for (int i = 0; i < 50; ++i) {
            try {
                size_t id = service.createWorker(
                    "Worker_" + std::to_string(i),
                    "worker" + std::to_string(i) + "@test.com",
                    skills, 1 + (i % 10), "Moscow", "Resume");
                if (id != 0) {
                    ++count;
                    std::lock_guard<std::mutex> lock(idsMutex);
                    workerIds.push_back(id);
                } else ++errors;
            } catch (...) { ++errors; }
        }
        return count;
    });

    auto fEmployers = pool.submit([&]() {
        size_t count = 0;
        for (int i = 0; i < 30; ++i) {
            try {
                size_t id = service.createEmployer(
                    "Company_" + std::to_string(i),
                    "company" + std::to_string(i) + "@test.com",
                    "Description " + std::to_string(i), "Moscow");
                if (id != 0) {
                    ++count;
                    std::lock_guard<std::mutex> lock(idsMutex);
                    employerIds.push_back(id);
                } else ++errors;
            } catch (...) { ++errors; }
        }
        return count;
    });

    size_t workersCreated = fWorkers.get();
    size_t employersCreated = fEmployers.get();
    std::cout << "  Workers created: " << workersCreated << ", Employers created: " << employersCreated << std::endl;

    std::vector<std::future<size_t>> jobFutures;
    for (size_t t = 0; t < numThreads; ++t) {
        jobFutures.push_back(pool.submit([&, t]() {
            std::vector<size_t> empIds;
            { std::lock_guard<std::mutex> lock(idsMutex); empIds = employerIds; }
            size_t added = 0;
            for (size_t i = t; i < empIds.size(); i += numThreads) {
                try {
                    size_t id = service.createJobPosting(empIds[i],
                        "Job_" + std::to_string(i), "Description", 50000 + int(i) * 1000,
                        "Moscow", (i % 2 == 0), "Engineer");
                    if (id != 0) ++added;
                } catch (...) { ++errors; }
            }
            return added;
        }));
    }
    size_t jobsCreated = 0;
    for (auto& f : jobFutures) jobsCreated += f.get();
    std::cout << "  Job postings created: " << jobsCreated << std::endl;

    std::vector<std::future<size_t>> readFutures;
    for (size_t t = 0; t < numThreads; ++t) {
        readFutures.push_back(pool.submit([&, t]() {
            size_t replies = 0;
            std::vector<size_t> wIds, eIds;
            { std::lock_guard<std::mutex> lock(idsMutex); wIds = workerIds; eIds = employerIds; }
            if (eIds.empty() || wIds.empty()) return size_t(0);
            for (size_t e : eIds) {
                auto jobs = service.getJobPostingsByEmployer(e);
                for (const auto& job : jobs) {
                    for (size_t wi = t; wi < wIds.size() && wi < t + 5; wi += numThreads) {
                        try {
                            if (service.createReply(job.getId(), wIds[wi]) != 0) ++replies;
                        } catch (...) { ++errors; }
                    }
                }
            }
            return replies;
        }));
    }
    size_t totalReplies = 0;
    for (auto& f : readFutures) totalReplies += f.get();
    std::cout << "  Replies created: " << totalReplies << std::endl;

    std::vector<std::future<size_t>> searchFutures;
    for (size_t t = 0; t < numThreads; ++t) {
        searchFutures.push_back(pool.submit([&]() {
            JobFilter filter;
            filter.setMinSalary(50000);
            filter.setIsRemote(true);
            auto jobs = service.searchJobPostings(filter);
            return jobs.size();
        }));
    }
    size_t searchCount = 0;
    for (auto& f : searchFutures) searchCount += f.get();
    std::cout << "  Search (min salary 50k, remote) total hits: " << searchCount << std::endl;

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "  Errors: " << errors.load() << std::endl;
    std::cout << "  Time: " << ms << " ms" << std::endl;
    std::cout << "=== Done ===" << std::endl;
    return errors.load() > 0 ? 1 : 0;
}
