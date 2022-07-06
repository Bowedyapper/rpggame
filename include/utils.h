namespace utils {
    std::string getSystemTime();
    auto getUnixSystemTime();
    static void debugLog(const char *type, std::string text);
    int generateRandomNumber(int min, int max);
    std::string generateRandomString(int length);
}

std::string utils::getSystemTime() {
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string s(30, '\0');
    std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
    return s;
}

auto utils::getUnixSystemTime() {
    auto cur_time = std::chrono::system_clock::now();
    std::cout << "TIME: " << std::chrono::system_clock::to_time_t(cur_time) << std::endl;
    auto unixtime = cur_time.time_since_epoch();
    auto unixtime_in_s = std::chrono::duration_cast<std::chrono::seconds>(unixtime);
    return unixtime_in_s.count();
}

void utils::debugLog(const char *type, std::string text) {
    std::cout << "[" << getSystemTime() << "] "
              << "[" << type << "] " << text << std::endl;
}

int utils::generateRandomNumber(int min, int max) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(min, max);

    return dist6(rng);
}

std::string utils::generateRandomString(int length) {
    std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string str(length, '\0');
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, chars.size() - 1);
    for (int i = 0; i < length; i++) {
        str[i] = chars[distribution(generator)];
    }
    return str;
}