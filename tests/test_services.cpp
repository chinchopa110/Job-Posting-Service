#include "service/WorkerService.hpp"
#include "service/EmployerService.hpp"
#include "service/JobPostingService.hpp"
#include "service/ReplyService.hpp"
#include "JobService.hpp"
#include "filter/JobFilter.hpp"
#include <gtest/gtest.h>
#include <vector>


TEST(WorkerService, CreateWorker) {
    WorkerService service;
    std::vector<std::string> skills = {"C++", "Python"};
    size_t id = service.createWorker("John Doe", "john@email.com", skills, 5, "Moscow", "Resume");
    EXPECT_NE(0u, id);
}

TEST(WorkerService, UpdateWorker) {
    WorkerService service;
    std::vector<std::string> skills = {"C++"};
    size_t id = service.createWorker("John", "john@email.com", skills, 5, "Moscow", "Resume");

    Worker worker(id, "John Updated", "john.updated@email.com", {"C++", "Java"}, 6, "Saint Petersburg", "Updated Resume");
    bool result = service.updateWorker(worker);
    EXPECT_TRUE(result);
}


TEST(EmployerService, CreateEmployer) {
    EmployerService service;
    size_t id = service.createEmployer("Tech Corp", "info@techcorp.com", "Description", "Moscow");
    EXPECT_NE(0u, id);
}

TEST(EmployerService, UpdateEmployer) {
    EmployerService service;
    size_t id = service.createEmployer("Tech Corp", "info@techcorp.com", "Description", "Moscow");

    Employer employer(id, "Tech Corp Updated", "updated@techcorp.com", "Updated Description", "Saint Petersburg");
    bool result = service.updateEmployer(employer);
    EXPECT_TRUE(result);
}

TEST(EmployerService, GetAllEmployers) {
    EmployerService service;
    service.createEmployer("Company 1", "c1@email.com", "Desc 1", "Moscow");
    service.createEmployer("Company 2", "c2@email.com", "Desc 2", "Saint Petersburg");

    auto employers = service.getAllEmployers();
    EXPECT_EQ(2u, employers.size());
}


TEST(JobPostingService, CreateJobPosting) {
    JobPostingService service;
    size_t id = service.createJobPosting(1, "Developer", "Job description", 100000, "Moscow", true, "Software Engineer");
    EXPECT_NE(0u, id);
}

TEST(JobPostingService, DeleteJobPosting) {
    JobPostingService service;
    size_t id = service.createJobPosting(1, "Developer", "Job description", 100000, "Moscow", true, "Software Engineer");

    bool result = service.deleteJobPosting(id);
    EXPECT_TRUE(result);
}

TEST(JobPostingService, GetJobPostingsByEmployer) {
    JobPostingService service;
    service.createJobPosting(1, "Job 1", "Desc 1", 50000, "Moscow", false, "Engineer");
    service.createJobPosting(1, "Job 2", "Desc 2", 60000, "Moscow", true, "Engineer");
    service.createJobPosting(2, "Job 3", "Desc 3", 70000, "Saint Petersburg", false, "Manager");

    auto jobs = service.getJobPostingsByEmployer(1);
    EXPECT_EQ(2u, jobs.size());
}

TEST(JobPostingService, SearchJobPostingsWithFilters) {
    JobPostingService service;
    service.createJobPosting(1, "Job 1", "Desc 1", 50000, "Moscow", false, "Engineer");
    service.createJobPosting(1, "Job 2", "Desc 2", 60000, "Moscow", true, "Engineer");
    service.createJobPosting(1, "Job 3", "Desc 3", 70000, "Moscow", true, "Engineer");

    JobFilter filter;
    filter.setMinSalary(55000);
    filter.setIsRemote(true);

    auto jobs = service.searchJobPostings(filter);
    EXPECT_EQ(2u, jobs.size());
}

TEST(JobPostingService, SearchJobPostingsNoFilters) {
    JobPostingService service;
    service.createJobPosting(1, "Job 1", "Desc 1", 50000, "Moscow", false, "Engineer");
    service.createJobPosting(1, "Job 2", "Desc 2", 60000, "Moscow", true, "Engineer");

    JobFilter filter;
    auto jobs = service.searchJobPostings(filter);
    EXPECT_EQ(2u, jobs.size());
}


TEST(ReplyService, CreateReply) {
    ReplyService service;
    size_t id = service.createReply(1, 1);
    EXPECT_NE(0u, id);
}

TEST(ReplyService, GetRepliesByJobPosting) {
    ReplyService service;
    service.createReply(1, 1);
    service.createReply(1, 2);
    service.createReply(2, 1);

    auto replies = service.getRepliesByJobPosting(1);
    EXPECT_EQ(2u, replies.size());
}


TEST(JobService, CreateWorker) {
    JobService service;
    std::vector<std::string> skills = {"C++"};
    size_t id = service.createWorker("John", "john@email.com", skills, 5, "Moscow", "Resume");
    EXPECT_NE(0u, id);
}

TEST(JobService, CreateEmployer) {
    JobService service;
    size_t id = service.createEmployer("Tech Corp", "info@techcorp.com", "Description", "Moscow");
    EXPECT_NE(0u, id);
}

TEST(JobService, CreateJobPosting) {
    JobService service;
    size_t employerId = service.createEmployer("Tech Corp", "info@techcorp.com", "Description", "Moscow");
    size_t id = service.createJobPosting(employerId, "Developer", "Job description", 100000, "Moscow", true, "Software Engineer");
    EXPECT_NE(0u, id);
}

TEST(JobService, CreateReply) {
    JobService service;
    size_t workerId = service.createWorker("John", "john@email.com", {"C++"}, 5, "Moscow", "Resume");
    size_t employerId = service.createEmployer("Tech Corp", "info@techcorp.com", "Description", "Moscow");
    size_t jobId = service.createJobPosting(employerId, "Developer", "Job description", 100000, "Moscow", true, "Software Engineer");

    size_t replyId = service.createReply(jobId, workerId);
    EXPECT_NE(0u, replyId);
}

TEST(JobService, DeleteJobPosting) {
    JobService service;
    size_t employerId = service.createEmployer("Tech Corp", "info@techcorp.com", "Description", "Moscow");
    size_t id = service.createJobPosting(employerId, "Developer", "Job description", 100000, "Moscow", true, "Software Engineer");

    bool result = service.deleteJobPosting(id);
    EXPECT_TRUE(result);
}

TEST(JobService, GetAllEmployers) {
    JobService service;
    service.createEmployer("Company 1", "c1@email.com", "Desc 1", "Moscow");
    service.createEmployer("Company 2", "c2@email.com", "Desc 2", "Saint Petersburg");

    auto employers = service.getAllEmployers();
    EXPECT_EQ(2u, employers.size());
}

TEST(JobService, GetJobPostingsByEmployer) {
    JobService service;
    size_t employerId = service.createEmployer("Tech Corp", "info@techcorp.com", "Description", "Moscow");
    service.createJobPosting(employerId, "Job 1", "Desc 1", 50000, "Moscow", false, "Engineer");
    service.createJobPosting(employerId, "Job 2", "Desc 2", 60000, "Moscow", true, "Engineer");

    auto jobs = service.getJobPostingsByEmployer(employerId);
    EXPECT_EQ(2u, jobs.size());
}

TEST(JobService, SearchJobPostings) {
    JobService service;
    size_t employerId = service.createEmployer("Tech Corp", "info@techcorp.com", "Description", "Moscow");
    service.createJobPosting(employerId, "Job 1", "Desc 1", 50000, "Moscow", false, "Engineer");
    service.createJobPosting(employerId, "Job 2", "Desc 2", 60000, "Moscow", true, "Engineer");

    JobFilter filter;
    filter.setIsRemote(true);

    auto jobs = service.searchJobPostings(filter);
    EXPECT_EQ(1u, jobs.size());
}

TEST(JobService, GetRepliesByJobPosting) {
    JobService service;
    size_t workerId1 = service.createWorker("John", "john@email.com", {"C++"}, 5, "Moscow", "Resume");
    size_t workerId2 = service.createWorker("Jane", "jane@email.com", {"Python"}, 3, "Moscow", "Resume");
    size_t employerId = service.createEmployer("Tech Corp", "info@techcorp.com", "Description", "Moscow");
    size_t jobId = service.createJobPosting(employerId, "Developer", "Job description", 100000, "Moscow", true, "Software Engineer");

    service.createReply(jobId, workerId1);
    service.createReply(jobId, workerId2);

    auto replies = service.getRepliesByJobPosting(jobId);
    EXPECT_EQ(2u, replies.size());
}
