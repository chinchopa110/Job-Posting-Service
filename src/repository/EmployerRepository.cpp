#include "repository/EmployerRepository.hpp"
#include <shared_mutex>

EmployerRepository::EmployerRepository()
    : nextId_(1) {}

EmployerRepository::~EmployerRepository() {}

size_t EmployerRepository::add(const Employer& employer) {
    std::unique_lock lock(mutex_);
    size_t id = nextId_++;
    Employer newEmployer = employer;
    newEmployer.setId(id);
    employers_[id] = newEmployer;
    return id;
}

bool EmployerRepository::update(const Employer& employer) {
    std::unique_lock lock(mutex_);
    auto it = employers_.find(employer.getId());
    if (it != employers_.end()) {
        employers_[employer.getId()] = employer;
        return true;
    }
    return false;
}

bool EmployerRepository::remove(size_t id) {
    std::unique_lock lock(mutex_);
    return employers_.erase(id) > 0;
}

std::optional<Employer> EmployerRepository::findById(size_t id) const {
    std::shared_lock lock(mutex_);
    auto it = employers_.find(id);
    if (it != employers_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<Employer> EmployerRepository::findAll() const {
    std::shared_lock lock(mutex_);
    std::vector<Employer> result;
    result.reserve(employers_.size());
    for (const auto& pair : employers_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<Employer> EmployerRepository::findByCity(const std::string& city) const {
    std::shared_lock lock(mutex_);
    std::vector<Employer> result;
    for (const auto& pair : employers_) {
        if (pair.second.getCity() == city) {
            result.push_back(pair.second);
        }
    }
    return result;
}