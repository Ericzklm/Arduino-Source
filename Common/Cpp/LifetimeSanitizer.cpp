/*  Lifetime Sanitizer
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <set>
//#include <exception>
#include <atomic>
#include <iostream>
#include "Concurrency/SpinLock.h"
#include "LifetimeSanitizer.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef PA_SANITIZER_ENABLE



namespace PokemonAutomation{


//#define PA_SANITIZER_PRINT_ALL
const std::set<std::string> SANITIZER_FILTER = {
//    "MultiSwitchProgramSession",
//    "MultiSwitchProgramWidget2",
//    "ControllerRequirements",
};

SpinLock sanitizer_lock;
std::set<const LifetimeSanitizer*> sanitizer_map;


std::atomic<bool> LifetimeSanitizer_enabled(true);
bool LifetimeSanitizer_has_been_disabled = false;

void LifetimeSanitizer::set_enabled(bool enabled){
    if (enabled){
        LifetimeSanitizer_enabled.store(true, std::memory_order_relaxed);
        return;
    }
    WriteSpinLock lg(sanitizer_lock);
    LifetimeSanitizer_has_been_disabled = true;
    LifetimeSanitizer_enabled.store(false, std::memory_order_relaxed);
}

PA_NO_INLINE void LifetimeSanitizer::terminate_with_dump(){
    //  Intentionally crash the program here and let the crash dump deal with
    //  the error reporting.

#ifdef _WIN32
    Sleep(500);
#endif

    std::cerr << (char*)nullptr << std::endl;
}




LifetimeSanitizer::LifetimeSanitizer(const char* name)
    : m_token(SANITIZER_TOKEN)
    , m_self(this)
    , m_name(name)
{
    if (!LifetimeSanitizer_enabled.load(std::memory_order_relaxed)){
        return;
    }
    internal_construct();
}
LifetimeSanitizer::~LifetimeSanitizer(){
    if (!LifetimeSanitizer_enabled.load(std::memory_order_relaxed)){
        m_self = nullptr;
        return;
    }
    internal_destruct();
}


LifetimeSanitizer::LifetimeSanitizer(LifetimeSanitizer&& x)
    : m_token(SANITIZER_TOKEN)
    , m_self(this)
    , m_name(x.m_name)
{
    if (!LifetimeSanitizer_enabled.load(std::memory_order_relaxed)){
        return;
    }
    x.check_usage();
    internal_construct();
}
void LifetimeSanitizer::operator=(LifetimeSanitizer&& x){
    if (!LifetimeSanitizer_enabled.load(std::memory_order_relaxed)){
        return;
    }
    check_usage();
    x.check_usage();
}
LifetimeSanitizer::LifetimeSanitizer(const LifetimeSanitizer& x)
    : m_token(SANITIZER_TOKEN)
    , m_self(this)
    , m_name(x.m_name)
{
    if (!LifetimeSanitizer_enabled.load(std::memory_order_relaxed)){
        return;
    }
    x.check_usage();
    internal_construct();
}
void LifetimeSanitizer::operator=(const LifetimeSanitizer& x){
    check_usage();
    x.check_usage();
}



void LifetimeSanitizer::check_usage() const{
    if (!LifetimeSanitizer_enabled.load(std::memory_order_relaxed)){
        return;
    }

    if (m_token != SANITIZER_TOKEN || m_self != this){
        std::cerr << "Use non-existant: " << this << " : " << m_name << std::endl;
        terminate_with_dump();
    }

    if (LifetimeSanitizer_has_been_disabled){
        return;
    }

    ReadSpinLock lg(sanitizer_lock);
    if (SANITIZER_FILTER.contains(m_name)){
        std::cout << "LifetimeSanitizer - Using: " << this << " : " << m_name << std::endl;
    }
    auto iter = sanitizer_map.find(this);
    if (iter != sanitizer_map.end()){
        return;
    }
    std::cerr << "Use non-existant: " << this << " : " << m_name << std::endl;
    terminate_with_dump();
}


void LifetimeSanitizer::internal_construct(){
    WriteSpinLock lg(sanitizer_lock);
    if (SANITIZER_FILTER.contains(m_name)){
        std::cout << "LifetimeSanitizer - Allocating: " << this << " : " << m_name << std::endl;
    }
    auto iter = sanitizer_map.find(this);
    if (iter == sanitizer_map.end()){
        sanitizer_map.insert(this);
        return;
    }
    std::cerr << "LifetimeSanitizer - Double allocation: " << this << " : " << m_name << std::endl;
    terminate_with_dump();
}
void LifetimeSanitizer::internal_destruct(){
    void* self = m_self;
    m_self = nullptr;

    WriteSpinLock lg(sanitizer_lock);
    if (m_token != SANITIZER_TOKEN || self != this){
        std::cerr << "LifetimeSanitizer - Free non-existant: " << this << " : " << m_name << std::endl;
        terminate_with_dump();
    }

    if (SANITIZER_FILTER.contains(m_name)){
        std::cout << "LifetimeSanitizer - Freeing: " << this << " : " << m_name << std::endl;
    }
    auto iter = sanitizer_map.find(this);
    if (iter != sanitizer_map.end()){
        sanitizer_map.erase(this);
        return;
    }

    //  Skip this check if we've been disabled before in case there's stuff we
    //  haven't tracked.
    if (LifetimeSanitizer_has_been_disabled){
        return;
    }

    std::cerr << "LifetimeSanitizer - Free non-existant: " << this << " : " << m_name << std::endl;
    terminate_with_dump();
}






#endif
}
