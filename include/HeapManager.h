#ifndef __HEAPMANAGER_H__
#define __HEAPMANAGER_H__

#include <list>

// Object lifetime manager. Uses type erasure to hold a list of pointers
// to a hetergenous collection of objects. Objects are deleted when
// the HeapManager's destructor is called.
class HeapManager
{
    private:
        // Type-erasure base class
        class Managed {
            public:
                virtual ~Managed() = default;
        };

        template<typename T>
        class ManagedSpecialized : public Managed {
            public:
                ManagedSpecialized(T * obj) : obj(obj) {}
                ManagedSpecialized(const ManagedSpecialized &) = delete;
                virtual ~ManagedSpecialized() { delete obj; }
                T * obj;
        };

        std::list<std::unique_ptr<Managed>> managed;
    public:
        template<typename T>
        void add(T * obj) {
            managed.push_back(std::make_unique<ManagedSpecialized<T>>(obj));
        }
};

#endif
