//Mike Maxim
//Pth Thread Management

#include <pththreadsvc.h>

PthThreadService::PthThreadService() : m_nextid(0), m_joining(false)  { }

typedef void* (*PTH_FUNCPTR)(void*); 

thr_id PthThreadService::create(void* func, void* args) {
    pthread_t thr;
    int id = m_nextid++;

    pthread_create(&thr, NULL, (PTH_FUNCPTR)func, args);

    m_thrmap[id] = thr;

    return id;
}

void PthThreadService::cancel(thr_id tid) {
    map<int,pthread_t>::iterator it;
    if (m_joining) return;
    if (m_thrmap.end() != (it = m_thrmap.find(tid))) {
        pthread_t thr = it->second;
        m_player->move_now();
        pthread_join(thr, NULL);
    }
}     

void PthThreadService::pause(thr_id tid) {
    WRITEPIPE("PthThreadService: pause() not implemented!\n");
}

void PthThreadService::resume(thr_id tid) {
    WRITEPIPE("PthThreadService:: resume() not implemented!\n");
}
