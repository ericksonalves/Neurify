#include <array>
#include <chrono>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#define BUFFER_SIZE 128
#define NEURIFY_PATH "./network_test"
#define PIPE_READ_MODE "r"

const std::vector<std::string> neurify_args{"models/iris.nnet"};

std::string build_neurify_command(const std::string &property) {
  std::stringstream stream;

  stream << NEURIFY_PATH << " ";

  stream << property << " ";

  for (const std::string &arg : neurify_args) {
    stream << arg << " ";
  }

  stream << " 2>&1";

  return stream.str();
}

std::vector<std::string> execute_command(const std::string &command) {
  std::vector<std::string> output;
  std::array<char, BUFFER_SIZE> buffer;
  std::shared_ptr<FILE> pipe(popen(command.c_str(), PIPE_READ_MODE), pclose);

  if (pipe) {
    while (!feof(pipe.get())) {
      if (fgets(buffer.data(), BUFFER_SIZE, pipe.get()) != nullptr) {
        std::string raw_data = std::string(buffer.data());
        std::string data = raw_data.substr(0, raw_data.length() - 1);
        output.push_back(data);
      }
    }
  }

  return output;
}

void verify_benchmark(const std::string &property) {
  std::cout << "[START] verification task: " << property << std::endl;

  auto begin = std::chrono::steady_clock::now();

  std::string command = build_neurify_command(property);

  std::cout << command << std::endl;

  std::vector<std::string> output = execute_command(command);

  auto end = std::chrono::steady_clock::now();

  auto elapsed_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
          .count();

  std::cout << "[STATUS] elapsed: " << elapsed_time << "ms" << std::endl;

  std::cout << "[STATUS] building verification log" << std::endl;

  std::experimental::filesystem::path path = "./" + property + ".txt";

  std::ofstream log;
  log.open(path.string().c_str());

  for (const std::string &data : output) {
    log << data << std::endl;
  }

  log.close();

  std::cout << "[FINISH] verification task: " << property << std::endl;
}

int main(int argc, const char *argv[]) {
  std::cout << "benchmark runner v0.1" << std::endl;

  std::vector<std::string> benchmarks{"1000", "1001", "1002"};

  for (const auto &benchmark : benchmarks) {
    verify_benchmark(benchmark);
  }

  std::cout << "finished" << std::endl;

  return 0;
}
