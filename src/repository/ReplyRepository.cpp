#include "repository/ReplyRepository.hpp"
#include <shared_mutex>

ReplyRepository::ReplyRepository()
    : nextId_(1) {}

ReplyRepository::~ReplyRepository() {}

size_t ReplyRepository::add(const Reply& reply) {
    std::unique_lock lock(mutex_);
    size_t id = nextId_++;
    Reply newReply = reply;
    newReply.setId(id);
    replies_[id] = newReply;
    return id;
}

bool ReplyRepository::update(const Reply& reply) {
    std::unique_lock lock(mutex_);
    auto it = replies_.find(reply.getId());
    if (it != replies_.end()) {
        replies_[reply.getId()] = reply;
        return true;
    }
    return false;
}

bool ReplyRepository::remove(size_t id) {
    std::unique_lock lock(mutex_);
    return replies_.erase(id) > 0;
}

std::optional<Reply> ReplyRepository::findById(size_t id) const {
    std::shared_lock lock(mutex_);
    auto it = replies_.find(id);
    if (it != replies_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<Reply> ReplyRepository::findAll() const {
    std::shared_lock lock(mutex_);
    std::vector<Reply> result;
    result.reserve(replies_.size());
    for (const auto& pair : replies_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<Reply> ReplyRepository::findByJobPostingId(size_t jobPostingId) const {
    std::shared_lock lock(mutex_);
    std::vector<Reply> result;
    for (const auto& pair : replies_) {
        if (pair.second.getJobPostingId() == jobPostingId) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Reply> ReplyRepository::findByWorkerId(size_t workerId) const {
    std::shared_lock lock(mutex_);
    std::vector<Reply> result;
    for (const auto& pair : replies_) {
        if (pair.second.getWorkerId() == workerId) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Reply> ReplyRepository::findByStatus(ReplyStatus status) const {
    std::shared_lock lock(mutex_);
    std::vector<Reply> result;
    for (const auto& pair : replies_) {
        if (pair.second.getStatus() == status) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool ReplyRepository::existsByJobPostingAndWorker(size_t jobPostingId, size_t workerId) const {
    std::shared_lock lock(mutex_);
    for (const auto& pair : replies_) {
        if (pair.second.getJobPostingId() == jobPostingId && 
            pair.second.getWorkerId() == workerId) {
            return true;
        }
    }
    return false;
}