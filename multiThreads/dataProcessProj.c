//
// Created by Peter on 2026/7/5.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#define SENSOR_QUEUE_SIZE       64
#define PROCESS_QUEUE_SIZE      32
#define RESULT_QUEUE_SIZE       16
#define WATCHDOG_TIMEOUT        5

typedef enum {
    SENSOR_TEMP,
    SENSOR_HUMIDITY,
    SENSOR_PRESSURE,
    SENSOR_ACCEL,
    SENSOR_MAX
} SensorType;

typedef struct {
    SensorType type;
    uint64_t timestamp;
    double value;
} SensorData;

typedef struct {
    SensorData data;
    double filteredValue;
    int quality;
}ProcessData;

typedef struct {
    uint64_t timestamp;
    double temperature;
    double humidity;
    double pressure;
    double accelX;
    double accelY;
    double accelZ;
    int checksum;
} ResultData;

typedef struct {
    SensorData items[SENSOR_QUEUE_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t notFull;  // producer uses to wait for not full, but signal not empty
    pthread_cond_t notEmpty; // consumer uses to wait for not empty, but signal not full
} SensorQueue;

typedef struct {
    ProcessData items[PROCESS_QUEUE_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t notFull;
    pthread_cond_t notEmpty;
} ProcessQueue;

typedef struct {
    ResultData items[RESULT_QUEUE_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t notFull;
    pthread_cond_t notEmpty;
} ResultQueue;

typedef struct {
    pthread_mutex_t mutex;
    int sensorAlive[SENSOR_MAX];
    int processAlive;
    int commAlive[3];
    time_t lastHeartbeat[SENSOR_MAX + 5];
} WatchDogStatus;

SensorQueue g_sensorQueue;
ProcessQueue g_processQueue;
ResultQueue g_resultQueue;
WatchDogStatus g_wdStatus;

int sensorQueueInit(SensorQueue *q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->notFull, NULL);
    pthread_cond_init(&q->notEmpty, NULL);
    return 0;
}

int sensorQueuePush(SensorQueue *q, const SensorData *data) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == SENSOR_QUEUE_SIZE) {
        pthread_cond_wait(&q->notFull, &q->mutex);
    }

    q->items[q->tail] = *data;
    q->tail = (q->tail + 1) % SENSOR_QUEUE_SIZE;
    q->count++;
    pthread_cond_signal(&q->notEmpty);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

int sensorQueuePop(SensorQueue *q, SensorData *data) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0) {
        pthread_cond_wait(&q->notEmpty, &q->mutex);
    }
    *data = q->items[q->head];
    q->head = (q->head + 1) % SENSOR_QUEUE_SIZE;
    q->count--;
    pthread_cond_signal(&q->notFull);
    pthread_mutex_unlock(&q->mutex);

    return 0;
}


int processQueueInit(ProcessQueue *q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->notFull, NULL);
    pthread_cond_init(&q->notEmpty, NULL);
    return 0;
}

int processQueuePush(ProcessQueue *q, const ProcessData *data) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == PROCESS_QUEUE_SIZE) {
        pthread_cond_wait(&q->notFull, &q->mutex);
    }
    q->items[q->tail] = *data;
    q->tail = (q->tail + 1) % PROCESS_QUEUE_SIZE;
    q->count++;
    pthread_cond_signal(&q->notEmpty);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

int processQueuePop(ProcessQueue *q, ProcessData *data) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0) {
        pthread_cond_wait(&q->notEmpty, &q->mutex);
    }
    *data = q->items[q->head];
    q->head = (q->head + 1) % PROCESS_QUEUE_SIZE;
    q->count--;
    pthread_cond_signal(&q->notFull);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

int resultQueueInit(ResultQueue *q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->notFull, NULL);
    pthread_cond_init(&q->notEmpty, NULL);
    return 0;
}

int resultQueuePush(ResultQueue *q, const ResultData *data) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == RESULT_QUEUE_SIZE) {
        pthread_cond_wait(&q->notFull, &q->mutex);
    }
    q->items[q->tail] = *data;
    q->tail = (q->tail + 1) % RESULT_QUEUE_SIZE;
    q->count++;
    pthread_cond_signal(&q->notEmpty);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

int resultQueuePop(ResultQueue *q, ResultData *data) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0) {
        pthread_cond_wait(&q->notEmpty, &q->mutex);
    }
    *data = q->items[q->head];
    q->head = (q->head + 1) % RESULT_QUEUE_SIZE;
    q->count--;
    pthread_cond_signal(&q->notFull);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

double getTemperature(SensorType type) {
    static double temp = 25.0;
    temp += (rand() % 100 - 50) / 100.0;
    return temp;
}

double getHumidity(SensorType type) {
    static double humidity = 60.0;
    humidity += (rand() % 100 - 50) / 100.0;
    return humidity;
}

double getPressure(SensorType type) {
    static double pressure = 1013.25;
    pressure += (rand() % 200 - 100) / 100.0;
    return pressure;
}

double getAcceleration(SensorType type) {
    static double accel = 0.0;
    accel += (rand() % 2000 - 1000) / 1000.0;
    return accel;
}

uint64_t getTimestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

double applyFilter(double value) {
    static double prevValue = 0.0;
    double filtered = 0.3 * value + 0.7 * prevValue;
    prevValue = value;
    return filtered;
}

int checkQuality(double value) {
    if (value < -1000 || value > 10000) return 0;
    if (value < -100 || value > 1000) return 1;
    return 2;
}

int calculateChecksum(ResultData *result) {
    uint8_t *data = (uint8_t *)result;
    int sum = 0;
    for (int i = 0; i < sizeof(ResultData) - sizeof(int); i++) {
        sum += data[i];
    }
    return sum & 0xFFFF;
}

void uartSend(const char *data) {
    printf("[UART] %s", data);
}

int sendOverNetwork(ResultData *data, size_t size) {
    printf("[NETWORK] 发送数据, 大小: %zu bytes\n", size);
    return 0;
}

void writeToFlash(ResultData *data, size_t size) {
    printf("[FLASH] 写入数据, 大小: %zu bytes\n", size);
}

void restartSensor(int index) {
    printf("[RESTART] 重启传感器%d\n", index);
}

void restartProcessThread() {
    printf("[RESTART] 重启预处理线程\n");
}

void restartComputeThread() {
    printf("[RESTART] 重启计算线程\n");
}

void restartCommThread(int index) {
    printf("[RESTART] 重启通信线程%d\n", index);
}

void *timerThread(void *arg) {
    while (1) {
        printf("[TIMER] 定时任务触发\n");
        sleep(5);
    }
    return NULL;
}

void *sensorThread(void *arg) {
    SensorType type = *(SensorType *) arg;
    SensorData data;
    double (*getSensorValue)(SensorType);

    switch (type) {
        case SENSOR_TEMP: getSensorValue = getTemperature; break;
        case SENSOR_HUMIDITY: getSensorValue = getHumidity; break;
        case SENSOR_PRESSURE: getSensorValue = getPressure; break;
        case SENSOR_ACCEL: getSensorValue = getAcceleration; break;
        default: return NULL;
    }

    while (1) {
        data.type = type;
        data.timestamp = getTimestamp();
        data.value = getSensorValue(type);

        // TODO: 这里的内存存在问题。
        sensorQueuePush(&g_sensorQueue, &data);

        pthread_mutex_lock(&g_wdStatus.mutex);
        g_wdStatus.sensorAlive[type] = 1;
        g_wdStatus.lastHeartbeat[type] = time(NULL);
        pthread_mutex_unlock(&g_wdStatus.mutex);

        switch (type) {
            case SENSOR_TEMP: usleep(100000); break;
            case SENSOR_HUMIDITY: usleep(100000); break;
            case SENSOR_PRESSURE: usleep(200000); break;
            case SENSOR_ACCEL: usleep(50000); break;
            default: usleep(100000); break;
        }

    }
    return NULL;
}

void *preprocessThread(void *arg) {
    SensorData sensorData;
    ProcessData processData;

    while (1) {
        sensorQueuePop(&g_sensorQueue, &sensorData);

        processData.data = sensorData;
        processData.filteredValue = applyFilter(sensorData.value);
        processData.quality = checkQuality(sensorData.value);

        processQueuePush(&g_processQueue, &processData);

        pthread_mutex_lock(&g_wdStatus.mutex);
        g_wdStatus.processAlive = 1;
        g_wdStatus.lastHeartbeat[SENSOR_MAX] = time(NULL);
        pthread_mutex_unlock(&g_wdStatus.mutex);
    }
    return NULL;
}

void *computeThread(void *arg) {
    ProcessData processData;
    ResultData result;
    int sensorCount = 0;

    memset(&result, 0, sizeof(result));

    while (1) {
        processQueuePop(&g_processQueue, &processData);

        result.timestamp = processData.data.timestamp;

        switch (processData.data.type) {
            case SENSOR_TEMP:
                result.temperature = processData.filteredValue;
                break;
            case SENSOR_HUMIDITY:
                result.humidity = processData.filteredValue;
                break;
            case SENSOR_PRESSURE:
                result.pressure = processData.filteredValue;
                break;
            case SENSOR_ACCEL:
                if (sensorCount % 3 == 0) result.accelX = processData.filteredValue;
                else if (sensorCount % 3 == 1) result.accelY = processData.filteredValue;
                else result.accelZ = processData.filteredValue;
                break;
            default:
                break;
        }

        sensorCount++;

        if (sensorCount >= 4) {
            result.checksum = calculateChecksum(&result);
            resultQueuePush(&g_resultQueue, &result);
            sensorCount = 0;
            memset(&result, 0, sizeof(result));
        }

        pthread_mutex_lock(&g_wdStatus.mutex);
        g_wdStatus.lastHeartbeat[SENSOR_MAX + 1] = time(NULL);
        pthread_mutex_unlock(&g_wdStatus.mutex);
    }
    return NULL;
}


void *uartThread(void *arg) {
    ResultData result;

    while (1) {
        resultQueuePop(&g_resultQueue, &result);

        char buf[128];
        snprintf(buf, sizeof(buf),
                 "T:%.2f H:%.2f P:%.2f AX:%.2f AY:%.2f AZ:%.2f CS:%d\n",
                 result.temperature, result.humidity, result.pressure,
                 result.accelX, result.accelY, result.accelZ,
                 result.checksum);

        uartSend(buf);

        pthread_mutex_lock(&g_wdStatus.mutex);
        g_wdStatus.commAlive[0] = 1;
        g_wdStatus.lastHeartbeat[SENSOR_MAX + 2] = time(NULL);
        pthread_mutex_unlock(&g_wdStatus.mutex);
    }
    return NULL;
}

void *networkThread(void *arg) {
    ResultData result;

    while (1) {
        resultQueuePop(&g_resultQueue, &result);

        sendOverNetwork(&result, sizeof(result));

        pthread_mutex_lock(&g_wdStatus.mutex);
        g_wdStatus.commAlive[1] = 1;
        g_wdStatus.lastHeartbeat[SENSOR_MAX + 3] = time(NULL);
        pthread_mutex_unlock(&g_wdStatus.mutex);
    }
    return NULL;
}

void *storageThread(void *arg) {
    ResultData result;

    while (1) {
        resultQueuePop(&g_resultQueue, &result);

        writeToFlash(&result, sizeof(result));

        pthread_mutex_lock(&g_wdStatus.mutex);
        g_wdStatus.commAlive[2] = 1;
        g_wdStatus.lastHeartbeat[SENSOR_MAX + 4] = time(NULL);
        pthread_mutex_unlock(&g_wdStatus.mutex);
    }
    return NULL;
}

void *watchdogThread(void *arg) {
    time_t now;
    int i;

    while (1) {
        now = time(NULL);

        pthread_mutex_lock(&g_wdStatus.mutex);

        for (i = 0; i < SENSOR_MAX; i++) {
            if (now - g_wdStatus.lastHeartbeat[i] > WATCHDOG_TIMEOUT) {
                printf("看门狗: 传感器%d超时!\n", i);
                g_wdStatus.sensorAlive[i] = 0;
                restartSensor(i);
            }
        }

        if (now - g_wdStatus.lastHeartbeat[SENSOR_MAX] > WATCHDOG_TIMEOUT) {
            printf("看门狗: 预处理线程超时!\n");
            restartProcessThread();
        }

        if (now - g_wdStatus.lastHeartbeat[SENSOR_MAX + 1] > WATCHDOG_TIMEOUT) {
            printf("看门狗: 计算线程超时!\n");
            restartComputeThread();
        }

        for (i = 0; i < 3; i++) {
            if (now - g_wdStatus.lastHeartbeat[SENSOR_MAX + 2 + i] > WATCHDOG_TIMEOUT) {
                printf("看门狗: 通信线程%d超时!\n", i);
                restartCommThread(i);
            }
        }

        pthread_mutex_unlock(&g_wdStatus.mutex);

        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t sensorTids[SENSOR_MAX];
    pthread_t preprocessTid, computeTid;
    pthread_t uartTid, networkTid, storageTid;
    pthread_t watchdogTid, timerTid;

    SensorType sensorTypes[SENSOR_MAX] = {
            SENSOR_TEMP, SENSOR_HUMIDITY, SENSOR_PRESSURE, SENSOR_ACCEL
    };

    sensorQueueInit(&g_sensorQueue);
    processQueueInit(&g_processQueue);
    resultQueueInit(&g_resultQueue);

    pthread_mutex_init(&g_wdStatus.mutex, NULL);

    for (int i = 0; i < SENSOR_MAX; i++) {
        pthread_create(&sensorTids[i], NULL, sensorThread, &sensorTypes[i]);
    }

    pthread_create(&preprocessTid, NULL, preprocessThread, NULL);
    pthread_create(&computeTid, NULL, computeThread, NULL);

    pthread_create(&uartTid, NULL, uartThread, NULL);
    pthread_create(&networkTid, NULL, networkThread, NULL);
    pthread_create(&storageTid, NULL, storageThread, NULL);

    pthread_create(&watchdogTid, NULL, watchdogThread, NULL);
    pthread_create(&timerTid, NULL, timerThread, NULL);

    for (int i = 0; i < SENSOR_MAX; i++) {
        pthread_join(sensorTids[i], NULL);
    }
    pthread_join(preprocessTid, NULL);
    pthread_join(computeTid, NULL);
    pthread_join(uartTid, NULL);
    pthread_join(networkTid, NULL);
    pthread_join(storageTid, NULL);
    pthread_join(watchdogTid, NULL);
    pthread_join(timerTid, NULL);

    return 0;
}