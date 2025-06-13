#include "HMACH.hpp"

// HMAC constructor, initialize key
HMACH::HMACH(const std::string& key) : key_(key), hmac_result_(nullptr), hmac_result_len_(SHA256_DIGEST_LENGTH) {}

// Compute MAC for a single input
std::string HMACH::compute_mac(const std::string& input) {
    return compute_hmac(input);
}

// Compute final result for multiple MACs
std::string HMACH::compute_mac_pairwise(const std::vector<std::string>& inputs) {
    std::vector<std::string> macs;

    // Compute HMAC for each input
    for (const auto& input : inputs) {
        macs.push_back(compute_mac(input));
    }

    // Iteratively compute pairwise MACs
    while (macs.size() > 1) {
        std::vector<std::string> new_macs;

        // Compute new MACs by combining pairs
        for (size_t i = 0; i < macs.size() - 1; i += 2) {
            new_macs.push_back(compute_mac(macs[i] + macs[i + 1]));
        }

        // If there's an odd number of inputs, keep the last one
        if (macs.size() % 2 != 0) {
            new_macs.push_back(macs.back());
        }

        macs = new_macs;
    }

    // Finally return the unique MAC obtained
    return macs.front();
}

// Compute HMAC
std::string HMACH::compute_hmac(const std::string& message) {
    // Create a buffer to hold the HMAC result
    unsigned char* result = new unsigned char[hmac_result_len_];

    // Compute HMAC using SHA256
    HMAC_CTX* ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key_.c_str(), key_.length(), EVP_sha256(), nullptr);
    HMAC_Update(ctx, reinterpret_cast<const unsigned char*>(message.c_str()), message.length());
    HMAC_Final(ctx, result, &hmac_result_len_);
    HMAC_CTX_free(ctx);

    // Convert the result to a hexadecimal string
    std::string mac;
    for (unsigned int i = 0; i < hmac_result_len_; ++i) {
        char buffer[3];
        sprintf(buffer, "%02x", result[i]);
        mac += std::string(buffer);
    }

    delete[] result;
    return mac;
}
