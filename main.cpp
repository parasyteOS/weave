#include "policy.hpp"

using namespace std;

[[noreturn]] static void usage(char *arg0) {
    fprintf(stderr,
R"EOF(Weave - SELinux Policy Patch Tool

Usage: %s [--options...] [policy statements...]

Options:
   --help            show help message for policy statements
   --load FILE       load monolithic sepolicy from FILE
   --save FILE       dump monolithic sepolicy to FILE
   --print-rules     print all rules in the loaded sepolicy

If --load is not tspecified, it will load from current live policies (/sys/fs/selinux/policy)
)EOF", arg0);
    exit(1);
}

int main(int argc, char *argv[]) {
    const char *out_file = nullptr;
    // vector<string_view> rule_files;
    sepolicy *sepol = nullptr;
    bool print = false;

    if (argc < 2) usage(argv[0]);
    int i = 1;
    for (; i < argc; ++i) {
        // Parse options
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            auto option = argv[i] + 2;
            if (option == "print-rules"sv)
                print = true;
            else if (option == "load"sv) {
                if (argv[i + 1] == nullptr)
                    usage(argv[0]);
                sepol = sepolicy::from_file(argv[i + 1]);
                if (!sepol) {
                    fprintf(stderr, "Cannot load policy from %s\n", argv[i + 1]);
                    return 1;
                }
                ++i;
            } else if (option == "save"sv) {
                if (argv[i + 1] == nullptr)
                    usage(argv[0]);
                out_file = argv[i + 1];
                ++i;
            // } else if (option == "apply"sv) {
            //     if (argv[i + 1] == nullptr)
            //         usage(argv[0]);
            //     rule_files.emplace_back(argv[i + 1]);
            //     ++i;
            } else if (option == "help"sv) {
                statement_help();
            } else {
                usage(argv[0]);
            }
        } else {
            break;
        }
    }

    // Use current policy if nothing is loaded
    if (sepol == nullptr && !(sepol = sepolicy::from_file(SELINUX_POLICY))) {
        fprintf(stderr, "Cannot load policy from " SELINUX_POLICY "\n");
        return 1;
    }

    if (print) {
        sepol->print_rules();
        return 0;
    }

    // if (!rule_files.empty())
    //     for (const auto &rule_file : rule_files)
    //         sepol->load_rule_file(rule_file.data());

    for (; i < argc; ++i)
        sepol->parse_statement(argv[i]);

    if (out_file && !sepol->to_file(out_file)) {
        fprintf(stderr, "Cannot dump policy to %s\n", out_file);
        return 1;
    }

    delete sepol;
    return 0;
}
