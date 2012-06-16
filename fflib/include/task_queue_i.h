#ifndef _TASK_QUEUE_I_
#define _TASK_QUEUE_I_

#include <list>
#include <vector>
using namespace std;

typedef void (*task_func_t)(void*);
struct task_t
{
    task_t(task_func_t f_ = 0, void* d_ = 0):
        data(d_),
        func(f_)
    {
    }
    void run()
    {
        func(data);
    }
    void*           data;
    task_func_t func;
};

class task_queue_i
{
public:
    typedef list<task_t> task_list_t;
public:
    virtual ~task_queue_i(){}
    virtual void close() = 0;
    virtual void produce(const task_t& task_) =0;
    virtual void multi_produce(const task_list_t& task_) =0;
    virtual int   consume(task_t& task_) = 0;
    virtual task_list_t* consume_all() = 0;
};

struct task_binder_t
{
    //! C function
    
    static task_t gen(void (*func_)(void*), void* p_)
    {
        return task_t(func_, p_);
    }
    template<typename RET>
    static task_t gen(RET (*func_)(void))
    {
        struct lambda_t
        {
            static void task_func(void* p_)
            {
                (*(RET(*)(void))p_)();
            };
        };
        return task_t(lambda_t::task_func, (void*)func_);
    }
    template<typename FUNCT, typename ARG1>
    static task_t gen(FUNCT func_, ARG1 arg1_)
    {
        struct lambda_t
        {
            FUNCT dest_func;
            ARG1 arg1;
            lambda_t(FUNCT func_, const ARG1& arg1_):
                dest_func(func_),
                arg1(arg1_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (*(destp->dest_func))(destp->arg1);
                delete destp;
            };
        };
        return task_t(&lambda_t::task_func, new lambda_t(func_, arg1_));
    }
    template<typename FUNCT, typename ARG1, typename ARG2>
    static task_t gen(FUNCT func_, ARG1 arg1_, ARG2 arg2_)
    {
        struct lambda_t
        {
            FUNCT dest_func;
            ARG1 arg1;
            ARG2 arg2;
            lambda_t(FUNCT func_, const ARG1& arg1_, const ARG2& arg2_):
                dest_func(func_),
                arg1(arg1_),
                arg2(arg2_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (*(destp->dest_func))(destp->arg1, destp->arg2);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, arg1_, arg2_));
    }
    template<typename FUNCT, typename ARG1, typename ARG2, typename ARG3>
    static task_t gen(FUNCT func_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_)
    {
        struct lambda_t
        {
            FUNCT dest_func;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            lambda_t(FUNCT func_, const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_):
                dest_func(func_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, arg1_, arg2_, arg3_));
    }
    template<typename FUNCT, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    static task_t gen(FUNCT func_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_, ARG4 arg4_)
    {
        struct lambda_t
        {
            FUNCT dest_func;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            lambda_t(FUNCT func_, const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_):
                dest_func(func_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, arg1_, arg2_, arg3_, arg4_));
    }
    template<typename FUNCT, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
    static task_t gen(FUNCT func_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_, ARG4 arg4_, ARG5 arg5_)
    {
        struct lambda_t
        {
            FUNCT dest_func;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            ARG5 arg5;
            lambda_t(FUNCT func_, const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_,
                     const ARG5& arg5_):
                dest_func(func_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_),
                arg5(arg5_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4, destp->arg5);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, arg1_, arg2_, arg3_, arg4_, arg5_));
    }
    template<typename FUNCT, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
    static task_t gen(FUNCT func_,
                       ARG1 arg1_, ARG2 arg2_, ARG3 arg3_, ARG4 arg4_,
                       ARG5 arg5_, ARG6 arg6_)
    {
        struct lambda_t
        {
            FUNCT dest_func;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            ARG5 arg5;
            ARG6 arg6;
            lambda_t(FUNCT func_,
                     const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_,
                     const ARG5& arg5_, const ARG6& arg6_):
                dest_func(func_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_),
                arg5(arg5_),
                arg6(arg6_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4, destp->arg5, destp->arg6);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, arg1_, arg2_, arg3_, arg4_, arg5_, arg6_));
    }
    template<typename FUNCT, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6,
            typename ARG7>
    static task_t gen(FUNCT func_,
                      ARG1 arg1_, ARG2 arg2_, ARG3 arg3_, ARG4 arg4_,
                      ARG5 arg5_, ARG6 arg6_, ARG7 arg7_)
    {
        struct lambda_t
        {
            FUNCT dest_func;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            ARG5 arg5;
            ARG6 arg6;
            ARG7 arg7;
            lambda_t(FUNCT func_, const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_,
                     const ARG5& arg5_, const ARG6& arg6_, const ARG7& arg7_):
                dest_func(func_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_),
                arg5(arg5_),
                arg6(arg6_),
                arg7(arg7_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4, destp->arg5, destp->arg6, destp->arg7);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, arg1_, arg2_, arg3_, arg4_, arg5_, arg6_, arg7_));
    }
    template<typename FUNCT, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6,
            typename ARG7, typename ARG8>
    static task_t gen(FUNCT func_,
                       ARG1 arg1_, ARG2 arg2_, ARG3 arg3_, ARG4 arg4_,
                       ARG5 arg5_, ARG6 arg6_, ARG7 arg7_, ARG8 arg8_)
    {
        struct lambda_t
        {
            FUNCT dest_func;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            ARG5 arg5;
            ARG6 arg6;
            ARG7 arg7;
            ARG8 arg8;
            lambda_t(FUNCT func_,
                     const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_,
                     const ARG5& arg5_, const ARG6& arg6_, const ARG7& arg7_, const ARG8& arg8_):
                dest_func(func_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_),
                arg5(arg5_),
                arg6(arg6_),
                arg7(arg7_),
                arg8(arg8_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4, destp->arg5, destp->arg6,
                                    destp->arg7, destp->arg8);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, arg1_, arg2_, arg3_, arg4_, arg5_, arg6_, arg7_, arg8_));
    }
    template<typename FUNCT, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6,
            typename ARG7, typename ARG8, typename ARG9>
    static task_t gen(FUNCT func_,
                      ARG1 arg1_, ARG2 arg2_, ARG3 arg3_, ARG4 arg4_,
                      ARG5 arg5_, ARG6 arg6_, ARG7 arg7_, ARG8 arg8_, ARG9 arg9_)
    {
        struct lambda_t
        {
            FUNCT dest_func;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            ARG5 arg5;
            ARG6 arg6;
            ARG7 arg7;
            ARG8 arg8;
            ARG9 arg9;
            lambda_t(FUNCT func_,
                     const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_,
                     const ARG5& arg5_, const ARG6& arg6_, const ARG7& arg7_, const ARG8& arg8_, const ARG9& arg9_):
                dest_func(func_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_),
                arg5(arg5_),
                arg6(arg6_),
                arg7(arg7_),
                arg8(arg8_),
                arg9(arg9_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4, destp->arg5, destp->arg6,
                                    destp->arg7, destp->arg8, destp->arg9);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, arg1_, arg2_, arg3_, arg4_, arg5_, arg6_, arg7_, arg8_, arg9_));
    }
    //! class fuctions
    template<typename T, typename RET>
    static task_t gen(RET (T::*func_)(void), T* obj_)
    {
        struct lambda_t
        {
            RET (T::*dest_func)(void);
            T* obj;
            lambda_t(RET (T::*func_)(void), T* obj_):
                dest_func(func_),
                obj(obj_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (destp->obj->*(destp->dest_func))();
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, obj_));
    }
    template<typename T, typename RET, typename FARG1, typename ARG1>
    static task_t gen(RET (T::*func_)(FARG1), T* obj_, ARG1 arg1_)
    {
        struct lambda_t
        {
            RET (T::*dest_func)(FARG1);
            T* obj;
            ARG1 arg1;
            lambda_t(RET (T::*func_)(FARG1), T* obj_, const ARG1& arg1_):
                dest_func(func_),
                obj(obj_),
                arg1(arg1_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (destp->obj->*(destp->dest_func))(destp->arg1);
                delete destp;
            };
        };
        return task_t(&lambda_t::task_func, new lambda_t(func_, obj_, arg1_));
    }
    template<typename T, typename RET, typename FARG1, typename FARG2, typename ARG1, typename ARG2>
    static task_t gen(RET (T::*func_)(FARG1, FARG2), T* obj_, ARG1 arg1_, ARG2 arg2_)
    {
        struct lambda_t
        {
            RET (T::*dest_func)(FARG1, FARG2);
            T* obj;
            ARG1 arg1;
            ARG2 arg2;
            lambda_t(RET (T::*func_)(FARG1, FARG2), T* obj_, const ARG1& arg1_, const ARG2& arg2_):
                dest_func(func_),
                obj(obj_),
                arg1(arg1_),
                arg2(arg2_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (destp->obj->*(destp->dest_func))(destp->arg1, destp->arg2);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, obj_, arg1_, arg2_));
    }
    template<typename T, typename RET, typename FARG1, typename FARG2, typename FARG3, typename ARG1, typename ARG2,
            typename ARG3>
    static task_t gen(RET (T::*func_)(FARG1, FARG2, FARG3), T* obj_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_)
    {
        struct lambda_t
        {
            RET (T::*dest_func)(FARG1, FARG2, FARG3);
            T* obj;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            lambda_t(RET (T::*func_)(FARG1, FARG2, FARG3), T* obj_, const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_):
                dest_func(func_),
                obj(obj_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (destp->obj->*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, obj_, arg1_, arg2_, arg3_));
    }
    template<typename T, typename RET, typename FARG1, typename FARG2, typename FARG3, typename FARG4,
             typename ARG1, typename ARG2, typename ARG3, typename ARG4>
    static task_t gen(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4), T* obj_, ARG1 arg1_, ARG2 arg2_, ARG3 arg3_, ARG4 arg4_)
    {
        struct lambda_t
        {
            RET (T::*dest_func)(FARG1, FARG2, FARG3, FARG4);
            T* obj;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            lambda_t(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4), T* obj_, const ARG1& arg1_, const ARG2& arg2_,
                     const ARG3& arg3_, const ARG4& arg4_):
                dest_func(func_),
                obj(obj_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (destp->obj->*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, obj_, arg1_, arg2_, arg3_, arg4_));
    }
    template<typename T, typename RET,  typename FARG1, typename FARG2, typename FARG3, typename FARG4, typename FARG5,
            typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
    static task_t gen(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4, FARG5), T* obj_, ARG1 arg1_, ARG2 arg2_,  ARG3 arg3_, ARG4 arg4_, ARG5 arg5_)
    {
        struct lambda_t
        {
            RET (T::*dest_func)(FARG1, FARG2, FARG3, FARG4, FARG5);
            T* obj;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            ARG5 arg5;
            lambda_t(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4, FARG5), T* obj_, const ARG1& arg1_, const ARG2& arg2_, const ARG3& arg3_, const ARG4& arg4_,
                     const ARG5& arg5_):
                dest_func(func_),
                obj(obj_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_),
                arg5(arg5_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (destp->obj->*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4, destp->arg5);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, obj_, arg1_, arg2_, arg3_, arg4_, arg5_));
    }
    template<typename T, typename RET, typename FARG1, typename FARG2, typename FARG3, typename FARG4, typename FARG5,
             typename FARG6, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6>
    static task_t gen(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6), T* obj_, ARG1 arg1_, ARG2 arg2_,
                      ARG3 arg3_, ARG4 arg4_, ARG5 arg5_, ARG6 arg6_)
    {
        struct lambda_t
        {
            RET (T::*dest_func)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6);
            T* obj;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            ARG5 arg5;
            ARG6 arg6;
            lambda_t(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6), T* obj_, const ARG1& arg1_, const ARG2& arg2_,
                     const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_, const ARG6& arg6_):
                dest_func(func_),
                obj(obj_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_),
                arg5(arg5_),
                arg6(arg6_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (destp->obj->*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4, destp->arg5, destp->arg6);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, obj_, arg1_, arg2_, arg3_, arg4_, arg5_, arg6_));
    }
    template<typename T, typename RET, typename FARG1, typename FARG2, typename FARG3, typename FARG4, typename FARG5,
             typename FARG6, typename FARG7,
             typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7>
    static task_t gen(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6, FARG7), T* obj_, ARG1 arg1_, ARG2 arg2_,
                      ARG3 arg3_, ARG4 arg4_, ARG5 arg5_, ARG6 arg6_, ARG7 arg7_)
    {
        struct lambda_t
        {
            RET (T::*dest_func)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6, FARG7);
            T* obj;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            ARG5 arg5;
            ARG6 arg6;
            ARG7 arg7;
            lambda_t(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6, FARG7), T* obj_, const ARG1& arg1_, const ARG2& arg2_,
                     const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_, const ARG6& arg6_, const ARG7& arg7_):
                dest_func(func_),
                obj(obj_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_),
                arg5(arg5_),
                arg6(arg6_),
                arg7(arg7_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (destp->obj->*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4, destp->arg5, destp->arg6,
                                                  destp->arg7);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, obj_, arg1_, arg2_, arg3_, arg4_, arg5_, arg6_, arg7_));
    }
    template<typename T, typename RET, typename FARG1, typename FARG2, typename FARG3, typename FARG4, typename FARG5,
    typename FARG6, typename FARG7, typename FARG8,
    typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7, typename ARG8>
    static task_t gen(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6, FARG7, FARG8), T* obj_, ARG1 arg1_, ARG2 arg2_,
                      ARG3 arg3_, ARG4 arg4_, ARG5 arg5_, ARG6 arg6_, ARG7 arg7_, ARG8 arg8_)
    {
        struct lambda_t
        {
            RET (T::*dest_func)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6, FARG7, FARG8);
            T* obj;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            ARG5 arg5;
            ARG6 arg6;
            ARG7 arg7;
            ARG8 arg8;
            lambda_t(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6, FARG7, FARG8), T* obj_, const ARG1& arg1_, const ARG2& arg2_,
                     const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_, const ARG6& arg6_, const ARG7& arg7_, const ARG8& arg8_):
                dest_func(func_),
                obj(obj_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_),
                arg5(arg5_),
                arg6(arg6_),
                arg7(arg7_),
                arg8(arg8_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (destp->obj->*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4, destp->arg5, destp->arg6,
                                                  destp->arg7, destp->arg8);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, obj_, arg1_, arg2_, arg3_, arg4_, arg5_, arg6_, arg7_));
    }
    template<typename T, typename RET, typename FARG1, typename FARG2, typename FARG3, typename FARG4, typename FARG5,
            typename FARG6, typename FARG7, typename FARG8, typename FARG9,
            typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5, typename ARG6, typename ARG7,
            typename ARG8, typename ARG9>
    static task_t gen(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6, FARG7, FARG8, FARG9), T* obj_, ARG1 arg1_, ARG2 arg2_,
                      ARG3 arg3_, ARG4 arg4_, ARG5 arg5_, ARG6 arg6_, ARG7 arg7_, ARG8 arg8_, ARG9 arg9_)
    {
        struct lambda_t
        {
            RET (T::*dest_func)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6, FARG7, FARG8, FARG9);
            T* obj;
            ARG1 arg1;
            ARG2 arg2;
            ARG3 arg3;
            ARG4 arg4;
            ARG5 arg5;
            ARG6 arg6;
            ARG7 arg7;
            ARG8 arg8;
            ARG9 arg9;
            lambda_t(RET (T::*func_)(FARG1, FARG2, FARG3, FARG4, FARG5, FARG6, FARG7, FARG8, FARG9), T* obj_,
                     const ARG1& arg1_, const ARG2& arg2_,
                     const ARG3& arg3_, const ARG4& arg4_, const ARG5& arg5_, const ARG6& arg6_, const ARG7& arg7_,
                     const ARG8& arg8_, const ARG9& arg9_):
                dest_func(func_),
                obj(obj_),
                arg1(arg1_),
                arg2(arg2_),
                arg3(arg3_),
                arg4(arg4_),
                arg5(arg5_),
                arg6(arg6_),
                arg7(arg7_),
                arg8(arg8_),
                arg9(arg9_)
            {}
            static void task_func(void* p_)
            {
                lambda_t* destp = (lambda_t*)p_;
                (destp->obj->*(destp->dest_func))(destp->arg1, destp->arg2, destp->arg3, destp->arg4, destp->arg5, destp->arg6,
                                                  destp->arg7, destp->arg8, destp->arg9);
                delete destp;
            };
        };
        return task_t(lambda_t::task_func, new lambda_t(func_, obj_, arg1_, arg2_, arg3_, arg4_, arg5_, arg6_, arg7_, arg9_));
    }
};
#endif
