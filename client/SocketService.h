#include "libwebsockets.h"
#include <atomic>
#include <thread>
#include <functional>
#include <string.h>
#include <unistd.h>
#include <cstdint>
#include <pthread.h>

#define MAX_PAYLOAD_LEN          (8*1024) 

inline void set_cpu_affinity(int32_t cpu_affinity) {
  if (cpu_affinity != -1) {
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET((cpu_affinity % std::thread::hardware_concurrency()), &cpus);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpus);
  }
}

class socketService;

/**
 * callback interface
 */
class clientCallback_t {
public:
    virtual ~clientCallback_t() = default;

    /**
     * onConnected invoked when connection established
     */
    virtual void onConnected() = 0;

    /**
     * onDisconnected invoked when connection closed
     */
    virtual void onDisconnected() = 0;

    /**
     * onError invoked when error occurred
     *
     * @param msg     Error message. Might be empty.
     * @param len     Error message length
     */
    virtual void onError(const char *msg, size_t len) = 0;

    /**
     * onData invoked when data received
     *
     * @param data        Data string
     * @param len         Current data length
     * @param remaining   How many data remains
     */
    virtual void onData(const char *data, size_t len, size_t remaining) = 0;
};

struct socketInfo {
  char payload[MAX_PAYLOAD_LEN]{};
  unsigned long len = 0;
  socketInfo() = default;

}


