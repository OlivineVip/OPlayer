//
// Created by Administrator on 2021/2/5.
//

#ifndef OPLAYER_NATIVEQUEUE_H
#define OPLAYER_NATIVEQUEUE_H

#include <queue>
#include <pthread.h>


using namespace std;

template<typename T>
class NativeQueue {

    typedef void (*ReleaseCallback)(T *);

private:
    queue<T> q;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int flag; // 标记队列释放工作[true=工作状态，false=非工作状态]
    ReleaseCallback releaseCallback = NULL;
public:
    NativeQueue() {
        pthread_mutex_init(&mutex, 0);
        pthread_cond_init(&cond, 0);
    }

    ~NativeQueue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
        flag = false;
    }

    void push(T value) {
        pthread_mutex_lock(&mutex);
        if (flag) {
            q.push(value);
            pthread_cond_signal(&cond); // 通知
        } else {
            if (releaseCallback) {
                releaseCallback(value);
            }
        }
        pthread_mutex_unlock(&mutex);

    }


    int pop(T &t) {
        int ret = 0;
        pthread_mutex_lock(&mutex);
        while (flag && q.empty()) {
            // 如果工作状态 并且 队列中没有数据，就等待）（排队）
            pthread_cond_wait(&cond, &mutex);
        }
        if (!q.empty()) {
            t = q.front();
            q.pop();
            ret = 1;
        }
        pthread_mutex_unlock(&mutex);
        return ret;
    }


    void setFlag(int flag) {
        pthread_mutex_lock(&mutex);
        this->flag = flag;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    int queueSize() {
        return q.size();
    }

    void clearQueue() {
        pthread_mutex_lock(&mutex); // 为了线程的安全性，锁上

        unsigned int size = q.size();
        for (int i = 0; i < size; ++i) {
            // 循环 一个个的释放
            T value = q.front();
            if (releaseCallback && value) {
                releaseCallback(&value);
            }
            q.pop();
        }
        pthread_mutex_unlock(&mutex); // 为了让其他线程可以进来，解锁
    }

    void setReleaseCallback(ReleaseCallback callback) {
        releaseCallback = callback;
    }


    void delReleaseCallback() {
        releaseCallback = NULL;
    }
};

#endif //OPLAYER_NATIVEQUEUE_H
