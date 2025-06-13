#ifndef HMACH_HPP
#define HMACH_HPP

#include "config.h"

class HMACH {
public:
    // Initialize HMAC with a key
    HMACH(const std::string& key);

    // Compute MAC for a single input
    std::string compute_mac(const std::string& input);

    // Compute final result for multiple MACs
    std::string compute_mac_pairwise(const std::vector<std::string>& inputs);

private:
    std::string key_;
    unsigned char* hmac_result_;
    unsigned int hmac_result_len_;

    // Compute HMAC
    std::string compute_hmac(const std::string& message);
};

#endif // HMAC_HPP
