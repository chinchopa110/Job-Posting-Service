#include "repository/JobPostingRepository.hpp"
#include <shared_mutex>

JobPostingRepository::JobPostingRepository()
    : nextId_(1) {}

JobPostingRepository::~JobPostingRepository() {}

size_t JobPostingRepository::add(const JobPosting& jobPosting) {
    std::unique_lock lock(mutex_);
    size_t id = nextId_++;
    JobPosting newJobPosting = jobPosting;
    newJobPosting.setId(id);
    jobPostings_[id] = newJobPosting;
    return id;
}

bool JobPostingRepository::update(const JobPosting& jobPosting) {
    std::unique_lock lock(mutex_);
    auto it = jobPostings_.find(jobPosting.getId());
    if (it != jobPostings_.end()) {
        jobPostings_[jobPosting.getId()] = jobPosting;
        return true;
    }
    return false;
}

bool JobPostingRepository::remove(size_t id) {
    std::unique_lock lock(mutex_);
    return jobPostings_.erase(id) > 0;
}

std::optional<JobPosting> JobPostingRepository::findById(size_t id) const {
    std::shared_lock lock(mutex_);
    auto it = jobPostings_.find(id);
    if (it != jobPostings_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<JobPosting> JobPostingRepository::findAll() const {
    std::shared_lock lock(mutex_);
    std::vector<JobPosting> result;
    result.reserve(jobPostings_.size());
    for (const auto& pair : jobPostings_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<JobPosting> JobPostingRepository::findByEmployerId(size_t employerId) const {
    std::shared_lock lock(mutex_);
    std::vector<JobPosting> result;
    for (const auto& pair : jobPostings_) {
        if (pair.second.getEmployerId() == employerId) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<JobPosting> JobPostingRepository::findByCity(const std::string& city) const {
    std::shared_lock lock(mutex_);
    std::vector<JobPosting> result;
    for (const auto& pair : jobPostings_) {
        if (pair.second.getCity() == city) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<JobPosting> JobPostingRepository::findByProfession(const std::string& profession) const {
    std::shared_lock lock(mutex_);
    std::vector<JobPosting> result;
    for (const auto& pair : jobPostings_) {
        if (pair.second.getProfession() == profession) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<JobPosting> JobPostingRepository::findByRemote(bool isRemote) const {
    std::shared_lock lock(mutex_);
    std::vector<JobPosting> result;
    for (const auto& pair : jobPostings_) {
        if (pair.second.isRemote() == isRemote) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<JobPosting> JobPostingRepository::findBySalaryRange(int minSalary, int maxSalary) const {
    std::shared_lock lock(mutex_);
    std::vector<JobPosting> result;
    for (const auto& pair : jobPostings_) {
        int salary = pair.second.getSalary();
        if (salary >= minSalary && salary <= maxSalary) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<JobPosting> JobPostingRepository::findByDateRange(
    const std::chrono::system_clock::time_point& startDate,
    const std::chrono::system_clock::time_point& endDate) const {
    std::shared_lock lock(mutex_);
    std::vector<JobPosting> result;
    for (const auto& pair : jobPostings_) {
        const auto& publishDate = pair.second.getPublishDate();
        if (publishDate >= startDate && publishDate <= endDate) {
            result.push_back(pair.second);
        }
    }
    return result;
}