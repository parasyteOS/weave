#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "policy.hpp"
#include "files.hpp"

sepolicy *sepolicy::from_data(char *data, size_t len) {
    policy_file_t pf;
    policy_file_init(&pf);
    pf.data = data;
    pf.len = len;
    pf.type = PF_USE_MEMORY;

    auto db = static_cast<policydb_t *>(malloc(sizeof(policydb_t)));
    if (policydb_init(db) || policydb_read(db, &pf, 0)) {
        free(db);
        return nullptr;
    }

    auto sepol = new sepol_impl(db);
    return sepol;
}

sepolicy *sepolicy::from_file(const char *file) {
    policy_file_t pf;
    policy_file_init(&pf);
    auto fp = xopen_file(file, "re");
    pf.fp = fp.get();
    pf.type = PF_USE_STDIO;

    auto db = static_cast<policydb_t *>(malloc(sizeof(policydb_t)));
    if (policydb_init(db) || policydb_read(db, &pf, 0)) {
        free(db);
        return nullptr;
    }

    auto sepol = new sepol_impl(db);
    return sepol;
}

sepol_impl::~sepol_impl() {
    policydb_destroy(db);
    free(db);
}

bool sepolicy::to_file(const char *file) {
    auto fp = xopen_file(file, "w");

    policy_file_t pf;
    policy_file_init(&pf);
    pf.type = PF_USE_STDIO;
    pf.fp = fp.get();

    if (policydb_write(impl->db, &pf)) {
        return false;
    }

    return true;
}
