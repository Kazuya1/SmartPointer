#ifndef __CS440P3__SharedPtr__
#define __CS440P3__SharedPtr__

#include <iostream>
#include <assert.h>
#include <atomic>
#include <functional>

namespace cs540{
    class Wrapper{
    public:
        std::function<void()> trueDestructor;
        
    };
    
    
    template<typename T> class SharedPtr {
        template <typename U> friend class SharedPtr;
        template <typename T1, typename T2> friend bool operator==(const SharedPtr<T1> &, const SharedPtr<T2> &);
        template <typename T1> friend bool operator==(const SharedPtr<T1> &, std::nullptr_t);
        template <typename T1> friend bool operator==(std::nullptr_t, const SharedPtr<T1> &);
        template <typename T1, typename T2> friend bool operator!=(const SharedPtr<T1>&, const SharedPtr<T2> &);
        template <typename T1> friend bool operator!=(const SharedPtr<T1> &, std::nullptr_t);
        template <typename T1> friend bool operator!=(std::nullptr_t, const SharedPtr<T1> &);
        template <typename T1, typename U> friend SharedPtr<T1> static_pointer_cast(const SharedPtr<U> &sp);
        template <typename T1, typename U> friend SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U> &sp);
    public:
        SharedPtr(){
            ptr = NULL;
            rc = NULL;
        }
        template <typename U> explicit SharedPtr(U *p){
            if(p!=NULL){
                ptr = p;
                rc = new std::atomic<int>(1);
                w = new Wrapper();
                w->trueDestructor = [p](){delete p;};
            }else{
                ptr = NULL;
                rc = NULL;
            }
        }
        SharedPtr(const SharedPtr &p){
            if(p){
                ptr = p.ptr;
                rc = p.rc;
                w = p.w;
                (*rc).fetch_add(1);
            }else{
                ptr = NULL;
                rc = NULL;
            }
        }
        template <typename U> SharedPtr(const SharedPtr<U> &p){
            if(p){
                ptr = p.ptr;
                rc = p.rc;
                w = p.w;
                (*rc).fetch_add(1);
            }else{
                ptr = NULL;
                rc = NULL;
            }
        }
        SharedPtr(SharedPtr &&p) : SharedPtr<T>(p){
            p.reset();
        }
        template <typename U> SharedPtr(SharedPtr<U> &&p) : SharedPtr(p){
            p.reset();
        }
        SharedPtr &operator=(const SharedPtr &p){
            if(*this!=p){
                reset();
                if(p){
                    ptr = p.ptr;
                    rc = p.rc;
                    w = p.w;
                    (*rc).fetch_add(1);
                }
            }
            return *this;
        }
        template <typename U> SharedPtr<T> &operator=(const SharedPtr<U> &p){
            if(*this!=p){
                reset();
                if(p){
                    ptr = p.ptr;
                    rc = p.rc;
                    w = p.w;
                    (*rc).fetch_add(1);
                }
            }
            return *this;
        }
        SharedPtr &operator=(SharedPtr &&p){
            if(*this!=p){
                *this = p;
                p.reset();
            }
            return *this;
        }
        template <typename U> SharedPtr &operator=(SharedPtr<U> &&p){
            if(*this!=p){
                *this = p;
                p.reset();
            }
            return *this;
        }
        ~SharedPtr(){
            reset();
        }
        void reset(){
            if(*this){
                if((*rc).load()<=0) exit(1);
                if ((*rc).fetch_sub(1)==1){
                    (w->trueDestructor)();
                    delete w;
                    delete rc;
                }
                ptr = NULL;
                rc = NULL;
                w = NULL;
            }
        }
        template <typename U> void reset(U *p){
            reset();
            if(p!=NULL){
                ptr = p;
                rc = new std::atomic<int>(1);
                w = new Wrapper();
                w->trueDestructor = [p](){delete p;};
            }
        }
        T *get() const{
            return ptr;
        }
        T &operator*() const{
            return *ptr;
        }
        T *operator->() const{
            return ptr;
        }
        explicit operator bool() const{
            return ptr!=NULL;
        }
    private:
        
        T* ptr;
        Wrapper *w;
        std::atomic<int> *rc;
        
    };
    
    template <typename T1, typename T2> bool operator==(const SharedPtr<T1> &obj1, const SharedPtr<T2> &obj2){
        return obj1.get() == obj2.get();
    }
    template <typename T1> bool operator==(const SharedPtr<T1> &obj, std::nullptr_t nullPtr){
        return obj.get() == nullPtr;
    }
    template <typename T1> bool operator==(std::nullptr_t nullPtr, const SharedPtr<T1> &obj){
        return obj.get() == nullPtr;
    }
    template <typename T1, typename T2> bool operator!=(const SharedPtr<T1>&obj1, const SharedPtr<T2> &obj2){
        return obj1.get() != obj2.get();
    }
    template <typename T1> bool operator!=(const SharedPtr<T1> &obj, std::nullptr_t nullPtr){
        return obj.get() != nullPtr;
    }
    template <typename T1> bool operator!=(std::nullptr_t nullPtr, const SharedPtr<T1> &obj){
        return obj.get() != nullPtr;
    }
    template <typename T1, typename U> SharedPtr<T1> static_pointer_cast(const SharedPtr<U> &sp){
        cs540::SharedPtr<T1> ret;
        ret.ptr = static_cast<T1*>(sp.get());
        if(ret){
            ret.rc = sp.rc;
            ret.w = sp.w;
            (*(ret.rc)).fetch_add(1);
        }
        return ret;
    }
    template <typename T1, typename U> SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U> &sp){
        cs540::SharedPtr<T1> ret;
        ret.ptr = dynamic_cast<T1*>(sp.get());
        if(ret){
            ret.rc = sp.rc;
            ret.w = sp.w;
            (*(ret.rc)).fetch_add(1);
        }
        return ret;
    }
}
#endif