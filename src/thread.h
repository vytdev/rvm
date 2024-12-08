#ifndef RVM_THREAD_H_
#define RVM_THREAD_H_

#if defined(__linux__)
#  include <pthread.h>
#  include <sched.h>

typedef pthread_t thread_t;
#  define thread_make(t,f,a) (pthread_create((t), NULL, (f), (a)) == 0)
#  define thread_join(t) (pthread_join((t), NULL) == 0)
#  define thread_yield() sched_yield()

typedef pthread_mutex_t mutex_t;
#  define mutex_make(m) (pthread_mutex_init((m), NULL) == 0)
#  define mutex_free(m) (pthread_mutex_destroy((m)) == 0)
#  define mutex_acqr(m) (pthread_mutex_lock((m)) == 0)
#  define mutex_rels(m) (pthread_mutex_unlock((m)) == 0)

typedef pthread_cond_t cond_t;
#  define cond_make(c) (pthread_cond_init((c), NULL) == 0)
#  define cond_free(c) (pthread_cond_destroy((c)) == 0)
#  define cond_wait(c,m) (pthread_cond_wait((c), (m)) == 0)
#  define cond_sgnl(c) (pthread_cond_signal((c)) == 0)
#  define cond_brcs(c) (pthread_cond_broadcast((c)) == 0)

#define TLOCAL __thread

#endif // defined(__linux__)

#endif // RVM_THREAD_H_

