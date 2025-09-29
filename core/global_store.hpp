#pragma once
#include <mutex>
#include <map>
#include <memory>
#include <string>

class GlobalStore {
public:
    template<typename T>
    void put(const std::string& key, const std::shared_ptr<T>& val){
        std::lock_guard<std::mutex> lk(m_); map_[key]=std::static_pointer_cast<void>(val);
    }
    template<typename T>
    std::shared_ptr<T> get(const std::string& key){
        std::lock_guard<std::mutex> lk(m_); std::map<std::string, std::shared_ptr<void> >::iterator it=map_.find(key);
        if(it==map_.end()) return std::shared_ptr<T>();
        return std::static_pointer_cast<T>(it->second);
    }
private:
    std::mutex m_;
    std::map<std::string, std::shared_ptr<void> > map_;
};
